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

#include "axmol/rhi/VertexLayout.h"
#include "axmol/base/Macros.h"
#include "axmol/base/Logging.h"
#include <assert.h>
#include <algorithm>
#include <string.h>
#include "xxhash/xxhash.h"
#include "axmol/tlx/utility.hpp"

namespace ax::rhi
{

static constexpr size_t s_vertexFormatSizeMap[] = {
    16,  // FLOAT4
    12,  // FLOAT3
    8,   // FLOAT2
    4,   // FLOAT
    16,  // INT4
    12,  // INT3
    8,   // INT2
    4,   // INT
    8,   // USHORT4
    4,   // USHORT2
    4,   // UBYTE4
    64,  // MAT4 (4x4)
};

static_assert(AX_ARRAYSIZE(s_vertexFormatSizeMap) == (int)VertexElementType::COUNT,
              "The vertex format size table is incomplete!");

const VertexSemantic VertexSemantic::POSITION{"POSITION", 0};
const VertexSemantic VertexSemantic::NORMAL{"NORMAL", 0};
const VertexSemantic VertexSemantic::TEXCOORD0{"TEXCOORD", 0};
const VertexSemantic VertexSemantic::TEXCOORD1{"TEXCOORD", 1};
const VertexSemantic VertexSemantic::TEXCOORD2{"TEXCOORD", 2};
const VertexSemantic VertexSemantic::TEXCOORD3{"TEXCOORD", 3};
const VertexSemantic VertexSemantic::TEXCOORD4{"TEXCOORD", 4};
const VertexSemantic VertexSemantic::TEXCOORD5{"TEXCOORD", 5};
const VertexSemantic VertexSemantic::TEXCOORD6{"TEXCOORD", 6};
const VertexSemantic VertexSemantic::TEXCOORD7{"TEXCOORD", 7};
const VertexSemantic VertexSemantic::COLOR0{"COLOR", 0};
const VertexSemantic VertexSemantic::COLOR1{"COLOR", 1};
const VertexSemantic VertexSemantic::TANGENT{"TANGENT", 0};
const VertexSemantic VertexSemantic::BINORMAL{"BINORMAL", 0};
const VertexSemantic VertexSemantic::BLENDINDICES{"BLENDINDICES", 0};
const VertexSemantic VertexSemantic::BLENDWEIGHT{"BLENDWEIGHT", 0};

VertexSemantic::VertexSemantic(std::string_view semanticName, uint16_t semanticIndex) : index(semanticIndex)
{
    nameLen = static_cast<uint16_t>(tlx::strlcpy(name, semanticName));  // ensure null-terminated

#if _AX_DEBUG >= 1
    auto namesv = getName();
    if (std::find_if(namesv.begin(), namesv.end(), [](char c) { return c >= 'a' && c <= 'z'; }) != namesv.end())
        AXLOGW("VertexSemantic '{}' contains lowercase; expect uppercase (e.g. POSITION)", semanticName);
#endif
}

std::string VertexSemantic::toString() const
{
    return fmt::format("{}{}", getName(), index);
}

/* InputBindingDesc */
InputBindingDesc::InputBindingDesc(std::string_view _semantic,
                                   int _index,
                                   VertexElementType _format,
                                   unsigned int _offset,
                                   bool needToBeNormallized,
                                   uint8_t instanceStepRate)
    : format(_format)
    , offset(_offset)
    , index(_index)
    , needToBeNormallized(needToBeNormallized)
    , instanceStepRate(instanceStepRate)
{
    auto len = std::min(_semantic.size(), sizeof(semantic) - 1);
    memcpy(semantic, _semantic.data(), len);
}

void VertexLayoutDesc::clear()
{
    _bindings.clear();
    _strides[0] = 0;
    _strides[1] = 0;
    _hash       = 0;
}

/* VertexLayoutDesc */
void VertexLayoutDesc::startLayout(size_t capacity)
{
    clear();
    _bindings.reserve(capacity);
}

void VertexLayoutDesc::endLayout(int stride)
{
    if (stride != -1)
        _strides[0] = stride;

    _hash = XXH32(_bindings.data(), _bindings.size() * sizeof(InputBindingDesc), _strides[0]);
}

void VertexLayoutDesc::addAttrib(const VertexInputDesc* desc,
                                 VertexElementType format,
                                 size_t offset,
                                 bool needToBeNormallized,
                                 uint8_t instanceStepRate)
{
    if (_hash) [[unlikely]]
    {
        assert(false && "VertexLayoutDesc is inmutable");
        AXLOGE("The vertex layout has been ended, can not add another attribute");
        return;
    }

    if (!desc) [[unlikely]]
    {
        AXLOGW("The vertex input vfmt={} does not exist, unused/optimized?", static_cast<int>(format));
        return;
    }

    if (format >= VertexElementType::COUNT) [[unlikely]]
    {
        AXLOGE("The vertex input '{}' has invalid format: {}", desc->semantic.toString(), static_cast<int>(format));
        return;
    }

    // auto reflect vertex size in bytes to increaset strides
    auto sizeInBytes = s_vertexFormatSizeMap[static_cast<int>(format)];
    if (instanceStepRate == 0)
        _strides[0] += static_cast<uint32_t>(sizeInBytes);
    else
        _strides[1] += static_cast<uint32_t>(sizeInBytes);

    _bindings.emplace_back(desc->semantic.name, desc->location, format, static_cast<unsigned int>(offset),
                           needToBeNormallized, instanceStepRate);
}

}  // namespace ax::rhi
