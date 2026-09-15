/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/renderer/QuadCommand.h"

#include "axmol/renderer/Material.h"
#include "axmol/renderer/Technique.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/renderer/Pass.h"
#include "axmol/renderer/Texture2D.h"
#include "axmol/base/Utils.h"

namespace ax
{

int QuadCommand::__indexCapacity = -1;
uint16_t* QuadCommand::__indices = nullptr;

void QuadCommand::destroyIsolatedIndices()
{
    AX_SAFE_DELETE_ARRAY(__indices);
    __indexCapacity = -1;
}

QuadCommand::QuadCommand() : _indexSize(-1), _ownedIndices() {}

QuadCommand::~QuadCommand()
{
    for (auto&& indices : _ownedIndices)
    {
        AX_SAFE_DELETE_ARRAY(indices);
    }
}

void QuadCommand::reIndex(int indicesCount)
{
    // first time init: create a decent buffer size for indices to prevent too much resizing
    if (__indexCapacity == -1)
    {
        indicesCount = std::max(indicesCount, 2048);
    }

    if (indicesCount > __indexCapacity)
    {
        // if resizing is needed, get needed size plus 25%, but not bigger that max size
        indicesCount *= 1.25;
        indicesCount = std::min(indicesCount, 65536);

        AXLOGD("QuadCommand: resizing index size from [{}] to [{}]", __indexCapacity, indicesCount);

        _ownedIndices.emplace_back(__indices);
        __indices       = new uint16_t[indicesCount];
        __indexCapacity = indicesCount;
    }

    for (int i = 0; i < __indexCapacity / 6; i++)
    {
        __indices[i * 6 + 0] = (uint16_t)(i * 4 + 0);
        __indices[i * 6 + 1] = (uint16_t)(i * 4 + 1);
        __indices[i * 6 + 2] = (uint16_t)(i * 4 + 2);
        __indices[i * 6 + 3] = (uint16_t)(i * 4 + 3);
        __indices[i * 6 + 4] = (uint16_t)(i * 4 + 2);
        __indices[i * 6 + 5] = (uint16_t)(i * 4 + 1);
    }

    _indexSize = indicesCount;
}

void QuadCommand::init(float globalOrder,
                       Texture2D* texture,
                       const BlendFunc& blendType,
                       V3F_T2F_C4B_Quad* quads,
                       ssize_t quadCount,
                       const Mat4& mv,
                       uint32_t flags,
                       const SceneViewData& view)
{
    if (quadCount * 6 > _indexSize)
        reIndex((int)quadCount * 6);

    Triangles triangles;
    triangles.verts      = &quads->tl;
    triangles.vertCount  = (int)quadCount * 4;
    triangles.indices    = __indices;
    triangles.indexCount = (int)quadCount * 6;
    TrianglesCommand::init(globalOrder, texture, blendType, triangles, mv, flags, view);
}

}  // namespace ax
