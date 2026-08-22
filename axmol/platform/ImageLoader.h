/****************************************************************************
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmol.dev/

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

#include "axmol/base/Data.h"
#include "axmol/rhi/RHITypes.h"

#include <functional>
#include <string_view>

namespace ax
{

class AX_DLL ImageLoader
{
public:
    struct DecodedImage
    {
        Data data;
        int width                    = 0;
        int height                   = 0;
        rhi::PixelFormat pixelFormat = rhi::PixelFormat::RGBA8;
        bool premultipliedAlpha      = false;
    };

    using Decoder = std::function<bool(const uint8_t*, ssize_t, DecodedImage&)>;

    /** Registers a decoder for a file extension. */
    static void registerDecoder(std::string_view extension, Decoder decoder);

    /** Uses O(1) extension lookup to decode the data. */
    static bool decode(std::string_view extension, const uint8_t* data, ssize_t dataLen, DecodedImage& result);
};

}  // namespace ax
