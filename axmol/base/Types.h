/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include <string>

#include "axmol/math/Math.h"
#include "axmol/base/Object.h"
#include "axmol/rhi/RHITypes.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

/**
 * @brief Effects used by `Label`
 */
enum class LabelEffect
{
    // FIXME: Covert them to bitwise. More than one effect should be supported
    NORMAL,
    OUTLINE,
    SHADOW,
    GLOW,
    ITALICS,
    BOLD,
    UNDERLINE,
    STRIKETHROUGH,
    ALL
};

/**
 * @brief Interval change reason.
 */
enum class SetIntervalReason : char
{
    BY_GAME = 0,
    BY_ENGINE,
    BY_SYSTEM,
    BY_SCENE_CHANGE,
    BY_DIRECTOR_PAUSE
};

/**
 * @brief Texture flags.
 */
struct TextureFlag
{
    enum
    {
        NONE               = 0,
        ANTIALIAS_ENABLED  = 1 << 1,
        PREMULTIPLIEDALPHA = 1 << 2,
    };
};

/**
 * @brief Texture sampler flags.
 */
struct TextureSamplerFlag
{
    enum
    {
        DEFAULT      = 0,
        DUAL_SAMPLER = 1 << 1,
    };
};

/**
 * @brief Matrix stack type.
 */
enum class MATRIX_STACK_TYPE
{
    /// Model view matrix stack
    MATRIX_STACK_MODELVIEW,

    /// projection matrix stack
    MATRIX_STACK_PROJECTION,

    /// texture matrix stack
    MATRIX_STACK_TEXTURE
};

/**
 * @brief Anchor presets used to position nodes in corners
 */
enum class AnchorPreset
{
    BOTTOM_LEFT   = 0,
    BOTTOM_CENTER = 1,
    BOTTOM_RIGHT  = 2,
    CENTER_LEFT   = 3,
    CENTER        = 4,
    CENTER_RIGHT  = 5,
    TOP_LEFT      = 6,
    TOP_CENTER    = 7,
    TOP_RIGHT     = 8
};

/**
 * Particle emission shapes.
 * Supported shapes are Point, Rectangle, RectangularTorus, Circle, Torus, Cone, Cone Torus, Texture alpha emission mask
 * @since axmol-1.0.0b8
 */
enum class EmissionShapeType
{
    // Emission shape of type point
    POINT,
    // Emission shape of type rectangle
    RECT,
    // Emission shape of type rectangular torus
    RECTTORUS,
    // Emission shape of type circle or cone
    CIRCLE,
    // Emission shape of type torus or cone torus
    TORUS,
    // Emission shape of type texture alpha mask
    TEXTURE_ALPHA_MASK
};

/** @struct Tex2F
 * A TEXCOORD composed of 2 floats: u, v
 * @since v3.0
 */
typedef Vec2 Tex2F;

/** @struct Quad2
 * A 2D Quad. 4 * 2 floats.
 */
struct Quad2
{
    Vec2 tl;
    Vec2 tr;
    Vec2 bl;
    Vec2 br;
};

/** @struct Quad3
 * A 3D Quad. 4 * 3 floats.
 */
struct Quad3
{
    Vec3 bl;
    Vec3 br;
    Vec3 tl;
    Vec3 tr;
};

/* @struct V2F_T2F
 * A Vec2 with a vertex point and a tex coord point.
 * MotionStreak
 */
struct V2F_T2F_C4B
{
    /// position (2F)
    Vec2 position;
    /// tex coords (2F)
    Tex2F texCoord;
    /// color (4F)
    Color32 color;
};

/** @struct V2F_T2F_C4F
 * A Vec2 with a vertex point, a tex coord point and a color 4F.
 */
struct V2F_T2F_C4F
{
    /// position (2F)
    Vec2 position;
    /// tex coords (2F)
    Tex2F texCoord;
    /// color (4F)
    Color color;
};

/** @struct V3F_T2F_C4B
 * A Vec2 with a vertex point, a tex coord point and a color 4B.
 */
struct V3F_T2F_C4B
{
    /// position (3F)
    Vec3 position;  // 12 bytes

    // tex coords (2F)
    Tex2F texCoord;  // 8 bytes

    /// color (4B)
    Color32 color;  // 4 bytes
};

