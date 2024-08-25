//
// Copyright (c) 2014-2020 Simdsoft Limited - All Rights Reserved
// Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
//
#include "ui/LayoutHelper.h"

using namespace ax;

Vec2 LayoutHelper::s_designSize;
float LayoutHelper::s_adjustedScale = 1.0f;

void LayoutHelper::setDesignSizeFixedEdge(const Vec2& designSize)
{
    LayoutHelper::s_designSize = designSize;

    // Set the design resolution//分辨率的大小
    GLView* pEGLView      = Director::getInstance()->getGLView();
    const Vec2& frameSize = pEGLView->getFrameSize();

    // Vec2 lsSize = lsaSize;

    float scaleX = (float)frameSize.width / designSize.width;
    float scaleY = (float)frameSize.height / designSize.height;

    // adjustedScale = 0.0f; // MAX(scaleX, scaleY);
    if (scaleX < scaleY)
    {
        pEGLView->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::FIXED_WIDTH);
    }
    else
    {
        pEGLView->setDesignResolutionSize(designSize.width, designSize.height, ResolutionPolicy::FIXED_HEIGHT);
    }
}

void LayoutHelper::setDesignSizeNoBorder(const Vec2& designSize)
{
    // save smart size
    LayoutHelper::s_designSize = designSize;

    // Set the design resolution//分辨率的大小
    GLView* pEGLView      = Director::getInstance()->getGLView();
    const Vec2& frameSize = pEGLView->getFrameSize();

    // Vec2 lsSize = lsaSize;

    float scaleX = (float)frameSize.width / LayoutHelper::s_designSize.width;
    float scaleY = (float)frameSize.height / LayoutHelper::s_designSize.height;

    LayoutHelper::s_adjustedScale = 0.0f;  // MAX(scaleX, scaleY);
    if (scaleX > scaleY)
    {
        LayoutHelper::s_adjustedScale = scaleX / (frameSize.height / LayoutHelper::s_designSize.height);
    }
    else
    {
        LayoutHelper::s_adjustedScale = scaleY / (frameSize.width / LayoutHelper::s_designSize.width);
    }

    AXLOGD("x: {}; y: {}; scale: {}", scaleX, scaleY, s_adjustedScale);

    pEGLView->setDesignResolutionSize(LayoutHelper::s_designSize.width * s_adjustedScale,
                                      LayoutHelper::s_designSize.height * s_adjustedScale, ResolutionPolicy::NO_BORDER);
}

ax::Vec2 LayoutHelper::getVisibleOrigin(void)
{
    const auto& adjustedDesignSize = Director::getInstance()->getGLView()->getDesignResolutionSize();
    return ax::Vec2((adjustedDesignSize.width - LayoutHelper::s_designSize.width) * .5f,
                         (adjustedDesignSize.height - LayoutHelper::s_designSize.height) * .5f);
}

Vec2 LayoutHelper::getVisibleSize(void)
{
    return LayoutHelper::s_designSize;
}

