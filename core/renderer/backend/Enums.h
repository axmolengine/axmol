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

#pragma once

#include "Macros.h"

#include <assert.h>
#include <cstdint>
#include <string>
#include "base/bitmask.h"

NS_AX_BACKEND_BEGIN

enum class BufferUsage : uint32_t
{
    STATIC,
    DYNAMIC
};

enum class BufferType : uint32_t
{
    ARRAY_BUFFER,
    ELEMENT_ARRAY_BUFFER,
    UNIFORM_BUFFER,
    PIXEL_PACK_BUFFER,
    COUNT,
    VERTEX  = ARRAY_BUFFER,
    INDEX   = ELEMENT_ARRAY_BUFFER,
    UNIFORM = UNIFORM_BUFFER
};

enum class ShaderStage
{
    UNKNOWN = -1,
    VERTEX,
    FRAGMENT
};

enum class VertexFormat : uint32_t
{
    FLOAT4,
    FLOAT3,
    FLOAT2,
    FLOAT,
    INT4,
    INT3,
    INT2,
    INT,
    USHORT4,
    USHORT2,
    UBYTE4
};
/** @typedef backend::PixelFormat
     Possible texture pixel formats
refer to:
  https://docs.gl/es3/glTexImage2D
  https://docs.gl/gl3/glTexImage2D
  https://developer.apple.com/documentation/metal/mtlpixelformat?language=objc
 */

enum class PixelFormat : uint32_t
{
    /* below is compression format */

    /* PVRTCV1, OpenGL support PVRTCv2, but Metal only support PVRTCv1*/
    //! 4-bit PVRTC-compressed texture: PVRTC4
    PVRTC4,
    //! 4-bit PVRTC-compressed texture: PVRTC4 (has alpha channel)
    PVRTC4A,
    //! 2-bit PVRTC-compressed texture: PVRTC2
    PVRTC2,
    //! 2-bit PVRTC-compressed texture: PVRTC2 (has alpha channel)
    PVRTC2A,

    //! ETC1-compressed texture: ETC1 4 BPP
    ETC1,
    //! ETC2-compressed texture: ETC2_RGB 4 BPP
    ETC2_RGB,
    //! ETC2-compressed texture: ETC2_RGBA 8 BPP
    ETC2_RGBA,

    //! S3TC-compressed texture: S3TC_Dxt1
    S3TC_DXT1,
    //! S3TC-compressed texture: S3TC_Dxt3
    S3TC_DXT3,
    //! S3TC-compressed texture: S3TC_Dxt5
    S3TC_DXT5,

    //! ATITC-compressed texture: ATC_RGB
    ATC_RGB,
    //! ATITC-compressed texture: ATC_EXPLICIT_ALPHA
    ATC_EXPLICIT_ALPHA,
    //! ATITC-compressed texture: ATC_INTERPOLATED_ALPHA
    ATC_INTERPOLATED_ALPHA,

    ASTC4x4,   //!< ASTC 4x4 8.0 BPP
    ASTC5x5,   //!< ASTC 5x5 5.12 BPP
    ASTC6x6,   //!< ASTC 6x6 3.56 BPP
    ASTC8x5,   //!< ASTC 8x5 3.20 BPP
    ASTC8x6,   //!< ASTC 8x6 2.67 BPP
    ASTC8x8,   //!< ASTC 8x8 2.0 BPP
    ASTC10x5,  //!< ASTC 10x5 2.56 BPP
    //!!!Please append compression pixel format

    /* below is normal pixel format */
    //! 32-bit texture: RGBA8888
    RGBA8,
    //! 32-bit texture: BGRA8888
    BGRA8,
    //! 24-bit texture: RGBA888
    RGB8,
    //! 16-bit texture without Alpha channel
    RGB565,  // !render as BGR565
    //! 16-bit textures: RGBA4444
    RGBA4,  // !render as ABGR4
    //! 16-bit textures: RGB5A1
    RGB5A1,  // !render as BGR5A1
    //! 8-bit textures used as red, GL3/GLES3/Metal     (R,G,0,0)
    R8,
    //! 16-bit read and green channel, GL3/GLES3/Metal  (R,G,0,0)
    RG8,
    //!!!Please append normal pixel format
    //! 32-bit texture: RGBA8888
    RGBA32F,

    /* below is depth compression format */
    // A packed 32-bit combined depth and stencil pixel format with two nomorlized unsigned integer
    // components: 24 bits, typically used for a depth render target, and 8 bits, typically used for
    // a stencil render target.
    D24S8,
    //!!!Please append depth stencil pixel format

