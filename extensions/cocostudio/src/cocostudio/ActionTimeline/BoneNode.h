/****************************************************************************
Copyright (c) 2015-2017 Chukong Technologies Inc.

https://axmol.dev/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __CCBONENODE_H__
#define __CCBONENODE_H__

#include "base/Protocols.h"
#include "2d/Node.h"
#include "renderer/CustomCommand.h"
#include "TimelineMacro.h"
#include "cocostudio/CocosStudioExport.h"
#include "SkinNode.h"

namespace ax
{
namespace backend
{
class ProgramState;
}
}  // namespace ax

NS_TIMELINE_BEGIN

class SkeletonNode;

class CCS_DLL BoneNode : public ax::Node, public ax::BlendProtocol
{
public:
    static BoneNode* create();
    static BoneNode* create(int length);

    using Node::addChild;
    // add child, and add child to bone list and skeleton's sub bone map or add it to skin list
    virtual void addChild(ax::Node* child, int localZOrder, std::string_view name) override;
    virtual void addChild(ax::Node* child, int localZOrder, int tag) override;

    // remove child, and remove child from bone list and skeleton's sub bone map or remove it from skin list
    virtual void removeChild(Node* child, bool cleanup) override;

    // get child bone list
    virtual const ax::Vector<BoneNode*>& getChildBones() const { return _childBones; }
    virtual ax::Vector<BoneNode*>& getChildBones() { return _childBones; }

    // get this bone's root skeleton, return null while bone is not in a skeleton
    virtual SkeletonNode* getRootSkeletonNode() const;

    /**
     * @brief: get all bones in this bone tree
     */
    ax::Vector<BoneNode*> getAllSubBones() const;

    /**
     *@brief: add a skin
     *@param: display, whether display this skin
     */
    virtual void addSkin(SkinNode* skin, bool display);

    /**
     *@brief: add a skin
     *@param: display, whether display this skin
     *@param: hideOthers, whether hide other skins added to this bone
     */
    virtual void addSkin(SkinNode* skin, bool display, bool hideOthers);

    /**
     * @brief: display skin
     * @param: hideOthers, set other skins invisible
     */
    virtual void displaySkin(SkinNode* skin, bool hideOthers);

    /**
     * @brief: display all skins named skinName, if hide display only one skin,
     *         prefer to use display(SkinNode* skin, bool hideOthers = false)
     * @param: hideOthers, set other skins invisible
     */
    virtual void displaySkin(std::string_view skinName, bool hideOthers);

    // get the skins which is visible (displaying skins)
    virtual ax::Vector<SkinNode*> getVisibleSkins() const;

    /**
     * get skins in this bone's children
     */
    virtual const ax::Vector<SkinNode*>& getSkins() const { return _boneSkins; }
    virtual ax::Vector<SkinNode*>& getSkins() { return _boneSkins; }

    /**
     * @brief: get all skins in this bone tree
     */
    ax::Vector<SkinNode*> getAllSubSkins() const;

    // blendFunc
    virtual void setBlendFunc(const ax::BlendFunc& blendFunc) override;
    virtual const ax::BlendFunc& getBlendFunc() const override { return _blendFunc; }

    // debug draw show, bone's debugdraw can be draw when bone is visible
    // when bone's added to skeleton, DebugDrawEnabled controlled by skeleton's DebugDrawEnabled
    virtual void setDebugDrawEnabled(bool isDebugDraw);
    virtual bool isDebugDrawEnabled() const { return _isRackShow; }

    // bone's debug draw's length
    virtual void setDebugDrawLength(float length);
    virtual float getDebugDrawLength() const { return _rackLength; }

    // bone's debug draw's width
    virtual void setDebugDrawWidth(float width);
    virtual float getDebugDrawWidth() const { return _rackWidth; }

    // bone's debug draw's width
    virtual void setDebugDrawColor(const ax::Color4F& color);
    virtual ax::Color4F getDebugDrawColor() const { return _rackColor; }

    // get bone's bounding box, depends on getVisibleSkinsRect, apply on node to parent's transform
    ax::Rect getBoundingBox() const override;

    /**
     *get displayings rect in self transform
     */
    virtual ax::Rect getVisibleSkinsRect() const;

    // transform & draw
    virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

    // set local zorder, and dirty the debugdraw to make debugdraw's render layer right
    virtual void setLocalZOrder(int localZOrder) override;

    // set name, and replace the subbone map in skeleton
    virtual void setName(std::string_view name) override;

    // set visible, and dirty the debugdraw to make debugdraw's render layer right
    virtual void setVisible(bool visible) override;

    // set contentsize, and recalculate debugdraw
    virtual void setContentSize(const ax::Size& contentSize) override;

    // set localzorder, and recalculate debugdraw
    virtual void setAnchorPoint(const ax::Vec2& anchorPoint) override;

