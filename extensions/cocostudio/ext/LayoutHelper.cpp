//
// Copyright (c) 2014-2017 purelib - All Rights Reserved
//
#include "LayoutHelper.h"

namespace LayoutHelper {
cocos2d::Rect ScreenVisibleRect::s_ScreenVisibleRect;

/// x-studio365: when design resolution changed, should call this func.
void ScreenVisibleRect::refresh(void)
{
    auto director = Director::getInstance();
    s_ScreenVisibleRect.origin = Director::getInstance()->getVisibleOrigin();
    s_ScreenVisibleRect.size = Director::getInstance()->getVisibleSize();
}

void ScreenVisibleRect::lazyInit()
{
    if (s_ScreenVisibleRect.size.width == 0.0f && s_ScreenVisibleRect.size.height == 0.0f)
    {
        auto director = Director::getInstance();
        auto glview = director->getOpenGLView();

        if (glview->getResolutionPolicy() == ResolutionPolicy::NO_BORDER) {
            s_ScreenVisibleRect.origin = director->getVisibleOrigin();
            s_ScreenVisibleRect.size = director->getVisibleSize();
        }
        else {
            s_ScreenVisibleRect.origin = nodes_layout::getVisibleOrigin();
            s_ScreenVisibleRect.size = nodes_layout::getVisibleSize();
        }
    }
}

cocos2d::Rect ScreenVisibleRect::getScreenVisibleRect()
{
    lazyInit();
    return cocos2d::Rect(s_ScreenVisibleRect.origin.x, s_ScreenVisibleRect.origin.y, s_ScreenVisibleRect.size.width, s_ScreenVisibleRect.size.height);
}

cocos2d::Size ScreenVisibleRect::size()
{
    lazyInit();
    return s_ScreenVisibleRect.size;
}

Point ScreenVisibleRect::left()
{
    lazyInit();
    return cocos2d::Point(s_ScreenVisibleRect.origin.x, s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height / 2);
}

Point ScreenVisibleRect::right()
{
    lazyInit();
    return cocos2d::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width, s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height / 2);
}

Point ScreenVisibleRect::top()
{
    lazyInit();
    return cocos2d::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width / 2, s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height);
}

Point ScreenVisibleRect::bottom()
{
    lazyInit();
    return cocos2d::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width / 2, s_ScreenVisibleRect.origin.y);
}

Point ScreenVisibleRect::center()
{
    lazyInit();
    return cocos2d::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width / 2, s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height / 2);
}

Point ScreenVisibleRect::leftTop()
{
    lazyInit();
    return cocos2d::Point(s_ScreenVisibleRect.origin.x, s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height);
}

Point ScreenVisibleRect::rightTop()
{
    lazyInit();
    return cocos2d::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width, s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height);
}

Point ScreenVisibleRect::leftBottom()
{
    lazyInit();
    return s_ScreenVisibleRect.origin;
}

Point ScreenVisibleRect::rightBottom()
{
    lazyInit();
    return cocos2d::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width, s_ScreenVisibleRect.origin.y);
}

const cocos2d::Point  nodes_layout::ANCHOR_CENTER = cocos2d::Point(0.5, 0.5); // center
const cocos2d::Point  nodes_layout::ANCHOR_LB = cocos2d::Point(0.0f, 0.0f);  // left bottom,
const cocos2d::Point  nodes_layout::ANCHOR_LT = cocos2d::Point(0.0f, 1.0f);  // left top
const cocos2d::Point  nodes_layout::ANCHOR_RB = cocos2d::Point(1.0f, 0.0f);  // rigth bottom
const cocos2d::Point  nodes_layout::ANCHOR_RT = cocos2d::Point(1.0f, 1.0f);  // rigth top

float nodes_layout::adjustedScale = 1.0f;
cocos2d::Size nodes_layout::designSize;

//cocos2d::Size nodes_layout::getScreenSize(void)
//{
//    return nodes_layout::designSize * nodes_layout::adjustedScale;
//}
//
//cocos2d::Vec2 nodes_layout::getSceenOffset(cocos2d::Point anchor)
//{
//    float offsetX = anchor.x * (nodes_layout::getScreenSize().width - nodes_layout::designSize.width);
//    float offsetY = anchor.y * (nodes_layout::getScreenSize().height - nodes_layout::designSize.height);
//    return cocos2d::Vec2(offsetX, offsetY);
//}

