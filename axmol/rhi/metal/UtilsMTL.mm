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

#include "axmol/rhi/metal/UtilsMTL.h"
#include "axmol/rhi/metal/DriverMTL.h"
#include "axmol/rhi/metal/TextureMTL.h"
#include "axmol/rhi/RHIUtils.h"

namespace ax::rhi::mtl
{
namespace
{
MTLPixelFormat getSupportedDepthStencilFormat()
{
    MTLPixelFormat pixelFormat = MTLPixelFormatDepth32Float_Stencil8;
#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
    bool isDepth24Stencil8PixelFormatSupported = DriverImpl::supportD24S8();
    if (isDepth24Stencil8PixelFormatSupported)
        pixelFormat = MTLPixelFormatDepth24Unorm_Stencil8;
#endif
    return pixelFormat;
}
}  // namespace

struct GPUTextureFormatInfo
{
    MTLPixelFormat fmt;
    MTLPixelFormat fmtSrgb;
};

static GPUTextureFormatInfo s_textureFormats[] = {
    /* pvrtc v1 */
    {MTLPixelFormat(162 /*PVRTC_RGB_4BPP*/), MTLPixelFormat(163 /*PVRTC_RGB_4BPP_sRGB*/)},    // PTC14
    {MTLPixelFormat(166 /*PVRTC_RGBA_4BPP*/), MTLPixelFormat(167 /*PVRTC_RGBA_4BPP_sRGB*/)},  // PTC14A
    {MTLPixelFormat(160 /*PVRTC_RGB_2BPP*/), MTLPixelFormat(161 /*PVRTC_RGB_2BPP_sRGB*/)},    // PTC12
    {MTLPixelFormat(164 /*PVRTC_RGBA_2BPP*/), MTLPixelFormat(165 /*PVRTC_RGBA_2BPP_sRGB*/)},  // PTC12A

    /* etc */
    {MTLPixelFormat(180 /*ETC2_RGB8*/), MTLPixelFormat(181 /*ETC2_RGB8_sRGB*/)},  // ETC1
    {MTLPixelFormat(180 /*ETC2_RGB8*/), MTLPixelFormat(181 /*ETC2_RGB8_sRGB*/)},  // ETC2
    {MTLPixelFormat(178 /*EAC_RGBA8*/), MTLPixelFormat(179 /*EAC_RGBA8_sRGB*/)},  // ETC2A

    /* s3tc */
    {MTLPixelFormat(130 /*BC1_RGBA*/), MTLPixelFormat(131 /*BC1_RGBA_sRGB*/)},  // S3TC_DXT1/BC1
    {MTLPixelFormat(132 /*BC2_RGBA*/), MTLPixelFormat(133 /*BC2_RGBA_sRGB*/)},  // S3TC_DXT3/BC2
    {MTLPixelFormat(134 /*BC3_RGBA*/), MTLPixelFormat(135 /*BC3_RGBA_sRGB*/)},  // S3TC_DXT5/BC3

    /* atc */
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ATC_RGB
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ATC_EXPLICIT_ALPHA
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ATC_INTERPOLATED_ALPHA

/* astc */
#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
    {MTLPixelFormatASTC_4x4_LDR, MTLPixelFormatASTC_4x4_sRGB},      // ASTC4x4
    {MTLPixelFormatASTC_5x4_LDR, MTLPixelFormatASTC_5x4_sRGB},      // ASTC5x4
    {MTLPixelFormatASTC_5x5_LDR, MTLPixelFormatASTC_5x5_sRGB},      // ASTC5x5
    {MTLPixelFormatASTC_6x5_LDR, MTLPixelFormatASTC_6x5_sRGB},      // ASTC6x5
    {MTLPixelFormatASTC_6x6_LDR, MTLPixelFormatASTC_6x6_sRGB},      // ASTC6x6
    {MTLPixelFormatASTC_8x5_LDR, MTLPixelFormatASTC_8x5_sRGB},      // ASTC8x5
    {MTLPixelFormatASTC_8x6_LDR, MTLPixelFormatASTC_8x6_sRGB},      // ASTC8x6
    {MTLPixelFormatASTC_8x8_LDR, MTLPixelFormatASTC_8x8_sRGB},      // ASTC8x8
    {MTLPixelFormatASTC_10x5_LDR, MTLPixelFormatASTC_10x5_sRGB},    // ASTC10x5
    {MTLPixelFormatASTC_10x6_LDR, MTLPixelFormatASTC_10x6_sRGB},    // ASTC10x6
    {MTLPixelFormatASTC_10x8_LDR, MTLPixelFormatASTC_10x8_sRGB},    // ASTC10x8
    {MTLPixelFormatASTC_10x10_LDR, MTLPixelFormatASTC_10x10_sRGB},  // ASTC10x10
    {MTLPixelFormatASTC_12x10_LDR, MTLPixelFormatASTC_12x10_sRGB},  // ASTC12x10
    {MTLPixelFormatASTC_12x12_LDR, MTLPixelFormatASTC_12x12_sRGB},  // ASTC12x12
#else
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC4x4
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC5x4
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC5x5
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC6x5
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC6x6
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC8x5
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC8x6
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC8x8
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC10x5
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC10x6
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC10x8
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC10x10
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC12x10
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},  // ASTC12x12
#endif

