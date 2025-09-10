/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2014-2020 Simdsoft Limited - All Rights Reserved
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "axmol/ui/UIHelper.h"
#include "axmol/ui/UIWidget.h"
#include "axmol/ui/UILayoutComponent.h"
#include "axmol/base/Director.h"
#include "axmol/base/text_utils.h"

namespace ax
{

namespace ui
{

static bool _activeLayout = true;

Widget* Helper::seekWidgetByTag(Widget* root, int tag)
{
    if (!root)
    {
        return nullptr;
    }
    if (root->getTag() == tag)
    {
        return root;
    }
    const auto& arrayRootChildren = root->getChildren();
    ssize_t length                = arrayRootChildren.size();
    for (ssize_t i = 0; i < length; i++)
    {
        Widget* child = dynamic_cast<Widget*>(arrayRootChildren.at(i));
        if (child)
        {
            Widget* res = seekWidgetByTag(child, tag);
            if (res != nullptr)
            {
                return res;
            }
        }
    }
    return nullptr;
}

Widget* Helper::seekWidgetByName(Widget* root, std::string_view name)
{
    if (!root)
    {
        return nullptr;
    }
    if (root->getName() == name)
    {
        return root;
    }
    const auto& arrayRootChildren = root->getChildren();
    for (auto&& subWidget : arrayRootChildren)
    {
        Widget* child = dynamic_cast<Widget*>(subWidget);
        if (child)
        {
            Widget* res = seekWidgetByName(child, name);
            if (res != nullptr)
            {
                return res;
            }
        }
    }
    return nullptr;
}

/*temp action*/
Widget* Helper::seekActionWidgetByActionTag(Widget* root, int tag)
{
    if (!root)
    {
        return nullptr;
    }
    if (root->getActionTag() == tag)
    {
        return root;
    }
    const auto& arrayRootChildren = root->getChildren();
    for (auto&& subWidget : arrayRootChildren)
    {
        Widget* child = dynamic_cast<Widget*>(subWidget);
        if (child)
        {
            Widget* res = seekActionWidgetByActionTag(child, tag);
            if (res != nullptr)
            {
                return res;
            }
        }
    }
    return nullptr;
}

std::string Helper::getSubStringOfUTF8String(std::string_view str,
                                             std::string::size_type start,
                                             std::string::size_type length)
{
    std::u32string utf32;
    if (!text_utils::UTF8ToUTF32(str, utf32))
    {
        AXLOGE("Can't convert string to UTF-32: {}", str);
        return "";
    }
    if (utf32.size() < start)
    {
        AXLOGE("'start' is out of range: {}, {}", static_cast<int32_t>(start), str);
        return "";
    }
    std::string result;
    if (!text_utils::UTF32ToUTF8(utf32.substr(start, length), result))
    {
        AXLOGE("Can't convert internal UTF-32 string to UTF-8: {}", str);
        return "";
    }
    return result;
}

void Helper::changeLayoutSystemActiveState(bool bActive)
{
    _activeLayout = bActive;
}
void Helper::doLayout(ax::Node* rootNode)
{
    if (!_activeLayout)
    {
        return;
    }

    for (auto&& node : rootNode->getChildren())
    {
        auto com     = node->getComponent(__LAYOUT_COMPONENT_NAME);
        Node* parent = node->getParent();
        if (nullptr != com && nullptr != parent)
        {
            LayoutComponent* layoutComponent = (LayoutComponent*)com;

            layoutComponent->refreshLayout();
        }
    }
}

Rect Helper::restrictCapInsetRect(const ax::Rect& capInsets, const Vec2& textureSize)
{
    float x      = capInsets.origin.x;
    float y      = capInsets.origin.y;
    float width  = capInsets.size.width;
    float height = capInsets.size.height;

    if (textureSize.width < width)
    {
        x     = textureSize.width / 2.0f;
        width = textureSize.width > 0 ? 1.0f : 0.0f;
    }
    if (textureSize.height < height)
    {
        y      = textureSize.height / 2.0f;
        height = textureSize.height > 0 ? 1.0f : 0.0f;
    }
    return Rect(x, y, width, height);
}

Rect Helper::convertBoundingBoxToScreen(Node* node)
{
    auto director   = Director::getInstance();
    auto renderView = director->getRenderView();
    auto winSize    = renderView->getWindowSize();

    auto logicalSize = director->getLogicalSize();
    auto leftBottom  = node->convertToWorldSpace(Point::ZERO);

    auto contentSize = node->getContentSize();
    auto rightTop    = node->convertToWorldSpace(Point(contentSize.width, contentSize.height));

    auto uiLeft   = winSize.width / 2 + (leftBottom.x - logicalSize.width / 2) * renderView->getScaleX();
    auto uiTop    = winSize.height / 2 - (rightTop.y - logicalSize.height / 2) * renderView->getScaleY();
    auto uiWidth  = (rightTop.x - leftBottom.x) * renderView->getScaleX();
    auto uiHeight = (rightTop.y - leftBottom.y) * renderView->getScaleY();

    return Rect(uiLeft, uiTop, uiWidth, uiHeight);
}

#pragma region Layout helper
Vec2 Helper::s_designSize;
float Helper::s_adjustedScale = 1.0f;

void Helper::setDesignSizeFixedEdge(const Vec2& designSize)
{
    Helper::s_designSize = designSize;

    // Set the design resolution
    RenderView* pERenderView = Director::getInstance()->getRenderView();
    const Vec2& windowSize   = pERenderView->getWindowSize();

    // Vec2 lsSize = lsaSize;

    float scaleX = (float)windowSize.width / designSize.width;
    float scaleY = (float)windowSize.height / designSize.height;

    // adjustedScale = 0.0f; // MAX(scaleX, scaleY);
    if (scaleX < scaleY)
    {
        pERenderView->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::FIXED_WIDTH);
    }
    else
    {
        pERenderView->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::FIXED_HEIGHT);
    }
}

