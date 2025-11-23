/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors

 https://axmol.dev/
 ****************************************************************************/
#include "axmol/rhi/d3d12/VertexLayout12.h"
#include "axmol/base/Logging.h"

namespace ax::rhi::d3d12
{

// Map VertexFormat to DXGI_FORMAT
static DXGI_FORMAT toDxgiFormat(VertexFormat format, bool unorm)
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

VertexLayoutImpl::VertexLayoutImpl(VertexLayoutDesc&& desc) : VertexLayout(std::move(desc))
{
    auto& bindingsDesc = getBindings();
    _elements.clear();

    for (auto& inputDesc : bindingsDesc)
    {
        if (inputDesc.format != VertexFormat::MAT4)
        {
            D3D12_INPUT_ELEMENT_DESC elem{};
            elem.SemanticName         = "TEXCOORD";  // or POSITION/NORMAL, depends on higher-level mapping
            elem.SemanticIndex        = inputDesc.index;
            elem.Format               = toDxgiFormat(inputDesc.format, inputDesc.needToBeNormallized);
            elem.InputSlot            = inputDesc.instanceStepRate ? 1 : 0;
            elem.AlignedByteOffset    = inputDesc.offset;
            elem.InputSlotClass       = inputDesc.instanceStepRate ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA
                                                                   : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            elem.InstanceDataStepRate = inputDesc.instanceStepRate;
            _elements.push_back(elem);
        }
        else
        {
            // MAT4 expands into 4 vec4 attributes
            for (uint32_t i = 0; i < 4; ++i)
            {
                D3D12_INPUT_ELEMENT_DESC elem{};
                elem.SemanticName         = "TEXCOORD";  // semantic naming can be customized
                elem.SemanticIndex        = inputDesc.index + i;
                elem.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
                elem.InputSlot            = inputDesc.instanceStepRate ? 1 : 0;
                elem.AlignedByteOffset    = inputDesc.offset + sizeof(float) * 4 * i;
                elem.InputSlotClass       = inputDesc.instanceStepRate ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA
                                                                       : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                elem.InstanceDataStepRate = inputDesc.instanceStepRate;
                _elements.push_back(elem);
            }
        }
    }

    _inputLayout.NumElements        = static_cast<UINT>(_elements.size());
    _inputLayout.pInputElementDescs = _elements.data();
}

}  // namespace ax::rhi::d3d12
