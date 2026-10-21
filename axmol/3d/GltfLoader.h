#pragma once

#include "axmol/3d/Bundle3DData.h"

namespace ax
{
namespace GltfLoader
{
bool load(MeshDatas& meshes, MaterialDatas& materials, NodeDatas& nodes, std::string_view path);
bool loadAnimationData(Animation3DData& animation, std::string_view path, std::string_view animationName = {});
}  // namespace GltfLoader
}  // namespace ax