/// Get node group size
Vec2 LayoutHelper::getNodeGroupSize(const std::vector<Node*>& nodes)
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
    float x = 0.0f, y = 0.0f;
    // float width = 0.0f, height = 0.f;
    for (size_t index = 1; index < nodes.size(); ++index)
    {
        Node* child = nodes[index];
        if (minX > (x = getNodeLeft(child)))
        {
            minX = x;
        }
        if (maxX < x + child->getContentSize().width /* * child->getScaleX()*/)
        {
            maxX = x + child->getContentSize().width /* * child->getScaleX()*/;
        }

        if (minY > (y = getNodeTop(child)))
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
void LayoutHelper::setNodeGroupSize(const std::vector<Node*>& nodes, const Vec2& newSize)
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

Vec2 LayoutHelper::getNodeGroupScaledSize(const std::vector<Node*>& nodes)
{
    if (nodes.empty())
    {
        return Vec2::ZERO;
    }

    auto scale = getScale2D(nodes[0]);

    // group nodes locators
    float minX = LayoutHelper::getNodeLeft(nodes[0]),
          maxX = LayoutHelper::getNodeLeft(nodes[0]) + nodes[0]->getContentSize().width * scale.x;
    float minY = LayoutHelper::getNodeTop(nodes[0]),
          maxY = LayoutHelper::getNodeTop(nodes[0]) + nodes[0]->getContentSize().height * scale.y;
    float x = 0.0f, y = 0.0f;
    // float width = 0.0f, height = 0.f;
    for (size_t index = 1; index < nodes.size(); ++index)
    {
        Node* child = nodes[index];

        scale = getScale2D(child);

        if (minX > (x = LayoutHelper::getNodeLeft(child)))
        {
            minX = x;
        }
        if (maxX < x + child->getContentSize().width * scale.x)
        {
            maxX = x + child->getContentSize().width * scale.x;
        }

        if (minY > (y = LayoutHelper::getNodeTop(child)))
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
float LayoutHelper::getNodeGroupLeft(const std::vector<Node*>& nodes)
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
float LayoutHelper::getNodeGroupTop(const std::vector<Node*>& nodes)
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
float LayoutHelper::getNodeGroupRight(const std::vector<Node*>& nodes)
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
float LayoutHelper::getNodeGroupBottom(const std::vector<Node*>& nodes)
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
void LayoutHelper::setNodeGroupLeft(const std::vector<Node*>& nodes, float left)
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
void LayoutHelper::setNodeGroupTop(const std::vector<Node*>& nodes, float top)
{
    float delta = top - getNodeGroupTop(nodes);

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        setNodeTop((*iter), getNodeTop((*iter)) + delta);
    }
}

void LayoutHelper::setNodeGroupLT(const std::vector<Node*>& nodes, const ax::Vec2& p)
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
void LayoutHelper::setNodeGroupRight(const std::vector<Node*>& nodes, float right)
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
void LayoutHelper::setNodeGroupBottom(const std::vector<Node*>& nodes, float bottom)
{
    float delta = bottom - getNodeGroupBottom(nodes);

    for (auto iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        setNodeBottom((*iter), getNodeBottom((*iter)) + delta);
    }
}

void LayoutHelper::moveNodeGroupHorizontally(const std::vector<Node*>& nodes, float delta)
{
    for (auto&& node : nodes)
    {
        node->setPositionX(node->getPositionX() + delta);
    }
}

void LayoutHelper::moveNodeGroupVertically(const std::vector<Node*>& nodes, float delta)
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
void LayoutHelper::centerHorizontally(const std::vector<Node*>& nodes)
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
    float x     = 0.0f;
    float width = 0.0f;
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin() + 1, nodes.end(), [&](Node* child) -> void {
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
    for (size_t index = 1; index < nodes.size(); ++index)
    {
        Node* child = nodes[index];
        if (minX > (x = getNodeLeft(child)))
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
    float dist       = ((nodes[0]->getParent()->getContentSize().width - groupWidth) / 2.0f) - minX;
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(),
                  [dist](Node* child) -> void { setNodeLeft(child, getNodeLeft(child) + dist); });
#else
    for (size_t index = 0; index < nodes.size(); ++index)
    {
        Node* child = nodes[index];
        setNodeLeft(child, getNodeLeft(child) + dist, 0);
    }
#endif
}
/// <summary>
/// 垂直居中
/// </summary>
/// <param name="nodes"></param>
void LayoutHelper::centerVertically(const std::vector<Node*>& nodes)
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
    float y      = 0.0f;
    float height = 0.0f;
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
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
#else
#endif

    float groupHeight = maxY - minY;
    float dist        = ((nodes[0]->getParent()->getContentSize().height - groupHeight) / 2.0f) - minY;
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(),
                  [dist](Node* child) -> void { setNodeTop(child, getNodeTop(child) + dist); });
#else
#endif
}

