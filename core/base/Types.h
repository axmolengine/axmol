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

#include "math/Math.h"
#include "base/Object.h"
#include "renderer/backend/Types.h"

#include "Enums.h"

/**
 * @addtogroup base
 * @{
 */

NS_AX_BEGIN

/** @struct Tex2F
 * A TEXCOORD composed of 2 floats: u, v
 * @since v3.0
 */
typedef Vec2 Tex2F;

/** @struct PointSprite
 * Vec2 Sprite component.
 */
struct AX_DLL PointSprite
{
    Vec2 pos;          // 8 bytes
    Color4B color;     // 4 bytes
    float size = 0.f;  // 4 bytes
};

/** @struct Quad2
 * A 2D Quad. 4 * 2 floats.
 */
struct AX_DLL Quad2
{
    Vec2 tl;
    Vec2 tr;
    Vec2 bl;
    Vec2 br;
};

/** @struct Quad3
 * A 3D Quad. 4 * 3 floats.
 */
struct AX_DLL Quad3
{
    Vec3 bl;
    Vec3 br;
    Vec3 tl;
    Vec3 tr;
};

/** @struct V2F_C4B_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4B.
 */
struct V2F_C4B_T2F
{
    /// vertices (2F)
    Vec2 vertices;
    /// colors (4B)
    Color4B colors;
    /// tex coords (2F)
    Tex2F texCoords;
};

/** @struct V2F_C4B_PF
 *
 */
struct V2F_C4B_PF
{
    /// vertices (2F)
    Vec2 vertices;
    /// colors (4B)
    Color4B colors;
    /// pointsize
    float pointSize = 0.f;
};

/** @struct V2F_C4F_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4F.
 */
struct AX_DLL V2F_C4F_T2F
{
    /// vertices (2F)
    Vec2 vertices;
    /// colors (4F)
    Color4F colors;
    /// tex coords (2F)
    Tex2F texCoords;
};

/** @struct V3F_C4B_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4B.
 */
struct AX_DLL V3F_C4B_T2F
{
    /// vertices (3F)
    Vec3 vertices;  // 12 bytes

    /// colors (4B)
    Color4B colors;  // 4 bytes

    // tex coords (2F)
    Tex2F texCoords;  // 8 bytes
};

/** @struct V3F_T2F
 * A Vec2 with a vertex point, a tex coord point.
 */
struct AX_DLL V3F_T2F
{
    /// vertices (2F)
    Vec3 vertices;
    /// tex coords (2F)
    Tex2F texCoords;
};

/** @struct V3F_C4F
 * A Vec3 with a vertex point, a color.
 */
struct AX_DLL V3F_C4F
{
    /// vertices (3F)
    Vec3 vertices;
    /// vertices (4F)
    Color4F colors;
};

struct V3F_C4B
{
    Vec3 vertices;
    Color4B colors;
};

struct V3F_T2F_C4F
{
    Vec3 position;
    Vec2 uv;
    Vec4 color;
};

struct V3F_T2F_N3F
{
    Vec3 position;
    Tex2F texcoord;
    Vec3 normal;
};

/** @struct V2F_C4B_T2F_Triangle
 * A Triangle of V2F_C4B_T2F.
 */
struct AX_DLL V2F_C4B_T2F_Triangle
{
    V2F_C4B_T2F a;
    V2F_C4B_T2F b;
    V2F_C4B_T2F c;
};

/** @struct V2F_C4B_T2F_Quad
 * A Quad of V2F_C4B_T2F.
 */
struct AX_DLL V2F_C4B_T2F_Quad
{
    /// bottom left
    V2F_C4B_T2F bl;
    /// bottom right
    V2F_C4B_T2F br;
    /// top left
    V2F_C4B_T2F tl;
    /// top right
    V2F_C4B_T2F tr;
};

/** @struct V3F_C4B_T2F_Quad
 * 4 Vertex3FTex2FColor4B.
 */
struct AX_DLL V3F_C4B_T2F_Quad
{
    /// top left
    V3F_C4B_T2F tl;
    /// bottom left
    V3F_C4B_T2F bl;
    /// top right
    V3F_C4B_T2F tr;
    /// bottom right
    V3F_C4B_T2F br;
};

/** @struct V2F_C4F_T2F_Quad
 * 4 Vertex2FTex2FColor4F Quad.
 */
struct AX_DLL V2F_C4F_T2F_Quad
{
    /// bottom left
    V2F_C4F_T2F bl;
    /// bottom right
    V2F_C4F_T2F br;
    /// top left
    V2F_C4F_T2F tl;
    /// top right
    V2F_C4F_T2F tr;
};

/** @struct V3F_T2F_Quad
 *
 */
struct AX_DLL V3F_T2F_Quad
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

namespace backend
{
enum class BlendFactor : uint32_t;
}

/** @struct BlendFunc
 * Blend Function used for textures.
 */
struct AX_DLL BlendFunc
{
    /** source blend function */
    backend::BlendFactor src;
    /** destination blend function */
    backend::BlendFactor dst;

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
    Color3B _strokeColor = Color3B::BLACK;
    /// stroke size
    float _strokeSize = 0.f;
    /// true if stroke enabled
    bool _strokeEnabled = false;
    /// stroke alpha
    uint8_t _strokeAlpha = 255;
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
    Color3B _fontFillColor = Color3B::WHITE;
    /// font alpha
    uint8_t _fontAlpha = 255;
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
using ScissorRect = RectI;  // both GL & Metal is integer type, GL: int, Metal: NSUInteger

using TextureUsage = backend::TextureUsage;
using PixelFormat  = backend::PixelFormat;

using TargetBufferFlags = backend::TargetBufferFlags;
using DepthStencilFlags = backend::DepthStencilFlags;
using ClearFlag         = backend::ClearFlag;

typedef void (*AsyncOperation)(void* param);

NS_AX_END
// end group
/// @}
