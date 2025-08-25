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
#include <cassert>
#include "xxhash/xxhash.h"

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

static_assert(AX_ARRAYSIZE(s_vertexFormatSizeMap) == (int)VertexFormat::COUNT,
              "The vertex format size table is incomplete!");

/* InputBindingDesc */
InputBindingDesc::InputBindingDesc(std::string_view _semantic,
                                   int _index,
                                   VertexFormat _format,
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
    // compute hash
    _hash = XXH32(_bindings.data(), _bindings.size() * sizeof(InputBindingDesc), 0);

    if (stride != -1)
        _strides[0] = stride;
}

void VertexLayoutDesc::addAttrib(std::string_view name,
                                 const VertexInputDesc* desc,
                                 VertexFormat format,
                                 std::size_t offset,
                                 bool needToBeNormallized,
                                 uint8_t instanceStepRate)
{
    if (_hash) [[unlikely]]
    {
        assert(false && "VertexLayoutDesc is inmutable");
        AXLOGE("The vertex layout has been ended, can not add attribute '{}'", name);
        return;
    }

    if (!desc) [[unlikely]]
    {
        AXLOGW("The vertex input '{}' vfmt={} not exist, unused/optimized?", name, static_cast<int>(format));
        return;
    }

    if (format >= VertexFormat::COUNT) [[unlikely]]
    {
        AXLOGE("The vertex input '{}' invalid format: {}", name, static_cast<int>(format));
        return;
    }

    // auto reflect vertex size in bytes to increaset strides
    auto sizeInBytes = s_vertexFormatSizeMap[static_cast<int>(format)];
    if (instanceStepRate == 0)
        _strides[0] += static_cast<uint32_t>(sizeInBytes);
    else
        _strides[1] += static_cast<uint32_t>(sizeInBytes);

    _bindings.emplace_back(desc->semantic, desc->location, format, offset, needToBeNormallized, instanceStepRate);
}

}  // namespace ax::rhi
