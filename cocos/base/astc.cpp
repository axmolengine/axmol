/******************************************************************************

 Copyright (c) 2021 Bytedance Inc.

 ASTC Texture Decompression.

 astc compression tool: https://github.com/ARM-software/astc-encoder/releases
 version required: v2.2+
 command:
   astcenc-sse4.2 -cl test1.png test1.astc 6x6 -medium -pp-premultiply

 ******************************************************************************/

#include "base/astc.h"
#include "astc/astcenc.h"
#include "astc/astcenc_internal.h"

#include "yasio/detail/utils.hpp"

#define ASTCDEC_NO_CONTEXT      1
#define ASTCDEC_PRINT_BENCHMARK 0

typedef std::mutex astc_decompress_mutex_t;

struct astc_decompress_task {
    astc_decompress_task(
        const uint8_t* in, uint8_t* out, unsigned int dim_x, unsigned int dim_y, int block_x, int block_y);

    ~astc_decompress_task();

    void addRef() {
        ++_nref;
    }

    void release() {
        if (--_nref == 0)
            delete this;
    }

    astcenc_config _config{};

    const uint8_t* _in_texels = nullptr;
    void* _out_texels[1]{};
    uint32_t _out_len = 0;
#if ASTCDEC_NO_CONTEXT
    ParallelManager _decompress_pm{};
    block_size_descriptor _bsd{};
#else
    astcenc_context* _context = nullptr;
#endif
    astcenc_image _image_out{};


    std::atomic<int> _nref;
};

class astc_decompress_job_manager {
public:
    static int s_thread_count;

    static astc_decompress_job_manager* get_instance() {
        static astc_decompress_job_manager s_instance;
        return &s_instance;
    }
    astc_decompress_job_manager() {
        s_thread_count = std::thread::hardware_concurrency();

        for (int i = 0; i < s_thread_count; ++i) {
            _threads.push_back(std::thread{&astc_decompress_job_manager::run, this});
        }
    }

    ~astc_decompress_job_manager() {
        _stopped = true;

        _taskQueueMtx.lock();
        for (auto task : _taskQueue)
            task->release();
        _taskQueue.clear();
        _taskQueueCV.notify_all();
        _taskQueueMtx.unlock();

        for (auto& t : _threads) {
            if (t.joinable())
                t.join();
        }
        _threads.clear();
    }