void nodes_layout::setDesignSizeNoBorder(const cocos2d::Size& designSize)
{
    // save smart size
    nodes_layout::designSize = designSize;

    // Set the design resolution//分辨率的大小
    GLView* pEGLView = Director::getInstance()->getOpenGLView();
    const cocos2d::Size& frameSize = pEGLView->getFrameSize();

    // Size lsSize = lsaSize;

    float scaleX = (float)frameSize.width / nodes_layout::designSize.width;
    float scaleY = (float)frameSize.height / nodes_layout::designSize.height;

    adjustedScale = 0.0f; // MAX(scaleX, scaleY);
    if (scaleX > scaleY) {
        adjustedScale = scaleX / (frameSize.height / nodes_layout::designSize.height);
    }
    else {
        adjustedScale = scaleY / (frameSize.width / nodes_layout::designSize.width);
    }

    cocos2d::log("x: %f; y: %f; scale: %f", scaleX, scaleY, adjustedScale);

    pEGLView->setDesignResolutionSize(nodes_layout::designSize.width * adjustedScale,
        nodes_layout::designSize.height * adjustedScale, ResolutionPolicy::NO_BORDER);
}

void nodes_layout::setDesignSizeFixedEdge(const Size& designSize)
{
    nodes_layout::designSize = designSize;

    // Set the design resolution//分辨率的大小
    GLView* pEGLView = Director::getInstance()->getOpenGLView();
    const Size& frameSize = pEGLView->getFrameSize();

    // Size lsSize = lsaSize;

    float scaleX = (float)frameSize.width / designSize.width;
    float scaleY = (float)frameSize.height / designSize.height;

    // adjustedScale = 0.0f; // MAX(scaleX, scaleY);
    if (scaleX < scaleY) {
        pEGLView->setDesignResolutionSize(designSize.width,
            designSize.height, ResolutionPolicy::FIXED_WIDTH);
    }
    else {
        pEGLView->setDesignResolutionSize(designSize.width,
            designSize.height, ResolutionPolicy::FIXED_HEIGHT);
    }
}

cocos2d::Vec2 nodes_layout::getVisibleOrigin(void)
{
    const auto& adjustedDesignSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    return cocos2d::Vec2((adjustedDesignSize.width - nodes_layout::designSize.width) * .5f,
        (adjustedDesignSize.height - nodes_layout::designSize.height) * .5f);
}

cocos2d::Size nodes_layout::getVisibleSize(void)
{
    return nodes_layout::designSize;
}

/// Get node group size
Size nodes_layout::getNodeGroupSize(const std::vector<Node*>& nodes)
{
    if (nodes.empty())
    {
        return Size::ZERO;
    }

    // group nodes locators
    float minX = getNodeLeft(nodes[0]), maxX = getNodeLeft(nodes[0]) + nodes[0]->getContentSize().width/* * nodes[0]->getScaleX()*/;
    float minY = getNodeTop(nodes[0]), maxY = getNodeTop(nodes[0]) + nodes[0]->getContentSize().height/* * nodes[0]->getScaleY()*/;
    float x = 0.0f, y = 0.0f;
    // float width = 0.0f, height = 0.f;
    for (size_t index = 1; index < nodes.size(); ++index) {
        Node* child = nodes[index];
        if (minX >(x = getNodeLeft(child)))
        {
            minX = x;
        }
        if (maxX < x + child->getContentSize().width/* * child->getScaleX()*/)
        {
            maxX = x + child->getContentSize().width/* * child->getScaleX()*/;
        }

        if (minY >(y = getNodeTop(child)))
        {
            minY = y;
        }
        if (maxY < y + child->getContentSize().height/* + child->getScaleY()*/)
        {
            maxY = y + child->getContentSize().height/* + child->getScaleY()*/;
        }
    }

    float groupWidth = maxX - minX;
    float groupHeight = maxY - minY;

    return Size(groupWidth, groupHeight);
}

/// Set nodes group size
void   nodes_layout::setNodeGroupSize(const std::vector<Node*>& nodes, const cocos2d::Size& newSize)
{
    cocos2d::Size groupSize = getNodeGroupScaledSize(nodes);
    if (groupSize.height == 0 || groupSize.width == 0 || newSize.width == 0 || newSize.height == 0)
    {
        return;
    }

    float scaleX = newSize.width / groupSize.width;
    float scaleY = newSize.height / groupSize.height;

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        auto detail = dynamic_cast<cocos2d::Sprite*>(*iter);
        if (detail != nullptr)
        {
            /*auto scaledWidth = (detail->getContentWidth() * detail->getScaleX());
            auto scaledHeight = (detail->getContentHeight() * detail->getScaleY());
            scaledWidth *= scaleX;
            scaledHeight *= scaleY;*/
            detail->setScale(detail->getScaleX() * scaleX, detail->getScaleY() * scaleY);
        }
        else { 
            (*iter)->setContentSize(
                Size(
                (*iter)->getContentSize().width * scaleX,
                (*iter)->getContentSize().height * scaleY));
        }
    }
}