void Helper::setDesignSizeNoBorder(const Vec2& designSize)
{
    // save smart size
    Helper::s_designSize = designSize;

    // Set the design resolution
    RenderView* pERenderView = Director::getInstance()->getRenderView();
    const Vec2& windowSize   = pERenderView->getWindowSize();

    // Vec2 lsSize = lsaSize;

    float scaleX = (float)windowSize.width / Helper::s_designSize.width;
    float scaleY = (float)windowSize.height / Helper::s_designSize.height;

    Helper::s_adjustedScale = 0.0f;  // MAX(scaleX, scaleY);
    if (scaleX > scaleY)
    {
        Helper::s_adjustedScale = scaleX / (windowSize.height / Helper::s_designSize.height);
    }
    else
    {
        Helper::s_adjustedScale = scaleY / (windowSize.width / Helper::s_designSize.width);
    }

    AXLOGD("x: {}; y: {}; scale: {}", scaleX, scaleY, s_adjustedScale);

    pERenderView->setDesignResolutionSize(Helper::s_designSize.width * s_adjustedScale,
                                          Helper::s_designSize.height * s_adjustedScale, ResolutionPolicy::NO_BORDER);
}

ax::Vec2 Helper::getVisibleOrigin(void)
{
    const auto& adjustedDesignSize = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    return ax::Vec2((adjustedDesignSize.width - Helper::s_designSize.width) * .5f,
                    (adjustedDesignSize.height - Helper::s_designSize.height) * .5f);
}

Vec2 Helper::getVisibleSize(void)
{
    return Helper::s_designSize;
}

/// Get node group size
Vec2 Helper::getNodeGroupSize(const std::vector<Node*>& nodes)
{
    if (nodes.empty())
    {
        return Vec2::ZERO;
    }

    // group nodes locators
    float minX = getNodeLeft(nodes[0]),
          maxX = getNodeLeft(nodes[0]) + nodes[0]->getContentSize().width /* * nodes[0]->getScaleX()*/;
    float minY = getNodeTop(nodes[0]),
          maxY = getNodeTop(nodes[0]) + nodes[0]->getContentSize().height /* * nodes[0]->getScaleY()*/;

    for (size_t index = 1; index < nodes.size(); ++index)
    {
        Node* child = nodes[index];
        auto x      = getNodeLeft(child);
        if (minX > x)
        {
            minX = x;
        }
        if (maxX < x + child->getContentSize().width /* * child->getScaleX()*/)
        {
            maxX = x + child->getContentSize().width /* * child->getScaleX()*/;
        }

        auto y = getNodeTop(child);
        if (minY > y)
        {
            minY = y;
        }
        if (maxY < y + child->getContentSize().height /* + child->getScaleY()*/)
        {
            maxY = y + child->getContentSize().height /* + child->getScaleY()*/;
        }
    }

    float groupWidth  = maxX - minX;
    float groupHeight = maxY - minY;

    return Vec2(groupWidth, groupHeight);
}

/// Set nodes group size
void Helper::setNodeGroupSize(const std::vector<Node*>& nodes, const Vec2& newSize)
{
    Vec2 groupSize = getNodeGroupScaledSize(nodes);
    if (groupSize.height == 0 || groupSize.width == 0 || newSize.width == 0 || newSize.height == 0)
    {
        return;
    }

    float scaleX = newSize.width / groupSize.width;
    float scaleY = newSize.height / groupSize.height;

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        auto detail = dynamic_cast<ax::Sprite*>(*iter);
        if (detail != nullptr)
        {
            /*auto scaledWidth = (detail->getContentWidth() * detail->getScaleX());
            auto scaledHeight = (detail->getContentHeight() * detail->getScaleY());
            scaledWidth *= scaleX;
            scaledHeight *= scaleY;*/
            detail->setScale(detail->getScaleX() * scaleX, detail->getScaleY() * scaleY);
        }
        else
        {
            (*iter)->setContentSize(
                Vec2((*iter)->getContentSize().width * scaleX, (*iter)->getContentSize().height * scaleY));
        }
    }
}

