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

#include "axmol/rhi/metal/ProgramMTL.h"
#include "axmol/rhi/metal/ShaderModuleMTL.h"
#include "axmol/base/Macros.h"
#include "axmol/rhi/metal/DriverMTL.h"

namespace ax::rhi::mtl
{
namespace
{
// constexpr std::string_view metalSpecificDefine = "#define METAL\n"sv;
}

ProgramImpl::ProgramImpl(std::string_view vertexShader, std::string_view fragmentShader)
    : Program(vertexShader, fragmentShader)
{
    _vertexShader = static_cast<ShaderModuleImpl*>(ShaderCache::getInstance()->acquireVertexShaderModule(_vsSource));
    _fragmentShader =
        static_cast<ShaderModuleImpl*>(ShaderCache::getInstance()->acquireFragmentShaderModule(_fsSource));
}

ProgramImpl::~ProgramImpl()
{
    AX_SAFE_RELEASE(_vertexShader);
    AX_SAFE_RELEASE(_fragmentShader);
}

const VertexInputDesc* ProgramImpl::getVertexInputDesc(VertexInputKind name) const
{
    return _vertexShader->getVertexInputDesc(name);
}

const VertexInputDesc* ProgramImpl::getVertexInputDesc(std::string_view name) const
{
    return _vertexShader->getVertexInputDesc(name);
}

UniformLocation ProgramImpl::getUniformLocation(rhi::Uniform name) const
{
    auto& vert = _vertexShader->getUniformInfo(name);
    auto& frag = _fragmentShader->getUniformInfo(name);

    return UniformLocation{{frag.location, frag.location == -1 ? -1 : static_cast<int>(frag.bufferOffset)},
                           {vert.location, vert.location == -1 ? -1 : static_cast<int>(vert.bufferOffset)}};
}

UniformLocation ProgramImpl::getUniformLocation(std::string_view uniform) const
{
    auto& vert = _vertexShader->getUniformInfo(uniform);
    auto& frag = _fragmentShader->getUniformInfo(uniform);

    if (vert.location != -1 && frag.location != -1)
        AXASSERT(vert.type == frag.type && vert.count == frag.count && vert.size == frag.size,
                 "Same vertex and fragment uniform must much in type and size");

    return UniformLocation{{frag.location, frag.location == -1 ? -1 : static_cast<int>(frag.bufferOffset)},
                           {vert.location, vert.location == -1 ? -1 : static_cast<int>(vert.bufferOffset)}};
}

int ProgramImpl::getMaxVertexLocation() const
{
    return _vertexShader->getMaxLocation();
}

int ProgramImpl::getMaxFragmentLocation() const
{
    return _fragmentShader->getMaxLocation();
}

const hlookup::string_map<VertexInputDesc>& ProgramImpl::getActiveVertexInputs() const
{
    return _vertexShader->getActiveVertexInputs();
}

std::size_t ProgramImpl::getUniformBufferSize(ShaderStage stage) const
{
    switch (stage)
    {
    case ShaderStage::VERTEX:
        return _vertexShader->getUniformBufferSize();
    case ShaderStage::FRAGMENT:
        return _fragmentShader->getUniformBufferSize();
    default:
        AXASSERT(false, "Invalid shader stage.");
        break;
    }
    return 0;
}

const hlookup::string_map<UniformInfo>& ProgramImpl::getActiveUniformInfos(ShaderStage stage) const
{
    switch (stage)
    {
    case ShaderStage::VERTEX:
        return _vertexShader->getActiveUniformInfos();
    case ShaderStage::FRAGMENT:
        return _fragmentShader->getActiveUniformInfos();
    default:
        AXASSERT(false, "Invalid shader stage.");
    }
}

}  // namespace ax::rhi::mtl