Size nodes_layout::getNodeGroupScaledSize(const std::vector<Node*>& nodes)
{
    if (nodes.empty())
    {
        return Size::ZERO;
    }

    auto scale = getScale2D(nodes[0]);

    // group nodes locators
    float minX = nodes_layout::getNodeLeft(nodes[0]), maxX = nodes_layout::getNodeLeft(nodes[0]) + nodes[0]->getContentSize().width * scale.x;
    float minY = nodes_layout::getNodeTop(nodes[0]), maxY = nodes_layout::getNodeTop(nodes[0]) + nodes[0]->getContentSize().height * scale.y;
    float x = 0.0f, y = 0.0f;
    // float width = 0.0f, height = 0.f;
    for (size_t index = 1; index < nodes.size(); ++index) {
        Node* child = nodes[index];

        scale = getScale2D(child);

        if (minX >(x = nodes_layout::getNodeLeft(child)))
        {
            minX = x;
        }
        if (maxX < x + child->getContentSize().width * scale.x)
        {
            maxX = x + child->getContentSize().width * scale.x;
        }

        if (minY >(y = nodes_layout::getNodeTop(child)))
        {
            minY = y;
        }
        if (maxY < y + child->getContentSize().height * scale.y)
        {
            maxY = y + child->getContentSize().height * scale.y;
        }
    }

    float groupWidth = maxX - minX;
    float groupHeight = maxY - minY;

    return cocos2d::Size(groupWidth, groupHeight);
}

/// Get Node group left
float  nodes_layout::getNodeGroupLeft(const std::vector<Node*>& nodes)
{
    size_t index = 0;
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
float  nodes_layout::getNodeGroupTop(const std::vector<Node*>& nodes)
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
float  nodes_layout::getNodeGroupRight(const std::vector<Node*>& nodes)
{
    size_t index = 0;
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
float  nodes_layout::getNodeGroupBottom(const std::vector<Node*>& nodes)
{
    size_t index = 0;
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
void nodes_layout::setNodeGroupLeft(const std::vector<Node*>& nodes, float left)
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
void nodes_layout::setNodeGroupTop(const std::vector<Node*>& nodes, float top)
{
    float delta = top - getNodeGroupTop(nodes);

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        setNodeTop((*iter), getNodeTop((*iter)) + delta);
    }
}

void nodes_layout::setNodeGroupLT(const std::vector<Node*>& nodes, const cocos2d::Vec2& p)
{
    float deltaL = p.x - getNodeGroupLeft(nodes);
    float deltaT = p.y - getNodeGroupTop(nodes);

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        setNodeLT((*iter), cocos2d::Vec2(getNodeLeft((*iter)) + deltaL, getNodeTop((*iter)) + deltaT));
    }
}

/*
** setNodeGroupRight
**
*/
void nodes_layout::setNodeGroupRight(const std::vector<Node*>& nodes, float right)
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
void nodes_layout::setNodeGroupBottom(const std::vector<Node*>& nodes, float bottom)
{
    float delta = bottom - getNodeGroupBottom(nodes);

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        setNodeBottom((*iter), getNodeBottom((*iter)) + delta);
    }
}

void nodes_layout::moveNodeGroupHorizontally(const std::vector<Node*>& nodes, float delta)
{
    for (auto node : nodes)
    {
        node->setPositionX(node->getPositionX() + delta);
    }
}

void nodes_layout::moveNodeGroupVertically(const std::vector<Node*>& nodes, float delta)
{
    for (auto node : nodes)
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
void nodes_layout::centerHorizontally(const std::vector<Node*>& nodes)
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
    float x = 0.0f;
    float width = 0.0f;
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin() + 1, nodes.end(), [&](Node* child)->void{
        if (minX > (x = getNodeLeft(child)))
        {
            minX = x;
        }
        if (maxX < x + child->getContentSize().width)
        {
            maxX = x + child->getContentSize().width;
        }
    });