    /* the count of pixel format supported by axmol */
    COUNT,

    NONE = 0xffff
};

enum class TextureUsage : uint32_t
{
    READ,
    WRITE,
    RENDER_TARGET
};

enum class IndexFormat : uint32_t
{
    U_SHORT = 1,
    U_INT   = 2,
};

enum class VertexStepMode : uint32_t
{
    VERTEX,
    INSTANCE
};

enum class PrimitiveType : uint32_t
{
    POINT,
    LINE,
    LINE_LOOP,
    LINE_STRIP,
    TRIANGLE,
    TRIANGLE_STRIP
};

enum class TextureType : uint32_t
{
    TEXTURE_2D,
    TEXTURE_CUBE
};

enum class SamplerAddressMode : uint32_t
{
    REPEAT,
    MIRROR_REPEAT,
    CLAMP_TO_EDGE,
    DONT_CARE,
};

enum class SamplerFilter : uint32_t
{
    NEAREST,
    NEAREST_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR,
    LINEAR,
    LINEAR_MIPMAP_LINEAR,
    LINEAR_MIPMAP_NEAREST,
    DONT_CARE,
};

enum class StencilOperation : uint32_t
{
    KEEP,
    ZERO,
    REPLACE,
    INVERT,
    INCREMENT_WRAP,
    DECREMENT_WRAP
};

enum class CompareFunction : uint32_t
{
    NEVER,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    EQUAL,
    NOT_EQUAL,
    ALWAYS
};

enum class BlendOperation : uint32_t
{
    ADD,
    SUBTRACT,
    REVERSE_SUBTRACT
};

enum class BlendFactor : uint32_t
{
    ZERO,
    ONE,
    SRC_COLOR,
    ONE_MINUS_SRC_COLOR,
    SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA,
    DST_COLOR,
    ONE_MINUS_DST_COLOR,
    DST_ALPHA,
    ONE_MINUS_DST_ALPHA,
    CONSTANT_ALPHA,
    SRC_ALPHA_SATURATE,
    ONE_MINUS_CONSTANT_ALPHA,
    BLEND_COLOR
};

enum class ColorWriteMask : uint32_t
{
    RED_BIT   = 0,
    GREEN_BIT = 1,
    BLUE_BIT  = 2,
    ALPHA_BIT = 3,
    NONE      = 0,
    RED       = 1 << RED_BIT,
    GREEN     = 1 << GREEN_BIT,
    BLUE      = 1 << BLUE_BIT,
    ALPHA     = 1 << ALPHA_BIT,
    ALL       = 0x0000000F
};
AX_ENABLE_BITMASK_OPS(ColorWriteMask)
AX_ENABLE_BITSHIFT_OPS(ColorWriteMask)

/**
 * Bitmask for selecting render buffers
 */
enum class TargetBufferFlags : uint8_t
{
    NONE              = 0x0u,    //!< No buffer selected.
    COLOR0            = 0x1u,    //!< Color buffer selected.
    COLOR1            = 0x2u,    //!< Color buffer selected.
    COLOR2            = 0x4u,    //!< Color buffer selected.
    COLOR3            = 0x8u,    //!< Color buffer selected.
    COLOR             = COLOR0,  //!< \deprecated
    COLOR_ALL         = COLOR0 | COLOR1 | COLOR2 | COLOR3,
    DEPTH             = 0x10u,                       //!< Depth buffer selected.
    STENCIL           = 0x20u,                       //!< Stencil buffer selected.
    DEPTH_AND_STENCIL = DEPTH | STENCIL,             //!< depth and stencil buffer selected.
    ALL               = COLOR_ALL | DEPTH | STENCIL  //!< Color, depth and stencil buffer selected.
};
AX_ENABLE_BITMASK_OPS(TargetBufferFlags)

enum class DepthStencilFlags : unsigned int
{
    NONE               = 0,
    DEPTH_TEST         = 1,
    DEPTH_WRITE        = 1 << 1,
    STENCIL_TEST       = 1 << 2,
    DEPTH_STENCIL_TEST = DEPTH_TEST | STENCIL_TEST,
    ALL                = DEPTH_TEST | STENCIL_TEST | DEPTH_WRITE,
};
AX_ENABLE_BITMASK_OPS(DepthStencilFlags)
AX_ENABLE_BITSHIFT_OPS(DepthStencilFlags)

enum class CullMode : uint32_t
{
    NONE  = 0x00000000,
    BACK  = 0x00000001,
    FRONT = 0x00000002
};

enum class Winding : uint32_t
{
    CLOCK_WISE,
    COUNTER_CLOCK_WISE
};

