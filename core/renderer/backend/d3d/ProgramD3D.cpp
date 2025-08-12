#include "ProgramD3D.h"
#include "BufferD3D.h"
#include "../ShaderCache.h"

namespace ax::backend::d3d
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

const hlookup::string_map<VertexInputDesc>& ProgramImpl::getActiveVertexInputs() const
{
    return _vertexShader->getActiveVertexInputs();
}

UniformLocation ProgramImpl::getUniformLocation(backend::Uniform name) const
{
    auto& vert = _vertexShader->getUniformInfo(name);
    auto& frag = _fragmentShader->getUniformInfo(name);

    return UniformLocation{{vert.location, vert.location == -1 ? -1 : static_cast<int>(vert.bufferOffset)},
                           {frag.location, frag.location == -1 ? -1 : static_cast<int>(frag.bufferOffset)}};
}

UniformLocation ProgramImpl::getUniformLocation(std::string_view uniform) const
{
    auto& vert = _vertexShader->getUniformInfo(uniform);
    auto& frag = _fragmentShader->getUniformInfo(uniform);

    if (vert.location != -1 && frag.location != -1)
        AXASSERT(vert.type == frag.type && vert.count == frag.count && vert.size == frag.size,
                 "Same vertex and fragment uniform must much in type and size");

    return UniformLocation{{vert.location, vert.location == -1 ? -1 : static_cast<int>(vert.bufferOffset)},
                           {frag.location, frag.location == -1 ? -1 : static_cast<int>(frag.bufferOffset)}};
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

const hlookup::string_map<UniformInfo>& ProgramImpl::getActiveUniformInfos(ShaderStage stage) const
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
}  // namespace ax::backend::d3d