Vec2 Helper::getNodeGroupScaledSize(const std::vector<Node*>& nodes)
{
    if (nodes.empty())
    {
        return Vec2::ZERO;
    }

    auto scale = getScale2D(nodes[0]);

    // group nodes locators
    float minX = Helper::getNodeLeft(nodes[0]),
          maxX = Helper::getNodeLeft(nodes[0]) + nodes[0]->getContentSize().width * scale.x;
    float minY = Helper::getNodeTop(nodes[0]),
          maxY = Helper::getNodeTop(nodes[0]) + nodes[0]->getContentSize().height * scale.y;

    for (size_t index = 1; index < nodes.size(); ++index)
    {
        Node* child = nodes[index];

        scale = getScale2D(child);

        auto x = Helper::getNodeLeft(child);
        if (minX > x)
        {
            minX = x;
        }
        if (maxX < x + child->getContentSize().width * scale.x)
        {
            maxX = x + child->getContentSize().width * scale.x;
        }

        auto y = Helper::getNodeTop(child);
        if (minY > y)
        {
            minY = y;
        }
        if (maxY < y + child->getContentSize().height * scale.y)
        {
            maxY = y + child->getContentSize().height * scale.y;
        }
    }

    float groupWidth  = maxX - minX;
    float groupHeight = maxY - minY;

    return Vec2(groupWidth, groupHeight);
}

/// Get Node group left
float Helper::getNodeGroupLeft(const std::vector<Node*>& nodes)
{
    size_t index  = 0;
    float minLeft = getNodeLeft(nodes[index]);
    for (index = 1; index < nodes.size(); ++index)
    {
        if (minLeft > getNodeLeft(nodes[index]))
        {
            minLeft = getNodeLeft(nodes[index]);
        }
    }
    return minLeft;
}

/// Get node group top
float Helper::getNodeGroupTop(const std::vector<Node*>& nodes)
{
    size_t index = 0;
    float minTop = getNodeTop(nodes[index]);
    for (index = 1; index < nodes.size(); ++index)
    {
        if (minTop > getNodeTop(nodes[index]))
        {
            minTop = getNodeTop(nodes[index]);
        }
    }
    return minTop;
}

/// Get node group right
float Helper::getNodeGroupRight(const std::vector<Node*>& nodes)
{
    size_t index   = 0;
    float minRight = getNodeRight(nodes[index]);
    for (index = 1; index < nodes.size(); ++index)
    {
        if (minRight > getNodeRight(nodes[index]))
        {
            minRight = getNodeRight(nodes[index]);
        }
    }
    return minRight;
}

/// Get node group bottom
float Helper::getNodeGroupBottom(const std::vector<Node*>& nodes)
{
    size_t index    = 0;
    float minBottom = getNodeBottom(nodes[index]);
    for (index = 1; index < nodes.size(); ++index)
    {
        if (minBottom > getNodeBottom(nodes[index]))
        {
            minBottom = getNodeBottom(nodes[index]);
        }
    }
    return minBottom;
}

/*
** setNodeGroupLeft
**
*/
void Helper::setNodeGroupLeft(const std::vector<Node*>& nodes, float left)
{
    float delta = left - getNodeGroupLeft(nodes);

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        setNodeLeft((*iter), getNodeLeft((*iter)) + delta);
    }
}

/*
** setNodeGroupLeft
**
*/
void Helper::setNodeGroupTop(const std::vector<Node*>& nodes, float top)
{
    float delta = top - getNodeGroupTop(nodes);

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        setNodeTop((*iter), getNodeTop((*iter)) + delta);
    }
}

void Helper::setNodeGroupLT(const std::vector<Node*>& nodes, const ax::Vec2& p)
{
    float deltaL = p.x - getNodeGroupLeft(nodes);
    float deltaT = p.y - getNodeGroupTop(nodes);

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        setNodeLT((*iter), ax::Vec2(getNodeLeft((*iter)) + deltaL, getNodeTop((*iter)) + deltaT));
    }
}

/*
** setNodeGroupRight
**
*/
void Helper::setNodeGroupRight(const std::vector<Node*>& nodes, float right)
{
    float delta = right - getNodeGroupRight(nodes);

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        setNodeRight((*iter), getNodeRight((*iter)) + delta);
    }
}

/*
** setNodeGroupRight
**
*/
void Helper::setNodeGroupBottom(const std::vector<Node*>& nodes, float bottom)
{
    float delta = bottom - getNodeGroupBottom(nodes);

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        setNodeBottom((*iter), getNodeBottom((*iter)) + delta);
    }
}

void Helper::moveNodeGroupHorizontally(const std::vector<Node*>& nodes, float delta)
{
    for (auto&& node : nodes)
    {
        node->setPositionX(node->getPositionX() + delta);
    }
}

void Helper::moveNodeGroupVertically(const std::vector<Node*>& nodes, float delta)
{
    for (auto&& node : nodes)
    {
        node->setPositionY(node->getPositionY() - delta);
    }
}

