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

#include <stdint.h>
#include <assert.h>
#include <string>
#include "axmol/tlx/bitmask.hpp"

#define MAX_COLOR_ATTCHMENT 4

#define MAX_INFLIGHT_BUFFER 3

#define AX_ARRAYSIZE(A)     (sizeof(A) / sizeof((A)[0]))

namespace ax::rhi
{
using namespace std::string_view_literals;

enum class BufferUsage : uint32_t
{
    STATIC,
    DYNAMIC,
    IMMUTABLE,  // d3d only
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
    UNKNOWN  = -1,
    FRAGMENT = 0,
    VERTEX,
    COMPUTE,
    DEFAULT = FRAGMENT
};

enum class VertexFormat : uint16_t
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
    UBYTE4,
    MAT4,
    COUNT,
};
/** @typedef rhi::PixelFormat
     Possible texture pixel formats
refer to:
  https://docs.gl/es3/glTexImage2D
  https://docs.gl/gl3/glTexImage2D
  https://developer.apple.com/documentation/metal/mtlpixelformat?language=objc
 */

enum class PixelFormat : uint8_t
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

    ASTC4x4,    //!< ASTC 4x4 8.0 BPP
    ASTC5x4,    //!< ASTC 5x4 6.40 BPP
    ASTC5x5,    //!< ASTC 5x5 5.12 BPP
    ASTC6x5,    //!< ASTC 6x5 4.27 BPP
    ASTC6x6,    //!< ASTC 6x6 3.56 BPP
    ASTC8x5,    //!< ASTC 8x5 3.20 BPP
    ASTC8x6,    //!< ASTC 8x6 2.67 BPP
    ASTC8x8,    //!< ASTC 8x8 2.0 BPP
    ASTC10x5,   //!< ASTC 10x5 2.56 BPP
    ASTC10x6,   //!< ASTC 10x6 2.13 BPP
    ASTC10x8,   //!< ASTC 10x8 1.60 BPP
    ASTC10x10,  //!< ASTC 10x10 1.28 BPP
    ASTC12x10,  //!< ASTC 12x10 1.07 BPP
    ASTC12x12,  //!< ASTC 12x12 0.89 BPP

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

    NONE = 0xFF,  //!< No pixel format, used for invalid texture
};

enum class TextureUsage : uint8_t
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

enum class PrimitiveType : uint32_t
{
    POINT,
    LINE,
    LINE_LOOP,
    LINE_STRIP,
    TRIANGLE,
    TRIANGLE_STRIP
};

// d3d12 requires this hints for create properly PSO
enum class PrimitiveGroup
{
    Point,    // D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT
    Line,     // D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE
    Triangle  // D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
};

enum class StencilOp : uint32_t
{
    KEEP,
    ZERO,
    REPLACE,
    INVERT,
    INCREMENT_WRAP,
    DECREMENT_WRAP
};

enum class BlendOp : uint32_t
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

inline TargetBufferFlags getMRTColorFlag(size_t index) noexcept
{
    assert(index < 4);
    return TargetBufferFlags(1u << index);
}

typedef TargetBufferFlags ClearFlag;

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

enum class TextureType : uint8_t
{
    TEXTURE_2D,
    TEXTURE_CUBE
};

enum class SamplerAddressMode : uint32_t
{
    REPEAT = 0,
    MIRROR,
    CLAMP,
    BORDER,
    // alias
    CLAMP_TO_EDGE = CLAMP,
};

enum class SamplerFilter : uint32_t
{
    // min filter
    MIN_NEAREST = 0,
    MIN_LINEAR,
    MIN_ANISOTROPIC,

    // mag filter
    MAG_NEAREST = 0,
    MAG_LINEAR,

    // mip filter
    MIP_NEAREST = 0,
    MIP_LINEAR,
    MIP_DEFAULT = 0b11,

    // alias
    NEAREST = 0,
    LINEAR
};