    int decompress_parallel_sync(const uint8_t* in, uint32_t inlen, uint8_t* out, unsigned int dim_x,
        unsigned int dim_y, int block_x, int block_y) {
        unsigned int xblocks = (dim_x + block_x - 1) / block_x;
        unsigned int yblocks = (dim_y + block_y - 1) / block_y;
        unsigned int zblocks = 1; // (dim_z + block_z - 1) / block_z;

        // Check we have enough output space (16 bytes per block)
        auto total_blocks  = xblocks * yblocks * zblocks;
        size_t size_needed = total_blocks * 16;
        if (inlen < size_needed) {
            return ASTCENC_ERR_OUT_OF_MEM;
        }
#if ASTCDEC_NO_CONTEXT
        static std::once_flag once_flag;
        std::call_once(once_flag, init_quant_mode_table);
#endif
        auto task = new astc_decompress_task(in, out, dim_x, dim_y, block_x, block_y);
#if ASTCDEC_NO_CONTEXT
        task->_decompress_pm.init(total_blocks);
#else
        task->_context->manage_decompress.init(total_blocks);
#endif
        _taskQueueMtx.lock();
        _taskQueue.push_back(task);
        _taskQueueMtx.unlock();
        _taskQueueCV.notify_all(); // notify all to work for the single decompress task

#if ASTCDEC_NO_CONTEXT
        task->_decompress_pm.wait();
#else
        task->_context->manage_decompress.wait();
#endif

        _taskQueueMtx.lock();
        assert(!_taskQueue.empty());

        auto t = _taskQueue.front();
        assert(t == task);
        _taskQueue.pop_front();
        _taskQueueMtx.unlock();

        task->release();

        return ASTCENC_SUCCESS;
    }

private:
    void run() {
        const astcenc_swizzle swz_decode{ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A};

        bool no_task_count = false;

        for (;;) {
            std::unique_lock<astc_decompress_mutex_t> lck(_taskQueueMtx);
            if (!_stopped && (_taskQueue.empty() || no_task_count))
                _taskQueueCV.wait(lck);

            if (_stopped)
                break;

            if (_taskQueue.empty())
                continue;
            lck.unlock(); // unlock make sure other thread can work for the task

            auto task       = _taskQueue.front();
            auto& image_out = task->_image_out;

            unsigned int block_x = task->_config.block_x;
            unsigned int block_y = task->_config.block_y;
            unsigned int block_z = task->_config.block_z;

            unsigned int xblocks = (image_out.dim_x + block_x - 1) / block_x;
            unsigned int yblocks = (image_out.dim_y + block_y - 1) / block_y;
            unsigned int zblocks = 1; // (image_out.dim_z + block_z - 1) / block_z;

            const auto profile = task->_config.profile;

#if ASTCDEC_NO_CONTEXT
            auto& bsd           = task->_bsd;
            auto& decompress_pm = task->_decompress_pm;
#else
            auto& bsd           = *task->_context->bsd;
            auto& decompress_pm = task->_context->manage_decompress;
#endif
            int row_blocks   = xblocks;
            int plane_blocks = xblocks * yblocks;

            image_block blk;

            auto data = task->_in_texels;
            for (;;) { // process the task
                unsigned int count = 0;
                unsigned int base  = decompress_pm.get_task_assignment(128, count);

                no_task_count = !count;
                if (no_task_count) { // this thread will going to suspend until new task added
                    break;
                }

                for (unsigned int i = base; i < base + count; i++) {
                    // Decode i into x, y, z block indices
                    int z            = i / plane_blocks;
                    unsigned int rem = i - (z * plane_blocks);
                    int y            = rem / row_blocks;
                    int x            = rem - (y * row_blocks);

                    unsigned int offset           = (((z * yblocks + y) * xblocks) + x) * 16;
                    const uint8_t* bp             = data + offset;
                    physical_compressed_block pcb = *(const physical_compressed_block*) bp;
                    symbolic_compressed_block scb;

                    physical_to_symbolic(bsd, pcb, scb);

                    decompress_symbolic_block(profile, bsd, x * block_x, y * block_y, z * block_z, scb, blk);

                    write_image_block(image_out, blk, bsd, x * block_x, y * block_y, z * block_z, swz_decode);
                }

                decompress_pm.complete_task_assignment(count);
            }
        }
    }

    std::deque<astc_decompress_task*> _taskQueue;
    astc_decompress_mutex_t _taskQueueMtx;
    std::condition_variable_any _taskQueueCV;

    std::vector<std::thread> _threads;
    bool _stopped = false;
};
int astc_decompress_job_manager::s_thread_count = 1;

astc_decompress_task::astc_decompress_task(
    const uint8_t* in, uint8_t* out, unsigned int dim_x, unsigned int dim_y, int block_x, int block_y) {
    _nref = 1;

    this->_in_texels     = in;
    this->_out_texels[0] = out;
    this->_image_out     = {dim_x, dim_y, 1, ASTCENC_TYPE_U8, this->_out_texels};

    (void) astcenc_config_init(ASTCENC_PRF_LDR, block_x, block_y, 1, 0, ASTCENC_FLG_DECOMPRESS_ONLY, &this->_config);

#if ASTCDEC_NO_CONTEXT
    init_block_size_descriptor(block_x, block_y, 1, false, 0 /*unused for decompress*/, this->_bsd);
#else
    (void) astcenc_context_alloc(&this->_config, astc_decompress_job_manager::s_thread_count, &this->_context);
#endif
}
astc_decompress_task::~astc_decompress_task() {
#if !ASTCDEC_NO_CONTEXT
    astcenc_context_free(this->_context);
#endif
}

int astc_decompress_image(const uint8_t* in, uint32_t inlen, uint8_t* out, uint32_t dim_x, uint32_t dim_y,
    uint32_t block_x, uint32_t block_y) {
#if ASTCDEC_PRINT_BENCHMARK
    struct benchmark_printer {
        benchmark_printer(const char* fmt, float den) : _fmt(fmt), _den(den), _start(yasio::highp_clock()) {}
        ~benchmark_printer() {
            cocos2d::log(_fmt, (yasio::highp_clock() - _start) / _den);
        }
        const char* _fmt;
        float _den;
        yasio::highp_time_t _start;
    };
    benchmark_printer __printer("decompress astc image cost: %.3lf(ms)", (float) std::milli::den);
#endif
    return astc_decompress_job_manager::get_instance()->decompress_parallel_sync(
        in, inlen, out, dim_x, dim_y, block_x, block_y);
}
