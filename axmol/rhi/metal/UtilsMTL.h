/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/rhi/RHITypes.h"
#include "axmol/rhi/PixelBufferDesc.h"
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include <functional>

namespace ax::rhi
{
class Texture;

namespace mtl
{

/**
 * @addtogroup _metal
 * @{
 */

/**
 * create and manager default depth and stencil attachment.
 * generate mipmaps.
 * read pixles from texture.
 */
struct UtilsMTL
{
    /*
     * init GPU texture supported formats
     */
    static void initGPUTextureFormats();

    static PixelFormat getDefaultColorAttachmentPixelFormat();

    static MTLPixelFormat getDefaultDepthStencilAttachmentPixelFormat();

    /**
     * Convert rhi::PixelFormat to MTLPixelFormat.
     * @param textureFormat Specifies a pixel format to be converted.
     * @return A MTLPixelFormat.
     */
    static MTLPixelFormat toMTLPixelFormat(PixelFormat textureFormat);

    /**
     * Generate a complete set of mipmaps in given texture.
     * @param texture Specifies a texture to generate mipmap.
     */
    static void generateMipmaps(id<MTLTexture> texture);

    /**
     * Swizzle the iamge form the given format to MTLPixelFormatRGBA8Unorm.
     * @param image Specifies the image to be swizzled.
     * @param width Specifies the width of the image.
     * @param heigth Specifies the height of the image.
     * @param format Specifies the format of the image.
     */
    static void swizzleImage(unsigned char* image, size_t width, size_t height, MTLPixelFormat format);
};

}  // namespace mtl

// end of _metal group
/// @}
}  // namespace ax::rhi
