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

// #define ASTC_ENABLE_PARALLEL_DECOMPRESS 1

#if defined(ASTC_ENABLE_PARALLEL_DECOMPRESS)
typedef std::mutex astc_decompress_mutex_t;

struct astc_decompress_task {
    astc_decompress_task() {

        _nref = 1;
    }

    void addRef() {
        ++_nref;
    }

    void release() {
        if (--_nref == 0)
            delete this;
    }

    astcenc_config config{};

    const uint8_t* _in_texels = nullptr;
    void* _out_texels[1]{};
    astcenc_context* _context = nullptr;
    astcenc_image _image_out;

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

    void decompress_parallel_sync(
        const uint8_t* in, uint8_t* out, unsigned int dim_x, unsigned int dim_y, int block_x, int block_y) {
        auto task = new astc_decompress_task();
        init_task(task, in, out, dim_x, dim_y, block_x, block_y);

        _taskQueueMtx.lock();
        _taskQueue.push_back(task);
        _taskQueueMtx.unlock();
        _taskQueueCV.notify_all(); // notify all to work for the single decompress task

        task->_context->manage_decompress.wait();

        _taskQueueMtx.lock();
        if (!_taskQueue.empty()) {
            auto t = _taskQueue.front();
            assert(t == task);
            _taskQueue.pop_front();
            t->release();
        }
        _taskQueueMtx.unlock();
    }

public:
    void init_task(astc_decompress_task* task, const uint8_t* in, uint8_t* out, unsigned int dim_x, unsigned int dim_y,
        int block_x, int block_y) {
        astcenc_error status =
            astcenc_config_init(ASTCENC_PRF_LDR, block_x, block_y, 1, 0, ASTCENC_FLG_DECOMPRESS_ONLY, &task->config);
        status               = astcenc_context_alloc(&task->config, s_thread_count, &task->_context);
        task->_in_texels     = in;
        task->_out_texels[0] = out;
        task->_image_out     = {dim_x, dim_y, 1, ASTCENC_TYPE_U8, task->_out_texels};

        // Only the first thread actually runs the initializer
        unsigned int xblocks = (task->_image_out.dim_x + block_x - 1) / block_x;
        unsigned int yblocks = (task->_image_out.dim_y + block_y - 1) / block_y;
        unsigned int zblocks = (task->_image_out.dim_z);
        task->_context->manage_decompress.init(zblocks * yblocks * xblocks);
    }

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
            auto ctx        = task->_context;
            auto& image_out = task->_image_out;

            unsigned int block_x = ctx->config.block_x;
            unsigned int block_y = ctx->config.block_y;
            unsigned int block_z = ctx->config.block_z;

            unsigned int xblocks = (image_out.dim_x + block_x - 1) / block_x;
            unsigned int yblocks = (image_out.dim_y + block_y - 1) / block_y;
            unsigned int zblocks = (image_out.dim_z + block_z - 1) / block_z;

            int row_blocks   = xblocks;
            int plane_blocks = xblocks * yblocks;

            // Check we have enough output space (16 bytes per block)
            /*size_t size_needed = xblocks * yblocks * zblocks * 16;
            if (data_len < size_needed) {
                return ASTCENC_ERR_OUT_OF_MEM;
            }*/

            image_block blk;

            auto data = task->_in_texels;
            for (;;) { // process the task
                unsigned int count = 0;
                unsigned int base  = ctx->manage_decompress.get_task_assignment(128, count);

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

                    physical_to_symbolic(*ctx->bsd, pcb, scb);

                    decompress_symbolic_block(
                        ctx->config.profile, *ctx->bsd, x * block_x, y * block_y, z * block_z, scb, blk);

                    write_image_block(image_out, blk, *ctx->bsd, x * block_x, y * block_y, z * block_z, swz_decode);
                }

                ctx->manage_decompress.complete_task_assignment(count);
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
#endif

uint8_t astc_decompress_image(const uint8_t* in, uint32_t inlen, uint8_t* out, uint32_t dim_x, uint32_t dim_y,
    uint32_t block_x, uint32_t block_y) {

    auto start = yasio::highp_clock();

#if defined(ASTC_ENABLE_PARALLEL_DECOMPRESS)
    astc_decompress_job_manager::get_instance()->decompress_parallel_sync(in, out, dim_x, dim_y, block_x, block_y);
#else
    static std::once_flag once_flag;
    std::call_once(once_flag, init_quant_mode_table);

    const unsigned int dim_z   = 1;
    const unsigned int block_z = 1;

    unsigned int xblocks = (dim_x + block_x - 1) / block_x;
    unsigned int yblocks = (dim_y + block_y - 1) / block_y;
    unsigned int zblocks = (dim_z + block_z - 1) / block_z;

    int row_blocks   = xblocks;
    int plane_blocks = xblocks * yblocks;

    // Check we have enough output space (16 bytes per block)
    size_t size_needed = xblocks * yblocks * zblocks * 16;
    if (inlen < size_needed) {
        return ASTCENC_ERR_OUT_OF_MEM;
    }

    block_size_descriptor* bsd = new block_size_descriptor(); // 3.3MB
    init_block_size_descriptor(block_x, block_y, 1, false, 0 /*unused for decompress*/, *bsd);

    image_block blk;
    void* data[1] = {out};
    astcenc_image image_out{dim_x, dim_y, 1, ASTCENC_TYPE_U8, data};
    const auto total_blocks = zblocks * yblocks * xblocks;
    const astcenc_swizzle swz_decode{ASTCENC_SWZ_R, ASTCENC_SWZ_G, ASTCENC_SWZ_B, ASTCENC_SWZ_A};
    for (unsigned int i = 0; i < total_blocks; ++i) {
        // Decode i into x, y, z block indices
        int z            = i / plane_blocks;
        unsigned int rem = i - (z * plane_blocks);
        int y            = rem / row_blocks;
        int x            = rem - (y * row_blocks);

        unsigned int offset           = (((z * yblocks + y) * xblocks) + x) * 16;
        const uint8_t* bp             = in + offset;
        physical_compressed_block pcb = *(const physical_compressed_block*) bp;
        symbolic_compressed_block scb;

        physical_to_symbolic(*bsd, pcb, scb);

        decompress_symbolic_block(ASTCENC_PRF_LDR, *bsd, x * block_x, y * block_y, z * block_z, scb, blk);

        write_image_block(image_out, blk, *bsd, x * block_x, y * block_y, z * block_z, swz_decode);
    }

    term_block_size_descriptor(*bsd);
    delete bsd;
#endif

    // CCLOG("decompress astc image cost: %.3lf(ms)", (yasio::highp_clock() - start) / (float) std::milli::den);

    return ASTCENC_SUCCESS;
}
