/******************************************************************************

 Copyright (c) 2021-2022 Bytedance Inc.

 ASTC Texture Decompression.

 astc compression tool: https://github.com/ARM-software/astc-encoder/releases
 version required: v2.2+
 command:
   astcenc-avx2 -cl test1.png test1.astc 6x6 -medium -pp-premultiply

 ******************************************************************************/

#include "base/astc.h"

#include <thread>
#include <mutex>
#include <memory>
#include <vector>
#include <deque>
#include "astcenc/astcenc.h"
#include "astcenc/astcenc_internal_entry.h"
#include "yasio/detail/utils.hpp"

#define ASTCDEC_NO_CONTEXT 1
#define ASTCDEC_PRINT_BENCHMARK 0

typedef std::mutex astc_decompress_mutex_t;

struct astc_decompress_task
{
    astc_decompress_task() {}
    ~astc_decompress_task()
    {
#if ASTCDEC_NO_CONTEXT
        if (_bsd)
            aligned_free<block_size_descriptor>(_bsd);
#else
        if (_context)
            astcenc_context_free(this->_context);
#endif
    }

    void wait_done()
    {
#if ASTCDEC_NO_CONTEXT
        _decompress_pm.wait();
#else
        _context->manage_decompress.wait();
#endif
    }

    const uint8_t* _in_texels = nullptr;
    void* _out_texels[1]{};
    unsigned int _xblocks, _yblocks;
#if ASTCDEC_NO_CONTEXT
    unsigned int _block_x, _block_y;
    ParallelManager _decompress_pm{};
    block_size_descriptor* _bsd = nullptr;
#else
    astcenc_config _config{};
    astcenc_context* _context = nullptr;
#endif
    astcenc_image _image_out{};
};

class astc_decompress_job_manager
{
public:
    static astc_decompress_job_manager* get_instance()
    {
        static astc_decompress_job_manager s_instance;
        return &s_instance;
    }
    astc_decompress_job_manager()
    {
        int thread_count = std::thread::hardware_concurrency();
        for (int i = 0; i < thread_count; ++i)
        {
            _threads.emplace_back(std::thread{&astc_decompress_job_manager::run, this});
        }
    }

    ~astc_decompress_job_manager()
    {
        _stopped = true;

        _task_queue_mtx.lock();
        _task_queue.clear();
        _task_queue_cv.notify_all();
        _task_queue_mtx.unlock();

        for (auto&& t : _threads)
        {
            if (t.joinable())
                t.join();
        }
        _threads.clear();
    }

