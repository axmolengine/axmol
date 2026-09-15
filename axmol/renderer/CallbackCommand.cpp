/****************************************************************************
 Copyright (c) 2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/renderer/CallbackCommand.h"
#include "axmol/rhi/GraphicsCore.h"

namespace ax
{

static uint64_t s_nextId = 1;

CallbackCommand::CallbackCommand()
{
    _type = RenderCommand::Type::CALLBACK_COMMAND;
}

void CallbackCommand::init(float globalOrder)
{
    _globalOrder = globalOrder;
}

void CallbackCommand::init(float globalOrder, const Mat4& transform, unsigned int flags, const SceneViewData& view)
{
    RenderCommand::init(globalOrder, transform, flags, view);
}

void CallbackCommand::reset()
{
    _globalOrder   = 0.0f;
    _isTransparent = true;
    _skipBatching  = false;
    _is3D          = false;
    _depth         = 0.0f;
}

void CallbackCommand::execute()
{
    if (func)
        func();
}

}  // namespace ax
