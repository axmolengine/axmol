/******************************************************************************
 
 ASTC Texture Decompression.
 astc compression tool: https://github.com/ARM-software/astc-encoder/releases
 version required: v2.2+
 command:
   astcenc-sse4.2 -cl test1.png test1.astc 6x6 -medium -pp-premultiply

 ******************************************************************************/

#ifndef __ASTC_H__
#define __ASTC_H__

#include <stdint.h>

// ASTC parameters
#define ASTC_HEAD_SIZE 16

/* ============================================================================
        ASTC compressed file loading, see also: astcenccli_image_load_store.cpp
============================================================================ */
struct astc_header {
    uint8_t magic[4];
    uint8_t block_x;
    uint8_t block_y;
    uint8_t block_z;
    uint8_t dim_x[3]; // dims = dim[0] + (dim[1] << 8) + (dim[2] << 16)
    uint8_t dim_y[3]; // Sizes are given in texels;
    uint8_t dim_z[3]; // block count is inferred
};

static const uint32_t ASTC_MAGIC_ID = 0x5CA1AB13;

static unsigned int astc_unpack_bytes(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return ((unsigned int) (a)) + ((unsigned int) (b) << 8) + ((unsigned int) (c) << 16) + ((unsigned int) (d) << 24);
}

#ifdef __cplusplus
extern "C" {
#endif
int astc_decompress_image(const uint8_t* in, uint32_t inlen, uint8_t* out, uint32_t xdim, uint32_t ydim, uint32_t xblock, uint32_t yblock);
#ifdef __cplusplus
}  // extern "C"
#endif

#endif //__ASTC_H__
