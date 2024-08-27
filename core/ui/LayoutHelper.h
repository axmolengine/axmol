//
// Copyright (c) 2014-2020 Simdsoft Limited - All Rights Reserved
// Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).


// This module is used by x-studio UI Editor to layout UI elements
// It's very useful for programer to operate UI elements in runtime,
// so, we publish it to here.
// usage:
//      #include "ui/LayoutHelper.h"
//      LayoutHelper::centerNode(node); // the node should be already have parent.
//      LayoutHelper::makeVerticalSpacingEqual(nodes); // all the nodes shoud be in the same parent.
//
#pragma once
#ifndef _LAYOUTHELPER_H_
#    define _LAYOUTHELPER_H_

#    include "axmol.h"
#    include "base/Macros.h"

// f(x) = s * a + x
#    define adjust_coord(__sz__, __achor__, __coord__) ((__sz__) * (__achor__) + (__coord__))

// f(y) = y - s * a
#    define adjust_coord_r(__sz__, __achor__, __coord__) ((__coord__) - (__sz__) * (__achor__))

// f(x) = S - (s - s * a + x)
#    define adjust_coord_neg(__SZ__, __sz__, __achor__, __coord__) \
        ((__SZ__) - ((__sz__) - (__sz__) * (__achor__) + (__coord__)))

// f(y) = S - (s - s * a + y)
#    define adjust_coord_neg_r adjust_coord_neg

#    define center_coord(__SZ__, __sz__, __achor__) (((__SZ__) - (__sz__) + 2 * (__sz__) * (__achor__)) * 0.5f)

using namespace ax;

// inline Vec2 operator*(const Vec2& left, const ax::Vec2& right)
//{
//     return Vec2(left.width * right.x, left.height * right.y);
// }

struct AX_DLL LayoutHelper
{

    static Vec2 s_designSize;
    static float s_adjustedScale;

    /// <summary>
    /// adatpe design size with fixed edge, normally, use this function for screen adatpe
    /// </summary>
    /// <param name="designSize"></param>
    static void setDesignSizeFixedEdge(const Vec2& designSize);

    /// <summary>
    /// adapte design size with no border
    /// </summary>
    /// <param name="designSize"></param>
    static void setDesignSizeNoBorder(const Vec2& designSize);

    static ax::Vec2 getVisibleOrigin(void);
    static Vec2 getVisibleSize(void);

    /// align type defination
    enum AlignType
    {
        ALIGN_NONE    = 1 >> 1,
        ALIGN_XCENTER = 1,
        ALIGN_YCENTER = 1 << 1,
        ALIGN_LEFT    = 1 << 2,
        ALIGN_RIGHT   = 1 << 3,
        ALIGN_TOP     = 1 << 4,
        ALIGN_BOTTOM  = 1 << 5,

        ALIGN_CENTER = ALIGN_XCENTER | ALIGN_YCENTER,
        ALIGN_CT     = ALIGN_XCENTER | ALIGN_TOP,     // center top
        ALIGN_CB     = ALIGN_XCENTER | ALIGN_BOTTOM,  // center bottom

        ALIGN_LC = ALIGN_LEFT | ALIGN_YCENTER,  // left center
        ALIGN_LT = ALIGN_LEFT | ALIGN_TOP,      // left top
        ALIGN_LB = ALIGN_LEFT | ALIGN_BOTTOM,   // left bottom

        ALIGN_RC = ALIGN_RIGHT | ALIGN_YCENTER,  // right center
        ALIGN_RT = ALIGN_RIGHT | ALIGN_TOP,      // right top
        ALIGN_RB = ALIGN_RIGHT | ALIGN_BOTTOM,   // right bottom
    };

#    define isIgnoreX(align) (align != ALIGN_LEFT) && (align != ALIGN_RIGHT)
#    define isIgnoreY(align) (align != ALIGN_TOP) && (align != ALIGN_BOTTOM)
#    define isIgnoreXY(align) (align == ALIGN_CENTER)

    static ax::Vec2 getScale2D(ax::Node* pNode)
    {
        return ax::Vec2(pNode->getScaleX(), pNode->getScaleY());
    }

    static float getNodeLeftX(ax::Node* pNode)
    {
        return pNode->getPositionX() - pNode->getAnchorPoint().x * pNode->getContentSize().width * pNode->getScaleX();
    }