#else
    for (size_t index = 1; index < nodes.size(); ++index) {
        Node* child = nodes[index];
        if (minX >(x = getNodeLeft(child)))
        {
            minX = x;
        }
        if (maxX < x + child->getContentSize().width)
        {
            maxX = x + child->getContentSize().width;
        }
    }
#endif

    float groupWidth = maxX - minX;
    float dist = ((nodes[0]->getParent()->getContentSize().width - groupWidth) / 2.0f) - minX;
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(), [dist](Node* child)->void{
        setNodeLeft(child, getNodeLeft(child) + dist);
    });
#else
    for (size_t index = 0; index < nodes.size(); ++index) {
        Node* child = nodes[index];
        setNodeLeft(child, getNodeLeft(child) + dist, 0);
    }
#endif
}
/// <summary>
/// 垂直居中
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::centerVertically(const std::vector<Node*>& nodes)
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
    float height = 0.0f;
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin() + 1, nodes.end(), [&](Node* child)->void{
        if (minY > (y = getNodeTop(child)))
        {
            minY = y;
        }
        if (maxY < y + child->getContentSize().height)
        {
            maxY = y + child->getContentSize().height;
        }
    });
#else
#endif

    float groupHeight = maxY - minY;
    float dist = ((nodes[0]->getParent()->getContentSize().height - groupHeight) / 2.0f) - minY;
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(), [dist](Node* child)->void{
        setNodeTop(child, getNodeTop(child) + dist);
    });
#else
#endif
}

/// <summary>
/// 居中
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::centerHoriVerti(const std::vector<Node*>& nodes)
{
    if (nodes.empty())
    {
        return;
    }
    else if (nodes.size() == 1)
    {
        centerNode(nodes[0]);
        return;
    }

    // group nodes locators
    float minX = getNodeLeft(nodes[0]), maxX = getNodeLeft(nodes[0]) + nodes[0]->getContentSize().width;
    float x = 0.0f;
    float width = 0.0f;

    float minY = getNodeTop(nodes[0]), maxY = getNodeTop(nodes[0]) + nodes[0]->getContentSize().height;
    float y = 0.0f;
    float height = 0.0f;

    std::for_each(nodes.begin() + 1, nodes.end(), [&](Node* child)->void{
        if (minX > (x = getNodeLeft(child)))
        {
            minX = x;
        }
        if (maxX < x + child->getContentSize().width)
        {
            maxX = x + child->getContentSize().width;
        }
        if (minY > (y = getNodeTop(child)))
        {
            minY = y;
        }
        if (maxY < y + child->getContentSize().height)
        {
            maxY = y + child->getContentSize().height;
        }
    });

    float groupWidth = maxX - minX;
    float groupHeight = maxY - minY;

    float distX = ((nodes[0]->getParent()->getContentSize().width - groupWidth) / 2.0f) - minX;
    float distY = ((nodes[0]->getParent()->getContentSize().height - groupHeight) / 2.0f) - minY;

    std::for_each(nodes.begin(), nodes.end(), [distX, distY](Node* child)->void{
        setNodeLT(child, cocos2d::Point(getNodeLeft(child) + distX, getNodeTop(child) + distY));
    });
}

/// <summary>
/// 左对齐
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::alignLefts(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index = 0;
    float minLeft = getNodeLeft(nodes[index]);
    for (index = 1; index < nodes.size(); ++index)
    {
        if (minLeft > getNodeLeft(nodes[index]))
        {
            minLeft = getNodeLeft(nodes[index]);
        }
    }
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(), [minLeft](Node* child)->void{
        setNodeLeft(child, minLeft);
    });
#else
#endif
}

/// <summary>
/// 右对齐
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::alignRights(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index = 0;
    float maxRight = getNodeLeft(nodes[index]) + nodes[index]->getContentSize().width;
    for (index = 1; index < nodes.size(); ++index)
    {
        if (maxRight < (getNodeLeft(nodes[index]) + nodes[index]->getContentSize().width))
        {
            maxRight = getNodeLeft(nodes[index]) + nodes[index]->getContentSize().width;
        }
    }

#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(), [maxRight](Node* child)->void{
        setNodeLeft(child, maxRight - child->getContentSize().width);
    });
#else
#endif
}

/// <summary>
/// 顶端对齐
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::alignTops(const std::vector<Node*>& nodes)
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
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(), [minTop](Node* child)->void{
        setNodeTop(child, minTop);
    });
#else
#endif
}

