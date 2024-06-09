/****************************************************************************
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
#include "2d/ClippingRectangleNode.h"
#include "base/Director.h"
#include "renderer/Renderer.h"
#include "math/Vec2.h"
#include "platform/GLView.h"

NS_AX_BEGIN

ClippingRectangleNode* ClippingRectangleNode::create(const Rect& clippingRegion)
{
    ClippingRectangleNode* node = new ClippingRectangleNode();
    if (node->init())
    {
        node->setClippingRegion(clippingRegion);
        node->autorelease();
    }
    else
        AX_SAFE_DELETE(node);

    return node;
}

ClippingRectangleNode* ClippingRectangleNode::create()
{
    ClippingRectangleNode* node = new ClippingRectangleNode();
    if (node->init())
        node->autorelease();
    else
        AX_SAFE_DELETE(node);

    return node;
}

void ClippingRectangleNode::setClippingRegion(const Rect& clippingRegion)
{
    _clippingRegion = clippingRegion;
}

void ClippingRectangleNode::onBeforeVisitScissor()
{
    if (_clippingEnabled)
    {
        auto renderer   = _director->getRenderer();
        _oldScissorTest = renderer->getScissorTest();
        renderer->setScissorTest(true);

        float scaleX = _scaleX;
        float scaleY = _scaleY;
        Node* parent = this->getParent();
        while (parent)
        {
            scaleX *= parent->getScaleX();
            scaleY *= parent->getScaleY();
            parent = parent->getParent();
        }

        const Point pos = convertToWorldSpace(Point(_clippingRegion.origin.x, _clippingRegion.origin.y));
        GLView* glView  = _director->getGLView();
        glView->setScissorInPoints(pos.x, pos.y, _clippingRegion.size.width * scaleX,
                                   _clippingRegion.size.height * scaleY);
    }
}

void ClippingRectangleNode::onAfterVisitScissor()
{
    if (_clippingEnabled)
        _director->getRenderer()->setScissorTest(_oldScissorTest);
}

void ClippingRectangleNode::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    auto beforeVisitCmdScissor = renderer->nextCallbackCommand();
    beforeVisitCmdScissor->init(_globalZOrder);
    beforeVisitCmdScissor->func = AX_CALLBACK_0(ClippingRectangleNode::onBeforeVisitScissor, this);
    renderer->addCommand(beforeVisitCmdScissor);

    Node::visit(renderer, parentTransform, parentFlags);

    auto afterVisitCmdScissor = renderer->nextCallbackCommand();
    afterVisitCmdScissor->init(_globalZOrder);
    afterVisitCmdScissor->func = AX_CALLBACK_0(ClippingRectangleNode::onAfterVisitScissor, this);
    renderer->addCommand(afterVisitCmdScissor);
}

NS_AX_END