/** @struct V3F_T2F_C4F
 * A Vec2 with a vertex point, a tex coord point and a color 4F.
 */
struct V3F_T2F_C4F
{
    /// position (3F)
    Vec3 position;  // 12 bytes

    // tex coords (2F)
    Tex2F texCoord;  // 8 bytes

    /// color (4F)
    Color color;  // 16 bytes
};

/** @struct V3F_T2F
 * A Vec2 with a vertex point, a tex coord point.
 */
struct V3F_T2F
{
    /// position (2F)
    Vec3 position;
    /// tex coords (2F)
    Tex2F texCoord;
};

/** @struct V3F_C4F
 * A Vec3 with a vertex point, a color.
 */
struct V3F_C4F
{
    /// position (3F)
    Vec3 position;
    /// color (4F)
    Color color;
};

struct V3F_T2F_N3F
{
    Vec3 position;
    Tex2F texcoord;
    Vec3 normal;
};

struct V2F_T2F_C4F_Triangle
{
    V2F_T2F_C4F a;
    V2F_T2F_C4F b;
    V2F_T2F_C4F c;
};

/** @struct V3F_T2F_C4F_Quad
 * 4 Vertex3FTex2FColor32.
 */
struct V3F_T2F_C4B_Quad
{
    /// top left
    V3F_T2F_C4B tl;
    /// bottom left
    V3F_T2F_C4B bl;
    /// top right
    V3F_T2F_C4B tr;
    /// bottom right
    V3F_T2F_C4B br;
};

/** @struct V3F_T2F_C4F_Quad
 * 4 Vertex3FTex2FColor4F.
 */
struct V3F_T2F_C4F_Quad
{
    /// top left
    V3F_T2F_C4F tl;
    /// bottom left
    V3F_T2F_C4F bl;
    /// top right
    V3F_T2F_C4F tr;
    /// bottom right
    V3F_T2F_C4F br;
};

/** @struct V3F_T2F_Quad
 *
 */
struct V3F_T2F_Quad
{
    /// bottom left
    V3F_T2F bl;
    /// bottom right
    V3F_T2F br;
    /// top left
    V3F_T2F tl;
    /// top right
    V3F_T2F tr;
};

/** @struct BlendFunc
 * Blend Function used for textures.
 */
struct AX_DLL BlendFunc
{
    /** source blend function */
    rhi::BlendFactor src;
    /** destination blend function */
    rhi::BlendFactor dst;

    /** Blending disabled. Uses {BlendFactor::ONE, BlendFactor::ZERO} */
    static const BlendFunc DISABLE;
    /** Blending enabled for textures with Alpha premultiplied. Uses {BlendFactor::ONE,
     * BlendFactor::ONE_MINUS_SRC_ALPHA} */
    static const BlendFunc ALPHA_PREMULTIPLIED;
    /** Blending enabled for textures with Alpha NON premultiplied. Uses {BlendFactor::SRC_ALPHA,
     * BlendFactor::ONE_MINUS_SRC_ALPHA} */
    static const BlendFunc ALPHA_NON_PREMULTIPLIED;
    /** Enables Additive blending. Uses {BlendFactor::SRC_ALPHA, BlendFactor::ONE} */
    static const BlendFunc ADDITIVE;

    bool operator==(const BlendFunc& a) const { return src == a.src && dst == a.dst; }

    bool operator!=(const BlendFunc& a) const { return src != a.src || dst != a.dst; }

    bool operator<(const BlendFunc& a) const { return src < a.src || (src == a.src && dst < a.dst); }
};

/** @enum TextVAlignment
 * Vertical text alignment type.
 *
 * @note If any of these enums are edited and/or reordered, update Texture2D.m.
 */
enum class TextVAlignment
{
    TOP,
    CENTER,
    BOTTOM
};

/** @enum TextHAlignment
 * Horizontal text alignment type.
 *
 * @note If any of these enums are edited and/or reordered, update Texture2D.m.
 */
enum class TextHAlignment
{
    LEFT,
    CENTER,
    RIGHT
};

/**
 * @brief Possible GlyphCollection used by Label.
 *
 * Specify a collections of characters to be load when Label created.
 * Consider using DYNAMIC.
 */
enum class GlyphCollection
{
    DYNAMIC,
    NEHE,
    ASCII,
    CUSTOM
};

