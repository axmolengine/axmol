#include "axmol/3d/Bundle3DData.h"

namespace ax
{

int MeshVertexAttrib::getAttribSizeBytes() const
{
    int ret = 0;
    switch (type)
    {
    case rhi::VertexFormat::FLOAT4:
    case rhi::VertexFormat::INT4:
        return 16;
    case rhi::VertexFormat::FLOAT3:
    case rhi::VertexFormat::INT3:
        return 12;
    case rhi::VertexFormat::FLOAT2:
    case rhi::VertexFormat::INT2:
    case rhi::VertexFormat::USHORT4:
        return 8;
    case rhi::VertexFormat::FLOAT:
    case rhi::VertexFormat::INT:
    case rhi::VertexFormat::UBYTE4:
    case rhi::VertexFormat::USHORT2:
        return 4;
    default:
        AXASSERT(false, "VertexFormat convert to size error");
    }
    return ret;
}

}  // namespace ax