/// <summary>
/// 底端对齐
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::alignBottoms(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index = 0;
    float maxBottom = getNodeTop(nodes[index]) + nodes[index]->getContentSize().height;
    for (index = 1; index < nodes.size(); ++index)
    {
        if (maxBottom < (getNodeTop(nodes[index]) + nodes[index]->getContentSize().height))
        {
            maxBottom = getNodeTop(nodes[index]) + nodes[index]->getContentSize().height;
        }
    }
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(), [maxBottom](Node* child)->void{
        setNodeTop(child, maxBottom - child->getContentSize().height);
    });
#else
#endif
}

/// <summary>
/// 水平对齐
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::alignHorizontals(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index = 0;
    float minCenterY, maxCenterY, centerY;
    Node* child = nodes[index];
    minCenterY = maxCenterY = getNodeBottom(child, 0.5f); // child.GetX(0.5f);

    for (index = 1; index < nodes.size(); ++index)
    {
        child = nodes[index];
        centerY = getNodeBottom(child, 0.5f); // child.GetX(0.5f);
        if (minCenterY > centerY)
        {
            minCenterY = centerY;
        }
        if (maxCenterY < centerY)
        {
            maxCenterY = centerY;
        }
    }

    std::for_each(nodes.begin(), nodes.end(), [minCenterY, maxCenterY](Node* child)->void{
        setNodeBottom(child, (minCenterY + maxCenterY) * 0.5f, 0.5f); // child.SetX((minCenterX + maxCenterX) * 0.5f, 0.5f);
    });
}

/// <summary>
/// 垂直对齐
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::alignVerticals(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index = 0;
    float minCenterX, maxCenterX, centerX;
    Node* child = nodes[index];
    minCenterX = maxCenterX = getNodeLeft(child, 0.5f); // child.GetX(0.5f);

    for (index = 1; index < nodes.size(); ++index)
    {
        child = nodes[index];
        centerX = getNodeLeft(child, 0.5f); // child.GetX(0.5f);
        if (minCenterX > centerX)
        {
            minCenterX = centerX;
        }
        if (maxCenterX < centerX)
        {
            maxCenterX = centerX;
        }
    }

    std::for_each(nodes.begin(), nodes.end(), [minCenterX, maxCenterX](Node* child)->void{
        setNodeLeft(child, (minCenterX + maxCenterX) * 0.5f, 0.5f); // child.SetX((minCenterX + maxCenterX) * 0.5f, 0.5f);
    });
}

/// <summary>
/// 中心原点对齐
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::alignCenters(const std::vector<Node*>& nodes)
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
void nodes_layout::makeSameWidth(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index = 0;
    float maxWidth = nodes[index]->getContentSize().width;
    for (index = 1; index < nodes.size(); ++index)
    {
        if (maxWidth < nodes[index]->getContentSize().width)
        {
            maxWidth = nodes[index]->getContentSize().width;
        }
    }
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(), [maxWidth](Node* child)->void{
        child->setContentSize(Size(maxWidth, child->getContentSize().height));
    });
#else
#endif
}

/// <summary>
/// 使高度相等
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::makeSameHeight(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index = 0;
    float minHeight = nodes[index]->getContentSize().height;
    for (index = 1; index < nodes.size(); ++index)
    {
        if (minHeight > nodes[index]->getContentSize().height)
        {
            minHeight = nodes[index]->getContentSize().height;
        }
    }
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(), [minHeight](Node* child)->void{
        child->setContentSize(Size(child->getContentSize().width, minHeight));
    });
#else
#endif
}

/// <summary>
/// 使得大小相等
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::makeSameSize(const std::vector<Node*>& nodes)
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
void nodes_layout::makeHorizontalSpacingEqual(std::vector<Node*>& nodes)
{
    if (nodes.size() < 3)
        return;

    float avgHSpacing = 0;
    Node* child, *childNext;
    if (nodes.size() < 3) return;
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    // sort by x firstly
    std::sort(nodes.begin(), nodes.end(), [](Node* const left, Node* const right){
        return getNodeLeft(left) < getNodeLeft(right);
    });
#else
#endif

    for (size_t index = 0; index < nodes.size() - 1; ++index)
    {
        child = nodes[index];
        childNext = nodes[index + 1];
        avgHSpacing += (getNodeLeft(childNext) - getNodeLeft(child) - child->getContentSize().width);
    }

    avgHSpacing /= (nodes.size() - 1);

    if (avgHSpacing < 0.0f) avgHSpacing = 0.0f;

    nodes_layout::makeHorizontalSpacingEqual(nodes, avgHSpacing);
}

