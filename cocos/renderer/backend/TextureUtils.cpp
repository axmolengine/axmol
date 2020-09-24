/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
 
#include "TextureUtils.h"
#include "Macros.h"


NS_CC_BEGIN

namespace backend { namespace PixelFormatUtils {

    static const PixelFormatDescriptor s_pixelFormatDescriptors[] =
    {
        //  +--------------------------------------------- bpp
        //  |   +----------------------------------------- block width
        //  |   |  +-------------------------------------- block height
        //  |   |  |   +---------------------------------- block size
        //  |   |  |   |  +------------------------------- min blocks x
        //  |   |  |   |  |  +---------------------------- min blocks y
        //  |   |  |   |  |  |   +------------------------ alpha
        //  |   |  |   |  |  |   |        +--------------- name
        {   4,  4, 4,  8, 2, 2,  false,   "PVRTC4"   }, // PVRTC4
        {   4,  4, 4,  8, 2, 2,  true,    "PVRTC4A"  }, // PVRTC4A
        {   2,  8, 4,  8, 2, 2,  false,   "PVRTC2"   }, // PVRTC2
        {   2,  8, 4,  8, 2, 2,  true,    "PVRTC2A"  }, // PVTC2A
        {   4,  4, 4,  8, 1, 1,  false,   "ETC1"     }, // ETC1
        {   4,  4, 4,  8, 1, 1,  false,   "ETC2"     }, // ETC2
        {   8,  4, 4, 16, 1, 1,  true,    "ETC2A"    }, // ETC2A
        {   4,  4, 4,  8, 1, 1,  true,    "BC1"      }, // S3TC_DXT1
        {   8,  4, 4, 16, 1, 1,  true,    "BC2"      }, // S3TC_DXT3
        {   8,  4, 4, 16, 1, 1,  true,    "BC3"      }, // S3TC_DXT5
        {   4,  4, 4,  8, 1, 1,  false,   "ATC"      }, // ATC_RGB
        {   8,  4, 4, 16, 1, 1,  true,    "ATCE"     }, // ATC_EXPLICIT_ALPHA
        {   8,  4, 4, 16, 1, 1,  true,    "ATCI"     }, // ATC_INTERPOLATED_ALPHA
        {   8,  4, 4, 16, 1, 1,  true,    "ASTC4x4"  }, // ASTC4x4
        {   6,  5, 5, 16, 1, 1,  true,    "ASTC5x5"  }, // ASTC5x5
        {   4,  6, 6, 16, 1, 1,  true,    "ASTC6x6"  }, // ASTC6x6
        {   4,  8, 5, 16, 1, 1,  true,    "ASTC8x5"  }, // ASTC8x5
        {   3,  8, 6, 16, 1, 1,  true,    "ASTC8x6"  }, // ASTC8x6
        {   2,  8, 8, 16, 1, 1,  true,    "ASTC8x8"  }, // ASTC8x8
        {   3, 10, 5, 16, 1, 1,  true,    "ASTC10x5" }, // ASTC10x5
        {  32,  1, 1,  4, 1, 1,  true,    "RGBA8"    }, // RGBA8
        {  32,  1, 1,  4, 1, 1,  true,    "BGRA8"    }, // BGRA8
        {  24,  1, 1,  3, 1, 1,  false,   "RGB8"     }, // RGB8
        {  16,  1, 1,  2, 1, 1,  false,   "R5G6B5"   }, // R5G6B5
        {  16,  1, 1,  2, 1, 1,  true,    "RGBA4"    }, // RGBA4
        {  16,  1, 1,  2, 1, 1,  true,    "RGB5A1"   }, // RGB5A1
        {   8,  1, 1,  1, 1, 1,  true,    "A8"       }, // A8
        {   8,  1, 1,  1, 1, 1,  false,   "L8"       }, // L8
        {  16,  1, 1,  2, 1, 1,  true,    "LA8"      }, // LA8
#if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS)
        {  32,  1, 1,  4, 1, 1,  false,   "D24S8"    }, // D24S8
#else
        {  64,  1, 1,  8, 1, 1,  false,   "D32FS8"   }, // D32FS8 iOS
#endif
    };

    static_assert(CC_ARRAYSIZE(s_pixelFormatDescriptors) == (int)PixelFormat::COUNT, "The pixel format descriptor table incomplete!");

