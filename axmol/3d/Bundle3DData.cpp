#include "axmol/3d/Bundle3DData.h"

namespace ax
{

int MeshVertexAttrib::getAttribSizeBytes() const
{
    int ret = 0;
    switch (type)
    {
    case rhi::VertexElementType::FLOAT4:
    case rhi::VertexElementType::INT4:
        return 16;
    case rhi::VertexElementType::FLOAT3:
    case rhi::VertexElementType::INT3:
        return 12;
    case rhi::VertexElementType::FLOAT2:
    case rhi::VertexElementType::INT2:
    case rhi::VertexElementType::USHORT4:
        return 8;
    case rhi::VertexElementType::FLOAT:
    case rhi::VertexElementType::INT:
    case rhi::VertexElementType::UBYTE4:
    case rhi::VertexElementType::USHORT2:
        return 4;
    default:
        AXASSERT(false, "VertexElementType convert to size error");
    }
    return ret;
}

}  // namespace ax