/* @brief: group layout and alignment
** @remark:
*/
/// <summary>
/// 水平居中
/// </summary>
/// <param name="nodes"></param>
void Helper::centerHorizontally(const std::vector<Node*>& nodes)
{
    if (nodes.empty())
    {
        return;
    }
    else if (nodes.size() == 1)
    {
        centerNodeX(nodes[0]);
        return;
    }

    // group nodes locators
    float minX = getNodeLeft(nodes[0]), maxX = getNodeLeft(nodes[0]) + nodes[0]->getContentSize().width;

    for (size_t index = 1; index < nodes.size(); ++index)
    {
        Node* child = nodes[index];
        auto x      = getNodeLeft(child);
        if (minX > x)
        {
            minX = x;
        }
        if (maxX < x + child->getContentSize().width)
        {
            maxX = x + child->getContentSize().width;
        }
    }

    float groupWidth = maxX - minX;
    float dist       = ((nodes[0]->getParent()->getContentSize().width - groupWidth) / 2.0f) - minX;

    for (size_t index = 0; index < nodes.size(); ++index)
    {
        Node* child = nodes[index];
        setNodeLeft(child, getNodeLeft(child) + dist, 0);
    }
}
/// <summary>
/// 垂直居中
/// </summary>
/// <param name="nodes"></param>
void Helper::centerVertically(const std::vector<Node*>& nodes)
{
    if (nodes.empty())
    {
        return;
    }
    else if (nodes.size() == 1)
    {
        centerNodeY(nodes[0]);
        return;
    }

    // group nodes locators
    float minY = getNodeTop(nodes[0]), maxY = getNodeTop(nodes[0]) + nodes[0]->getContentSize().height;
    float y = 0.0f;

    std::for_each(nodes.begin() + 1, nodes.end(), [&](Node* child) -> void {
        if (minY > (y = getNodeTop(child)))
        {
            minY = y;
        }
        if (maxY < y + child->getContentSize().height)
        {
            maxY = y + child->getContentSize().height;
        }
    });

    float groupHeight = maxY - minY;
    float dist        = ((nodes[0]->getParent()->getContentSize().height - groupHeight) / 2.0f) - minY;
    std::for_each(nodes.begin(), nodes.end(),
                  [dist](Node* child) -> void { setNodeTop(child, getNodeTop(child) + dist); });
}

/// <summary>
/// 居中
/// </summary>
/// <param name="nodes"></param>
void Helper::centerToParent(const std::vector<Node*>& nodes)
{
    if (nodes.empty())
    {
        return;
    }
    else if (nodes.size() == 1)
    {
        Helper::centerNode(nodes[0]);
        return;
    }

    // group nodes locators
    float minX = getNodeLeft(nodes[0]), maxX = getNodeLeft(nodes[0]) + nodes[0]->getContentSize().width;
    float minY = getNodeTop(nodes[0]), maxY = getNodeTop(nodes[0]) + nodes[0]->getContentSize().height;

    std::for_each(nodes.begin() + 1, nodes.end(), [&](Node* child) -> void {
        auto x = getNodeLeft(child);
        if (minX > x)
        {
            minX = x;
        }
        if (maxX < x + child->getContentSize().width)
        {
            maxX = x + child->getContentSize().width;
        }

        auto y = getNodeTop(child);
        if (minY > y)
        {
            minY = y;
        }
        if (maxY < y + child->getContentSize().height)
        {
            maxY = y + child->getContentSize().height;
        }
    });

    float groupWidth  = maxX - minX;
    float groupHeight = maxY - minY;

    float distX = ((nodes[0]->getParent()->getContentSize().width - groupWidth) / 2.0f) - minX;
    float distY = ((nodes[0]->getParent()->getContentSize().height - groupHeight) / 2.0f) - minY;

    std::for_each(nodes.begin(), nodes.end(), [distX, distY](Node* child) -> void {
        setNodeLT(child, ax::Point(getNodeLeft(child) + distX, getNodeTop(child) + distY));
    });
}

/// <summary>
/// 左对齐
/// </summary>
/// <param name="nodes"></param>
void Helper::alignLefts(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index  = 0;
    float minLeft = getNodeLeft(nodes[index]);
    for (index = 1; index < nodes.size(); ++index)
    {
        if (minLeft > getNodeLeft(nodes[index]))
        {
            minLeft = getNodeLeft(nodes[index]);
        }
    }
    std::for_each(nodes.begin(), nodes.end(), [minLeft](Node* child) -> void { setNodeLeft(child, minLeft); });
}

/// <summary>
/// 右对齐
/// </summary>
/// <param name="nodes"></param>
void Helper::alignRights(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index   = 0;
    float maxRight = getNodeLeft(nodes[index]) + nodes[index]->getContentSize().width;
    for (index = 1; index < nodes.size(); ++index)
    {
        if (maxRight < (getNodeLeft(nodes[index]) + nodes[index]->getContentSize().width))
        {
            maxRight = getNodeLeft(nodes[index]) + nodes[index]->getContentSize().width;
        }
    }

    std::for_each(nodes.begin(), nodes.end(),
                  [maxRight](Node* child) -> void { setNodeLeft(child, maxRight - child->getContentSize().width); });
}

/// <summary>
/// 顶端对齐
/// </summary>
/// <param name="nodes"></param>
void Helper::alignTops(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index = 0;
    float minTop = getNodeTop(nodes[index]);
    for (index = 1; index < nodes.size(); ++index)
    {
        if (minTop > getNodeTop(nodes[index]))
        {
            minTop = getNodeTop(nodes[index]);
        }
    }
    std::for_each(nodes.begin(), nodes.end(), [minTop](Node* child) -> void { setNodeTop(child, minTop); });
}