enum class TextureCubeFace : uint32_t
{
    POSITIVE_X = 0,
    NEGATIVE_X = 1,
    POSITIVE_Y = 2,
    NEGATIVE_Y = 3,
    POSITIVE_Z = 4,
    NEGATIVE_Z = 5
};

// clang-format off
struct ProgramType
{
    enum : uint32_t
    {
        POSITION_COLOR_LENGTH_TEXTURE,        // positionColorLengthTexture_vert, positionColorLengthTexture_frag
        POSITION_COLOR_TEXTURE_AS_POINTSIZE,  // positionColorTextureAsPointsize_vert, positionColor_frag
        POSITION_COLOR,                       // positionColor_vert,              positionColor_frag
        POSITION_UCOLOR,                      // positionUColor_vert,             positionUColor_frag
        POSITION_TEXTURE,                     // positionTexture_vert,            positionTexture_frag
        POSITION_TEXTURE_COLOR,               // positionTextureColor_vert,       positionTextureColor_frag
        POSITION_TEXTURE_COLOR_ALPHA_TEST,    // positionTextureColor_vert,       positionTextureColorAlphaTest_frag
        LABEL_NORMAL,                         // positionTextureColor_vert,       label_normal_frag
        LABLE_OUTLINE,                        // positionTextureColor_vert,       labelOutline_frag
        LABEL_DISTANCE_NORMAL,                // positionTextureColor_vert,       label_distanceNormal_frag
        LABEL_DISTANCE_OUTLINE,               // positionTextureColor_vert,       label_distanceOutline_frag
        LABLE_DISTANCE_GLOW,                  // positionTextureColor_vert,       label_distanceGlow_frag

        LAYER_RADIA_GRADIENT,                 // position_vert,                   layer_radialGradient_frag

        DUAL_SAMPLER,
        DUAL_SAMPLER_GRAY,
        ETC1      = DUAL_SAMPLER,             // positionTextureColor_vert,       dualSampler_frag
        ETC1_GRAY = DUAL_SAMPLER_GRAY,        // positionTextureColor_vert,       dualSamplerGray_frag
        GRAY_SCALE,                           // positionTextureColor_vert,       grayScale_frag
        CAMERA_CLEAR,                         // cameraClear_vert,                cameraClear_frag

        TERRAIN_3D,                           // terrain_vert,                    terrain_frag
        LINE_COLOR_3D,                        // lineColor_vert,                  lineColor_frag
        SKYBOX_3D,                            // skybox_vert,                     skybox_frag
        SKINPOSITION_TEXTURE_3D,              // skinPositionTexture_vert,        colorTexture_frag
        SKINPOSITION_NORMAL_TEXTURE_3D,       // skinPositionNormalTexture_vert,  colorNormalTexture_frag
        POSITION_NORMAL_TEXTURE_3D,           // positionNormalTexture_vert,      colorNormalTexture_frag
        POSITION_NORMAL_3D,                   // positionNormalTexture_vert,      colorNormal_frag
        POSITION_TEXTURE_3D,                  // positionTexture3D_vert,          colorTexture_frag
        POSITION_TEXTURE_3D_INSTANCE,         // positionTextureInstance_vert,    colorTexture_frag
        POSITION_3D,                          // positionTexture_vert,            color_frag
        POSITION_BUMPEDNORMAL_TEXTURE_3D,     // positionNormalTexture_vert,      colorNormalTexture_frag
        SKINPOSITION_BUMPEDNORMAL_TEXTURE_3D, // skinPositionNormalTexture_vert,  colorNormalTexture_frag
        PARTICLE_TEXTURE_3D,                  // particle_vert,                   particleTexture_frag
        PARTICLE_COLOR_3D,                    // particle_vert,                   particleColor_frag

        QUAD_COLOR_2D,                        // quad_vert,                       quadColor_frag
        QUAD_TEXTURE_2D,                      // quad_vert,                       quadTexture_frag

        HSV,
        HSV_DUAL_SAMPLER,
        HSV_ETC1 = HSV_DUAL_SAMPLER,

        VIDEO_TEXTURE_YUY2,
        VIDEO_TEXTURE_NV12,
        VIDEO_TEXTURE_I420, // For some android 11 and older devices
        VIDEO_TEXTURE_BGR32,

        BUILTIN_COUNT,

        VIDEO_TEXTURE_RGB32 = POSITION_TEXTURE_COLOR,
        CUSTOM_PROGRAM      = 0x1000,  // user-define program, used by engine
    };
};
// clang-format on

NS_AX_BACKEND_END
