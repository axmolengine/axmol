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
        RENDERTARGET       = 1 << 3,
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

}