/// <summary>
/// 居中
/// </summary>
/// <param name="nodes"></param>
void LayoutHelper::centerToParent(const std::vector<Node*>& nodes)
{
    if (nodes.empty())
    {
        return;
    }
    else if (nodes.size() == 1)
    {
        LayoutHelper::centerNode(nodes[0]);
        return;
    }

    // group nodes locators
    float minX = getNodeLeft(nodes[0]), maxX = getNodeLeft(nodes[0]) + nodes[0]->getContentSize().width;
    float x     = 0.0f;
    float width = 0.0f;

    float minY = getNodeTop(nodes[0]), maxY = getNodeTop(nodes[0]) + nodes[0]->getContentSize().height;
    float y      = 0.0f;
    float height = 0.0f;

    std::for_each(nodes.begin() + 1, nodes.end(), [&](Node* child) -> void {
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
void LayoutHelper::alignLefts(const std::vector<Node*>& nodes)
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
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(), [minLeft](Node* child) -> void { setNodeLeft(child, minLeft); });
#else
#endif
}

/// <summary>
/// 右对齐
/// </summary>
/// <param name="nodes"></param>
void LayoutHelper::alignRights(const std::vector<Node*>& nodes)
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

#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(),
                  [maxRight](Node* child) -> void { setNodeLeft(child, maxRight - child->getContentSize().width); });
#else
#endif
}

/// <summary>
/// 顶端对齐
/// </summary>
/// <param name="nodes"></param>
void LayoutHelper::alignTops(const std::vector<Node*>& nodes)
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
    std::for_each(nodes.begin(), nodes.end(), [minTop](Node* child) -> void { setNodeTop(child, minTop); });
#else
#endif
}

/// <summary>
/// 底端对齐
/// </summary>
/// <param name="nodes"></param>
void LayoutHelper::alignBottoms(const std::vector<Node*>& nodes)
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
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(),
                  [maxBottom](Node* child) -> void { setNodeTop(child, maxBottom - child->getContentSize().height); });
#else
#endif
}