// Types for animation in particle systems

/** @struct T2F_Quad
 * Texture coordinates for a quad.
 */
struct AX_DLL T2F_Quad
{
    /// bottom left
    Tex2F bl;
    /// bottom right
    Tex2F br;
    /// top left
    Tex2F tl;
    /// top right
    Tex2F tr;
};

/** @struct AnimationFrameData
 * Struct that holds the size in pixels, texture coordinates and delays for animated ParticleSystemQuad.
 */
struct AX_DLL AnimationFrameData
{
    T2F_Quad texCoords;
    Vec2 size;
    float delay = 0.f;
};

/**
 types used for defining fonts properties (i.e. font name, size, stroke or shadow)
 */

/** @struct FontShadow
 * Shadow attributes.
 */
struct AX_DLL FontShadow
{
    /// shadow x and y offset
    Vec2 _shadowOffset;
    /// shadow blurriness
    float _shadowBlur = 0.f;
    /// shadow opacity
    float _shadowOpacity = 0.f;
    /// true if shadow enabled
    bool _shadowEnabled = false;
};

/** @struct FontStroke
 * Stroke attributes.
 */
struct AX_DLL FontStroke
{
    /// stroke color
    Color32 _strokeColor = Color32::BLACK;
    /// stroke size
    float _strokeSize = 0.f;
    /// true if stroke enabled
    bool _strokeEnabled = false;
};

/** @struct FontDefinition
 * Font attributes.
 */
struct AX_DLL FontDefinition
{
    /// font name
    std::string _fontName;
    /// font size
    int _fontSize = 0;
    /// horizontal alignment
    TextHAlignment _alignment = TextHAlignment::CENTER;
    /// vertical alignment
    TextVAlignment _vertAlignment = TextVAlignment::TOP;
    /// rendering box
    Vec2 _dimensions = Vec2::ZERO;
    /// font color
    Color32 _fontFillColor = Color32::WHITE;
    /// font shadow
    FontShadow _shadow;
    /// font stroke
    FontStroke _stroke;
    /// enable text wrap
    bool _enableWrap = true;
    /** There are 4 overflows: none, clamp, shrink and resize_height.
     *  The corresponding integer values are 0, 1, 2, 3 respectively
     * For more information, please refer to Label::Overflow enum class.
     */
    int _overflow = 0;
};

// d3d RHI spec
enum class PowerPreference
{
    Auto,            // Let the system decide
    LowPower,        // Prefer integrated GPU
    HighPerformance  // Prefer discrete GPU
};

enum class RenderScaleMode
{
    Default,  // Use the system's default scaling behavior
    Logical,  // Use logical pixels (do not apply DPI scaling)
    Physical  // Use logical pixels multiplied by the DPI scale factor
};

/** @struct ContextAttrs
 *
 * The axmol Engine attributes.
 */
struct ContextAttrs
{
    int redBits{8};
    int greenBits{8};
    int blueBits{8};
    int alphaBits{8};
    int depthBits{24};
    int stencilBits{8};
    int multisamplingCount{0};
    bool visible{true};
    bool decorated{true};
    bool vsync{true};
    bool debugLayerEnabled{false};
    void* windowParent{nullptr};  // win32-spec
    PowerPreference powerPreference{PowerPreference::Auto};
    RenderScaleMode renderScaleMode{RenderScaleMode::Default};
};

/** @struct Acceleration
 * The device accelerometer reports values for each axis in units of g-force.
 */
class AX_DLL Acceleration : public Object
{
public:
    double x = 0;
    double y = 0;
    double z = 0;

    double timestamp = 0;
};

extern const std::string AX_DLL STD_STRING_EMPTY;
extern const ssize_t AX_DLL AX_INVALID_INDEX;

using Viewport    = rhi::Viewport;
using ScissorRect = rhi::ScissorRect;  // both GL & Metal is integer type, GL: int, Metal: NSUInteger

using TextureUsage = rhi::TextureUsage;
using PixelFormat  = rhi::PixelFormat;

using TargetBufferFlags = rhi::TargetBufferFlags;
using DepthStencilFlags = rhi::DepthStencilFlags;
using ClearFlag         = rhi::ClearFlag;

typedef void (*AsyncOperation)(void* param);

}  // namespace ax
// end group
/// @}
