/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/PixelBufferDesc.h"
#include "axmol/renderer/RenderCommand.h"
#include "axmol/base/RefPtr.h"

/**
 * @addtogroup renderer
 * @{
 */

namespace ax
{

namespace rhi
{
class Texture;
}

/**
Callback command is used to invoke a callback function when this command is
executed. You can do some logic opertion in the callback, such as invoking
renderer to set depth/stencil test. Don't suggest to invoke backen API in
the callback function.
*/
class AX_DLL CallbackCommand : public RenderCommand
{
    // only allow render to manage the callbackCommand
    friend class Renderer;
    CallbackCommand();
    ~CallbackCommand() {};

public:
    void init(float globalZOrder);
    void init(float globalZorder, const Mat4& transform, unsigned int, const SceneViewData& view);

    /**
     * @brief Reset the command state for reuse
     *
     */
    void reset();

    /**
     Execute the render command and call callback functions.
     */
    void execute();
    /**Callback function.*/
    std::function<void()> func;
};

}  // namespace ax
/**
 end of support group
 @}
 */