    //////////////////////////////////////////////////////////////////////////
    // pixel format helper functions
    const PixelFormatDescriptor& getFormatDescriptor(PixelFormat format)
    {
        if(UTILS_LIKELY(format < PixelFormat::COUNT) )
            return s_pixelFormatDescriptors[(uint32_t)format];

        static const PixelFormatDescriptor s_invalidDescriptor = {};
        return s_invalidDescriptor;
    }

    uint32_t computeRowPitch(PixelFormat format, uint32_t width)
    {
        if(UTILS_LIKELY(format < PixelFormat::COUNT) ) {
            if(UTILS_LIKELY(format >= PixelFormat::ETC1))
            { // @MTL: PVRTC.rowPitc must be 0
                auto& descriptor = s_pixelFormatDescriptors[(uint32_t)format];
                if(format < PixelFormat::RGBA8) {
                    auto blocksPerRow = (width + (descriptor.blockWidth - 1)) / descriptor.blockWidth;
                    return blocksPerRow * descriptor.blockSize;
                }
                return width * descriptor.bpp / 8;
            }
        }
        return 0;
    }

    //////////////////////////////////////////////////////////////////////////
    //convertor function
    
    // IIIIIIII -> RRRRRRRRGGGGGGGGGBBBBBBBB
    void convertL8ToRGB8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (size_t i = 0; i < dataLen; ++i)
        {
            *outData++ = data[i];     //R
            *outData++ = data[i];     //G
            *outData++ = data[i];     //B
        }
    }
    