#ifdef AX_STUDIO_ENABLED_VIEW
    // hit test , bonePoint is in self coordinate
    virtual bool isPointOnRack(const ax::Vec2& bonePoint);
#endif

    BoneNode() = default;
    virtual ~BoneNode();
    virtual bool init() override;

protected:
    virtual void addToChildrenListHelper(Node* child);
    virtual void removeFromChildrenListHelper(Node* child);

    // add bone to children bone list, and add bone to skeleton's subbone map
    virtual void addToBoneList(BoneNode* bone);

    // remove bone from children bone list, and remove bone from skeleton's subbone map
    virtual void removeFromBoneList(BoneNode* bone);

    // add skin to skin list
    virtual void addToSkinList(SkinNode* skin);

    // remove skin from skin list
    virtual void removeFromSkinList(SkinNode* skin);

    // sort all _children ,  bone list and skin list
    virtual void sortAllChildren() override;

    virtual void updateVertices();
    virtual void updateColor() override;

    // bone's color and opacity cannot cascade to bone
    virtual void updateDisplayedColor(const ax::Color3B& parentColor) override;
    virtual void updateDisplayedOpacity(uint8_t parentOpacity) override;
    virtual void disableCascadeOpacity() override;
    virtual void disableCascadeColor() override;

    // override Node::visit, just visit bones in children
    virtual void visit(ax::Renderer* renderer,
                       const ax::Mat4& parentTransform,
                       uint32_t parentFlags) override;

    // a help function for SkeletonNode
    // for batch bone's draw to _rootSkeleton
    virtual void batchBoneDrawToSkeleton(BoneNode* bone) const;

    // a help function for SkeletonNode
    // @param bone, visit bone's skins
    virtual void visitSkins(ax::Renderer* renderer, BoneNode* bone) const;

    // a help function for SkeletonNode
    // set bone's rootSkeleton = skeleton
    void setRootSkeleton(BoneNode* bone, SkeletonNode* skeleton) const;

protected:
    ax::CustomCommand _customCommand;
    ax::backend::UniformLocation _mvpLocation;

    ax::BlendFunc _blendFunc = ax::BlendFunc::ALPHA_NON_PREMULTIPLIED;

    bool _isRackShow            = false;
    ax::Color4F _rackColor = ax::Color4F::WHITE;
    float _rackLength           = 50.0f;
    float _rackWidth            = 20.0f;

    ax::Vector<BoneNode*> _childBones;
    ax::Vector<SkinNode*> _boneSkins;
    SkeletonNode* _rootSkeleton = nullptr;

private:
    struct VertexData
    {
        ax::Color4F squareColor;
        ax::Vec3 noMVPVertices;
    };

    ax::Vec2 _squareVertices[4];
    VertexData _vertexData[4];

    AX_DISALLOW_COPY_AND_ASSIGN(BoneNode);
};

NS_TIMELINE_END
#endif  //__CCBONENODE_H__
