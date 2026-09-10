/****************************************************************************
Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT

Based upon code from the DirectX Tool Kit by Microsoft Corporation,
obtained from https://directxtk.codeplex.com
****************************************************************************/

#pragma once

#include "axmol/base/Config.h"

#if AX_USE_WIC

#    include <memory>
#    include <string>
#    include <stdint.h>
#    include <wincodec.h>

#    include "axmol/platform/PlatformMacros.h"

namespace ax
{

struct WICConvert
{
    WICPixelFormatGUID source;
    WICPixelFormatGUID target;
};

class AX_DLL WICImageLoader
{
public:
    WICImageLoader();
    ~WICImageLoader();

    int getWidth();
    int getHeight();
    size_t getImageDataSize();
    WICPixelFormatGUID getPixelFormat();
    size_t getImageData(WICInProcPointer rawData, size_t dataLen);
    bool decodeImageData(const uint8_t* data, size_t dataLen);
    bool encodeImageData(std::string_view path,
                         const uint8_t* data,
                         size_t dataLen,
                         WICPixelFormatGUID pixelFormat,
                         int width,
                         int height,
                         GUID containerFormat);

protected:
    bool processImage(IWICBitmapDecoder* decoder);
    size_t getBitsPerPixel(WICPixelFormatGUID format);
    HRESULT convertFormatIfRequired(IWICBitmapFrameDecode* pFrame, IWICFormatConverter** ppConv);

    static IWICImagingFactory* getWICFactory();

private:
    int _height;
    int _width;
    size_t _dataLen;
    size_t _bpp;
    WICPixelFormatGUID _format;
    BYTE* _data;

    static IWICImagingFactory* _wicFactory;
};

}  // namespace ax

#endif
