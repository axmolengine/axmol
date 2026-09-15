/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "axmol/2d/Sprite.h"
#include "axmol/renderer/QuadCommand.h"

#include "sceneext/ArmatureDefine.h"
#include "sceneext/Bone.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{

class SCNEXT_API Skin : public ax::Sprite
{
public:
    static Skin* create();
    static Skin* createWithSpriteFrameName(std::string_view pszSpriteFrameName);
    static Skin* create(std::string_view pszFileName);

public:
    /**
     */
    Skin();

    bool initWithSpriteFrameName(std::string_view spriteFrameName) override;
    bool initWithFile(std::string_view filename) override;

    void updateArmatureTransform();
    void updateTransform() override;

    ax::Mat4 getNodeToWorldTransform() const override;
    ax::Mat4 getNodeToWorldTransformAR() const;

    void draw(const ax::SceneRenderState& state, const ax::Mat4& transform, uint32_t flags) override;

    /**
     *  @lua NA
     */
    virtual void setSkinData(const BaseData& data);
    /**
     *  @lua NA
     */
    virtual const BaseData& getSkinData() const;

    virtual void setBone(Bone* bone);
    virtual Bone* getBone() const;

    virtual std::string_view getDisplayName() const { return _displayName; }

protected:
    BaseData _skinData;
    Bone* _bone;
    Armature* _armature;
    ax::Mat4 _skinTransform;
    std::string _displayName;
    ax::QuadCommand _quadCommand;  // quad command
};

}  // namespace ax::ext