/// <summary>
/// 使水平间距相等
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::makeHorizontalSpacingEqual(const std::vector<Node*>& nodes, float theSpacing)
{
    Node* child, *childNext;
    for (size_t index = 0; index < nodes.size() - 1; ++index)
    {
        child = nodes[index];
        childNext = nodes[index + 1];
        setNodeLeft(childNext, theSpacing + getNodeLeft(child) + child->getContentSize().width);
    }
}

/// <summary>
/// 使垂直间距相等
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::makeVerticalSpacingEqual(std::vector<Node*>& nodes)
{
    if (nodes.size() < 3)
        return;

    float avgVSpacing = 0;
    Node* child, *childNext;
    if (nodes.size() < 3) return;
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    // sort by y firstly
    std::sort(nodes.begin(), nodes.end(), [](Node* const left, Node* const right)->bool{
        return getNodeTop(left) < getNodeTop(right);
    });
#else
#endif
    for (size_t index = 0; index < nodes.size() - 1; ++index)
    {
        child = nodes[index];
        childNext = nodes[index + 1];
        avgVSpacing += (getNodeTop(childNext) - getNodeTop(child) - child->getContentSize().height);
    }

    avgVSpacing /= (nodes.size() - 1);

    if (avgVSpacing < 0.0f) avgVSpacing = 0.0f;

    nodes_layout::makeVerticalSpacingEqual(nodes, avgVSpacing);
}

void nodes_layout::increaseHorizontalSpacing(std::vector<Node*>& nodes, float theSpacing)
{
    if (nodes.size() < 2)
        return;

    std::sort(nodes.begin(), nodes.end(), [](Node* const left, Node* const right)->bool{
        return getNodeLeft(left) < getNodeLeft(right);
    });

    for (size_t idx = 1; idx < nodes.size(); ++idx)
    {
        nodes[idx]->setPositionX(nodes[idx]->getPositionX() + theSpacing * idx);
    }
}

void nodes_layout::increaseVerticalSpacing(std::vector<Node*>& nodes, float theSpacing)
{
    if (nodes.size() < 2)
        return;

    std::sort(nodes.begin(), nodes.end(), [](Node* const left, Node* const right)->bool{
        return getNodeTop(left) < getNodeTop(right);
    });

    for (size_t idx = 1; idx < nodes.size(); ++idx)
    {
        nodes[idx]->setPositionY(nodes[idx]->getPositionY() - theSpacing * idx);
    }
}


void nodes_layout::decreaseHorizontalSpacing(std::vector<Node*>& nodes, float theSpacing)
{
    increaseHorizontalSpacing(nodes, -theSpacing);
}


void nodes_layout::decreaseVerticalSpacing(std::vector<Node*>& nodes, float theSpacing)
{
    increaseVerticalSpacing(nodes, -theSpacing);
}


void nodes_layout::removeHorizontalSpacing(const std::vector<Node*>& nodes)
{
    nodes_layout::makeHorizontalSpacingEqual(nodes, 0);
}

void nodes_layout::removeVerticalSpacing(const std::vector<Node*>& nodes) 
{
    nodes_layout::makeVerticalSpacingEqual(nodes, 0);
}

/// <summary>
/// 使垂直间距相等
/// </summary>
/// <param name="nodes"></param>
void nodes_layout::makeVerticalSpacingEqual(const std::vector<Node*>& nodes, float theSpacing)
{
    Node* child, *childNext;
    for (size_t index = 0; index < nodes.size() - 1; ++index)
    {
        child = nodes[index];
        childNext = nodes[index + 1];
        setNodeTop(childNext, theSpacing + getNodeTop(child) + child->getContentSize().height);
    }
}