/// <summary>
/// 底端对齐
/// </summary>
/// <param name="nodes"></param>
void Helper::alignBottoms(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index    = 0;
    float maxBottom = getNodeTop(nodes[index]) + nodes[index]->getContentSize().height;
    for (index = 1; index < nodes.size(); ++index)
    {
        if (maxBottom < (getNodeTop(nodes[index]) + nodes[index]->getContentSize().height))
        {
            maxBottom = getNodeTop(nodes[index]) + nodes[index]->getContentSize().height;
        }
    }

    std::for_each(nodes.begin(), nodes.end(),
                  [maxBottom](Node* child) -> void { setNodeTop(child, maxBottom - child->getContentSize().height); });
}

/// <summary>
/// 水平对齐
/// </summary>
/// <param name="nodes"></param>
void Helper::alignHorizontals(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index = 0;
    float minCenterY, maxCenterY;
    Node* child = nodes[index];
    minCenterY = maxCenterY = getNodeBottom(child, 0.5f);  // child.GetX(0.5f);

    for (index = 1; index < nodes.size(); ++index)
    {
        child        = nodes[index];
        auto centerY = getNodeBottom(child, 0.5f);  // child.GetX(0.5f);
        if (minCenterY > centerY)
        {
            minCenterY = centerY;
        }
        if (maxCenterY < centerY)
        {
            maxCenterY = centerY;
        }
    }

    std::for_each(nodes.begin(), nodes.end(), [minCenterY, maxCenterY](Node* child) -> void {
        setNodeBottom(child, (minCenterY + maxCenterY) * 0.5f,
                      0.5f);  // child.SetX((minCenterX + maxCenterX) * 0.5f, 0.5f);
    });
}

/// <summary>
/// 垂直对齐
/// </summary>
/// <param name="nodes"></param>
void Helper::alignVerticals(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index = 0;
    float minCenterX, maxCenterX;
    Node* child = nodes[index];
    minCenterX = maxCenterX = getNodeLeft(child, 0.5f);  // child.GetX(0.5f);

    for (index = 1; index < nodes.size(); ++index)
    {
        child        = nodes[index];
        auto centerX = getNodeLeft(child, 0.5f);  // child.GetX(0.5f);
        if (minCenterX > centerX)
        {
            minCenterX = centerX;
        }
        if (maxCenterX < centerX)
        {
            maxCenterX = centerX;
        }
    }

    std::for_each(nodes.begin(), nodes.end(), [minCenterX, maxCenterX](Node* child) -> void {
        setNodeLeft(child, (minCenterX + maxCenterX) * 0.5f,
                    0.5f);  // child.SetX((minCenterX + maxCenterX) * 0.5f, 0.5f);
    });
}

/// <summary>
/// 中心原点对齐
/// </summary>
/// <param name="nodes"></param>
void Helper::alignCenters(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    alignHorizontals(nodes);
    alignVerticals(nodes);
}

/// <summary>
/// 使宽度相等
/// </summary>
/// <param name="nodes"></param>
void Helper::makeSameWidth(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index   = 0;
    float maxWidth = nodes[index]->getContentSize().width;
    for (index = 1; index < nodes.size(); ++index)
    {
        if (maxWidth < nodes[index]->getContentSize().width)
        {
            maxWidth = nodes[index]->getContentSize().width;
        }
    }

    std::for_each(nodes.begin(), nodes.end(), [maxWidth](Node* child) -> void {
        child->setContentSize(Vec2(maxWidth, child->getContentSize().height));
    });
}

/// <summary>
/// 使高度相等
/// </summary>
/// <param name="nodes"></param>
void Helper::makeSameHeight(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index    = 0;
    float minHeight = nodes[index]->getContentSize().height;
    for (index = 1; index < nodes.size(); ++index)
    {
        if (minHeight > nodes[index]->getContentSize().height)
        {
            minHeight = nodes[index]->getContentSize().height;
        }
    }

    std::for_each(nodes.begin(), nodes.end(), [minHeight](Node* child) -> void {
        child->setContentSize(Vec2(child->getContentSize().width, minHeight));
    });
}

/// <summary>
/// 使得大小相等
/// </summary>
/// <param name="nodes"></param>
void Helper::makeSameSize(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    makeSameHeight(nodes);
    makeSameWidth(nodes);
}

/// <summary>
/// 使水平间距相等
/// </summary>
/// <param name="nodes"></param>
void Helper::makeHorizontalSpacingEqual(std::vector<Node*>& nodes)
{
    if (nodes.size() < 3)
        return;

    // sort by x firstly
    std::sort(nodes.begin(), nodes.end(),
              [](Node* const left, Node* const right) { return getNodeLeft(left) < getNodeLeft(right); });

    float avgHSpacing = 0;
    for (size_t index = 0; index < nodes.size() - 1; ++index)
    {
        auto child     = nodes[index];
        auto childNext = nodes[index + 1];
        avgHSpacing += (getNodeLeft(childNext) - getNodeLeft(child) - child->getContentSize().width);
    }

    avgHSpacing /= (nodes.size() - 1);

    if (avgHSpacing < 0.0f)
        avgHSpacing = 0.0f;

    Helper::makeHorizontalSpacingEqual(nodes, avgHSpacing);
}

