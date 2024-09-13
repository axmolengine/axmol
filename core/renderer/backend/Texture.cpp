/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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

#include "Texture.h"
#include "PixelFormatUtils.h"
#include <cassert>
NS_AX_BACKEND_BEGIN

TextureBackend::~TextureBackend() {}

void TextureBackend::updateTextureDescriptor(const ax::backend::TextureDescriptor& descriptor, int /*index*/)
{
    _bitsPerPixel  = PixelFormatUtils::getBitsPerPixel(descriptor.textureFormat);
    _textureType   = descriptor.textureType;
    _textureFormat = descriptor.textureFormat;
    _textureUsage  = descriptor.textureUsage;
    _width        = (std::max)(descriptor.width, (uint32_t)1);
    _height       = (std::max)(descriptor.height, (uint32_t)1);

    if (_bitsPerPixel == 0)
    {
        _bitsPerPixel = (uint8_t)(8 * 4);
    }
}

NS_AX_BACKEND_END
