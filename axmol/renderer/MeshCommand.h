/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include <unordered_map>
#include "axmol/renderer/RenderCommand.h"
#include "axmol/renderer/RenderState.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/renderer/CustomCommand.h"
#include "axmol/math/Math.h"

namespace ax
{

class CustomEventListener;
class CustomEvent;
class Material;

// it is a common mesh
class AX_DLL MeshCommand : public CustomCommand
{
public:
    // using PrimitiveType = rhi::PrimitiveType;
    /**
    Buffer usage of vertex/index buffer. If the contents is not updated every frame,
    then use STATIC, other use DYNAMIC.
    */
    using BufferUsage = rhi::BufferUsage;
    /**
    The index format determine the size for index data. U_SHORT is enough for most
    cases.
    */
    using IndexFormat = rhi::IndexFormat;

    MeshCommand();
    virtual ~MeshCommand();

    MeshCommand(const MeshCommand&) = default;
    MeshCommand(MeshCommand&&)      = default;

    MeshCommand& operator=(MeshCommand&&)      = default;
    MeshCommand& operator=(const MeshCommand&) = default;

    /**
    Init function. The render command will be in 2D mode.
    @param globalZOrder GlobalZOrder of the render command.
    */
    void init(float globalZOrder);

    void init(float globalZOrder, const Mat4& transform, const SceneViewData& view);

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    void listenRendererRecreated(CustomEvent* event);
#endif

protected:
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    CustomEventListener* _rendererRecreatedListener;
#endif
};

}  // namespace ax
