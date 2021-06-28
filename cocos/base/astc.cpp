/******************************************************************************

 ASTC Texture Decompression.

 ******************************************************************************/

#include "base/astc.h"
#include "astc/astcenc.h"
#include "astc/astcenc_internal.h"

uint8_t astc_decompress_image(
    const uint8_t* in, uint32_t inlen, uint8_t* out, uint32_t dim_x, uint32_t dim_y, uint32_t block_x, uint32_t block_y) {

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

    return ASTCENC_SUCCESS;
}
