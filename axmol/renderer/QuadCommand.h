/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include <vector>

#include "axmol/renderer/TrianglesCommand.h"

/**
 * @addtogroup renderer
 * @{
 */

namespace ax
{

/**
 Command used to render one or more Quads, similar to TrianglesCommand.
 Every QuadCommand will have generate material ID by give textureID, programState, Blend function
 if the material id is the same, these QuadCommands could be batched to save draw call.
 */
class AX_DLL QuadCommand : public TrianglesCommand
{
public:
    /**Constructor.*/
    QuadCommand();
    /**Destructor.*/
    ~QuadCommand();

    /** Initializes the command.
     @param globalOrder GlobalZOrder of the command.
     @param texture The texture used in the command.
     @param blendType Blend function for the command.
     @param quads Rendered quads for the command.
     @param quadCount The number of quads when rendering.
     @param mv ModelView matrix for the command.
     @param flags to indicate that the command is using 3D rendering or not.
     */
    void init(float globalOrder,
              Texture2D* texture,
              const BlendFunc& blendType,
              V3F_T2F_C4B_Quad* quads,
              ssize_t quadCount,
              const Mat4& mv,
              uint32_t flags,
              const SceneViewData& view);

    static void destroyIsolatedIndices();

protected:
    void reIndex(int indices);

    int _indexSize;
    std::vector<uint16_t*> _ownedIndices;

    // shared across all instances
    static int __indexCapacity;
    static uint16_t* __indices;
};

}  // namespace ax

/**
 end of support group
 @}
 */
