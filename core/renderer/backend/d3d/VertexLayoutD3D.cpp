#include "VertexLayoutD3D.h"
#include "ProgramD3D.h"
#include "DriverD3D.h"

namespace ax::backend::d3d
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

VertexLayoutImpl::~VertexLayoutImpl()
{
    SafeRelease(_inputLayout);
}

void VertexLayoutImpl::apply(ID3D11DeviceContext* context, Program* program) const
{
    if (!_inputLayout)
    {
        auto progImpl = static_cast<ProgramImpl*>(program);
        auto device   = static_cast<DriverImpl*>(DriverBase::getInstance())->getDevice();

        axstd::pod_vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
        inputElements.reserve(_inputs.size());

        for (auto& [_, inputDesc] : _inputs)
        {
            if (inputDesc.format != VertexFormat::MAT4)
            {
                D3D11_INPUT_ELEMENT_DESC desc{};

                desc.SemanticName          = inputDesc.name.c_str();  // attributeInfo.semanticName.c_str();
                desc.SemanticIndex         = inputDesc.index;         // attributeInfo.semanticIndex;
                desc.Format                = toDXGIFormat(inputDesc.format, inputDesc.needToBeNormallized);
                desc.InputSlot             = 0,  // attributeInfo.inputSlot;
                    desc.AlignedByteOffset = inputDesc.offset;
                // attributeInfo.perInstance ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA
                desc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
                desc.InstanceDataStepRate = 0;  // attributeInfo.perInstance ? 1 : 0;

                inputElements.push_back(desc);
            }
            else {
                for (UINT i = 0; i < 4; ++i)
                {
                    D3D11_INPUT_ELEMENT_DESC desc{};
                    desc.SemanticName         = inputDesc.name.c_str();
                    desc.SemanticIndex        = inputDesc.index + i;
                    desc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
                    desc.InputSlot            = 0;
                    desc.AlignedByteOffset    = inputDesc.offset + sizeof(float) * 4 * i;
                    desc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
                    desc.InstanceDataStepRate = 0;

                    inputElements.push_back(desc);
                }
            }
        }

        ID3DBlob* vsBlob = progImpl->getVSBlob();
        HRESULT hr = device->CreateInputLayout(inputElements.data(), static_cast<UINT>(inputElements.size()),
                                               vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &_inputLayout);
    }

    if (_inputLayout)
    {
        context->IASetInputLayout(_inputLayout);
    }
    else
    {
        AXLOGE("Create input layout fail");
    }
}

VertexLayoutImpl* VertexLayoutImpl::clone()
{
    auto vl = new VertexLayoutImpl(*this);
    if (vl->_inputLayout)
        vl->_inputLayout->AddRef();
    return vl;
}

}  // namespace ax::backend::d3d