    static float getNodeRightX(ax::Node* pNode)
    {
        return pNode->getPositionX() +
               (1 - pNode->getAnchorPoint().x) * pNode->getContentSize().width * pNode->getScaleX();
    }

    static float getNodeTopY(ax::Node* pNode)
    {
        return pNode->getPositionY() +
               (1 - pNode->getAnchorPoint().y) * pNode->getContentSize().height * pNode->getScaleY();
    }

    static float getNodeBottomY(ax::Node* pNode)
    {
        return pNode->getPositionY() + pNode->getAnchorPoint().y * pNode->getContentSize().height * pNode->getScaleY();
    }

    /* @brief: nodes_layout set node position
    **
    */
    /*
    ** @brief: setNodePosition with achor point modified
    ** @params
    **         pNode:      the node to be set
    **         align:      align type, see enum AlighType
    **         x:          specify coord x.so
    **         y:          specify coord y.
    */
    static void setNodePosition(ax::Node* pNode, const int align, float x, float y)
    {
        const AlignType alignType = (const AlignType)align;
        if (alignType == ALIGN_NONE)
        {
            pNode->setPosition(x, y);
            return;
        }
        if (alignType & ALIGN_XCENTER)
        {
            centerNodeX(pNode);
        }
        if (alignType & ALIGN_YCENTER)
        {
            centerNodeY(pNode);
        }
        if (alignType & ALIGN_LEFT)
        {
            setNodeLeft(pNode, x);
        }
        if (alignType & ALIGN_RIGHT)
        {
            setNodeRight(pNode, x);
        }
        if (alignType & ALIGN_TOP)
        {
            setNodeTop(pNode, y);
        }
        if (alignType & ALIGN_BOTTOM)
        {
            setNodeBottom(pNode, y);
        }
    }

    /*
    ** @brief: setNodePosition with achor point modified
    ** @params
    **         pNode:      the node to be set
    **         achorPoint: specify new achor point for the node
    **         align:      align type, see enum AlighType
    **         x:          specify coord x.
    **         y:          specify coord y.
    **
    */
    static void setNodePosition(ax::Node* pNode,
                                const ax::Point& anchorPoint,
                                const int align,
                                float x,
                                float y)
    {
        pNode->setAnchorPoint(anchorPoint);
        setNodePosition(pNode, align, x, y);
    }

    static void setNodePosition(ax::Node* pNode, const int align, float value = 0.0f)
    {  // ignore x or y
        setNodePosition(pNode, align, value, value);
    }

    static void setNodePosition(ax::Node* pNode,
                                const ax::Point& anchorPoint,
                                const int align,
                                float value = 0.0f)
    {  // ignore x or y
        setNodePosition(pNode, anchorPoint, align, value, value);
    }