/// <summary>
/// 水平对齐
/// </summary>
/// <param name="nodes"></param>
void LayoutHelper::alignHorizontals(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index = 0;
    float minCenterY, maxCenterY, centerY;
    Node* child = nodes[index];
    minCenterY = maxCenterY = getNodeBottom(child, 0.5f);  // child.GetX(0.5f);

    for (index = 1; index < nodes.size(); ++index)
    {
        child   = nodes[index];
        centerY = getNodeBottom(child, 0.5f);  // child.GetX(0.5f);
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
void LayoutHelper::alignVerticals(const std::vector<Node*>& nodes)
{
    if (nodes.size() < 2)
        return;

    size_t index = 0;
    float minCenterX, maxCenterX, centerX;
    Node* child = nodes[index];
    minCenterX = maxCenterX = getNodeLeft(child, 0.5f);  // child.GetX(0.5f);

    for (index = 1; index < nodes.size(); ++index)
    {
        child   = nodes[index];
        centerX = getNodeLeft(child, 0.5f);  // child.GetX(0.5f);
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
void LayoutHelper::alignCenters(const std::vector<Node*>& nodes)
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
void LayoutHelper::makeSameWidth(const std::vector<Node*>& nodes)
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
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(), [maxWidth](Node* child) -> void {
        child->setContentSize(Vec2(maxWidth, child->getContentSize().height));
    });
#else
#endif
}

/// <summary>
/// 使高度相等
/// </summary>
/// <param name="nodes"></param>
void LayoutHelper::makeSameHeight(const std::vector<Node*>& nodes)
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
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    std::for_each(nodes.begin(), nodes.end(), [minHeight](Node* child) -> void {
        child->setContentSize(Vec2(child->getContentSize().width, minHeight));
    });
#else
#endif
}

/// <summary>
/// 使得大小相等
/// </summary>
/// <param name="nodes"></param>
void LayoutHelper::makeSameSize(const std::vector<Node*>& nodes)
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
void LayoutHelper::makeHorizontalSpacingEqual(std::vector<Node*>& nodes)
{
    if (nodes.size() < 3)
        return;

    float avgHSpacing = 0;
    Node *child, *childNext;
    if (nodes.size() < 3)
        return;
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    // sort by x firstly
    std::sort(nodes.begin(), nodes.end(),
              [](Node* const left, Node* const right) { return getNodeLeft(left) < getNodeLeft(right); });
#else
#endif

    for (size_t index = 0; index < nodes.size() - 1; ++index)
    {
        child     = nodes[index];
        childNext = nodes[index + 1];
        avgHSpacing += (getNodeLeft(childNext) - getNodeLeft(child) - child->getContentSize().width);
    }

    avgHSpacing /= (nodes.size() - 1);

    if (avgHSpacing < 0.0f)
        avgHSpacing = 0.0f;

    LayoutHelper::makeHorizontalSpacingEqual(nodes, avgHSpacing);
}

/// <summary>
/// 使水平间距相等
/// </summary>
/// <param name="nodes"></param>
void LayoutHelper::makeHorizontalSpacingEqual(const std::vector<Node*>& nodes, float theSpacing)
{
    Node *child, *childNext;
    for (size_t index = 0; index < nodes.size() - 1; ++index)
    {
        child     = nodes[index];
        childNext = nodes[index + 1];
        setNodeLeft(childNext, theSpacing + getNodeLeft(child) + child->getContentSize().width);
    }
}

/// <summary>
/// 使垂直间距相等
/// </summary>
/// <param name="nodes"></param>
void LayoutHelper::makeVerticalSpacingEqual(std::vector<Node*>& nodes)
{
    if (nodes.size() < 3)
        return;

    float avgVSpacing = 0;
    Node *child, *childNext;
    if (nodes.size() < 3)
        return;
#if (defined(_WIN32) && _MSC_VER >= 1700) || !defined(_WIN32)
    // sort by y firstly
    std::sort(nodes.begin(), nodes.end(),
              [](Node* const left, Node* const right) -> bool { return getNodeTop(left) < getNodeTop(right); });
#else
#endif
    for (size_t index = 0; index < nodes.size() - 1; ++index)
    {
        child     = nodes[index];
        childNext = nodes[index + 1];
        avgVSpacing += (getNodeTop(childNext) - getNodeTop(child) - child->getContentSize().height);
    }

    avgVSpacing /= (nodes.size() - 1);

    if (avgVSpacing < 0.0f)
        avgVSpacing = 0.0f;

    LayoutHelper::makeVerticalSpacingEqual(nodes, avgVSpacing);
}

void LayoutHelper::increaseHorizontalSpacing(std::vector<Node*>& nodes, float theSpacing)
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

void LayoutHelper::increaseVerticalSpacing(std::vector<Node*>& nodes, float theSpacing)
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

void LayoutHelper::decreaseHorizontalSpacing(std::vector<Node*>& nodes, float theSpacing)
{
    increaseHorizontalSpacing(nodes, -theSpacing);
}

void LayoutHelper::decreaseVerticalSpacing(std::vector<Node*>& nodes, float theSpacing)
{
    increaseVerticalSpacing(nodes, -theSpacing);
}

void LayoutHelper::removeHorizontalSpacing(const std::vector<Node*>& nodes)
{
    LayoutHelper::makeHorizontalSpacingEqual(nodes, 0);
}

void LayoutHelper::removeVerticalSpacing(const std::vector<Node*>& nodes)
{
    LayoutHelper::makeVerticalSpacingEqual(nodes, 0);
}

/// <summary>
/// 使垂直间距相等
/// </summary>
/// <param name="nodes"></param>
void LayoutHelper::makeVerticalSpacingEqual(const std::vector<Node*>& nodes, float theSpacing)
{
    Node *child, *childNext;
    for (size_t index = 0; index < nodes.size() - 1; ++index)
    {
        child     = nodes[index];
        childNext = nodes[index + 1];
        setNodeTop(childNext, theSpacing + getNodeTop(child) + child->getContentSize().height);
    }
}

// ----------------- LayoutHelper::VisibleRect --------------------------

ax::Rect LayoutHelper::VisibleRect::s_ScreenVisibleRect;

/// x-studio: when design resolution changed, should call this func.
void LayoutHelper::VisibleRect::refresh(void)
{
    auto director              = Director::getInstance();
    s_ScreenVisibleRect.origin = Director::getInstance()->getVisibleOrigin();
    s_ScreenVisibleRect.size   = Director::getInstance()->getVisibleSize();
}

void LayoutHelper::VisibleRect::lazyInit()
{
    if (s_ScreenVisibleRect.size.width == 0.0f && s_ScreenVisibleRect.size.height == 0.0f)
    {
        auto director = Director::getInstance();
        auto glView   = director->getGLView();

        if (glView->getResolutionPolicy() == ResolutionPolicy::NO_BORDER)
        {
            s_ScreenVisibleRect.origin = director->getVisibleOrigin();
            s_ScreenVisibleRect.size   = director->getVisibleSize();
        }
        else
        {
            s_ScreenVisibleRect.origin = LayoutHelper::getVisibleOrigin();
            s_ScreenVisibleRect.size   = LayoutHelper::getVisibleSize();
        }
    }
}

ax::Rect LayoutHelper::VisibleRect::getScreenVisibleRect()
{
    lazyInit();
    return ax::Rect(s_ScreenVisibleRect.origin.x, s_ScreenVisibleRect.origin.y, s_ScreenVisibleRect.size.width,
                         s_ScreenVisibleRect.size.height);
}

Vec2 LayoutHelper::VisibleRect::size()
{
    lazyInit();
    return s_ScreenVisibleRect.size;
}

Point LayoutHelper::VisibleRect::left()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x,
                          s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height / 2);
}