    int decompress_parallel_sync(const uint8_t* in,
                                 uint32_t inlen,
                                 uint8_t* out,
                                 unsigned int dim_x,
                                 unsigned int dim_y,
                                 int block_x,
                                 int block_y)
    {

        auto task = make_task(in, inlen, out, dim_x, dim_y, block_x, block_y);
        if (!task)
            return ASTCENC_ERR_OUT_OF_MEM;

        _task_queue_mtx.lock();
        _task_queue.emplace_back(task);
        _task_queue_mtx.unlock();
        _task_queue_cv.notify_all();  // notify all thread to process the single decompress task parallel

        task->wait_done();

        _task_queue_mtx.lock();
        assert(!_task_queue.empty());
        auto t = _task_queue.front();
        assert(t.get() == task.get());
        _task_queue.pop_front();
        _task_queue_mtx.unlock();

        return ASTCENC_SUCCESS;
    }

private:
    std::shared_ptr<astc_decompress_task> make_task(const uint8_t* in,
                                                    unsigned int inlen,
                                                    uint8_t* out,
                                                    unsigned int dim_x,
                                                    unsigned int dim_y,
                                                    int block_x,
                                                    int block_y)
    {
        unsigned int xblocks = (dim_x + block_x - 1) / block_x;
        unsigned int yblocks = (dim_y + block_y - 1) / block_y;
        unsigned int zblocks = 1;  // (dim_z + block_z - 1) / block_z;

        // Check we have enough output space (16 bytes per block)
        auto total_blocks  = xblocks * yblocks * zblocks;
        size_t size_needed = total_blocks * 16;
        if (inlen < size_needed)
            return nullptr;

        auto task            = std::make_shared<astc_decompress_task>();
        task->_in_texels     = in;
        task->_out_texels[0] = out;
        task->_image_out     = {dim_x, dim_y, 1, ASTCENC_TYPE_U8, task->_out_texels};

        task->_xblocks = xblocks;
        task->_yblocks = yblocks;
#if ASTCDEC_NO_CONTEXT
        // since astcenc-3.3, doesn't required
        // static std::once_flag once_flag;
        // std::call_once(once_flag, init_quant_mode_table);

        task->_block_x = block_x;
        task->_block_y = block_y;
        task->_bsd     = aligned_malloc<block_size_descriptor>(sizeof(block_size_descriptor), ASTCENC_VECALIGN);
        init_block_size_descriptor(block_x, block_y, 1, false, 0 /*unused for decompress*/, 0, *task->_bsd);
        task->_decompress_pm.init(total_blocks);
#else
        (void)astcenc_config_init(ASTCENC_PRF_LDR, block_x, block_y, 1, 0, ASTCENC_FLG_DECOMPRESS_ONLY, &task->_config);
        (void)astcenc_context_alloc(&task->_config, (unsigned int)_threads.size(), &task->_context);
        task->_context->manage_decompress.init(total_blocks);
#endif
        return task;
    }
    void run()
    {
        const astcenc_swizzle swz_decode{ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A};

        bool no_task_count = false;

        for (;;)
        {
            std::unique_lock<astc_decompress_mutex_t> lck(_task_queue_mtx);
            if (!_stopped && (_task_queue.empty() || no_task_count))
                _task_queue_cv.wait(lck);

            if (_stopped)
                break;

            if (_task_queue.empty())
                continue;
            auto task = _task_queue.front();
            lck.unlock();  // unlock make sure other thread can work for the task

            auto& image_out = task->_image_out;

#if ASTCDEC_NO_CONTEXT
            unsigned int block_x = task->_block_x;
            unsigned int block_y = task->_block_y;
            unsigned int block_z = 1;  // task->block_z;
            auto& bsd            = *task->_bsd;
            auto& decompress_pm  = task->_decompress_pm;
#else
            unsigned int block_x = task->_config.block_x;
            unsigned int block_y = task->_config.block_y;
            unsigned int block_z = 1;  // task->_config.block_z;
            auto& bsd = *task->_context->context.bsd;
            auto& decompress_pm = task->_context->manage_decompress;
#endif
            unsigned int xblocks = task->_xblocks;
            unsigned int yblocks = task->_yblocks;
            unsigned int zblocks = 1;  // (image_out.dim_z + block_z - 1) / block_z;

            int row_blocks   = xblocks;
            int plane_blocks = xblocks * yblocks;

            image_block blk;
            auto data = task->_in_texels;
            for (;;)
            {  // process the task
                unsigned int count = 0;
                unsigned int base  = decompress_pm.get_task_assignment(128, count);

                no_task_count = !count;
                if (no_task_count)
                {  // this thread will going to suspend until new task added
                    break;
                }

                for (unsigned int i = base; i < base + count; i++)
                {
                    // Decode i into x, y, z block indices
                    int z            = i / plane_blocks;
                    unsigned int rem = i - (z * plane_blocks);
                    int y            = rem / row_blocks;
                    int x            = rem - (y * row_blocks);

                    unsigned int offset           = (((z * yblocks + y) * xblocks) + x) * 16;
                    const uint8_t* bp             = data + offset;
                    physical_compressed_block pcb = *(const physical_compressed_block*)bp;
                    symbolic_compressed_block scb;

                    physical_to_symbolic(bsd, pcb, scb);

                    decompress_symbolic_block(ASTCENC_PRF_LDR, bsd, x * block_x, y * block_y, z * block_z, scb, blk);

                    store_image_block(image_out, blk, bsd, x * block_x, y * block_y, z * block_z, swz_decode);
                }

                decompress_pm.complete_task_assignment(count);
            }
        }
    }

    std::vector<std::thread> _threads;

    std::deque<std::shared_ptr<astc_decompress_task>> _task_queue;
    astc_decompress_mutex_t _task_queue_mtx;
    std::condition_variable_any _task_queue_cv;

    bool _stopped = false;
};

int astc_decompress_image(const uint8_t* in,
                          uint32_t inlen,
                          uint8_t* out,
                          uint32_t dim_x,
                          uint32_t dim_y,
                          uint32_t block_x,
                          uint32_t block_y)
{
#if ASTCDEC_PRINT_BENCHMARK
    struct benchmark_printer
    {
        benchmark_printer(const char* fmt, int w, int h, float den)
            : _fmt(fmt), _w(w), _h(h), _den(den), _start(yasio::highp_clock())
        {}
        ~benchmark_printer() { ax::log(_fmt, _w, _h, (yasio::highp_clock() - _start) / _den); }
        const char* _fmt;
        int _w, _h;
        float _den;
        yasio::highp_time_t _start;
    };
    benchmark_printer __printer("decompress astc image (%dx%d) cost: %.3lf(ms)", dim_x, dim_y, (float)std::milli::den);
#endif
    return astc_decompress_job_manager::get_instance()->decompress_parallel_sync(in, inlen, out, dim_x, dim_y, block_x,
                                                                                 block_y);
}
