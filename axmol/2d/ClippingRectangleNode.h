/*
 * Copyright (c) 2012 Pierre-David Bélanger
 * Copyright (c) 2012 cocos2d-x.org
 * Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 * Copyright (c) 2019-present Simdsoft Limited.
 *
 * https://axmol.dev/
 *
 * SPDX-License-Identifier: MIT
 *
 */
#pragma once

#include "axmol/scene/Node.h"
#include "axmol/renderer/CallbackCommand.h"

namespace ax
{

/**
 * @addtogroup _2d
 * @{
 */

/**
@brief Clipping Rectangle Node.
@details A node that clipped with specified rectangle.
 The region of ClippingRectangleNode doesn't support any transform except scale.
*/
class AX_DLL ClippingRectangleNode : public Node
{
public:
    /**
    @brief Create node with specified clipping region.
    @param clippingRegion Specify the clipping rectangle.
    @return If the creation success, return a pointer of ClippingRectangleNode; otherwise return nil.
    */
    static ClippingRectangleNode* create(const Rect& clippingRegion);
    /**
    @brief Create a clipping rectangle node.
    @return If the creation success, return a pointer of ClippingRectangleNode; otherwise return nil.
    */
    static ClippingRectangleNode* create();

    /**
    @brief Get the clipping rectangle.
    @return The clipping rectangle.
    */
    const Rect& getClippingRegion() const { return _clippingRegion; }
    /**
    @brief Set the clipping rectangle.
    @param clippingRegion Specify the clipping rectangle.
    */
    void setClippingRegion(const Rect& clippingRegion);

    /**
    @brief Get whether the clipping is enabled or not.
    @return Whether the clipping is enabled or not. Default is true.
    */
    bool isClippingEnabled() const { return _clippingEnabled; }

    /**
    @brief Enable/Disable the clipping.
    @param enabled Pass true to enable clipping. Pass false to disable clipping.
    */
    void setClippingEnabled(bool enabled) { _clippingEnabled = enabled; }

    // virtual void draw(Renderer* renderer, const Mat4 &transform, uint32_t flags) override;
    void visit(const SceneRenderState& state, const Mat4& parentTransform, uint32_t parentFlags) override;

protected:
    ClippingRectangleNode() = default;

    void onBeforeVisitScissor();
    void onAfterVisitScissor();

    Rect _clippingRegion;
    bool _clippingEnabled = true;

    bool _oldScissorTest = false;

    CallbackCommand* _beforeVisitCmdScissor;
    CallbackCommand* _afterVisitCmdScissor;
};

// end of _2d group
/// @}

}  // namespace ax