Point LayoutHelper::VisibleRect::right()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width,
                          s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height / 2);
}

Point LayoutHelper::VisibleRect::top()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width / 2,
                          s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height);
}

Point LayoutHelper::VisibleRect::bottom()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width / 2,
                          s_ScreenVisibleRect.origin.y);
}

Point LayoutHelper::VisibleRect::center()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width / 2,
                          s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height / 2);
}

Point LayoutHelper::VisibleRect::leftTop()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x, s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height);
}

Point LayoutHelper::VisibleRect::rightTop()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width,
                          s_ScreenVisibleRect.origin.y + s_ScreenVisibleRect.size.height);
}

Point LayoutHelper::VisibleRect::leftBottom()
{
    lazyInit();
    return s_ScreenVisibleRect.origin;
}

Point LayoutHelper::VisibleRect::rightBottom()
{
    lazyInit();
    return ax::Point(s_ScreenVisibleRect.origin.x + s_ScreenVisibleRect.size.width, s_ScreenVisibleRect.origin.y);
}

/// visual screen
float LayoutHelper::VisibleRect::getNodeLeft(Node* pNode)
{
    ax::Point ptLocal(LayoutHelper::getNodeLeft(pNode), 0);
    auto ptWorld = pNode->getParent()->convertToWorldSpace(ptLocal);
    return ptWorld.x;
}
float LayoutHelper::VisibleRect::getNodeBottom(Node* pNode)
{
    ax::Point ptLocal(0, LayoutHelper::getNodeBottom(pNode));
    auto ptWorld = pNode->getParent()->convertToWorldSpace(ptLocal);

    return ptWorld.y;
}
float LayoutHelper::VisibleRect::getNodeRight(Node* pNode)
{
    ax::Point ptLocal(LayoutHelper::getNodeLeft(pNode) + pNode->getContentSize().width /* * pNode->getScaleX()*/,
                           0);
    auto ptWorld = pNode->getParent()->convertToWorldSpace(ptLocal);

    auto visibleRect = LayoutHelper::VisibleRect::getScreenVisibleRect();
    return visibleRect.size.width - ptWorld.x;
}
float LayoutHelper::VisibleRect::getNodeTop(Node* pNode)
{
    ax::Point ptLocal(
        0, LayoutHelper::getNodeBottom(pNode) + pNode->getContentSize().height /* * pNode->getScaleY()*/);
    auto ptWorld     = pNode->getParent()->convertToWorldSpace(ptLocal);
    auto visibleRect = LayoutHelper::VisibleRect::getScreenVisibleRect();

    return visibleRect.size.height - ptWorld.y;
}

