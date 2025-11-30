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
#include "axmol/rhi/d3d11/VertexLayout11.h"
#include "axmol/rhi/d3d11/Program11.h"
#include "axmol/rhi/d3d11/Driver11.h"
#include "axmol/rhi/d3d11/RenderContext11.h"

namespace ax::rhi::d3d11
{

static DXGI_FORMAT toDXGIFormat(VertexFormat format, bool unorm)
{
    switch (format)
    {
    case VertexFormat::FLOAT4:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case VertexFormat::FLOAT3:
        return DXGI_FORMAT_R32G32B32_FLOAT;
    case VertexFormat::FLOAT2:
        return DXGI_FORMAT_R32G32_FLOAT;
    case VertexFormat::FLOAT:
        return DXGI_FORMAT_R32_FLOAT;

    case VertexFormat::INT4:
        return DXGI_FORMAT_R32G32B32A32_SINT;
    case VertexFormat::INT3:
        return DXGI_FORMAT_R32G32B32_SINT;
    case VertexFormat::INT2:
        return DXGI_FORMAT_R32G32_SINT;
    case VertexFormat::INT:
        return DXGI_FORMAT_R32_SINT;

    case VertexFormat::USHORT4:
        return unorm ? DXGI_FORMAT_R16G16B16A16_UNORM : DXGI_FORMAT_R16G16B16A16_UINT;
    case VertexFormat::USHORT2:
        return unorm ? DXGI_FORMAT_R16G16_UNORM : DXGI_FORMAT_R16G16_UINT;

    case VertexFormat::UBYTE4:
        return unorm ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UINT;

    case VertexFormat::MAT4:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;

    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

VertexLayoutImpl::VertexLayoutImpl(VertexLayoutDesc&& desc) : VertexLayout(std::move(desc)) {}

VertexLayoutImpl::~VertexLayoutImpl()
{
    SafeRelease(_d3dVL);
}

void VertexLayoutImpl::apply(ID3D11DeviceContext* context, Program* program) const
{
    if (!_d3dVL)
    {
        auto progImpl = static_cast<ProgramImpl*>(program);
        auto device   = static_cast<DriverImpl*>(DriverBase::getInstance())->getDevice();

        tlx::pod_vector<D3D11_INPUT_ELEMENT_DESC> inputElements;

        auto& bindings = getBindings();
        inputElements.reserve(bindings.size());

        auto appendElement = [&inputElements](const InputBindingDesc& inputDesc) {
            const auto inputSlot = inputDesc.instanceStepRate ? RenderContextImpl::VI_INSTANCING_BINDING_INDEX
                                                              : RenderContextImpl::VI_BINDING_INDEX;
            const auto inputSlotClass =
                inputDesc.instanceStepRate ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
            if (inputDesc.format != VertexFormat::MAT4)
            {
                D3D11_INPUT_ELEMENT_DESC desc{};

                desc.SemanticName         = inputDesc.semantic;
                desc.SemanticIndex        = inputDesc.index;
                desc.Format               = toDXGIFormat(inputDesc.format, inputDesc.needToBeNormallized);
                desc.InputSlot            = inputSlot;
                desc.AlignedByteOffset    = inputDesc.offset;
                desc.InputSlotClass       = inputSlotClass;
                desc.InstanceDataStepRate = inputDesc.instanceStepRate;

                inputElements.push_back(desc);
            }
            else
            {
                for (UINT i = 0; i < 4; ++i)
                {
                    D3D11_INPUT_ELEMENT_DESC desc{};
                    desc.SemanticName         = inputDesc.semantic;
                    desc.SemanticIndex        = inputDesc.index + i;
                    desc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
                    desc.InputSlot            = inputSlot;
                    desc.AlignedByteOffset    = inputDesc.offset + sizeof(float) * 4 * i;
                    desc.InputSlotClass       = inputSlotClass;
                    desc.InstanceDataStepRate = inputDesc.instanceStepRate;

                    inputElements.push_back(desc);
                }
            }
        };

        for (auto& inputDesc : bindings)
            appendElement(inputDesc);

        ID3DBlob* vsBlob = progImpl->getVSBlob();
        HRESULT hr       = device->CreateInputLayout(inputElements.data(), static_cast<UINT>(inputElements.size()),
                                                     vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &_d3dVL);

        if (!_d3dVL)
        {
            AXLOGE("Create input layout fail");
            return;
        }
    }

    context->IASetInputLayout(_d3dVL);
}

}  // namespace ax::rhi::d3d11
