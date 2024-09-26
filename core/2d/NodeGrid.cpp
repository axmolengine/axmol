/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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
#include "2d/NodeGrid.h"
#include "2d/Grid.h"
#include "renderer/Renderer.h"

namespace ax
{

NodeGrid* NodeGrid::create()
{
    NodeGrid* ret = new NodeGrid();
    if (ret->init())
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }
    return ret;
}

NodeGrid* NodeGrid::create(const ax::Rect& rect)
{
    NodeGrid* ret = NodeGrid::create();
    if (ret)
    {
        ret->setGridRect(rect);
    }
    return ret;
}

NodeGrid::NodeGrid() {}

void NodeGrid::setTarget(Node* target)
{
#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        if (_gridTarget)
            sEngine->releaseScriptObject(this, _gridTarget);
        if (target)
            sEngine->retainScriptObject(this, target);
    }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    AX_SAFE_RELEASE(_gridTarget);
    AX_SAFE_RETAIN(target);
    _gridTarget = target;
}

NodeGrid::~NodeGrid()
{
    AX_SAFE_RELEASE(_nodeGrid);
    AX_SAFE_RELEASE(_gridTarget);
}

void NodeGrid::onGridBeginDraw()
{
    if (_nodeGrid && _nodeGrid->isActive())
    {
        _nodeGrid->beforeDraw();
    }
}

void NodeGrid::onGridEndDraw()
{
    if (_nodeGrid && _nodeGrid->isActive())
    {
        _nodeGrid->afterDraw(this);
    }
}

void NodeGrid::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    // quick return if not visible. children won't be drawn.
    if (!_visible)
    {
        return;
    }

    bool dirty = (parentFlags & FLAGS_TRANSFORM_DIRTY) || _transformUpdated;
    if (dirty)
        _modelViewTransform = this->transform(parentTransform);
    _transformUpdated = false;

    // IMPORTANT:
    // To ease the migration to v3.0, we still support the Mat4 stack,
    // but it is deprecated and your code should not rely on it
    _director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    _director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

    Director::Projection beforeProjectionType = Director::Projection::DEFAULT;
    if (_nodeGrid && _nodeGrid->isActive())
    {
        beforeProjectionType = _director->getProjection();
        _nodeGrid->set2DProjection();
    }

    onGridBeginDraw();

    if (_gridTarget)
    {
        _gridTarget->visit(renderer, _modelViewTransform, dirty);
    }

    int i                = 0;
    bool visibleByCamera = isVisitableByVisitingCamera();

    if (!_children.empty())
    {
        sortAllChildren();
        // draw children zOrder < 0
        for (auto size = _children.size(); i < size; ++i)
        {
            auto node = _children.at(i);

            if (node && node->getLocalZOrder() < 0)
                node->visit(renderer, _modelViewTransform, dirty);
            else
                break;
        }
        // self draw,currently we have nothing to draw on NodeGrid, so there is no need to add render command
        if (visibleByCamera)
            this->draw(renderer, _modelViewTransform, dirty);

        for (auto it = _children.cbegin() + i, itCend = _children.cend(); it != itCend; ++it)
        {
            (*it)->visit(renderer, _modelViewTransform, dirty);
        }
    }
    else if (visibleByCamera)
    {
        this->draw(renderer, _modelViewTransform, dirty);
    }

    // FIX ME: Why need to set _orderOfArrival to 0??
    // Please refer to https://github.com/cocos2d/cocos2d-x/pull/6920
    // setOrderOfArrival(0);

    if (_nodeGrid && _nodeGrid->isActive())
    {
        // restore projection
        _director->setProjection(beforeProjectionType);
    }

    onGridEndDraw();

    _director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void NodeGrid::setGrid(GridBase* grid)
{
    AX_SAFE_RELEASE(_nodeGrid);
    AX_SAFE_RETAIN(grid);
    _nodeGrid = grid;
}

}