/// <summary>
/// 使水平间距相等
/// </summary>
/// <param name="nodes"></param>
void Helper::makeHorizontalSpacingEqual(const std::vector<Node*>& nodes, float theSpacing)
{
    for (size_t index = 0; index < nodes.size() - 1; ++index)
    {
        auto child     = nodes[index];
        auto childNext = nodes[index + 1];
        setNodeLeft(childNext, theSpacing + getNodeLeft(child) + child->getContentSize().width);
    }
}

/// <summary>
/// 使垂直间距相等
/// </summary>
/// <param name="nodes"></param>
void Helper::makeVerticalSpacingEqual(std::vector<Node*>& nodes)
{
    if (nodes.size() < 3)
        return;

    float avgVSpacing = 0;

    // sort by y firstly
    std::sort(nodes.begin(), nodes.end(),
              [](Node* const left, Node* const right) -> bool { return getNodeTop(left) < getNodeTop(right); });
#
    for (size_t index = 0; index < nodes.size() - 1; ++index)
    {
        auto child     = nodes[index];
        auto childNext = nodes[index + 1];
        avgVSpacing += (getNodeTop(childNext) - getNodeTop(child) - child->getContentSize().height);
    }

    avgVSpacing /= (nodes.size() - 1);

    if (avgVSpacing < 0.0f)
        avgVSpacing = 0.0f;

    Helper::makeVerticalSpacingEqual(nodes, avgVSpacing);
}

void Helper::increaseHorizontalSpacing(std::vector<Node*>& nodes, float theSpacing)
{
    if (nodes.size() < 2)
        return;

    std::sort(nodes.begin(), nodes.end(),
              [](Node* const left, Node* const right) -> bool { return getNodeLeft(left) < getNodeLeft(right); });

    for (size_t idx = 1; idx < nodes.size(); ++idx)
    {
        nodes[idx]->setPositionX(nodes[idx]->getPositionX() + theSpacing * idx);
    }
}

void Helper::increaseVerticalSpacing(std::vector<Node*>& nodes, float theSpacing)
{
    if (nodes.size() < 2)
        return;

    std::sort(nodes.begin(), nodes.end(),
              [](Node* const left, Node* const right) -> bool { return getNodeTop(left) < getNodeTop(right); });

    for (size_t idx = 1; idx < nodes.size(); ++idx)
    {
        nodes[idx]->setPositionY(nodes[idx]->getPositionY() - theSpacing * idx);
    }
}

void Helper::decreaseHorizontalSpacing(std::vector<Node*>& nodes, float theSpacing)
{
    increaseHorizontalSpacing(nodes, -theSpacing);
}

void Helper::decreaseVerticalSpacing(std::vector<Node*>& nodes, float theSpacing)
{
    increaseVerticalSpacing(nodes, -theSpacing);
}

void Helper::removeHorizontalSpacing(const std::vector<Node*>& nodes)
{
    Helper::makeHorizontalSpacingEqual(nodes, 0);
}

void Helper::removeVerticalSpacing(const std::vector<Node*>& nodes)
{
    Helper::makeVerticalSpacingEqual(nodes, 0);
}

/// <summary>
/// 使垂直间距相等
/// </summary>
/// <param name="nodes"></param>
void Helper::makeVerticalSpacingEqual(const std::vector<Node*>& nodes, float theSpacing)
{
    for (size_t index = 0; index < nodes.size() - 1; ++index)
    {
        auto child     = nodes[index];
        auto childNext = nodes[index + 1];
        setNodeTop(childNext, theSpacing + getNodeTop(child) + child->getContentSize().height);
    }
}

// ----------------- Helper::VisibleRect --------------------------

ax::Rect Helper::VisibleRect::s_ScreenVisibleRect;

/// x-studio: when design resolution changed, should call this func.
void Helper::VisibleRect::refresh(void)
{
    auto director              = Director::getInstance();
    s_ScreenVisibleRect.origin = Director::getInstance()->getVisibleOrigin();
    s_ScreenVisibleRect.size   = Director::getInstance()->getVisibleSize();
}

void Helper::VisibleRect::lazyInit()
{
    if (s_ScreenVisibleRect.size.width == 0.0f && s_ScreenVisibleRect.size.height == 0.0f)
    {
        auto director   = Director::getInstance();
        auto renderView = director->getRenderView();

        if (renderView->getResolutionPolicy() == ResolutionPolicy::NO_BORDER)
        {
            s_ScreenVisibleRect.origin = director->getVisibleOrigin();
            s_ScreenVisibleRect.size   = director->getVisibleSize();
        }
        else
        {
            s_ScreenVisibleRect.origin = Helper::getVisibleOrigin();
            s_ScreenVisibleRect.size   = Helper::getVisibleSize();
        }
    }
}

ax::Rect Helper::VisibleRect::getScreenVisibleRect()
{
    lazyInit();
    return ax::Rect(s_ScreenVisibleRect.origin.x, s_ScreenVisibleRect.origin.y, s_ScreenVisibleRect.size.width,
                    s_ScreenVisibleRect.size.height);
}

