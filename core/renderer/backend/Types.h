/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2020 C4games Ltd.

 https://axmolengine.github.io/

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

#include "Enums.h"

using namespace std::string_view_literals;

NS_AX_BACKEND_BEGIN

inline TargetBufferFlags getMRTColorFlag(size_t index) noexcept
{
    assert(index < 4);
    return TargetBufferFlags(1u << index);
}

typedef TargetBufferFlags ClearFlag;
typedef TargetBufferFlags RenderTargetFlag;

struct SamplerDescriptor
{
    SamplerFilter magFilter         = SamplerFilter::LINEAR;
    SamplerFilter minFilter         = SamplerFilter::LINEAR;
    SamplerAddressMode sAddressMode = SamplerAddressMode::CLAMP_TO_EDGE;
    SamplerAddressMode tAddressMode = SamplerAddressMode::CLAMP_TO_EDGE;

    SamplerDescriptor() {}

    SamplerDescriptor(SamplerFilter _magFilter,
                      SamplerFilter _minFilter,
                      SamplerAddressMode _sAddressMode,
                      SamplerAddressMode _tAddressMode)
        : magFilter(_magFilter), minFilter(_minFilter), sAddressMode(_sAddressMode), tAddressMode(_tAddressMode)
    {}
};

struct UniformInfo
{
    int count    = 0;
    int location = -1;

    // in opengl, type means uniform data type, i.e. GL_FLOAT_VEC2, while in metal type means data basic type, i.e.
    // float
    unsigned int type         = 0;
    bool isArray              = false;
    unsigned int size         = 0;
    unsigned int bufferOffset = 0;

    // only used in metal
    bool isMatrix    = false;
    bool needConvert = false;
};

struct UniformLocation
{
    /**
     * in metal, those two locations represent to vertex and fragment location.
     * in opengl, location[0] represent the location, and location[1] represent location offset in uniform buffer.
     */
    int location[2]         = {-1, -1};
    ShaderStage shaderStage = ShaderStage::VERTEX;
    UniformLocation()       = default;
    operator bool()
    {
        if (shaderStage == ShaderStage::VERTEX_AND_FRAGMENT)
            return location[0] >= 0 && location[1] >= 0;
        else
            return location[int(shaderStage)] >= 0;
    }
    void reset() { location[0] = location[1] = -1; }
    bool operator==(const UniformLocation& other) const;
    std::size_t operator()(const UniformLocation& uniform) const;
};

struct AttributeBindInfo
{
    std::string attributeName;
    int location = -1;
    int size     = 0;
    int type     = 0;
};

/// built-in uniform name
static constexpr auto UNIFORM_NAME_MVP_MATRIX   = "u_MVPMatrix"sv;
static constexpr auto UNIFORM_NAME_TEXTURE      = "u_tex0"sv;
static constexpr auto UNIFORM_NAME_TEXTURE1     = "u_tex1"sv;
static constexpr auto UNIFORM_NAME_TEXTURE2     = "u_tex2"sv;
static constexpr auto UNIFORM_NAME_TEXTURE3     = "u_tex3"sv;
static constexpr auto UNIFORM_NAME_TEXT_COLOR   = "u_textColor"sv;
static constexpr auto UNIFORM_NAME_EFFECT_COLOR = "u_effectColor"sv;
static constexpr auto UNIFORM_NAME_EFFECT_TYPE  = "u_effectType"sv;

/// built-in attribute name
static constexpr auto ATTRIBUTE_NAME_POSITION  = "a_position"sv;
static constexpr auto ATTRIBUTE_NAME_COLOR     = "a_color"sv;
static constexpr auto ATTRIBUTE_NAME_TEXCOORD  = "a_texCoord"sv;
static constexpr auto ATTRIBUTE_NAME_TEXCOORD1 = "a_texCoord1"sv;
static constexpr auto ATTRIBUTE_NAME_TEXCOORD2 = "a_texCoord2"sv;
static constexpr auto ATTRIBUTE_NAME_TEXCOORD3 = "a_texCoord3"sv;
static constexpr auto ATTRIBUTE_NAME_NORMAL    = "a_normal"sv;

/**
 * @brief a structor to store blend descriptor
 */
struct BlendDescriptor
{
    ColorWriteMask writeMask = ColorWriteMask::ALL;

    bool blendEnabled = false;

    BlendOperation rgbBlendOperation   = BlendOperation::ADD;
    BlendOperation alphaBlendOperation = BlendOperation::ADD;

    BlendFactor sourceRGBBlendFactor        = BlendFactor::ONE;
    BlendFactor destinationRGBBlendFactor   = BlendFactor::ZERO;
    BlendFactor sourceAlphaBlendFactor      = BlendFactor::ONE;
    BlendFactor destinationAlphaBlendFactor = BlendFactor::ZERO;
};

NS_AX_BACKEND_END