enum class CompareFunc : uint32_t
{
    NEVER,
    LESS,
    EQUAL,
    LESS_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_EQUAL,
    ALWAYS
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

/// @remark sizeof(SamplerDesc) == 4 as key for caching sampler fast
struct SamplerDesc
{
    SamplerFilter minFilter : 2         = SamplerFilter::MIN_LINEAR;
    SamplerFilter magFilter : 2         = SamplerFilter::MAG_LINEAR;
    SamplerFilter mipFilter : 2         = SamplerFilter::MIP_DEFAULT;
    SamplerAddressMode sAddressMode : 2 = SamplerAddressMode::CLAMP;
    SamplerAddressMode tAddressMode : 2 = SamplerAddressMode::CLAMP;
    SamplerAddressMode wAddressMode : 2 = SamplerAddressMode::CLAMP;
    CompareFunc compareFunc : 4         = CompareFunc::NEVER;
    uint32_t anisotropy : 4             = 1;
    uint32_t reserved : 12              = 0;
};
static_assert(sizeof(SamplerDesc) == 4, "incompatible type: SamplerDesc");

// The builtin sampler index
struct SamplerIndex
{
    enum enum_type : uint32_t
    {
        // --- Linear sampling ---
        LinearClamp,   // Linear, clamp to edge
        LinearWrap,    // Linear, repeat
        LinearMirror,  // Linear, mirror repeat
        LinearBorder,  // Linear, border color

        // --- Point sampling ---
        PointClamp,   // Nearest, clamp to edge
        PointWrap,    // Nearest, repeat
        PointMirror,  // Nearest, mirror repeat
        PointBorder,  // Nearest, border color

        // --- Linear + Mipmap ---
        LinearMipClamp,   // Linear min/mag, mip linear, clamp
        LinearMipWrap,    // Linear min/mag, mip linear, wrap
        LinearMipMirror,  // Linear min/mag, mip linear, mirror
        LinearMipBorder,  // Linear min/mag, mip linear, border

        // --- Anisotropic filtering ---
        AnisoClamp,   // Anisotropic, clamp to edge
        AnisoWrap,    // Anisotropic, repeat
        AnisoMirror,  // Anisotropic, mirror repeat
        AnisoBorder,  // Anisotropic, border color

        // --- Depth comparison samplers (shadow maps) ---
        ShadowCmpClamp,   // Compare sampler, clamp to edge
        ShadowCmpWrap,    // Compare sampler, repeat
        ShadowCmpMirror,  // Compare sampler, mirror repeat
        ShadowCmpBorder,  // Compare sampler, border color

        // --- Special cases ---
        LinearNoMipClamp,  // Linear min/mag, no mip, clamp (UI, 2D sprites)
        PointNoMipClamp,   // Point min/mag, no mip, clamp (pixel art)

        //
        Count
    };
};

using SamplerHandle = void*;

/**
 * Store texture description.
 */
struct TextureDesc
{
    uint16_t width     = 1;
    uint16_t height    = 1;
    uint16_t arraySize = 1;
    // =1: no mipmaps, =0: generate mipmaps by GPU, >1: mipmaps enabled manually
    uint16_t mipLevels        = 1;
    TextureType textureType   = TextureType::TEXTURE_2D;
    PixelFormat pixelFormat   = PixelFormat::RGBA8;
    TextureUsage textureUsage = TextureUsage::READ;
    SamplerDesc samplerDesc{};
};

/**
 * @brief a structor to store blend descriptor
 */
struct BlendDesc
{
    ColorWriteMask writeMask = ColorWriteMask::ALL;

    BlendOp rgbBlendOp   = BlendOp::ADD;
    BlendOp alphaBlendOp = BlendOp::ADD;

    BlendFactor sourceRGBBlendFactor        = BlendFactor::ONE;
    BlendFactor destinationRGBBlendFactor   = BlendFactor::ZERO;
    BlendFactor sourceAlphaBlendFactor      = BlendFactor::ONE;
    BlendFactor destinationAlphaBlendFactor = BlendFactor::ZERO;

    bool blendEnabled = false;
    char padding[3]   = {};
};

struct UniformInfo
{
    uint16_t count        = 0;   // element count
    uint16_t sampler_slot = 0;   // sampler slot
    int location          = -1;  // see also @StageUniformLocation

    // in opengl, type means uniform data type, i.e. GL_FLOAT_VEC2, while in metal type means data basic type, i.e.
    // float
    unsigned int type         = 0;
    unsigned int size         = 0;  // element size
    unsigned int bufferOffset = 0;
};

struct StageUniformLocation
{
    /*
     * gl: base_offset
     * d3d: semantic_index
     * metal: binding_index
     * vulkan: binding_index
     */
    int location = -1;
    int offset   = -1;

    operator bool() const { return location != -1; }
    bool operator==(const StageUniformLocation& other) const
    {
        return location == other.location && offset == other.offset;
    }
};

struct UniformLocation
{
    UniformLocation() = default;
    UniformLocation(StageUniformLocation&& s0, StageUniformLocation&& s1)
    {
        stages[0] = s0;
        stages[1] = s1;
    }