/// visual screen
float nodes_layout::scr::getNodeLeft(Node* pNode)
{
    cocos2d::Point ptLocal(nodes_layout::getNodeLeft(pNode), 0);
    auto ptWorld = pNode->getParent()->convertToWorldSpace(ptLocal);
    return ptWorld.x;
}
float nodes_layout::scr::getNodeBottom(Node* pNode)
{
    cocos2d::Point ptLocal(0, nodes_layout::getNodeBottom(pNode));
    auto ptWorld = pNode->getParent()->convertToWorldSpace(ptLocal);

    return ptWorld.y;
}
float nodes_layout::scr::getNodeRight(Node* pNode)
{
    cocos2d::Point ptLocal(nodes_layout::getNodeLeft(pNode) + pNode->getContentSize().width/* * pNode->getScaleX()*/, 0);
    auto ptWorld = pNode->getParent()->convertToWorldSpace(ptLocal);

    auto ScreenVisibleRect = ScreenVisibleRect::getScreenVisibleRect();
    return ScreenVisibleRect.size.width - ptWorld.x;
}
float nodes_layout::scr::getNodeTop(Node* pNode)
{
    cocos2d::Point ptLocal(0, nodes_layout::getNodeBottom(pNode) + pNode->getContentSize().height/* * pNode->getScaleY()*/);
    auto ptWorld = pNode->getParent()->convertToWorldSpace(ptLocal);
    auto ScreenVisibleRect = ScreenVisibleRect::getScreenVisibleRect();

    return ScreenVisibleRect.size.height - ptWorld.y;
}