    /* normal */
    {MTLPixelFormatRGBA8Unorm, MTLPixelFormatRGBA8Unorm_sRGB},    // RGBA8
    {MTLPixelFormatBGRA8Unorm, MTLPixelFormatBGRA8Unorm_sRGB},    // BGRA8
    {MTLPixelFormatInvalid, MTLPixelFormatInvalid},               // RGB8
    {MTLPixelFormat(40 /*B5G6R5Unorm*/), MTLPixelFormatInvalid},  // R5G6B5
    {MTLPixelFormat(42 /*ABGR4Unorm*/), MTLPixelFormatInvalid},   // RGBA4
    {MTLPixelFormat(41 /*A1BGR5Unorm*/), MTLPixelFormatInvalid},  // RGB5A1
    {MTLPixelFormatR8Unorm, MTLPixelFormatInvalid},               // R8 In Shader: (R,0,0,0)
    {MTLPixelFormatRG8Unorm, MTLPixelFormatInvalid},              // RG8 In Shader: (R,G,0,0)
    {MTLPixelFormatRGBA32Float, MTLPixelFormatInvalid},           // RGBA32Fs
    /* depth stencil */
    {MTLPixelFormat(255 /*Depth24Unorm_Stencil8*/), MTLPixelFormatInvalid},  // D24S8
};
static_assert(AX_ARRAYSIZE(s_textureFormats) == (int)PixelFormat::COUNT,
              "The OpenGL GPU texture format info table incomplete!");

void UtilsMTL::initGPUTextureFormats()
{
    // on mac, D24S8 means MTLPixelFormatDepth24Unorm_Stencil8, while on ios it means
    // MTLPixelFormatDepth32Float_Stencil8
    auto& info = s_textureFormats[(int)PixelFormat::D24S8];
    info.fmt   = getSupportedDepthStencilFormat();
}

MTLPixelFormat UtilsMTL::getDefaultColorAttachmentPixelFormat()
{
    return MTLPixelFormatBGRA8Unorm;
}

MTLPixelFormat UtilsMTL::getDefaultDepthStencilAttachmentPixelFormat()
{
    return getSupportedDepthStencilFormat();
}

MTLPixelFormat UtilsMTL::toMTLPixelFormat(PixelFormat textureFormat)
{
    if (AX_LIKELY(textureFormat < PixelFormat::COUNT))
    {
        return s_textureFormats[(int)textureFormat].fmt;
    }
    return MTLPixelFormatInvalid;
}

void UtilsMTL::generateMipmaps(id<MTLTexture> texture)
{
    auto cmdQueue = static_cast<DriverImpl*>(DriverBase::getInstance())->getMTLCmdQueue();
    @autoreleasepool
    {
        auto oneOffBuffer                        = [cmdQueue commandBuffer];
        id<MTLBlitCommandEncoder> commandEncoder = [oneOffBuffer blitCommandEncoder];
        [commandEncoder generateMipmapsForTexture:texture];
        [commandEncoder endEncoding];
        [oneOffBuffer commit];
    }
}

void UtilsMTL::swizzleImage(unsigned char* image, std::size_t width, std::size_t height, MTLPixelFormat format)
{
    if (!image)
        return;

    auto len = width * height;
    switch (format)
    {
    // convert to RGBA
    case MTLPixelFormatBGRA8Unorm:
        for (int i = 0; i < len; i++)
        {
            unsigned char temp = image[i * 4];
            image[i * 4]       = image[i * 4 + 2];
            image[i * 4 + 2]   = temp;
        }
        break;

    default:
        break;
    }
}

}  // namespace ax::rhi::mtl