    /*
     * OpenGL(ES): all uniform linked to stages[0]
     * Other APIs: stages[0] for fragment shader, stages[1] for vertex shader
     */
    StageUniformLocation stages[2];

    operator bool() const { return stages[0] || stages[1]; }
    void reset()
    {
        stages[0] = {};
        stages[1] = {};
    }
    bool operator==(const UniformLocation& other) const
    {
        return stages[0] == other.stages[0] && stages[1] == other.stages[1];
    }
    std::size_t operator()(const UniformLocation&) const  // used as a hash function
    {
        return size_t(stages[0].location) ^ size_t(stages[1].location << 16);
    }
};

// vertex input descriptor in vertex shader
struct VertexInputDesc
{
    std::string semantic;
    // gl: location,
    // d3d: semantic_index
    // metal: binding_index
    // vulkan: binding_index
    int location = -1;
    int count    = 0;
    int format   = 0;
};

/// built-in uniform name
static constexpr auto UNIFORM_NAME_MVP_MATRIX      = "u_MVPMatrix"sv;
static constexpr auto UNIFORM_NAME_TEXTURE         = "u_tex0"sv;
static constexpr auto UNIFORM_NAME_TEXTURE1        = "u_tex1"sv;
static constexpr auto UNIFORM_NAME_TEXTURE2        = "u_tex2"sv;
static constexpr auto UNIFORM_NAME_TEXTURE3        = "u_tex3"sv;
static constexpr auto UNIFORM_NAME_TEXT_COLOR      = "u_textColor"sv;
static constexpr auto UNIFORM_NAME_EFFECT_COLOR    = "u_effectColor"sv;
static constexpr auto UNIFORM_NAME_EFFECT_WIDTH    = "u_effectWidth"sv;
static constexpr auto UNIFORM_NAME_LABEL_PASS      = "u_labelPass"sv;
static constexpr auto UNIFORM_NAME_DISTANCE_SPREAD = "u_distanceSpread"sv;

/// built-in attribute name
static constexpr auto VERTEX_INPUT_NAME_POSITION  = "a_position"sv;
static constexpr auto VERTEX_INPUT_NAME_COLOR     = "a_color"sv;
static constexpr auto VERTEX_INPUT_NAME_TEXCOORD  = "a_texCoord"sv;
static constexpr auto VERTEX_INPUT_NAME_TEXCOORD1 = "a_texCoord1"sv;
static constexpr auto VERTEX_INPUT_NAME_TEXCOORD2 = "a_texCoord2"sv;
static constexpr auto VERTEX_INPUT_NAME_TEXCOORD3 = "a_texCoord3"sv;
static constexpr auto VERTEX_INPUT_NAME_NORMAL    = "a_normal"sv;
static constexpr auto VERTEX_INPUT_NAME_INSTANCE  = "a_instance"sv;

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
        POSITION_TEXTURE_GRAY_ALPHA,          // positionTextureColor_vert,       positionTextureGrayAlpha_frag
        POSITION_TEXTURE_GRAY,                // positionTextureColor_vert,       positionTextureGray_frag
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
        UNLIT,                                // unlit_vert,                      colorTexture_frag
        UNLIT_INSTANCE,                       // unlitInstance_vert,              colorTexture_frag
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

        POS_UV_COLOR_2D,

        BUILTIN_COUNT,

        VIDEO_TEXTURE_RGB32 = POSITION_TEXTURE_COLOR,
        CUSTOM_PROGRAM      = 0x1000,  // user-define program, used by engine
    };
};
// clang-format on

struct RectI
{
    RectI() { this->x = this->y = this->w = this->h = 0; }
    int x;
    int y;

    union
    {
        struct
        {
            int width;
            int height;
        };
        struct
        {
            int w;
            int h;
        };
    };

    inline bool operator==(const RectI& v) const
    {
        return this->x == v.x && this->y == v.y && this->width == v.width && this->height == v.height;
    }
    inline RectI& set(int x, int y, int w, int h)
    {
        this->x      = x;
        this->y      = y;
        this->width  = w;
        this->height = h;
        return *this;
    }
};

using Viewport    = RectI;
using ScissorRect = RectI;

template <typename T, unsigned int N>
inline void SafeRelease(T (&resourceBlock)[N])
{
    for (unsigned int i = 0; i < N; i++)
    {
        SafeRelease(resourceBlock[i]);
    }
}

template <typename T>
inline void SafeRelease(T& resource)
{
    if (resource)
    {
        resource->Release();
        resource = nullptr;
    }
}

}  // namespace ax::rhi