void nodes_layout::scr::setNodeLeft(Node* pNode, float left)
{
    CC_ASSERT(pNode);
    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(0, scrSize.height) - ScreenVisibleRect::leftTop();
    Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    cocos2d::Point ptWorld(adjust_coord(size.width, achorPoint.x, left)/* - delta.x*/, 0);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionX(ptLocal.x);

}
void nodes_layout::scr::setNodeTop(Node* pNode, float top)
{
    CC_ASSERT(pNode);
    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(0, scrSize.height) - ScreenVisibleRect::leftTop();
    Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    cocos2d::Point ptWorld(0,
        adjust_coord_neg(scrSize.height, size.height, achorPoint.y, top) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionY(ptLocal.y);

}
void nodes_layout::scr::setNodeRight(Node* pNode, float right)
{
    CC_ASSERT(pNode);
    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(scrSize.width, 0) - ScreenVisibleRect::rightBottom();
    Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    cocos2d::Point ptWorld(adjust_coord_neg(scrSize.width, size.width, achorPoint.x, right)/* - delta.x*/,
        0);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionX(ptLocal.x);
}
void nodes_layout::scr::setNodeBottom(Node* pNode, float bottom)
{
    CC_ASSERT(pNode);
    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(scrSize.width, 0) - ScreenVisibleRect::rightBottom();
    Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    cocos2d::Point ptWorld(0,
        adjust_coord(size.height, achorPoint.y, bottom) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionY(ptLocal.y);

}

void nodes_layout::scr::setNodeLT(Node* pNode, const cocos2d::Point& p)
{
    CC_ASSERT(pNode);
    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(0, scrSize.height) - ScreenVisibleRect::leftTop();
    Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    cocos2d::Point ptWorld(adjust_coord(size.width, achorPoint.x, p.x),
        adjust_coord_neg(scrSize.height, size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void nodes_layout::scr::setNodeRT(Node* pNode, const cocos2d::Point& p)
{ // right top
    CC_ASSERT(pNode);
    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(scrSize.width, scrSize.height) - ScreenVisibleRect::rightTop();
    Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    cocos2d::Point ptWorld(adjust_coord_neg(scrSize.width, size.width, achorPoint.x, p.x)/* - delta.x*/,
        adjust_coord_neg(scrSize.height, size.height, achorPoint.y, p.y)/* - delta.y*/);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void nodes_layout::scr::setNodeLB(Node* pNode, const cocos2d::Point& p)
{ // left bottom
    CC_ASSERT(pNode);
    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(0, 0) - ScreenVisibleRect::leftBottom();
    Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    cocos2d::Point ptWorld(adjust_coord(size.width, achorPoint.x, p.x) - delta.x,
        adjust_coord(size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void nodes_layout::scr::setNodeRB(Node* pNode, const cocos2d::Point& p)
{ // right bottom
    CC_ASSERT(pNode);
    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(scrSize.width, 0) - ScreenVisibleRect::rightBottom();
    cocos2d::Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    cocos2d::Point ptWorld(adjust_coord_neg(scrSize.width, size.width, achorPoint.x, p.x)/* - delta.x*/,
        adjust_coord(size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}

/// ratio position
void nodes_layout::scr::setNodeNormalizedLT(Node* pNode, const cocos2d::Point& ratio)
{
    CC_ASSERT(pNode);

    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(0, scrSize.height) - ScreenVisibleRect::leftTop();

    cocos2d::Size vscrSize = ScreenVisibleRect::size();
    cocos2d::Point p = cocos2d::Vec2(vscrSize.width * ratio.x, vscrSize.height * ratio.y);

    cocos2d::Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    cocos2d::Point ptWorld(adjust_coord(size.width, achorPoint.x, p.x) - delta.x,
        adjust_coord_neg(scrSize.height, size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void nodes_layout::scr::setNodeNormalizedRT(Node* pNode, const cocos2d::Point& ratio)
{ // right top
    CC_ASSERT(pNode);
    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(scrSize.width, scrSize.height) - ScreenVisibleRect::rightTop();

    cocos2d::Size vscrSize = ScreenVisibleRect::size();
    cocos2d::Point p = cocos2d::Vec2(vscrSize.width * ratio.x, vscrSize.height * ratio.y);

    cocos2d::Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    cocos2d::Point ptWorld(adjust_coord_neg(scrSize.width, size.width, achorPoint.x, p.x) - delta.x, adjust_coord_neg(scrSize.height, size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void nodes_layout::scr::setNodeNormalizedLB(Node* pNode, const cocos2d::Point& ratio)
{ // left bottom
    CC_ASSERT(pNode);
    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(0, 0) - ScreenVisibleRect::leftBottom();

    cocos2d::Size vscrSize = ScreenVisibleRect::size();
    cocos2d::Point p = cocos2d::Vec2(vscrSize.width * ratio.x, vscrSize.height * ratio.y);

    cocos2d::Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    cocos2d::Point ptWorld(adjust_coord(size.width, achorPoint.x, p.x) - delta.x, adjust_coord(size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void nodes_layout::scr::setNodeNormalizedRB(Node* pNode, const cocos2d::Point& ratio)
{ // right bottom
    CC_ASSERT(pNode);
    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(scrSize.width, 0) - ScreenVisibleRect::rightBottom();

    cocos2d::Size vscrSize = ScreenVisibleRect::size();
    cocos2d::Point p = cocos2d::Vec2(vscrSize.width * ratio.x, vscrSize.height * ratio.y);

    cocos2d::Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    cocos2d::Point ptWorld(adjust_coord_neg(scrSize.width, size.width, achorPoint.x, p.x) - delta.x, adjust_coord(size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}

void nodes_layout::scr::setNodeNormalizedTop(Node* pNode, const float ratioTop)
{ // right top
    CC_ASSERT(pNode);
    cocos2d::Size scrSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    cocos2d::Point delta = cocos2d::Vec2(scrSize.width, scrSize.height) - ScreenVisibleRect::rightTop();

    cocos2d::Size vscrSize = ScreenVisibleRect::size();
    float top = vscrSize.width * ratioTop;

    cocos2d::Size size = pNode->getContentSize() * getScale2D(pNode);
    cocos2d::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    cocos2d::Point ptWorld(0, adjust_coord_neg(scrSize.height, size.height, achorPoint.y, top) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionY(ptWorld.y);
}

void nodes_layout::scr::setNodeNormalizedPositionX(cocos2d::Node* pNode, float ratio)
{
    CC_ASSERT(pNode);
    cocos2d::Rect ScreenVisibleRect = LayoutHelper::ScreenVisibleRect::getScreenVisibleRect();
    cocos2d::Point ptWorld(ScreenVisibleRect.size.width * ratio + ScreenVisibleRect.origin.x, 0);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionX(ptLocal.x);
}

void nodes_layout::scr::setNodeNormalizedPositionY(cocos2d::Node* pNode, float ratio)
{
    CC_ASSERT(pNode);
    cocos2d::Rect ScreenVisibleRect = LayoutHelper::ScreenVisibleRect::getScreenVisibleRect();

    cocos2d::Point ptWorld(0, ScreenVisibleRect.size.height * ratio + ScreenVisibleRect.origin.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionY(ptLocal.y);
}
void nodes_layout::scr::setNodeNormalizedPosition(Node* pNode, const cocos2d::Point& ratio)
{
    CC_ASSERT(pNode);
    pNode->setIgnoreAnchorPointForPosition(false);
    pNode->setAnchorPoint(cocos2d::Vec2(.5f, .5f));
    cocos2d::Rect ScreenVisibleRect = LayoutHelper::ScreenVisibleRect::getScreenVisibleRect();
    cocos2d::Point ptWorld(ScreenVisibleRect.size.width * ratio.x + ScreenVisibleRect.origin.x, ScreenVisibleRect.size.height * ratio.y + ScreenVisibleRect.origin.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}

}
