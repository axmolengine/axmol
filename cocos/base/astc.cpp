/******************************************************************************

 ASTC Texture Decompression.

 ******************************************************************************/

#include "base/astc.h"
#include "astc/astc_codec_internals.h"

 // Functions that are used in compilation units we depend on, but don't actually
 // use.
//int astc_codec_unlink(const char* filename) { return 0; }
//void astc_codec_internal_error(const char* filename, int linenum) {}
astc_codec_image* load_ktx_uncompressed_image(const char* filename, int padding, int* result) { return 0; }
astc_codec_image* load_dds_uncompressed_image(const char* filename, int padding, int* result) { return 0; }
astc_codec_image* load_tga_image(const char* tga_filename, int padding, int* result) { return 0; }
astc_codec_image* load_image_with_stb(const char* filename, int padding, int* result) { return 0; }
int store_ktx_uncompressed_image(const astc_codec_image* img, const char* filename, int bitness) { return 0; }
int store_dds_uncompressed_image(const astc_codec_image* img, const char* filename, int bitness) { return 0; }
int store_tga_image(const astc_codec_image* img, const char* tga_filename, int bitness) { return 0; }


static int ASTC_INIT = 0;

uint8_t float2byte(float f) {

    if (f > 1.0f) { return 255; }
    if (f < 0.0f) { return 0; }

    return (uint8_t)(f * 255.0f + 0.5f);
}

uint8_t decompress_astc(const uint8_t* in, uint8_t* out, uint32_t width, uint32_t height, uint32_t xdim, uint32_t ydim, uint32_t datalen)
{
    //init astc mode table only once
    if (ASTC_INIT <= 0)
    {
        ASTC_INIT = 1;
        build_quantization_mode_table();
    }

    uint32_t  xblocks = (width + xdim - 1) / xdim;
    uint32_t  yblocks = (height + ydim - 1) / ydim;

    imageblock pb;
    for (uint32_t by = 0; by < yblocks; by++) {
        for (uint32_t bx = 0; bx < xblocks; bx++) {

            physical_compressed_block pcb = *(physical_compressed_block*)in;
            symbolic_compressed_block scb;
            physical_to_symbolic(xdim, ydim, 1, pcb, &scb);
            decompress_symbolic_block(DECODE_LDR_SRGB, xdim, ydim, 1, bx * xdim, by * ydim, 0, &scb, &pb);
            in += 16;

            const float* data = pb.orig_data;
            for (uint32_t dy = 0; dy < ydim; dy++) {
                uint32_t y = by * ydim + dy;
                for (uint32_t dx = 0; dx < xdim; dx++) {
                    uint32_t x = bx * xdim + dx;
                    if (x < width && y < height) {
                        uint8_t* pxl = &out[(width * y + x) * 4];
                        pxl[0] = float2byte(data[0]);
                        pxl[1] = float2byte(data[1]);
                        pxl[2] = float2byte(data[2]);
                        pxl[3] = float2byte(data[3]);
                    }
                    data += 4;
                }
            }
        }
    }
    return 0;
}