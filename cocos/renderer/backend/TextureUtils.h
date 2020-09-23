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
 
#pragma once

#include "CCPlatformMacros.h"
#include "renderer/CCTexture2D.h"

NS_CC_BEGIN

namespace backend { 
    namespace PixelFormatUtils {
        typedef cocos2d::backend::PixelFormat PixelFormat;

        struct PixelBlockInfo
        {
            uint8_t bpp; // bitsPerPixel: !!!regard bpp=0 as invalid pixel format
            uint8_t blockWidth;
            uint8_t blockHeight;
            uint8_t blockSize;
            uint8_t minBlockX;
            uint8_t minBlockY;
            uint8_t depthBits;
            uint8_t stencilBits;
            uint8_t rBits; // for compression texture is 0
            uint8_t gBits; // for compression texture is 0
            uint8_t bBits; // for compression texture is 0
            uint8_t aBits; // for compression texture, 0: no alpha, 1: has alpha
            uint8_t encoding;
        };

        const PixelBlockInfo& getBlockInfo(PixelFormat format);
        inline uint8_t getBitsPerPixel(PixelFormat format) { return getBlockInfo(format).bpp; }
        inline bool isCompressed(PixelFormat format) { return format < PixelFormat::RGBA8; }

        /**convert functions*/

        /**
        Convert the format to the format param you specified, if the format is PixelFormat::NONE, it will detect it automatically and convert to the closest format for you.
        It will return the converted format to you. if the outData != data, you must delete it manually.
        */
        PixelFormat convertDataToFormat(const unsigned char* data, size_t dataLen, PixelFormat originFormat, PixelFormat format, unsigned char** outData, size_t* outDataLen);

        PixelFormat convertI8ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen);
        PixelFormat convertAI88ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen);
        PixelFormat convertRGB888ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen);
        PixelFormat convertRGBA8888ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen);
        PixelFormat convertRGB5A1ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen);
        PixelFormat convertRGB565ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen);
        PixelFormat convertA8ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen);
        PixelFormat convertRGBA4444ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen);
        PixelFormat convertBGRA8888ToFormat(const unsigned char* data, size_t dataLen, PixelFormat format, unsigned char** outData, size_t* outDataLen);

        //I8 to XXX
        void convertI8ToRGB888(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertI8ToRGBA8888(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertI8ToRGB565(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertI8ToRGBA4444(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertI8ToRGB5A1(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertI8ToAI88(const unsigned char* data, size_t dataLen, unsigned char* outData);

        //AI88 to XXX
        void convertAI88ToRGB888(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertAI88ToRGBA8888(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertAI88ToRGB565(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertAI88ToRGBA4444(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertAI88ToRGB5A1(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertAI88ToA8(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertAI88ToI8(const unsigned char* data, size_t dataLen, unsigned char* outData);

        //RGB888 to XXX
        void convertRGB888ToRGBA8888(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGB888ToRGB565(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGB888ToA8(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGB888ToI8(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGB888ToAI88(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGB888ToRGBA4444(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGB888ToRGB5A1(const unsigned char* data, size_t dataLen, unsigned char* outData);

        //RGBA8888 to XXX
        void convertRGBA8888ToRGB888(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGBA8888ToRGB565(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGBA8888ToI8(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGBA8888ToA8(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGBA8888ToAI88(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGBA8888ToRGBA4444(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGBA8888ToRGB5A1(const unsigned char* data, size_t dataLen, unsigned char* outData);


        void convertRGB5A1ToRGBA8888(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGB565ToRGBA8888(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertA8ToRGBA8888(const unsigned char* data, size_t dataLen, unsigned char* outData);
        void convertRGBA4444ToRGBA8888(const unsigned char* data, size_t dataLen, unsigned char* outData);
        
        //BGRA8888 to XXX
        void convertBGRA8888ToRGBA8888(const unsigned char* data, size_t dataLen, unsigned char* outData);
    };
}
NS_CC_END
