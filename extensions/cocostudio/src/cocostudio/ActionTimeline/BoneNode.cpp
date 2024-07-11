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

#include "base/Director.h"
#include "base/Utils.h"
#include "renderer/Renderer.h"
#include "renderer/Shaders.h"
#include "renderer/backend/ProgramState.h"

#include "BoneNode.h"
#include "SkeletonNode.h"

NS_TIMELINE_BEGIN

BoneNode* BoneNode::create()
{
    BoneNode* ret = new BoneNode();
    if (ret->init())
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

BoneNode* BoneNode::create(int length)
{
    BoneNode* ret = new BoneNode();
    if (ret->init())
    {
        ret->setDebugDrawLength(length);
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

BoneNode::~BoneNode() {}

bool BoneNode::init()
{
    _rackLength = 50;
    _rackWidth  = 20;
    updateVertices();
    updateColor();

    auto& pipelineDescriptor = _customCommand.getPipelineDescriptor();
    auto* program =
        ax::backend::Program::getBuiltinProgram(ax::backend::ProgramType::POSITION_COLOR);  // TODO: noMVP?
    setProgramState(new ax::backend::ProgramState(program), true);
    pipelineDescriptor.programState = _programState;

    _mvpLocation = _programState->getUniformLocation("u_MVPMatrix"sv);

    _customCommand.createVertexBuffer(sizeof(_vertexData[0]), 4, ax::CustomCommand::BufferUsage::DYNAMIC);
    _customCommand.createIndexBuffer(ax::CustomCommand::IndexFormat::U_SHORT, 6,
                                     ax::CustomCommand::BufferUsage::STATIC);
    unsigned short indices[6] = {0, 1, 2, 0, 2, 3};
    _customCommand.updateIndexBuffer(indices, sizeof(indices));

    return true;
}

void BoneNode::addChild(ax::Node* child, int localZOrder, int tag)
{
    addToChildrenListHelper(child);
    Node::addChild(child, localZOrder, tag);
}

void BoneNode::addChild(Node* child, int localZOrder, std::string_view name)
{
    addToChildrenListHelper(child);
    Node::addChild(child, localZOrder, name);
}

void BoneNode::addSkin(SkinNode* skin, bool isDisplay, bool hideOthers)
{
    AXASSERT(skin != nullptr, "Argument must be non-nil");
    if (hideOthers)
    {
        for (auto&& bonskin : _boneSkins)
        {
            bonskin->setVisible(false);
        }
    }
    Node::addChild(skin);
    skin->setVisible(isDisplay);
}

void BoneNode::addSkin(SkinNode* skin, bool display)
{
    addSkin(skin, display, false);
}

void BoneNode::removeChild(Node* child, bool cleanup /* = true */)
{
    ssize_t index = _children.getIndex(child);
    if (index != ax::AX_INVALID_INDEX)
    {
        removeFromChildrenListHelper(child);
        Node::removeChild(child, cleanup);
    }
}

void BoneNode::removeFromBoneList(BoneNode* bone)
{
    if (_rootSkeleton != nullptr)
    {
        auto skeletonNode = dynamic_cast<SkeletonNode*>(bone);
        if (skeletonNode == nullptr)  // is not a nested skeleton
        {
            auto subBones = bone->getAllSubBones();
            subBones.pushBack(bone);
            for (auto&& subBone : subBones)
            {
                if (subBone->_rootSkeleton == nullptr)
                    continue;
                subBone->_rootSkeleton = nullptr;

                auto toremoveIter = _rootSkeleton->_subBonesMap.find(subBone->getName());
                if (toremoveIter != _rootSkeleton->_subBonesMap.end())
                {
                    _rootSkeleton->_subBonesMap.erase(toremoveIter);
                    _rootSkeleton->_subBonesDirty      = true;
                    _rootSkeleton->_subBonesOrderDirty = true;
                }
            }
        }
        else
        {
            _rootSkeleton->_subBonesDirty      = true;
            _rootSkeleton->_subBonesOrderDirty = true;
        }
    }
    _childBones.eraseObject(bone);
}

void BoneNode::addToBoneList(BoneNode* bone)
{
    _childBones.pushBack(bone);
    if (_rootSkeleton != nullptr)
    {
        auto skeletonNode = dynamic_cast<SkeletonNode*>(bone);
        if (skeletonNode == nullptr && bone->_rootSkeleton == nullptr)  // not nest skeleton
        {
            auto subBones = bone->getAllSubBones();
            subBones.pushBack(bone);
            for (auto&& subBone : subBones)
            {
                subBone->_rootSkeleton = _rootSkeleton;
                auto bonename          = subBone->getName();
                if (_rootSkeleton->_subBonesMap.find(bonename) == _rootSkeleton->_subBonesMap.end())
                {
                    _rootSkeleton->_subBonesMap.insert(subBone->getName(), subBone);

                    _rootSkeleton->_subBonesDirty      = true;
                    _rootSkeleton->_subBonesOrderDirty = true;
                }
                else
                    AXLOGD("already has a bone named {} in skeleton {}", bonename,
                          _rootSkeleton->getName());
            }
        }
        else
        {
            _rootSkeleton->_subBonesDirty      = true;
            _rootSkeleton->_subBonesOrderDirty = true;
        }
    }
}

void BoneNode::addToSkinList(SkinNode* skin)
{
    _boneSkins.pushBack(skin);
    auto blendSkin = dynamic_cast<BlendProtocol*>(skin);
    if (nullptr != blendSkin && _blendFunc != blendSkin->getBlendFunc())
    {
        blendSkin->setBlendFunc(_blendFunc);
    }
}

void BoneNode::removeFromSkinList(SkinNode* skin)
{
    _boneSkins.eraseObject(skin);
}

void BoneNode::displaySkin(SkinNode* skin, bool hideOthers)
{
    for (auto&& boneskin : _boneSkins)
    {
        if (boneskin == skin)
        {
            boneskin->setVisible(true);
        }
        else if (hideOthers)
        {
            boneskin->setVisible(false);
        }
    }
}

void BoneNode::displaySkin(std::string_view skinName, bool hideOthers)
{
    for (auto&& skin : _boneSkins)
    {
        if (skinName == skin->getName())
        {
            skin->setVisible(true);
        }
        else if (hideOthers)
        {
            skin->setVisible(false);
        }
    }
}

ax::Vector<SkinNode*> BoneNode::getVisibleSkins() const
{
    ax::Vector<SkinNode*> displayingSkins;
    for (const auto& boneskin : _boneSkins)
    {
        if (boneskin->isVisible())
        {
            displayingSkins.pushBack(boneskin);
        }
    }
    return displayingSkins;
}

ax::Rect BoneNode::getBoundingBox() const
{
    ax::Rect boundingBox = getVisibleSkinsRect();
    return RectApplyAffineTransform(boundingBox, this->getNodeToParentAffineTransform());
}

ax::Rect BoneNode::getVisibleSkinsRect() const
{
    float minx, miny, maxx, maxy = 0;
    minx = miny = maxx = maxy;
    bool first         = true;

    ax::Rect displayRect = ax::Rect(0, 0, 0, 0);
    if (_isRackShow && _rootSkeleton != nullptr && _rootSkeleton->_isRackShow)
    {
        maxx  = _rackLength;
        maxy  = _rackWidth;
        first = false;
    }

    for (const auto& skin : _boneSkins)
    {
        ax::Rect r = skin->getBoundingBox();
        if (!skin->isVisible() || r.equals(ax::Rect::ZERO))
            continue;

        if (first)
        {
            minx = r.getMinX();
            miny = r.getMinY();
            maxx = r.getMaxX();
            maxy = r.getMaxY();

            first = false;
        }
        else
        {
            minx = MIN(r.getMinX(), minx);
            miny = MIN(r.getMinY(), miny);
            maxx = MAX(r.getMaxX(), maxx);
            maxy = MAX(r.getMaxY(), maxy);
        }
        displayRect.setRect(minx, miny, maxx - minx, maxy - miny);
    }
    return displayRect;
}

void BoneNode::setBlendFunc(const ax::BlendFunc& blendFunc)
{
    if (_blendFunc != blendFunc)
    {
        _blendFunc = blendFunc;
        for (auto&& skin : _boneSkins)
        {
            auto blendSkin = dynamic_cast<BlendProtocol*>(skin);
            if (nullptr != blendSkin)
            {
                blendSkin->setBlendFunc(_blendFunc);
            }
        }
    }
}

void BoneNode::setDebugDrawLength(float length)
{
    _rackLength = length;
    updateVertices();
}

void BoneNode::setDebugDrawWidth(float width)
{
    _rackWidth = width;
    updateVertices();
}

void BoneNode::setDebugDrawEnabled(bool isDebugDraw)
{
    if (_isRackShow == isDebugDraw)
        return;
    _isRackShow = isDebugDraw;
}

void BoneNode::setDebugDrawColor(const ax::Color4F& color)
{
    _rackColor = color;
    updateColor();
}

void BoneNode::visit(ax::Renderer* renderer, const ax::Mat4& parentTransform, uint32_t parentFlags)
{
    // quick return if not visible. children won't be drawn.
    if (!_visible)
    {
        return;
    }

    uint32_t flags = processParentFlags(parentTransform, parentFlags);

    // IMPORTANT:
    // To ease the migration to v3.0, we still support the Mat4 stack,
    // but it is deprecated and your code should not rely on it
    _director->pushMatrix(ax::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    _director->loadMatrix(ax::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

    bool visibleByCamera = isVisitableByVisitingCamera();
    bool isdebugdraw     = visibleByCamera && _isRackShow && nullptr == _rootSkeleton;
    int i                = 0;

    if (!_children.empty())
    {
        sortAllChildren();
        // draw children zOrder < 0
        for (; i < _children.size(); i++)
        {
            auto node = _children.at(i);
            if (_rootSkeleton != nullptr && _boneSkins.contains(node))  // skip skin when bone is in a skeleton
                continue;
            if (node && node->getLocalZOrder() < 0)
                node->visit(renderer, _modelViewTransform, flags);
            else
                break;
        }
        // self draw
        if (isdebugdraw)
            this->draw(renderer, _modelViewTransform, flags);

        for (auto it = _children.cbegin() + i; it != _children.cend(); ++it)
        {
            auto node = (*it);
            if (_rootSkeleton != nullptr && _boneSkins.contains(node))  // skip skin when bone is in a skeleton
                continue;
            node->visit(renderer, _modelViewTransform, flags);
        }
    }
    else if (isdebugdraw)
    {
        this->draw(renderer, _modelViewTransform, flags);
    }

    _director->popMatrix(ax::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

    // FIX ME: Why need to set _orderOfArrival to 0??
    // Please refer to https://github.com/cocos2d/cocos2d-x/pull/6920
    // reset for next frame
    // _orderOfArrival = 0;
}

void BoneNode::draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags)
{
    _customCommand.init(_globalZOrder, _blendFunc);
    renderer->addCommand(&_customCommand);

#ifdef AX_STUDIO_ENABLED_VIEW
// TODO
//     glVertexAttribPointer(ax::GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, _noMVPVertices);
//     glVertexAttribPointer(ax::GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, _squareColors);
//
//     glEnable(GL_LINE_SMOOTH);
//     glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
//     glDrawArrays(GL_LINE_LOOP, 0, 4);
//     AX_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, 8);
#endif  // AX_STUDIO_ENABLED_VIEW

    for (int i = 0; i < 4; ++i)
    {
        ax::Vec4 pos;
        pos.x = _squareVertices[i].x;
        pos.y = _squareVertices[i].y;
        pos.z = _positionZ;
        pos.w = 1;
        _modelViewTransform.transformVector(&pos);
        _vertexData[i].noMVPVertices = ax::Vec3(pos.x, pos.y, pos.z) / pos.w;
    }
    _customCommand.updateVertexBuffer(_vertexData, sizeof(_vertexData));

    _programState->setUniform(_mvpLocation, transform.m, sizeof(transform.m));
}

void BoneNode::updateVertices()
{
    if (_rackLength != _squareVertices[2].x - _anchorPointInPoints.x ||
        _squareVertices[3].y != _rackWidth / 2 - _anchorPointInPoints.y)
    {
        _squareVertices[1].x = _squareVertices[1].y = _squareVertices[3].y = .0f;
        _squareVertices[0].x = _squareVertices[2].x = _rackLength * .1f;
        _squareVertices[2].y                        = _rackWidth * .5f;
        _squareVertices[0].y                        = -_squareVertices[2].y;
        _squareVertices[3].x                        = _rackLength;

        for (int i = 0; i < 4; i++)
        {
            _squareVertices[i] += _anchorPointInPoints;
        }
        _transformUpdated = _transformDirty = _inverseDirty = _contentSizeDirty = true;
    }
}

void BoneNode::updateColor()
{
    for (unsigned int i = 0; i < 4; i++)
    {
        _vertexData[i].squareColor = _rackColor;
    }
    _transformUpdated = _transformDirty = _inverseDirty = _contentSizeDirty = true;
}

void BoneNode::updateDisplayedColor(const ax::Color3B& /*parentColor*/)
{
    if (_cascadeColorEnabled)
    {
        for (const auto& child : _boneSkins)
        {
            child->updateDisplayedColor(_displayedColor);
        }
    }
}

void BoneNode::updateDisplayedOpacity(uint8_t /*parentOpacity*/)
{
    if (_cascadeOpacityEnabled)
    {
        for (const auto& child : _boneSkins)
        {
            child->updateDisplayedOpacity(_displayedOpacity);
        }
    }
}

void BoneNode::disableCascadeOpacity()
{
    for (const auto& child : _boneSkins)
    {
        child->updateDisplayedOpacity(255);
    }
}

void BoneNode::disableCascadeColor()
{
    for (const auto& child : _boneSkins)
    {
        child->updateDisplayedColor(ax::Color3B::WHITE);
    }
}

ax::Vector<BoneNode*> BoneNode::getAllSubBones() const
{
    ax::Vector<BoneNode*> allBones;
    std::stack<BoneNode*> boneStack;  // for avoid recursive
    for (const auto& bone : _childBones)
    {
        boneStack.push(bone);
    }

    while (boneStack.size() > 0)
    {
        auto top = boneStack.top();
        allBones.pushBack(top);
        boneStack.pop();
        auto topchildren = top->getChildBones();
        for (const auto& childbone : topchildren)
        {
            boneStack.push(childbone);
        }
    }
    return allBones;
}

ax::Vector<SkinNode*> BoneNode::getAllSubSkins() const
{
    auto allbones = getAllSubBones();
    ax::Vector<SkinNode*> allskins;
    for (const auto& bone : allbones)
    {
        for (const auto& skin : bone->getSkins())
        {
            allskins.pushBack(skin);
        }
    }
    return allskins;
}

void BoneNode::sortAllChildren()
{
    if (_reorderChildDirty)
    {
        sortNodes(_childBones);
        sortNodes(_boneSkins);
        Node::sortAllChildren();
    }
}

SkeletonNode* BoneNode::getRootSkeletonNode() const
{
    return _rootSkeleton;
}

#ifdef AX_STUDIO_ENABLED_VIEW
bool BoneNode::isPointOnRack(const ax::Vec2& bonePoint)
{

    if (bonePoint.x >= 0.0f && bonePoint.y >= _squareVertices[0].y && bonePoint.x <= _rackLength &&
        bonePoint.y <= _squareVertices[2].y)
    {
        if (_rackLength != 0.0f && _rackWidth != 0.0f)
        {
            float a1 = (_squareVertices[2].y - _squareVertices[3].y) / (_squareVertices[3].x - _squareVertices[0].x);
            float a2 = (_squareVertices[2].y - _squareVertices[3].y) / (_squareVertices[0].x - _squareVertices[1].x);
            float b1 = a1 * _squareVertices[3].x;
            float y1 = bonePoint.y - _squareVertices[1].y;
            if (y1 >= a1 * bonePoint.x - b1 && y1 <= a2 * bonePoint.x && y1 >= -a2 * bonePoint.x &&
                y1 <= -a1 * bonePoint.x + b1)
                return true;
        }
    }
    return false;
}
#endif  // AX_STUDIO_ENABLED_VIEW

void BoneNode::batchBoneDrawToSkeleton(BoneNode* bone) const
{
    bool visibleByCamera = bone->isVisitableByVisitingCamera();
    if (!visibleByCamera)
    {
        return;
    }

    ax::Vec3 vpos[4];
    for (int i = 0; i < 4; i++)
    {
        ax::Vec4 pos;
        pos.x = bone->_squareVertices[i].x;
        pos.y = bone->_squareVertices[i].y;
        pos.z = bone->_positionZ;
        pos.w = 1;
        bone->_modelViewTransform.transformVector(&pos);  // call after visit
        vpos[i] = ax::Vec3(pos.x, pos.y, pos.z) / pos.w;
    }

    int count = bone->_rootSkeleton->_batchedVeticesCount;
    if (count + 8 > (int)(bone->_rootSkeleton->_batchedBoneVertexData.size()))
    {
        bone->_rootSkeleton->_batchedBoneVertexData.resize(count + 100);
    }
    for (int i = 0; i < 4; i++)
    {
        bone->_rootSkeleton->_batchedBoneVertexData[count + i].vertex = vpos[i];
        bone->_rootSkeleton->_batchedBoneVertexData[count + i].color  = bone->_vertexData[i].squareColor;
    }
    bone->_rootSkeleton->_batchedVeticesCount += 4;
}

// call after self visit
void BoneNode::visitSkins(ax::Renderer* renderer, BoneNode* bone) const
{
    // quick return if not visible. children won't be drawn.
    if (!bone->_visible)
    {
        return;
    }

    // IMPORTANT:
    // To ease the migration to v3.0, we still support the Mat4 stack,
    // but it is deprecated and your code should not rely on it
    _director->pushMatrix(ax::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    _director->loadMatrix(ax::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, bone->_modelViewTransform);

    if (!bone->_boneSkins.empty())
    {
        bone->sortAllChildren();
        for (auto it = bone->_boneSkins.cbegin(); it != bone->_boneSkins.cend(); ++it)
            (*it)->visit(renderer, bone->_modelViewTransform, true);
    }

    _director->popMatrix(ax::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

    // FIX ME: Why need to set _orderOfArrival to 0??
    // Please refer to https://github.com/cocos2d/cocos2d-x/pull/6920
    // reset for next frame
    // _orderOfArrival = 0;
}

void BoneNode::setRootSkeleton(BoneNode* bone, SkeletonNode* skeleton) const
{
    bone->_rootSkeleton = skeleton;
}

void BoneNode::setLocalZOrder(int localZOrder)
{
    Node::setLocalZOrder(localZOrder);
    if (_rootSkeleton != nullptr)
        _rootSkeleton->_subBonesOrderDirty = true;
}

void BoneNode::setName(std::string_view name)
{
    auto oldname = getName();
    Node::setName(name);
    if (_rootSkeleton != nullptr)
    {
        auto oiter = _rootSkeleton->_subBonesMap.find(oldname);
        auto niter = _rootSkeleton->_subBonesMap.find(name);
        if (oiter != _rootSkeleton->_subBonesMap.end() && niter == _rootSkeleton->_subBonesMap.end())
        {
            auto node = oiter->second;
            _rootSkeleton->_subBonesMap.erase(oiter);
            _rootSkeleton->_subBonesMap.insert(name, node);
        }
    }
}

void BoneNode::addToChildrenListHelper(Node* child)
{
    BoneNode* bone = dynamic_cast<BoneNode*>(child);
    if (nullptr != bone)
    {
        addToBoneList(bone);
    }
    else
    {
        SkinNode* skin = dynamic_cast<SkinNode*>(child);
        if (nullptr != skin)
        {
            addToSkinList(skin);
        }
    }
}

void BoneNode::removeFromChildrenListHelper(Node* child)
{
    BoneNode* bone = dynamic_cast<BoneNode*>(child);
    if (nullptr != bone)
    {
        removeFromBoneList(bone);
    }
    else
    {
        SkinNode* skin = dynamic_cast<SkinNode*>(child);
        if (nullptr != skin)
        {
            removeFromSkinList(skin);
        }
    }
}

void BoneNode::setVisible(bool visible)
{
    if (_visible == visible)
        return;

    Node::setVisible(visible);
    if (_rootSkeleton != nullptr)
    {
        _rootSkeleton->_subBonesDirty      = true;
        _rootSkeleton->_subBonesOrderDirty = true;
    }
}

void BoneNode::setContentSize(const ax::Size& contentSize)
{
    Node::setContentSize(contentSize);
    updateVertices();
}

void BoneNode::setAnchorPoint(const ax::Vec2& anchorPoint)
{
    Node::setAnchorPoint(anchorPoint);
    updateVertices();
}

NS_TIMELINE_END