Vec2 Helper::VisibleRect::size()
{
    lazyInit();
    return s_ScreenVisibleRect.size;
}

Point Helper::VisibleRect::left()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x, s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height / 2);
}

Point Helper::VisibleRect::right()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width,
                     s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height / 2);
}

Point Helper::VisibleRect::top()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width / 2,
                     s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height);
}

Point Helper::VisibleRect::bottom()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width / 2, s_ScreenVisibleRect.origin.y);
}

Point Helper::VisibleRect::center()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width / 2,
                     s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height / 2);
}

Point Helper::VisibleRect::leftTop()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x, s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height);
}

Point Helper::VisibleRect::rightTop()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width,
                     s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height);
}

Point Helper::VisibleRect::leftBottom()
{
    lazyInit();
    return s_ScreenVisibleRect.origin;
}

Point Helper::VisibleRect::rightBottom()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width, s_ScreenVisibleRect.origin.y);
}

/// visual screen
float Helper::VisibleRect::getNodeLeft(Node* pNode)
{
    ax::Point ptLocal(Helper::getNodeLeft(pNode), 0);
    auto ptWorld = pNode->getParent()->convertToWorldSpace(ptLocal);
    return ptWorld.x;
}
float Helper::VisibleRect::getNodeBottom(Node* pNode)
{
    ax::Point ptLocal(0, Helper::getNodeBottom(pNode));
    auto ptWorld = pNode->getParent()->convertToWorldSpace(ptLocal);

    return ptWorld.y;
}
float Helper::VisibleRect::getNodeRight(Node* pNode)
{
    ax::Point ptLocal(Helper::getNodeLeft(pNode) + pNode->getContentSize().width /* * pNode->getScaleX()*/, 0);
    auto ptWorld = pNode->getParent()->convertToWorldSpace(ptLocal);

    auto visibleRect = Helper::VisibleRect::getScreenVisibleRect();
    return visibleRect.size.width - ptWorld.x;
}
float Helper::VisibleRect::getNodeTop(Node* pNode)
{
    ax::Point ptLocal(0, Helper::getNodeBottom(pNode) + pNode->getContentSize().height /* * pNode->getScaleY()*/);
    auto ptWorld     = pNode->getParent()->convertToWorldSpace(ptLocal);
    auto visibleRect = Helper::VisibleRect::getScreenVisibleRect();

    return visibleRect.size.height - ptWorld.y;
}

