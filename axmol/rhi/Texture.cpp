/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/rhi/Texture.h"
#include "axmol/rhi/RHIUtils.h"
#include "axmol/tlx/byte_buffer.hpp"
#include <assert.h>
namespace ax::rhi
{

Texture::~Texture() {}

void Texture::updateTextureDesc(const TextureDesc& desc)
{
    _bitsPerPixel = RHIUtils::getBitsPerPixel(desc.pixelFormat);
    _desc         = desc;

    if (_bitsPerPixel == 0)
        _bitsPerPixel = (uint8_t)(8 * 4);

    _overrideMipLevels = desc.mipLevels;
}

void Texture::invalidate() {}

void Texture::updateData3D(const void* data, int width, int height, int depth, int level)
{
    updateSubData3D(0, 0, 0, width, height, depth, level, data);
}

void Texture::updateSubData3D(int xoffset,
                              int yoffset,
                              int zoffset,
                              int width,
                              int height,
                              int depth,
                              int level,
                              const void* data)
{
    AX_UNUSED_PARAM(xoffset);
    AX_UNUSED_PARAM(yoffset);
    AX_UNUSED_PARAM(zoffset);
    AX_UNUSED_PARAM(width);
    AX_UNUSED_PARAM(height);
    AX_UNUSED_PARAM(depth);
    AX_UNUSED_PARAM(level);
    AX_UNUSED_PARAM(data);
    AXASSERT(false, "Texture3D upload is not implemented by this backend");
}

void Texture::zeroTexData()
{
    if (!RHIUtils::isCompressed(_desc.pixelFormat))
    {
        const auto w = getWidth();
        const auto h = getHeight();
        auto size    = w * h * _bitsPerPixel / 8;
        assert(size > 0);

        tlx::byte_buffer blackPixels;
        blackPixels.resize(size, 0);

        updateData(blackPixels.data(), w, h, 0, 0);
    }
}

}  // namespace ax::rhi
