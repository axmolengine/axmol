#pragma once

#include "axmol/rhi/RHITypes.h"

namespace ax
{

// Numeric values are part of the legacy Bundle3D vertex-usage format.
enum class MeshVertexAttribute : int
{
    POSITION     = 0,
    COLOR        = 1,
    TEXCOORD0    = 2,
    TEXCOORD1    = 3,
    TEXCOORD2    = 4,
    TEXCOORD3    = 5,
    NORMAL       = 6,
    BLENDWEIGHT  = 7,
    BLENDINDICES = 8,
    TANGENT      = 9,
    BINORMAL     = 10,
    COUNT        = 11,
    INVALID      = -1,
};

inline const rhi::VertexSemantic& toVertexSemantic(MeshVertexAttribute attribute)
{
    switch (attribute)
    {
    case MeshVertexAttribute::POSITION:
        return rhi::VertexSemantic::POSITION;
    case MeshVertexAttribute::COLOR:
        return rhi::VertexSemantic::COLOR0;
    case MeshVertexAttribute::TEXCOORD0:
        return rhi::VertexSemantic::TEXCOORD0;
    case MeshVertexAttribute::TEXCOORD1:
        return rhi::VertexSemantic::TEXCOORD1;
    case MeshVertexAttribute::TEXCOORD2:
        return rhi::VertexSemantic::TEXCOORD2;
    case MeshVertexAttribute::TEXCOORD3:
        return rhi::VertexSemantic::TEXCOORD3;
    case MeshVertexAttribute::NORMAL:
        return rhi::VertexSemantic::NORMAL;
    case MeshVertexAttribute::BLENDWEIGHT:
        return rhi::VertexSemantic::BLENDWEIGHT;
    case MeshVertexAttribute::BLENDINDICES:
        return rhi::VertexSemantic::BLENDINDICES;
    case MeshVertexAttribute::TANGENT:
        return rhi::VertexSemantic::TANGENT;
    case MeshVertexAttribute::BINORMAL:
        return rhi::VertexSemantic::BINORMAL;
    default:
        assert(false && "invalid mesh vertex attribute");
        return rhi::VertexSemantic::POSITION;
    }
}

}  // namespace ax
