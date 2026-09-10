/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#pragma once

#include "axmol/scene/Node.h"
#include "sceneext/ArmatureDefine.h"
#include "sceneext/SceneExtMacros.h"

namespace ax
{
class GroupCommand;
}

namespace ax::ext
{

class SCNEXT_API BatchNode : public ax::Node
{
public:
    static BatchNode* create();

public:
    BatchNode();
    /**
     * @ js NA
     * @ lua NA
     */
    ~BatchNode();
    /**
     */
    bool init() override;
    using Node::addChild;
    void addChild(ax::Node* pChild, int zOrder, int tag) override;
    void addChild(ax::Node* pChild, int zOrder, std::string_view name) override;
    void removeChild(ax::Node* child, bool cleanup) override;
    virtual void visit(const ax::SceneRenderState& state,
                       const ax::Mat4& parentTransform,
                       uint32_t parentFlags) override;
    void draw(const ax::SceneRenderState& state, const ax::Mat4& transform, uint32_t flags) override;

protected:
    void generateGroupCommand();
};

}  // namespace ax::ext
