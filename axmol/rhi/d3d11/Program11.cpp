/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/rhi/d3d11/Program11.h"
#include "axmol/rhi/d3d11/Buffer11.h"
#include "axmol/rhi/GraphicsCore.h"
#include "axmol/rhi/ShaderCache.h"

namespace ax::rhi::d3d11
{
ProgramImpl::ProgramImpl(Data& vsData, Data& fsData) : Program(vsData, fsData)
{
    auto driver = axdrv;
    for (auto& uboInfo : _activeUniformBlockInfos)
    {
        _uniformBuffers.push_back(driver->createBuffer(uboInfo.sizeBytes, BufferType::UNIFORM, BufferUsage::DYNAMIC));
    }
}

ProgramImpl::ProgramImpl(Data& csData) : Program(csData)
{
    auto driver = axdrv;
    for (auto& uboInfo : _activeUniformBlockInfos)
    {
        _uniformBuffers.push_back(driver->createBuffer(uboInfo.sizeBytes, BufferType::UNIFORM, BufferUsage::DYNAMIC));
    }
}

ProgramImpl ::~ProgramImpl()
{
    if (_uniformBuffers.empty())
        return;
    for (auto ubo : _uniformBuffers)
        delete ubo;
    _uniformBuffers.clear();
}

std::span<uint8_t> ProgramImpl::getVSBlob() const
{
    return static_cast<ShaderModuleImpl*>(_vsModule)->getBlob();
}

void ProgramImpl::bindUniformBuffers(ID3D11DeviceContext* context, const uint8_t* buffer, size_t bufferSize)
{
    const auto uboCount = _activeUniformBlockInfos.size();
    for (size_t i = 0; i < uboCount; ++i)
    {
        auto& info = _activeUniformBlockInfos[i];
        auto ubo   = static_cast<BufferImpl*>(_uniformBuffers[i]);
        ubo->updateData(buffer + info.cpuOffset, info.sizeBytes);
        ID3D11Buffer* nativeUbo = static_cast<ID3D11Buffer*>(ubo->internalHandle());
        switch (info.stage)
        {
        case ShaderStage::VERTEX:
            context->VSSetConstantBuffers(info.binding, 1, &nativeUbo);
            break;
        case ShaderStage::FRAGMENT:
            context->PSSetConstantBuffers(info.binding, 1, &nativeUbo);
            break;
        case ShaderStage::COMPUTE:
            context->CSSetConstantBuffers(info.binding, 1, &nativeUbo);
            break;
        default:
            break;
        }
    }
}

}  // namespace ax::rhi::d3d11