    // @version 1
    static void centerNodeX(ax::Node* pNode)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if (pNodeParent)
        {
            centerNodeX(pNode, pNodeParent->getContentSize());
        }
    }

    static void centerNodeY(ax::Node* pNode)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            centerNodeY(pNode, pNodeParent->getContentSize());
        }
    }

    static void centerNode(ax::Node* pNode)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            centerNode(pNode, pNodeParent->getContentSize());
        }
    }

    // @version 2
    static void centerNodeX(ax::Node* pNode, const Vec2& parentSize)
    {
        AX_ASSERT(pNode);

        Vec2 size    = pNode->getContentSize() * getScale2D(pNode);
        float achorX = 0.0f;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorX = pNode->getAnchorPoint().x;
        }
        pNode->setPositionX(center_coord(parentSize.width, size.width, achorX) /*parentSize.width * 0.5f*/);
    }

    static void centerNodeY(ax::Node* pNode, const Vec2& parentSize)
    {
        AX_ASSERT(pNode);

        Vec2 size    = pNode->getContentSize() * getScale2D(pNode);
        float achorY = 0.0f;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorY = pNode->getAnchorPoint().y;
        }
        pNode->setPositionY(center_coord(parentSize.height, size.height, achorY));
    }

    static void centerNode(ax::Node* pNode, const Vec2& parentSize)
    {
        AX_ASSERT(pNode);

        Vec2 size            = pNode->getContentSize() * getScale2D(pNode);
        ax::Point achor = ax::Vec2::ZERO;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achor = pNode->getAnchorPoint();
        }
        pNode->setPosition(ax::Vec2(center_coord(parentSize.width, size.width, achor.x),
                                         center_coord(parentSize.height, size.height, achor.y)));
    }

    // @version 1
    static void setNodeLeft(ax::Node* pNode, float left, float anchor = 0.0f)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            setNodeLeft(pNode, pNodeParent->getContentSize(), left, anchor);
        }
    }

    static float getNodeLeft(ax::Node* pNode, float anchor = 0.0f)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            return getNodeLeft(pNode, pNodeParent->getContentSize(), anchor);
        }
        return 0.0f;
    }

    static void setNodeTop(ax::Node* pNode, float top, float anchor = 0.0f)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            setNodeTop(pNode, pNodeParent->getContentSize(), top, anchor);
        }
    }

    static float getNodeTop(ax::Node* pNode, float anchor = 0.0f)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            return getNodeTop(pNode, pNodeParent->getContentSize(), anchor);
        }
        return 0.0f;
    }

    static void setNodeRight(ax::Node* pNode, float right)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            setNodeRight(pNode, pNodeParent->getContentSize(), right);
        }
    }
    static float getNodeRight(ax::Node* pNode, float anchor = 0.0f)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            return getNodeRight(pNode, pNodeParent->getContentSize(), anchor);
        }
        return 0.0f;
    }

    static void setNodeBottom(ax::Node* pNode, float bottom, float anchor = .0f)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            setNodeBottom(pNode, pNodeParent->getContentSize(), bottom, anchor);
        }
    }

    static float getNodeBottom(ax::Node* pNode, float anchor = 0.0f)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            return getNodeBottom(pNode, pNodeParent->getContentSize(), anchor);
        }
        return 0.0f;
    }

    static void setNodeLB(ax::Node* pNode, const ax::Point& p)
    {  // left bottom
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            setNodeLB(pNode, pNodeParent->getContentSize(), p);
        }
    }

    static void setNodeRB(ax::Node* pNode, const ax::Point& p)
    {  // right bottom
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            setNodeRB(pNode, pNodeParent->getContentSize(), p);
        }
    }

    static void setNodeLT(ax::Node* pNode, const ax::Point& p)
    {  // left top
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            setNodeLT(pNode, pNodeParent->getContentSize(), p);
        }
    }

    static void setNodeRT(ax::Node* pNode, const ax::Point& p)
    {  // right top
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            setNodeRT(pNode, pNodeParent->getContentSize(), p);
        }
    }

    // @version 2 used as internal interfaces
    static void setNodeLeft(ax::Node* pNode, const Vec2& parentSize, float left, float anchor = 0.0f)
    {
        AX_ASSERT(pNode);

        Vec2 size    = pNode->getContentSize() * getScale2D(pNode);
        float achorX = 0.0f;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorX = pNode->getAnchorPoint().x;
        }
        pNode->setPositionX(adjust_coord(size.width, achorX, left) - size.width * anchor);
    }
    static float getNodeLeft(ax::Node* pNode, const Vec2& parentSize, float anchor = 0.0f)
    {
        AX_ASSERT(pNode);

        Vec2 size    = pNode->getContentSize() * getScale2D(pNode);
        float achorX = 0.0f;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorX = pNode->getAnchorPoint().x;
        }
        return adjust_coord_r(size.width, achorX, pNode->getPositionX()) + anchor * size.width;
    }

    static void setNodeTop(ax::Node* pNode, const Vec2& parentSize, float top, float anchor = 0.0f)
    {
        AX_ASSERT(pNode);

        Vec2 size    = pNode->getContentSize() * getScale2D(pNode);
        float achorY = 0.0f;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorY = pNode->getAnchorPoint().y;
        }
        pNode->setPositionY(adjust_coord_neg(parentSize.height, size.height, achorY, top) - size.height * anchor);
    }
    static float getNodeTop(ax::Node* pNode, const Vec2& parentSize, float anchor = 0.0f)
    {
        AX_ASSERT(pNode);

        Vec2 size    = pNode->getContentSize() * getScale2D(pNode);
        float achorY = 0.0f;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorY = pNode->getAnchorPoint().y;
        }
        return adjust_coord_neg_r(parentSize.height, size.height, achorY, pNode->getPositionY()) + size.height * anchor;
    }

    static void setNodeRight(ax::Node* pNode, const Vec2& parentSize, float right)
    {
        AX_ASSERT(pNode);

        Vec2 size    = pNode->getContentSize() * getScale2D(pNode);
        float achorX = 0.0f;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorX = pNode->getAnchorPoint().x;
        }
        pNode->setPositionX(adjust_coord_neg(parentSize.width, size.width, achorX, right));
    }

    static float getNodeRight(ax::Node* pNode, const Vec2& parentSize, float anchor = 0.0f)
    {
        AX_ASSERT(pNode);

        Vec2 size    = pNode->getContentSize() * getScale2D(pNode);
        float achorX = 0.0f;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorX = pNode->getAnchorPoint().x;
        }
        return adjust_coord_neg_r(parentSize.width, size.width, achorX, pNode->getPositionX()) + anchor * size.width;
    }

    static void setNodeBottom(ax::Node* pNode, const Vec2& parentSize, float bottom, float anchor = 0.0f)
    {
        AX_ASSERT(pNode);

        Vec2 size    = pNode->getContentSize() * getScale2D(pNode);
        float achorY = 0.0f;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorY = pNode->getAnchorPoint().y;
        }
        pNode->setPositionY(adjust_coord(size.height, achorY, bottom) - anchor * size.height);
    }

    static float getNodeBottom(ax::Node* pNode, const Vec2& parentSize, float anchor = 0.f)
    {
        AX_ASSERT(pNode);

        Vec2 size    = pNode->getContentSize() * getScale2D(pNode);
        float achorY = 0.0f;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorY = pNode->getAnchorPoint().y;
        }
        return adjust_coord_r(size.height, achorY, pNode->getPositionY()) + size.height * anchor;
    }

    static void setNodeLB(ax::Node* pNode, const Vec2& parentSize, const ax::Point& p)
    {  // left bottom
        AX_ASSERT(pNode);

        Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
        ax::Point achorPoint = ax::Vec2::ZERO;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorPoint = pNode->getAnchorPoint();
        }
        pNode->setPosition(
            ax::Vec2(adjust_coord(size.width, achorPoint.x, p.x), adjust_coord(size.height, achorPoint.y, p.y)));
    }

    static void setNodeRB(ax::Node* pNode, const Vec2& parentSize, const ax::Point& p)
    {  // right bottom
        AX_ASSERT(pNode);

        Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
        ax::Point achorPoint = ax::Vec2::ZERO;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorPoint = pNode->getAnchorPoint();
        }
        pNode->setPosition(ax::Vec2(adjust_coord_neg(parentSize.width, size.width, achorPoint.x, p.x),
                                         adjust_coord(size.height, achorPoint.y, p.y)));
    }

    static void setNodeLT(ax::Node* pNode, const Vec2& parentSize, const ax::Point& p)
    {  // left top
        AX_ASSERT(pNode);

        Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
        ax::Point achorPoint = ax::Vec2::ZERO;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorPoint = pNode->getAnchorPoint();
        }
        pNode->setPosition(ax::Vec2(adjust_coord(size.width, achorPoint.x, p.x),
                                         adjust_coord_neg(parentSize.height, size.height, achorPoint.y, p.y)));
    }

    static void setNodeRT(ax::Node* pNode, const Vec2& parentSize, const ax::Point& p)
    {  // right top
        AX_ASSERT(pNode);

        Vec2 size                 = pNode->getContentSize() * getScale2D(pNode);
        ax::Point achorPoint = ax::Vec2::ZERO;
        if (!pNode->isIgnoreAnchorPointForPosition())
        {
            achorPoint = pNode->getAnchorPoint();
        }
        pNode->setPosition(ax::Vec2(adjust_coord_neg(parentSize.width, size.width, achorPoint.x, p.x),
                                         adjust_coord_neg(parentSize.height, size.height, achorPoint.y, p.y)));
    }

    /* set node position as normalized: @version 1 */
    static void setNodeNormalizedPositionX(ax::Node* pNode, float ratio)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            setNodeNormalizedPositionX(pNode, pNodeParent->getContentSize(), ratio);
        }
    }

    static void setNodeNormalizedPositionY(ax::Node* pNode, float ratio)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            setNodeNormalizedPositionY(pNode, pNodeParent->getContentSize(), ratio);
        }
    }
    static void setNodeNormalizedPosition(ax::Node* pNode, const ax::Point& ratio)
    {
        ax::Node* pNodeParent = pNode->getParent();
        if ((pNodeParent))
        {
            setNodeNormalizedPosition(pNode, pNodeParent->getContentSize(), ratio);
        }
    }

    /* set node position as normalized: @version 2 */
    static void setNodeNormalizedPositionX(ax::Node* pNode, const Vec2& parentSize, float ratio)
    {
        AX_ASSERT(pNode);

        pNode->setPositionX(parentSize.width * ratio);
    }

    static void setNodeNormalizedPositionY(ax::Node* pNode, const Vec2& parentSize, float ratio)
    {
        AX_ASSERT(pNode);

        pNode->setPositionY(parentSize.height * ratio);
    }
    static void setNodeNormalizedPosition(ax::Node* pNode, const Vec2& parentSize, const ax::Point& ratio)
    {
        AX_ASSERT(pNode);

        pNode->setPosition(ax::Point(parentSize.width * ratio.x, parentSize.height * ratio.y));
    }

    /// Get node group size
    static Vec2 getNodeGroupSize(const std::vector<ax::Node*>& nodes);

    static Vec2 getNodeGroupScaledSize(const std::vector<ax::Node*>& nodes);

    /// Set nodes group size
    static void setNodeGroupSize(const std::vector<ax::Node*>& nodes, const Vec2& newSize);

    /// Get Node group left
    static float getNodeGroupLeft(const std::vector<ax::Node*>& nodes);

    /// Get node group top
    static float getNodeGroupTop(const std::vector<ax::Node*>& nodes);

    /// Get node group right
    static float getNodeGroupRight(const std::vector<ax::Node*>& nodes);

    /// Get node group bottom
    static float getNodeGroupBottom(const std::vector<ax::Node*>& nodes);

    /*
    ** setNodeGroupLeft
    **
    */
    static void setNodeGroupLeft(const std::vector<ax::Node*>& nodes, float left);

    /*
    ** setNodeGroupLeft
    **
    */
    static void setNodeGroupTop(const std::vector<ax::Node*>& nodes, float top);

    static void setNodeGroupLT(const std::vector<ax::Node*>& nodes, const ax::Vec2& p);

    /*
    ** setNodeGroupRight
    **
    */
    static void setNodeGroupRight(const std::vector<ax::Node*>& nodes, float right);

    /*
    ** setNodeGroupRight
    **
    */
    static void setNodeGroupBottom(const std::vector<ax::Node*>& nodes, float bottom);

    //// move node group, use UI direction
    static void moveNodeGroupHorizontally(const std::vector<ax::Node*>& nodes, float delta);
    static void moveNodeGroupVertically(const std::vector<ax::Node*>& nodes, float delta);

    /* @brief: group layout and alignment
    ** @remark:
    */
    /// <summary>
    /// Center horiz to parent
    /// </summary>
    /// <param name="nodes"></param>
    static void centerHorizontally(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Center vertical to parent
    /// </summary>
    /// <param name="nodes"></param>
    static void centerVertically(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Center to parent
    /// </summary>
    /// <param name="nodes"></param>
    static void centerToParent(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Align lefts
    /// </summary>
    /// <param name="nodes"></param>
    static void alignLefts(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Align rights
    /// </summary>
    /// <param name="nodes"></param>
    static void alignRights(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Align tops
    /// </summary>
    /// <param name="nodes"></param>
    static void alignTops(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Align bottoms
    /// </summary>
    /// <param name="nodes"></param>
    static void alignBottoms(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Align horiz
    /// </summary>
    /// <param name="nodes"></param>
    static void alignHorizontals(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Align vertical
    /// </summary>
    /// <param name="nodes"></param>
    static void alignVerticals(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Align centers
    /// </summary>
    /// <param name="nodes"></param>
    static void alignCenters(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Make same width
    /// </summary>
    /// <param name="nodes"></param>
    static void makeSameWidth(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Make same height
    /// </summary>
    /// <param name="nodes"></param>
    static void makeSameHeight(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Make same size
    /// </summary>
    /// <param name="nodes"></param>
    static void makeSameSize(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Make horiz spacing equal
    /// </summary>
    /// <param name="nodes"></param>
    static void makeHorizontalSpacingEqual(std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Make vertical spacing equal
    /// </summary>
    /// <param name="nodes"></param>
    static void makeVerticalSpacingEqual(std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Increease horiz spacing
    /// </summary>
    /// <param name="nodes"></param>
    static void increaseHorizontalSpacing(std::vector<ax::Node*>& nodes, float theSpacing);

    /// <summary>
    /// Increase vertical spacing
    /// </summary>
    /// <param name="nodes"></param>
    static void increaseVerticalSpacing(std::vector<ax::Node*>& nodes, float theSpacing);

    /// <summary>
    /// Decrease horiz spacing
    /// </summary>
    /// <param name="nodes"></param>
    static void decreaseHorizontalSpacing(std::vector<ax::Node*>& nodes, float theSpacing);

    /// <summary>
    /// Decrease vertical spacing
    /// </summary>
    /// <param name="nodes"></param>
    static void decreaseVerticalSpacing(std::vector<ax::Node*>& nodes, float theSpacing);

    /// <summary>
    /// Remove horiz spacing
    /// </summary>
    /// <param name="nodes"></param>
    static void removeHorizontalSpacing(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// Remove Vertical spacing
    /// </summary>
    /// <param name="nodes"></param>
    static void removeVerticalSpacing(const std::vector<ax::Node*>& nodes);

    /// <summary>
    /// maybe for internal use
    /// </summary>
    /// <param name="nodes"></param>
    static void makeHorizontalSpacingEqual(const std::vector<ax::Node*>& nodes, float theSpacing);

    /// <summary>
    /// maybe for internal use
    /// </summary>
    /// <param name="nodes"></param>
    static void makeVerticalSpacingEqual(const std::vector<ax::Node*>& nodes, float theSpacing);

    /// <summary>
    /// CLASS VisibleRect
    /// </summary>
    class AX_DLL VisibleRect
    {
    public:
        static void refresh(void);

        static ax::Rect getScreenVisibleRect();
        static Vec2 size();
        static ax::Point left();
        static ax::Point right();
        static ax::Point top();
        static ax::Point bottom();
        static ax::Point center();
        static ax::Point leftTop();
        static ax::Point rightTop();
        static ax::Point leftBottom();
        static ax::Point rightBottom();

        // ------- The APIs for set layout node in visible screen rect ------
        static void setNodePosition(ax::Node* pNode, const ax::Point& p)
        {
            setNodeNormalizedPosition(pNode, ax::Vec2(p.x / s_designSize.width * s_adjustedScale,
                                                           p.y / s_designSize.height * s_adjustedScale));
        }
        static void centerNode(ax::Node* pNode) { setNodeNormalizedPosition(pNode, ax::Vec2(.5f, .5f)); }
        static void centerNodeX(ax::Node* pNode) { setNodeNormalizedPositionX(pNode, .5f); }
        static void centerNodeY(ax::Node* pNode) { setNodeNormalizedPositionY(pNode, .5f); }

        static void setNodeLeft(ax::Node* pNode, float left);
        static void setNodeTop(ax::Node* pNode, float top);
        static void setNodeRight(ax::Node* pNode, float right);
        static void setNodeBottom(ax::Node* pNode, float bottom);

        static float getNodeLeft(ax::Node* pNode);
        static float getNodeTop(ax::Node* pNode);
        static float getNodeRight(ax::Node* pNode);
        static float getNodeBottom(ax::Node* pNode);

        static void setNodeLT(ax::Node* pNode, const ax::Point& p);
        static void setNodeRT(ax::Node* pNode, const ax::Point& p);
        static void setNodeLB(ax::Node* pNode, const ax::Point& p);
        static void setNodeRB(ax::Node* pNode, const ax::Point& p);

        /// ratio position
        static void setNodeNormalizedLT(ax::Node* pNode, const ax::Point& ratio);
        static void setNodeNormalizedRT(ax::Node* pNode, const ax::Point& ratio);
        static void setNodeNormalizedLB(ax::Node* pNode, const ax::Point& ratio);
        static void setNodeNormalizedRB(ax::Node* pNode, const ax::Point& ratio);

        static void setNodeNormalizedTop(ax::Node* pNode, const float ratioTop);

        static void setNodeNormalizedPositionX(ax::Node* pNode, float ratio);

        static void setNodeNormalizedPositionY(ax::Node* pNode, float ratio);
        static void setNodeNormalizedPosition(ax::Node* pNode, const ax::Point& ratio);

    private:
        static void lazyInit();
        static ax::Rect s_ScreenVisibleRect;
    };
};

#endif
