#include "2d/SpriteSheetLoader.h"
#include "base/Director.h"
#include <vector>

using namespace std;

namespace ax
{

void SpriteSheetLoader::initializePolygonInfo(const Vec2& textureSize,
                                              const Vec2& spriteSize,
                                              const std::vector<int>& vertices,
                                              const std::vector<int>& verticesUV,
                                              const std::vector<int>& triangleIndices,
                                              PolygonInfo& info)
{
    const auto vertexCount = vertices.size();
    const auto indexCount  = triangleIndices.size();

    const auto scaleFactor = AX_CONTENT_SCALE_FACTOR();

    auto* vertexData = new V3F_C4B_T2F[vertexCount];
    for (size_t i = 0; i < vertexCount / 2; i++)
    {
        vertexData[i].colors = Color4B::WHITE;
        vertexData[i].vertices =
            Vec3(vertices[i * 2] / scaleFactor, (spriteSize.height - vertices[i * 2 + 1]) / scaleFactor, 0);
        vertexData[i].texCoords =
            Tex2F(verticesUV[i * 2] / textureSize.width, verticesUV[i * 2 + 1] / textureSize.height);
    }

    auto* indexData = new unsigned short[indexCount];
    for (size_t i = 0; i < indexCount; i++)
    {
        indexData[i] = static_cast<unsigned short>(triangleIndices[i]);
    }

    info.triangles.vertCount  = static_cast<int>(vertexCount);
    info.triangles.verts      = vertexData;
    info.triangles.indexCount = static_cast<int>(indexCount);
    info.triangles.indices    = indexData;
    info.setRect(Rect(0, 0, spriteSize.width, spriteSize.height));
}

}
