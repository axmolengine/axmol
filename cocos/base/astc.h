/******************************************************************************
 
 ASTC Texture Decompression.
 astc compression tool: https://github.com/ARM-software/astc-encoder/releases
 
 ******************************************************************************/

#ifndef __ASTC_H__
#define __ASTC_H__

#include <stdint.h>

// ASTC parameters
#define ASTC_HEAD_SIZE 16

#ifdef __cplusplus
extern "C" {
#endif
    uint8_t decompress_astc(const uint8_t* in, uint8_t* out, uint32_t width, uint32_t height, uint32_t xdim, uint32_t ydim, uint32_t datalen);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //__ASTC_H__
