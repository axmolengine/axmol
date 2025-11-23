/****************************************************************************
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
#include "axmol/rhi/d3d11/Program11.h"
#include "axmol/rhi/d3d11/Buffer11.h"
#include "axmol/rhi/ShaderCache.h"

namespace ax::rhi::d3d11
{
ProgramImpl::ProgramImpl(std::string_view vertexShader, std::string_view fragmentShader)
    : Program(vertexShader, fragmentShader)
{
    _vertexShader = static_cast<ShaderModuleImpl*>(ShaderCache::getInstance()->acquireVertexShaderModule(_vsSource));
    _fragmentShader =
        static_cast<ShaderModuleImpl*>(ShaderCache::getInstance()->acquireFragmentShaderModule(_fsSource));
}
ProgramImpl ::~ProgramImpl()
{
    AX_SAFE_RELEASE(_vertexShader);
    AX_SAFE_RELEASE(_fragmentShader);

    AX_SAFE_RELEASE(_vsUniformBuffer);
    AX_SAFE_RELEASE(_fsUniformBuffer);
}

const VertexInputDesc* ProgramImpl::getVertexInputDesc(VertexInputKind name) const
{
    return _vertexShader->getVertexInputDesc(name);
}

const VertexInputDesc* ProgramImpl::getVertexInputDesc(std::string_view name) const
{
    return _vertexShader->getVertexInputDesc(name);
}

const axstd::string_map<VertexInputDesc>& ProgramImpl::getActiveVertexInputs() const
{
    return _vertexShader->getActiveVertexInputs();
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

const axstd::string_map<UniformInfo>& ProgramImpl::getActiveUniformInfos(ShaderStage stage) const
{
    return stage == ShaderStage::VERTEX ? _vertexShader->getActiveUniformInfos()
                                        : _fragmentShader->getActiveUniformInfos();
}

ID3DBlob* ProgramImpl::getVSBlob() const
{
    return _vertexShader->getShaderBlob();
}

void ProgramImpl::bindVertexUniformBuffer(ID3D11DeviceContext* context,
                                          const char* data,
                                          size_t size,
                                          UINT bindingIndex)
{
    if (!_vsUniformBuffer)
        _vsUniformBuffer = static_cast<BufferImpl*>(
            DriverBase::getInstance()->createBuffer(size, BufferType::UNIFORM, BufferUsage::DYNAMIC));
    _vsUniformBuffer->updateData(data, size);

    ID3D11Buffer* vsUniformBuffer = static_cast<ID3D11Buffer*>(_vsUniformBuffer->internalHandle());
    context->VSSetConstantBuffers(bindingIndex, 1, &vsUniformBuffer);
}

void ProgramImpl::bindFragmentUniformBuffer(ID3D11DeviceContext* context,
                                            const char* data,
                                            size_t size,
                                            UINT bindingIndex)
{
    if (!_fsUniformBuffer)
        _fsUniformBuffer = static_cast<BufferImpl*>(
            DriverBase::getInstance()->createBuffer(size, BufferType::UNIFORM, BufferUsage::DYNAMIC));
    _fsUniformBuffer->updateData(data, size);

    ID3D11Buffer* psUniformBuffer = static_cast<ID3D11Buffer*>(_fsUniformBuffer->internalHandle());
    context->PSSetConstantBuffers(bindingIndex, 1, &psUniformBuffer);
}
}  // namespace ax::rhi::d3d11