    // IIIIIIIIAAAAAAAA -> RRRRRRRRGGGGGGGGBBBBBBBB
    void convertLA8ToRGB8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *outData++ = data[i];     //R
            *outData++ = data[i];     //G
            *outData++ = data[i];     //B
        }
    }
    
    // IIIIIIII -> RRRRRRRRGGGGGGGGGBBBBBBBBAAAAAAAA
    void convertL8ToRGBA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (size_t i = 0; i < dataLen; ++i)
        {
            *outData++ = data[i];     //R
            *outData++ = data[i];     //G
            *outData++ = data[i];     //B
            *outData++ = 0xFF;        //A
        }
    }
    
    // IIIIIIIIAAAAAAAA -> RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA
    void convertLA8ToRGBA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *outData++ = data[i];     //R
            *outData++ = data[i];     //G
            *outData++ = data[i];     //B
            *outData++ = data[i + 1]; //A
        }
    }
    
    // IIIIIIII -> RRRRRGGGGGGBBBBB
    void convertL8ToRGB565(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (size_t i = 0; i < dataLen; ++i)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i] & 0x00FC) << 3         //G
            | (data[i] & 0x00F8) >> 3;        //B
        }
    }
    
    // IIIIIIIIAAAAAAAA -> RRRRRGGGGGGBBBBB
    void convertLA8ToRGB565(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i] & 0x00FC) << 3         //G
            | (data[i] & 0x00F8) >> 3;        //B
        }
    }
    
    // IIIIIIII -> RRRRGGGGBBBBAAAA
    void convertL8ToRGBA4(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (size_t i = 0; i < dataLen; ++i)
        {
            *out16++ = (data[i] & 0x00F0) << 8    //R
            | (data[i] & 0x00F0) << 4             //G
            | (data[i] & 0x00F0)                  //B
            | 0x000F;                             //A
        }
    }
    
    // IIIIIIIIAAAAAAAA -> RRRRGGGGBBBBAAAA
    void convertLA8ToRGBA4(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *out16++ = (data[i] & 0x00F0) << 8    //R
            | (data[i] & 0x00F0) << 4             //G
            | (data[i] & 0x00F0)                  //B
            | (data[i + 1] & 0x00F0) >> 4;          //A
        }
    }
    
    // IIIIIIIIAAAAAAAA -> BBBBBGGG GGGRRRR
    void convertLA8ToBGR565(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        uint16_t* out16 = (uint16_t*)outData;
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i] & 0x00FC) << 3             //G
            | (data[i] & 0x00F8) >> 3;            //B
        }
    }
    
    // IIIIIIIIAAAAAAAA -> BBBBBGGG GGRRRRRA
    void convertLA8ToBGR5A1(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        uint16_t* out16 = (uint16_t*)outData;
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *out16++ = (data[i] & 0x00F8) << 7    //R
            | (data[i] & 0x00F8) << 2             //G
            | (data[i] & 0x00F8) >> 3             //B
            | (data[i + 1] & 0x0080) << 8;          //A
        }
    }
    
    // IIIIIIIIAAAAAAAA -> AAAABBBB GGGGRRRR
    void convertLA8ToABGR4(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        uint16_t* out16 = (uint16_t*)outData;
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *out16++ = (data[i] & 0x00F0) << 8    //R
            | (data[i] & 0x00F0) << 4             //G
            | (data[i] & 0x00F0)                  //B
            | (data[i + 1] & 0x00F0) >> 4;        //A
        }
    }
    
    
    // IIIIIIII -> RRRRRGGGGGBBBBBA
    void convertL8ToRGB5A1(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (size_t i = 0; i < dataLen; ++i)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i] & 0x00F8) << 3         //G
            | (data[i] & 0x00F8) >> 2         //B
            | 0x0001;                         //A
        }
    }
    
    /// IIIIIIII -> BBBBBGGG GGRRRRRA
    void convertL8ToBGR5A1(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        uint16_t *out16 = (uint16_t*)outData;
        for (size_t i = 0; i < dataLen; ++i)
        {
            *out16++ = (data[i] & 0xF8) << 7    //R
            | (data[i] & 0xF8) << 2             //G
            | (data[i] & 0xF8) >> 3             //B
            | 0x8000;                           //A
        }
    }
    
    // IIIIIIIII -> BBBBBGGG GGGRRRRR
    void convertL8ToBGR565(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        uint16_t *out16 = (uint16_t*)outData;
        for (size_t i = 0; i < dataLen; ++i)
        {
            *out16++ = (data[i] & 0xF8) << 8    //R
            | (data[i] & 0xFC) << 3             //G
            | (data[i] & 0xF8) >> 3             //B
            ;
        }
    }
    
    // IIIIIIIII -> AAAABBBBB GGGGRRRR
    void convertL8ToABGR4(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        uint16_t *out16 = (uint16_t*)outData;
        for (size_t i = 0; i < dataLen; ++i)
        {
            *out16++ = (data[i] & 0xF0) << 8    //R
            | (data[i] & 0xF0) << 4             //G
            | (data[i] & 0xF0)                  //B
            | 0x000F;
        }
    }
    
    // IIIIIIIIAAAAAAAA -> RRRRRGGGGGBBBBBA
    void convertLA8ToRGB5A1(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i] & 0x00F8) << 3         //G
            | (data[i] & 0x00F8) >> 2         //B
            | (data[i + 1] & 0x0080) >> 7;    //A
        }
    }
    
    // IIIIIIII -> IIIIIIIIAAAAAAAA
    void convertL8ToLA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (size_t i = 0; i < dataLen; ++i)
        {
            *out16++ = 0xFF00     //A
            | data[i];            //I
        }
    }
    
    // IIIIIIIIAAAAAAAA -> AAAAAAAA
    void convertLA8ToA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (size_t i = 1; i < dataLen; i += 2)
        {
            *outData++ = data[i]; //A
        }
    }
    
    // IIIIIIIIAAAAAAAA -> IIIIIIII
    void convertLA8ToL8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 1; i < l; i += 2)
        {
            *outData++ = data[i]; //R
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA
    void convertRGB8ToRGBA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *outData++ = data[i];         //R
            *outData++ = data[i + 1];     //G
            *outData++ = data[i + 2];     //B
            *outData++ = 0xFF;            //A
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRRRRRGGGGGGGGBBBBBBBB
    void convertRGBA8ToRGB8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
        {
            *outData++ = data[i];         //R
            *outData++ = data[i + 1];     //G
            *outData++ = data[i + 2];     //B
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRGGGGGGBBBBB
    void convertRGB8ToRGB565(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i + 1] & 0x00FC) << 3     //G
            | (data[i + 2] & 0x00F8) >> 3;    //B
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRRGGGGGGBBBBB
    void convertRGBA8ToRGB565(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i + 1] & 0x00FC) << 3     //G
            | (data[i + 2] & 0x00F8) >> 3;    //B
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBB -> AAAAAAAA
    void convertRGB8ToA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //A =  (R*299 + G*587 + B*114 + 500) / 1000
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBB -> IIIIIIII
    void convertRGB8ToL8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> IIIIIIII
    void convertRGBA8ToL8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
        {
            *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> AAAAAAAA
    void convertRGBA8ToA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
        {
            *outData++ = data[i + 3]; //A
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBB -> IIIIIIIIAAAAAAAA
    void convertRGB8ToLA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
            *outData++ = 0xFF;
        }
    }
    
    
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> IIIIIIIIAAAAAAAA
    void convertRGBA8ToLA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
        {
            *outData++ = (data[i] * 299 + data[i + 1] * 587 + data[i + 2] * 114 + 500) / 1000;  //I =  (R*299 + G*587 + B*114 + 500) / 1000
            *outData++ = data[i + 3];
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRGGGGBBBBAAAA
    void convertRGB8ToRGBA4(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *out16++ = ((data[i] & 0x00F0) << 8           //R
                        | (data[i + 1] & 0x00F0) << 4     //G
                        | (data[i + 2] & 0xF0)            //B
                        | 0x0F);                         //A
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRGGGGBBBBAAAA
    void convertRGBA8ToRGBA4(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 3; i < l; i += 4)
        {
            *out16++ = (data[i] & 0x00F0) << 8    //R
            | (data[i + 1] & 0x00F0) << 4         //G
            | (data[i + 2] & 0xF0)                //B
            | (data[i + 3] & 0xF0) >> 4;         //A
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBB -> RRRRRGGGGGBBBBBA
    void convertRGB8ToRGB5A1(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 3)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i + 1] & 0x00F8) << 3     //G
            | (data[i + 2] & 0x00F8) >> 2     //B
            | 0x01;                          //A
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBB -> BBBBBGGG GGGRRRRR
    void convertRGB8ToB5G6R5(const unsigned char *data, size_t dataLen, unsigned char *out)
    {
        uint16_t *outData = (uint16_t*) out;
        for(size_t i = 0;i < dataLen ; i += 3)
        {
            *outData++ = ((data[i] & 0xF8) << 8)|
            ((data[i + 1] &0xFC) << 3) |
            ((data[i + 2] & 0xF8)>> 3);
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBB -> BBBBBGGG GGRRRRRA
    void convertRGB8ToBGR5A1(const unsigned char *data, size_t dataLen, unsigned char *out)
    {
        uint16_t *outData = (uint16_t*) out;
        for(size_t i = 0;i < dataLen ; i += 3)
        {
            *outData++ = ((data[i] & 0xF8) << 7) |
            ((data[i + 1] & 0xF8) << 2) |
            ((data[i + 2] &0xF8) >> 3) | 0x8000;
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBB -> AAAABBBB GGGGRRRR
    void convertRGB8ToABGR4(const unsigned char *data, size_t dataLen, unsigned char *out)
    {
        uint16_t *outData = (uint16_t*) out;
        for(size_t i = 0;i < dataLen ; i += 3)
        {
            *outData++ = ((data[i] & 0xF0) << 8) | //r
            ((data[i + 1] & 0xF0) << 4) |          //g
            ((data[i + 2] & 0xF0)) |               //b
            0x000F;                                //a
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> RRRRRGGG GGBBBBBA
    void convertRGBA8ToRGB5A1(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        unsigned short* out16 = (unsigned short*)outData;
        for (ssize_t i = 0, l = dataLen - 2; i < l; i += 4)
        {
            *out16++ = (data[i] & 0x00F8) << 8    //R
            | (data[i + 1] & 0x00F8) << 3     //G
            | (data[i + 2] & 0x00F8) >> 2     //B
            | (data[i + 3] & 0x0080) >> 7;   //A
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> BBBBBGGG GGGRRRR
    void convertRGBA8ToBGR565(const unsigned char *data, size_t dataLen, unsigned char *out)
    {
        uint16_t *outData = (uint16_t*)out;
        const size_t pixelCnt = dataLen / 4;
        for(size_t i=0;i < pixelCnt; i++ )
        {
            outData[i] = ((data[i*4 + 2] & 0xF8) >> 3) |     //b
            ((data[i * 4 + 1] & 0xFC ) << 3) |           //g
            (((data[i * 4 + 0] & 0xF8) << 8));             //r
        }
    }

    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> AAAABBBB GGGGRRRR
    void convertRGBA8ToABGR4(const unsigned char *data, size_t dataLen, unsigned char *out)
    {
        uint16_t *outData = (uint16_t*)out;
        for(size_t i=0;i < dataLen; i+=4 )
        {
            *outData++ = (data[i] & 0x00F0) >> 4 //R
                | (data[i + 1] & 0x00F0)         //G
                | (data[i + 2] & 0x00F0) << 4    //B
                | (data[i + 3] & 0x00F0) << 8;   //A
        }
    }
    
    // RRRRRRRRGGGGGGGGBBBBBBBBAAAAAAAA -> BBBBBGGG GGRRRRRA
    void convertRGBA8ToBGR5A1(const unsigned char *data, size_t dataLen, unsigned char *out)
    {
        uint16_t *outData = (uint16_t*)out;
        for(size_t i = 0; i < dataLen; i += 4)
        {
            *outData++ = ((data[i + 2] & 0xF8) >> 3)|     //b
            ((data[i + 1] & 0xF8 ) << 2) |                //g
            ((data[i + 0] & 0xF8 ) << 7) |                //r
            ((data[i + 3] & 0x80) << 8);                  //a
        }
    }
    
    
    void convertRGB5A1ToRGBA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        uint16_t *inData = (uint16_t*)data;
        const size_t pixelLen = dataLen / 2;
        uint16_t pixel;
        for (size_t i = 0; i < pixelLen; i++)
        {
            pixel = inData[i];
            *outData++ = (pixel & (0x001F << 11)) >> 8;
            *outData++ = (pixel & (0x001F << 6)) >> 3;
            *outData++ = (pixel & (0x001F << 1)) << 2;
            *outData++ = (pixel & 0x0001) * 255;
        }
    }
    
    // ABBBBBGG GGGRRRRR  -> BBBBBGGG GGRRRRRA
    void convertRGB5A1ToBGR5A1(const unsigned char *data, size_t dataLen, unsigned char *out)
    {
        const size_t pixelLen = dataLen / 2;
        const uint16_t *inData = (uint16_t*) data;
        uint16_t *outData = (uint16_t*) out;
        uint16_t pixel;
        for (size_t i = 0; i < pixelLen; i++ )
        {
            pixel = inData[i];
            outData[i] = (pixel >> 1) | ((pixel & 0x0001) << 15);
        }
    }
    
    
    void convertRGB565ToRGBA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        uint16_t *inData = (uint16_t*)data;
        const size_t pixelLen = dataLen / 2;
        uint16_t pixel;
        for (size_t i = 0; i < pixelLen; i++)
        {
            pixel = inData[i];
            *outData++ = (pixel & (0x001F << 11)) >> 8;
            *outData++ = (pixel & (0x003F << 5)) >> 3;
            *outData++ = (pixel & (0x001F)) << 3;
            *outData++ = 0xFF;
        }
    }
    
    // BBBBBGGG GGGRRRRR -> BBBGGG GGGRRRRR
    // void convertRGB565ToB5G6R5(const unsigned char *data, ssize_t dataLen, unsigned char *out)
    
    
    void convertRGBA4ToRGBA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        uint16_t *inData = (uint16_t*)data;
        const size_t pixelLen = dataLen / 2;
        uint16_t pixel;
        for (size_t i = 0; i < pixelLen; i++)
        {
            pixel = inData[i];
            *outData++ = ((pixel & 0xF000) >> 12) * 17;
            *outData++ = ((pixel & 0x0F00) >> 8) * 17;
            *outData++ = ((pixel & 0x00F0) >> 4) * 17;
            *outData++ = ((pixel & 0x000F) * 17);
        }
        
    }
    
    // AAAABBBBGGGGRRRR -> AAAABBBB GGGGRRRR
    //void convertRGBA4ToABGR4444(const unsigned char *data, ssize_t dataLen, unsigned char *out)
    
    void convertA8ToRGBA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        for (size_t i = 0; i < dataLen; i++)
        {
            *outData++ = 0;
            *outData++ = 0;
            *outData++ = 0;
            *outData++ = data[i];
        }
    }
    
    void convertBGRA8ToRGBA8(const unsigned char* data, size_t dataLen, unsigned char* outData)
    {
        const size_t pixelCounts = dataLen / 4;
        for (size_t i = 0; i < pixelCounts; i++)
        {
            *outData++ = data[i*4 + 2];
            *outData++ = data[i*4 + 1];
            *outData++ = data[i*4 + 0];
            *outData++ = data[i*4 + 3];
        }
    }
    
    // converter function end
    //////////////////////////////////////////////////////////////////////////
    
    
    
    cocos2d::backend::PixelFormat convertL8ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen)
    {
        switch (format)
        {
            case PixelFormat::RGBA8:
                *outDataLen = dataLen*4;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertL8ToRGBA8(data, dataLen, *outData);
                break;
            case PixelFormat::RGB8:
                *outDataLen = dataLen*3;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertL8ToRGB8(data, dataLen, *outData);
                break;
            case PixelFormat::RGB565:
                *outDataLen = dataLen*2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertL8ToBGR565(data, dataLen, *outData);
                break;
            case PixelFormat::RGBA4:
                *outDataLen = dataLen * 2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertL8ToABGR4(data, dataLen, *outData);
                break;
            case PixelFormat::RGB5A1:
                *outDataLen = dataLen * 2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertL8ToBGR5A1(data, dataLen, *outData);
                break;
            case PixelFormat::A8:
                *outData = (unsigned char*)data;
                *outDataLen = dataLen;
                break;
            case PixelFormat::LA8:
                *outDataLen = dataLen*2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertL8ToLA8(data, dataLen, *outData);
                break;
            default:
                // unsupported conversion or don't need to convert
                if (format != PixelFormat::L8)
                {
                    CCLOG("Can not convert image format PixelFormat::L8 to format ID:%d, we will use it's origin format PixelFormat::L8", static_cast<int>(format));
                }
                
                *outData = (unsigned char*)data;
                *outDataLen = dataLen;
                return PixelFormat::L8;
        }
        
        return format;
    }
    
    cocos2d::backend::PixelFormat convertLA8ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen)
    {
        switch (format)
        {
            case PixelFormat::RGBA8:
                *outDataLen = dataLen*2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertLA8ToRGBA8(data, dataLen, *outData);
                break;
            case PixelFormat::RGB8:
                *outDataLen = dataLen/2*3;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertLA8ToRGB8(data, dataLen, *outData);
                break;
            case PixelFormat::RGB565:
                *outDataLen = dataLen;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertLA8ToBGR565(data, dataLen, *outData);
                break;
            case PixelFormat::RGBA4:
                *outDataLen = dataLen;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertLA8ToABGR4(data, dataLen, *outData);
                break;
            case PixelFormat::RGB5A1:
                *outDataLen = dataLen;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertLA8ToBGR5A1(data, dataLen, *outData);
                break;
            case PixelFormat::A8:
                *outDataLen = dataLen/2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertLA8ToA8(data, dataLen, *outData);
                break;
            case PixelFormat::L8:
                *outDataLen = dataLen/2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertLA8ToL8(data, dataLen, *outData);
                break;
            default:
                // unsupported conversion or don't need to convert
                if (format != PixelFormat::LA8)
                {
                    CCLOG("Can not convert image format PixelFormat::LA8 to format ID:%d, we will use it's origin format PixelFormat::LA8", static_cast<int>(format));
                }
                *outData = (unsigned char*)data;
                *outDataLen = dataLen;
                return PixelFormat::LA8;
                break;
        }
        
        return format;
    }
    
    cocos2d::backend::PixelFormat convertRGB8ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen)
    {
        switch (format)
        {
            case PixelFormat::RGBA8:
                *outDataLen = dataLen/3*4;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGB8ToRGBA8(data, dataLen, *outData);
                break;
            case PixelFormat::RGB565:
                *outDataLen = dataLen/3*2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGB8ToB5G6R5(data, dataLen, *outData);
                break;
            case PixelFormat::RGBA4:
                *outDataLen = dataLen / 3 * 2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGB8ToABGR4(data, dataLen, *outData);
                break;
            case PixelFormat::RGB5A1:
                *outDataLen = dataLen / 3 * 2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGB8ToBGR5A1(data, dataLen, *outData);
                break;
            case PixelFormat::A8:
                *outDataLen = dataLen/3;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGB8ToA8(data, dataLen, *outData);
                break;
            case PixelFormat::L8:
                *outDataLen = dataLen/3;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGB8ToL8(data, dataLen, *outData);
                break;
            case PixelFormat::LA8:
                *outDataLen = dataLen/3*2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGB8ToLA8(data, dataLen, *outData);
                break;
            default:
                // unsupported conversion or don't need to convert
                if (format != PixelFormat::RGB8)
                {
                    CCLOG("Can not convert image format PixelFormat::RGB8 to format ID:%d, we will use it's origin format PixelFormat::RGB8", static_cast<int>(format));
                }
                
                *outData = (unsigned char*)data;
                *outDataLen = dataLen;
                return PixelFormat::RGB8;
        }
        return format;
    }
    
    cocos2d::backend::PixelFormat convertRGBA8ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen)
    {
        
        switch (format)
        {
            case PixelFormat::RGB8:
                *outDataLen = dataLen/4*3;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGBA8ToRGB8(data, dataLen, *outData);
                break;
            case PixelFormat::RGB565:
                *outDataLen = dataLen/2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGBA8ToBGR565(data, dataLen, *outData);
                break;
            case PixelFormat::RGBA4:
                *outDataLen = dataLen / 2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                // convertRGBA8ToABGR4(data, dataLen, *outData);
                convertRGBA8ToRGBA4(data, dataLen, *outData); // == MTLPixelFormatABGR4Unorm ??
                break;
            case PixelFormat::RGB5A1:
                *outDataLen = dataLen / 2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGBA8ToBGR5A1(data, dataLen, *outData);
                break;
            case PixelFormat::A8:
                *outDataLen = dataLen/4;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGBA8ToA8(data, dataLen, *outData);
                break;
            case PixelFormat::L8:
                *outDataLen = dataLen/4;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGBA8ToL8(data, dataLen, *outData);
                break;
            case PixelFormat::LA8:
                *outDataLen = dataLen/2;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGBA8ToLA8(data, dataLen, *outData);
                break;
            default:
                // unsupported conversion or don't need to convert
                if (format != PixelFormat::RGBA8)
                {
                    CCLOG("Can not convert image format PixelFormat::RGBA8 to format ID:%d, we will use it's origin format PixelFormat::RGBA8", static_cast<int>(format));
                }
                
                *outData = (unsigned char*)data;
                *outDataLen = dataLen;
                return PixelFormat::RGBA8;
        }
        
        return format;
    }
    
    cocos2d::backend::PixelFormat convertRGB5A1ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen)
    {
        switch (format)
        {
            case PixelFormat::RGBA8:
                *outDataLen = dataLen/2*4;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGB5A1ToRGBA8(data, dataLen, *outData);
                break;
            case PixelFormat::RGB5A1:
                *outDataLen = dataLen;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGB5A1ToBGR5A1(data, dataLen, *outData);
                break;
            default:
                // unsupported conversion or don't need to convert
                if (format != PixelFormat::RGBA8)
                {
                    CCLOG("Can not convert image format PixelFormat::RGB5A1 to format ID:%d, we will use it's origin format PixelFormat::RGB51A", static_cast<int>(format));
                }
                *outData = (unsigned char*)data;
                *outDataLen = dataLen;
                return PixelFormat::RGBA8;
        }
        
        return format;
    }
    
    cocos2d::backend::PixelFormat convertRGB565ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen)
    {
        switch (format)
        {
            case PixelFormat::RGBA8:
                *outDataLen = dataLen/2*4;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGB565ToRGBA8(data, dataLen, *outData);
                break;
            case PixelFormat::RGB565:
                *outDataLen = dataLen;
                *outData = (unsigned char *)data;
                // they are identical
                //convertRGB565ToB5G6R5(data, dataLen, *outData);
                break;
            default:
                // unsupported conversion or don't need to convert
                if (format != PixelFormat::RGBA8)
                {
                    CCLOG("Can not convert image format PixelFormat::RGB565 to format ID:%d, we will use it's origin format PixelFormat::RGB565", static_cast<int>(format));
                }
                *outData = (unsigned char*)data;
                *outDataLen = dataLen;
                return PixelFormat::RGBA8;
        }
        
        return format;
    }
    
    cocos2d::backend::PixelFormat convertA8ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen)
    {
        switch (format)
        {
            case PixelFormat::RGBA8:
                *outDataLen = dataLen*4;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertA8ToRGBA8(data, dataLen, *outData);
                break;
            default:
                // unsupported conversion or don't need to convert
                if (format != PixelFormat::RGBA8)
                {
                    CCLOG("Can not convert image format PixelFormat::A8 to format ID:%d, we will use it's origin format PixelFormat::A8", static_cast<int>(format));
                }
                *outData = (unsigned char*)data;
                *outDataLen = dataLen;
                return PixelFormat::RGBA8;
        }
        
        return format;
    }
    
    cocos2d::backend::PixelFormat convertRGBA4ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen)
    {
        switch (format)
        {
            case PixelFormat::RGBA8:
                *outDataLen = dataLen/ 2 * 4;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertRGBA4ToRGBA8(data, dataLen, *outData);
                break;
            case PixelFormat::RGBA4:
                *outDataLen = dataLen;
                *outData = (unsigned char *)data;
                //convertRGBA4ToABGR4444(data, dataLen, *outData);
                break;
            default:
                // unsupported conversion or don't need to convert
                if (format != PixelFormat::RGBA8)
                {
                    CCLOG("Can not convert image format PixelFormat::RGBA444 to format ID:%d, we will use it's origin format PixelFormat::RGBA4", static_cast<int>(format));
                }
                *outData = (unsigned char*)data;
                *outDataLen = dataLen;
                return PixelFormat::RGBA8;
        }
        
        return format;
    }
    
    PixelFormat convertBGRA8ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen)
    {
        switch (format) {
            case PixelFormat::RGBA8:
                *outDataLen = dataLen;
                *outData = (unsigned char*)malloc(sizeof(unsigned char) * (*outDataLen));
                convertBGRA8ToRGBA8(data, dataLen, *outData);
                break;
                
            default:
                break;
        }
        return format;
    }
    
    /*
     convert map:
     1.PixelFormat::RGBA8
     2.PixelFormat::RGB8
     3.PixelFormat::RGB565
     4.PixelFormat::A8
     5.PixelFormat::L8
     6.PixelFormat::LA8
     7.PixelFormat::RGBA4
     8.PixelFormat::RGB5A1
     
     gray(5) -> 1235678
     gray alpha(6) -> 12345678
     rgb(2) -> 1235678
     rgba(1) -> 12345678
     
     */
    cocos2d::backend::PixelFormat convertDataToFormat(const unsigned char* data, size_t dataLen, PixelFormat originFormat, PixelFormat format, unsigned char** outData, size_t* outDataLen)
    {
        // don't need to convert
        if (format == originFormat)
        {
            *outData = (unsigned char*)data;
            *outDataLen = dataLen;
            return originFormat;
        }
        
        switch (originFormat)
        {
            case PixelFormat::L8:
                return convertL8ToFormat(data, dataLen, format, outData, outDataLen);
            case PixelFormat::LA8:
                return convertLA8ToFormat(data, dataLen, format, outData, outDataLen);
            case PixelFormat::RGB8:
                return convertRGB8ToFormat(data, dataLen, format, outData, outDataLen);
            case PixelFormat::RGBA8:
                return convertRGBA8ToFormat(data, dataLen, format, outData, outDataLen);
            case PixelFormat::RGB5A1:
                return convertRGB5A1ToFormat(data, dataLen, format, outData, outDataLen);
            case PixelFormat::RGB565:
                return convertRGB565ToFormat(data, dataLen, format, outData, outDataLen);
#ifdef CC_USE_METAL
            case PixelFormat::RGBA4:
                return convertRGBA4ToFormat(data, dataLen, format, outData, outDataLen);
            case PixelFormat::A8:
                return convertA8ToFormat(data, dataLen, format, outData, outDataLen);
                
#endif
            case PixelFormat::BGRA8:
                return convertBGRA8ToFormat(data, dataLen, format, outData, outDataLen);
            default:
                CCLOG("unsupported conversion from format %d to format %d", static_cast<int>(originFormat), static_cast<int>(format));
                *outData = (unsigned char*)data;
                *outDataLen = dataLen;
                return originFormat;
        }
    }
}
}

NS_CC_END