void LayoutHelper::VisibleRect::setNodeLeft(Node* pNode, float left)
{
    AX_ASSERT(pNode);
    Vec2 scrSize              = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(0, scrSize.height) - LayoutHelper::VisibleRect::leftTop();
    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(adjust_coord(size.width, achorPoint.x, left) /* - delta.x*/, 0);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionX(ptLocal.x);
}
void LayoutHelper::VisibleRect::setNodeTop(Node* pNode, float top)
{
    AX_ASSERT(pNode);
    Vec2 scrSize              = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(0, scrSize.height) - LayoutHelper::VisibleRect::leftTop();
    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(0, adjust_coord_neg(scrSize.height, size.height, achorPoint.y, top) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionY(ptLocal.y);
}
void LayoutHelper::VisibleRect::setNodeRight(Node* pNode, float right)
{
    AX_ASSERT(pNode);
    Vec2 scrSize              = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(scrSize.width, 0) - LayoutHelper::VisibleRect::rightBottom();
    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(adjust_coord_neg(scrSize.width, size.width, achorPoint.x, right) /* - delta.x*/, 0);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionX(ptLocal.x);
}
void LayoutHelper::VisibleRect::setNodeBottom(Node* pNode, float bottom)
{
    AX_ASSERT(pNode);
    Vec2 scrSize              = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(scrSize.width, 0) - LayoutHelper::VisibleRect::rightBottom();
    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(0, adjust_coord(size.height, achorPoint.y, bottom) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionY(ptLocal.y);
}

void LayoutHelper::VisibleRect::setNodeLT(Node* pNode, const ax::Point& p)
{
    AX_ASSERT(pNode);
    Vec2 scrSize              = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(0, scrSize.height) - LayoutHelper::VisibleRect::leftTop();
    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    ax::Point ptWorld(adjust_coord(size.width, achorPoint.x, p.x),
                           adjust_coord_neg(scrSize.height, size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void LayoutHelper::VisibleRect::setNodeRT(Node* pNode, const ax::Point& p)
{  // right top
    AX_ASSERT(pNode);
    Vec2 scrSize              = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(scrSize.width, scrSize.height) - LayoutHelper::VisibleRect::rightTop();
    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(adjust_coord_neg(scrSize.width, size.width, achorPoint.x, p.x) /* - delta.x*/,
                           adjust_coord_neg(scrSize.height, size.height, achorPoint.y, p.y) /* - delta.y*/);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void LayoutHelper::VisibleRect::setNodeLB(Node* pNode, const ax::Point& p)
{  // left bottom
    AX_ASSERT(pNode);
    Vec2 scrSize              = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(0, 0) - LayoutHelper::VisibleRect::leftBottom();
    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(adjust_coord(size.width, achorPoint.x, p.x) - delta.x,
                           adjust_coord(size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void LayoutHelper::VisibleRect::setNodeRB(Node* pNode, const ax::Point& p)
{  // right bottom
    AX_ASSERT(pNode);
    Vec2 scrSize              = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta      = ax::Vec2(scrSize.width, 0) - LayoutHelper::VisibleRect::rightBottom();
    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    ax::Point ptWorld(adjust_coord_neg(scrSize.width, size.width, achorPoint.x, p.x) /* - delta.x*/,
                           adjust_coord(size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}

/// ratio position
void LayoutHelper::VisibleRect::setNodeNormalizedLT(Node* pNode, const ax::Point& ratio)
{
    AX_ASSERT(pNode);

    Vec2 scrSize         = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta = ax::Vec2(0, scrSize.height) - LayoutHelper::VisibleRect::leftTop();

    Vec2 vscrSize    = LayoutHelper::VisibleRect::size();
    ax::Point p = ax::Vec2(vscrSize.width * ratio.x, vscrSize.height * ratio.y);

    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    ax::Point ptWorld(adjust_coord(size.width, achorPoint.x, p.x) - delta.x,
                           adjust_coord_neg(scrSize.height, size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void LayoutHelper::VisibleRect::setNodeNormalizedRT(Node* pNode, const ax::Point& ratio)
{  // right top
    AX_ASSERT(pNode);
    Vec2 scrSize         = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta = ax::Vec2(scrSize.width, scrSize.height) - LayoutHelper::VisibleRect::rightTop();

    Vec2 vscrSize    = LayoutHelper::VisibleRect::size();
    ax::Point p = ax::Vec2(vscrSize.width * ratio.x, vscrSize.height * ratio.y);

    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    ax::Point ptWorld(adjust_coord_neg(scrSize.width, size.width, achorPoint.x, p.x) - delta.x,
                           adjust_coord_neg(scrSize.height, size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void LayoutHelper::VisibleRect::setNodeNormalizedLB(Node* pNode, const ax::Point& ratio)
{  // left bottom
    AX_ASSERT(pNode);
    Vec2 scrSize         = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta = ax::Vec2(0, 0) - LayoutHelper::VisibleRect::leftBottom();

    Vec2 vscrSize    = LayoutHelper::VisibleRect::size();
    ax::Point p = ax::Vec2(vscrSize.width * ratio.x, vscrSize.height * ratio.y);

    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    ax::Point ptWorld(adjust_coord(size.width, achorPoint.x, p.x) - delta.x,
                           adjust_coord(size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
void LayoutHelper::VisibleRect::setNodeNormalizedRB(Node* pNode, const ax::Point& ratio)
{  // right bottom
    AX_ASSERT(pNode);
    Vec2 scrSize         = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta = ax::Vec2(scrSize.width, 0) - LayoutHelper::VisibleRect::rightBottom();

    Vec2 vscrSize    = LayoutHelper::VisibleRect::size();
    ax::Point p = ax::Vec2(vscrSize.width * ratio.x, vscrSize.height * ratio.y);

    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }

    ax::Point ptWorld(adjust_coord_neg(scrSize.width, size.width, achorPoint.x, p.x) - delta.x,
                           adjust_coord(size.height, achorPoint.y, p.y) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}

void LayoutHelper::VisibleRect::setNodeNormalizedTop(Node* pNode, const float ratioTop)
{  // right top
    AX_ASSERT(pNode);
    Vec2 scrSize         = Director::getInstance()->getGLView()->getDesignResolutionSize();
    ax::Point delta = ax::Vec2(scrSize.width, scrSize.height) - LayoutHelper::VisibleRect::rightTop();

    Vec2 vscrSize = LayoutHelper::VisibleRect::size();
    float top     = vscrSize.width * ratioTop;

    Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
    ax::Point achorPoint = Vec2::ZERO;
    if (!pNode->isIgnoreAnchorPointForPosition())
    {
        achorPoint = pNode->getAnchorPoint();
    }
    ax::Point ptWorld(0, adjust_coord_neg(scrSize.height, size.height, achorPoint.y, top) - delta.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionY(ptWorld.y);
}

void LayoutHelper::VisibleRect::setNodeNormalizedPositionX(ax::Node* pNode, float ratio)
{
    AX_ASSERT(pNode);
    ax::Rect visibleRect = LayoutHelper::LayoutHelper::VisibleRect::getScreenVisibleRect();
    ax::Point ptWorld(visibleRect.size.width * ratio + visibleRect.origin.x, 0);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionX(ptLocal.x);
}

void LayoutHelper::VisibleRect::setNodeNormalizedPositionY(ax::Node* pNode, float ratio)
{
    AX_ASSERT(pNode);
    ax::Rect visibleRect = LayoutHelper::LayoutHelper::VisibleRect::getScreenVisibleRect();

    ax::Point ptWorld(0, visibleRect.size.height * ratio + visibleRect.origin.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPositionY(ptLocal.y);
}
void LayoutHelper::VisibleRect::setNodeNormalizedPosition(Node* pNode, const ax::Point& ratio)
{
    AX_ASSERT(pNode);
    pNode->setIgnoreAnchorPointForPosition(false);
    pNode->setAnchorPoint(ax::Vec2(.5f, .5f));
    ax::Rect visibleRect = LayoutHelper::LayoutHelper::VisibleRect::getScreenVisibleRect();
    ax::Point ptWorld(visibleRect.size.width * ratio.x + visibleRect.origin.x,
                           visibleRect.size.height * ratio.y + visibleRect.origin.y);
    auto ptLocal = pNode->getParent()->convertToNodeSpace(ptWorld);
    pNode->setPosition(ptLocal);
}
