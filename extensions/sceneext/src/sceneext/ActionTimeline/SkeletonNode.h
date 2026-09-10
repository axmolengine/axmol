/****************************************************************************
Copyright (c) 2015-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/ActionTimeline/TimelineMacro.h"
#include "axmol/renderer/Renderer.h"
#include "sceneext/SceneExtMacros.h"
#include "axmol/base/Map.h"

#include "sceneext/ActionTimeline/BoneNode.h"

NS_TIMELINE_BEGIN

class SCNEXT_API SkeletonNode : public BoneNode
{
    friend class BoneNode;

public:
    static SkeletonNode* create();

    /**
     *get bonenode in skeleton node by bone name
     */
    BoneNode* getBoneNode(std::string_view boneName);

    /**
     *get All bones in this skeleton, <bone's name, BoneNode>
     */
    const ax::StringMap<BoneNode*>& getAllSubBonesMap() const;

    /**
     *@brief: change displays
     *@param: boneSkinNameMap, map <name of bone, name of skin to display which added to bone>
     */
    void changeSkins(const tlx::string_map<std::string>& boneSkinNameMap);

    /**
     *@brief: change displays
     *@param: skinGroupName have
     */
    void changeSkins(std::string_view skinGroupName);

    /**
     *@brief: add a boneSkinNameMap as a SkinGroup named groupName
     *@param: groupName, key
     *@param: boneSkinNameMap, map <name of bone, name of skin to display which added to bone>
     */
    void addSkinGroup(std::string_view groupName, tlx::string_map<std::string> boneSkinNameMap);

    ax::Rect getBoundingBox() const override;

    SkeletonNode();
    virtual ~SkeletonNode();
    bool init() override;

protected:
    void updateVertices() override;
    void updateColor() override;

    void visit(const ax::SceneRenderState& state, const ax::Mat4& parentTransform, uint32_t parentFlags) override;
    void draw(const ax::SceneRenderState& state, const ax::Mat4& transform, uint32_t flags) override;

protected:
    ax::StringMap<BoneNode*> _subBonesMap;

private:
    struct VertexData
    {
        ax::Vec3 vertex;
        ax::Color color;
    };

    ax::Vec2 _squareVertices[8];
    VertexData _vertexData[8];

    // map< suit name, map< bone name, skin name> >
    tlx::string_map<tlx::string_map<std::string>> _skinGroupMap;
    AX_DISALLOW_COPY_AND_ASSIGN(SkeletonNode);

    void checkSubBonesDirty();
    // for draw skins as ordered bones' local z
    ax::Vector<BoneNode*> _subOrderedAllBones;
    void updateOrderedAllbones();
    void sortOrderedAllBones();
    // for batch draw sub bones
    bool _subBonesDirty;
    bool _subBonesOrderDirty;
    std::vector<VertexData> _batchedBoneVertexData;
    int _batchedVeticesCount;
    ax::CustomCommand _batchBoneCommand;

    void batchDrawAllSubBones(const ax::SceneRenderState& state);
};

NS_TIMELINE_END
