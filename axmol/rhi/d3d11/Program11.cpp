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
ProgramImpl::ProgramImpl(Data& vsData, Data& fsData) : Program(vsData, fsData)
{
    auto driver = DriverBase::getInstance();
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

ID3DBlob* ProgramImpl::getVSBlob() const
{
    return static_cast<ShaderModuleImpl*>(_vsModule)->getShaderBlob();
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
        default:
            break;
        }
    }
}

}  // namespace ax::rhi::d3d11