void Helper::VisibleRect::setNodeLeft(Node* pNode, float left)
{
    AX_ASSERT(pNode);
    Vec2 scrSize         = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(0, scrSize.height) - Helper::VisibleRect::leftTop();
    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(adjustCoordByAnchor(size.width, achorPoint.x, left) /* - delta.x*/, 0);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionX(ptLocal.x);
}
void Helper::VisibleRect::setNodeTop(Node* pNode, float top)
{
    AX_ASSERT(pNode);
    Vec2 scrSize         = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(0, scrSize.height) - Helper::VisibleRect::leftTop();
    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(0, adjustCoordNegative(scrSize.height, size.height, achorPoint.y, top) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionY(ptLocal.y);
}
void Helper::VisibleRect::setNodeRight(Node* pNode, float right)
{
    AX_ASSERT(pNode);
    Vec2 scrSize         = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(scrSize.width, 0) - Helper::VisibleRect::rightBottom();
    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(adjustCoordNegative(scrSize.width, size.width, achorPoint.x, right) /* - delta.x*/, 0);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionX(ptLocal.x);
}
void Helper::VisibleRect::setNodeBottom(Node* pNode, float bottom)
{
    AX_ASSERT(pNode);
    Vec2 scrSize         = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(scrSize.width, 0) - Helper::VisibleRect::rightBottom();
    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(0, adjustCoordByAnchor(size.height, achorPoint.y, bottom) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionY(ptLocal.y);
}

void Helper::VisibleRect::setNodeLT(Node* pNode, const ax::Point& p)
{
    AX_ASSERT(pNode);
    Vec2 scrSize         = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(0, scrSize.height) - Helper::VisibleRect::leftTop();
    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    ax::Point ptWorld(adjustCoordByAnchor(size.width, achorPoint.x, p.x),
                      adjustCoordNegative(scrSize.height, size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void Helper::VisibleRect::setNodeRT(Node* pNode, const ax::Point& p)
{  // right top
    AX_ASSERT(pNode);
    Vec2 scrSize         = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(scrSize.width, scrSize.height) - Helper::VisibleRect::rightTop();
    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(adjustCoordNegative(scrSize.width, size.width, achorPoint.x, p.x) /* - delta.x*/,
                      adjustCoordNegative(scrSize.height, size.height, achorPoint.y, p.y) /* - delta.y*/);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void Helper::VisibleRect::setNodeLB(Node* pNode, const ax::Point& p)
{  // left bottom
    AX_ASSERT(pNode);
    Vec2 scrSize         = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(0, 0) - Helper::VisibleRect::leftBottom();
    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(adjustCoordByAnchor(size.width, achorPoint.x, p.x) - delta.x,
                      adjustCoordByAnchor(size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void Helper::VisibleRect::setNodeRB(Node* pNode, const ax::Point& p)
{  // right bottom
    AX_ASSERT(pNode);
    Vec2 scrSize         = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(scrSize.width, 0) - Helper::VisibleRect::rightBottom();
    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    ax::Point ptWorld(adjustCoordNegative(scrSize.width, size.width, achorPoint.x, p.x) /* - delta.x*/,
                      adjustCoordByAnchor(size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}

/// ratio position
void Helper::VisibleRect::setNodeNormalizedLT(Node* pNode, const ax::Point& ratio)
{
    AX_ASSERT(pNode);

    Vec2 scrSize    = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta = ax::Vec2(0, scrSize.height) - Helper::VisibleRect::leftTop();

    Vec2 vscrSize = Helper::VisibleRect::size();
    ax::Point p   = ax::Vec2(vscrSize.width * ratio.x, vscrSize.height * ratio.y);

    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    ax::Point ptWorld(adjustCoordByAnchor(size.width, achorPoint.x, p.x) - delta.x,
                      adjustCoordNegative(scrSize.height, size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void Helper::VisibleRect::setNodeNormalizedRT(Node* pNode, const ax::Point& ratio)
{  // right top
    AX_ASSERT(pNode);
    Vec2 scrSize    = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta = ax::Vec2(scrSize.width, scrSize.height) - Helper::VisibleRect::rightTop();

    Vec2 vscrSize = Helper::VisibleRect::size();
    ax::Point p   = ax::Vec2(vscrSize.width * ratio.x, vscrSize.height * ratio.y);

    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    ax::Point ptWorld(adjustCoordNegative(scrSize.width, size.width, achorPoint.x, p.x) - delta.x,
                      adjustCoordNegative(scrSize.height, size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void Helper::VisibleRect::setNodeNormalizedLB(Node* pNode, const ax::Point& ratio)
{  // left bottom
    AX_ASSERT(pNode);
    Vec2 scrSize    = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta = ax::Vec2(0, 0) - Helper::VisibleRect::leftBottom();

    Vec2 vscrSize = Helper::VisibleRect::size();
    ax::Point p   = ax::Vec2(vscrSize.width * ratio.x, vscrSize.height * ratio.y);

    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    ax::Point ptWorld(adjustCoordByAnchor(size.width, achorPoint.x, p.x) - delta.x,
                      adjustCoordByAnchor(size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void Helper::VisibleRect::setNodeNormalizedRB(Node* pNode, const ax::Point& ratio)
{  // right bottom
    AX_ASSERT(pNode);
    Vec2 scrSize    = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta = ax::Vec2(scrSize.width, 0) - Helper::VisibleRect::rightBottom();

    Vec2 vscrSize = Helper::VisibleRect::size();
    ax::Point p   = ax::Vec2(vscrSize.width * ratio.x, vscrSize.height * ratio.y);

    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    ax::Point ptWorld(adjustCoordNegative(scrSize.width, size.width, achorPoint.x, p.x) - delta.x,
                      adjustCoordByAnchor(size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}

void Helper::VisibleRect::setNodeNormalizedTop(Node* pNode, const float ratioTop)
{  // right top
    AX_ASSERT(pNode);
    Vec2 scrSize    = Director::getInstance()->getRenderView()->getDesignResolutionSize();
    ax::Point delta = ax::Vec2(scrSize.width, scrSize.height) - Helper::VisibleRect::rightTop();

    Vec2 vscrSize = Helper::VisibleRect::size();
    float top     = vscrSize.width * ratioTop;

    Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(0, adjustCoordNegative(scrSize.height, size.height, achorPoint.y, top) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionY(ptWorld.y);
}

void Helper::VisibleRect::setNodeNormalizedPositionX(ax::Node* pNode, float ratio)
{
    AX_ASSERT(pNode);
    ax::Rect visibleRect = Helper::Helper::VisibleRect::getScreenVisibleRect();
    ax::Point ptWorld(visibleRect.size.width * ratio + visibleRect.origin.x, 0);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionX(ptLocal.x);
}

void Helper::VisibleRect::setNodeNormalizedPositionY(ax::Node* pNode, float ratio)
{
    AX_ASSERT(pNode);
    ax::Rect visibleRect = Helper::Helper::VisibleRect::getScreenVisibleRect();

    ax::Point ptWorld(0, visibleRect.size.height * ratio + visibleRect.origin.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionY(ptLocal.y);
}
void Helper::VisibleRect::setNodeNormalizedPosition(Node* pNode, const ax::Point& ratio)
{
    AX_ASSERT(pNode);
    pNode->setIgnoreAnchorPointForPosition(false);
    pNode->setAnchorPoint(ax::Vec2(.5f, .5f));
    ax::Rect visibleRect = Helper::Helper::VisibleRect::getScreenVisibleRect();
    ax::Point ptWorld(visibleRect.size.width * ratio.x + visibleRect.origin.x,
                      visibleRect.size.height * ratio.y + visibleRect.origin.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}

#pragma endregion

}  // namespace ui

}  // namespace ax
