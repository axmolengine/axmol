#include "3d/Bundle3DData.h"

namespace ax
{

int MeshVertexAttrib::getAttribSizeBytes() const
{
    int ret = 0;
    switch (type)
    {
    case backend::VertexFormat::FLOAT4:
    case backend::VertexFormat::INT4:
        return 16;
    case backend::VertexFormat::FLOAT3:
    case backend::VertexFormat::INT3:
        return 12;
    case backend::VertexFormat::FLOAT2:
    case backend::VertexFormat::INT2:
    case backend::VertexFormat::USHORT4:
        return 8;
    case backend::VertexFormat::FLOAT:
    case backend::VertexFormat::INT:
    case backend::VertexFormat::UBYTE4:
    case backend::VertexFormat::USHORT2:
        return 4;
    default:
        AXASSERT(false, "VertexFormat convert to size error");
    }
    return ret;
}

}