//
// Copyright (c) 2014-2017 purelib,x-studio365 - All Rights Reserved
// This module is used by x-studio365 UI Editor to layout UI elements
// It's very useful for programer to operate UI elements in runtime,
// so, we publish it to here.
// usage:
//      #include "cocostudio/ext/LayoutHelper"
//          nodes_layout::centerNode(node); // the node should be already have parent.
//          nodes_layout::makeVerticalSpacingEqual(nodes); // all the nodes shoud be in the same parent.
// 
#ifndef _NXNODESLAYOUT_H_
#define _NXNODESLAYOUT_H_

#include "politedef.h"
#include "cocos2d.h"
#include "editor-support/cocostudio/CocosStudioExport.h"

using namespace cocos2d;
// using namespace cocos2d::extension;
using namespace purelib;

// f(x) = s * a + x
#define adjust_coord(__sz__,__achor__,__coord__)             ( (__sz__) * (__achor__) + (__coord__) )

// f(y) = y - s * a
#define adjust_coord_r(__sz__,__achor__,__coord__)           ( (__coord__) - (__sz__) * (__achor__) )

// f(x) = S - (s - s * a + x)
#define adjust_coord_neg(__SZ__,__sz__,__achor__,__coord__)  ( (__SZ__) - ( (__sz__) - (__sz__) * (__achor__) + (__coord__) ) )

// f(y) = S - (s - s * a + y)
#define adjust_coord_neg_r adjust_coord_neg

#define center_coord(__SZ__,__sz__,__achor__) ( ( (__SZ__) - (__sz__) + 2 * (__sz__) * (__achor__) ) * 0.5f )


inline cocos2d::Size operator*(const cocos2d::Size& left, const cocos2d::Size& right)
{
    return cocos2d::Size(left.width * right.width, left.height * right.height);
}

inline cocos2d::Size operator*(const cocos2d::Size& left, const cocos2d::Vec2& right)
{
    return cocos2d::Size(left.width * right.x, left.height * right.y);
}

namespace purelib {

    class CC_STUDIO_DLL ScreenVisibleRect
    {
    public:
        static void   refresh(void);

        static cocos2d::Rect getScreenVisibleRect();
        static cocos2d::Size size();
        static cocos2d::Point left();
        static cocos2d::Point right();
        static cocos2d::Point top();
        static cocos2d::Point bottom();
        static cocos2d::Point center();
        static cocos2d::Point leftTop();
        static cocos2d::Point rightTop();
        static cocos2d::Point leftBottom();
        static cocos2d::Point rightBottom();

    private:
        static void lazyInit();
        static cocos2d::Rect s_ScreenVisibleRect;
    };

    class CC_STUDIO_DLL nodes_layout {
        PLIB_STATICIZE_CLASS(nodes_layout);

        static float adjustedScale;
        static cocos2d::Size designSize;

        static void setDesignSizeNoBorder(const cocos2d::Size& designSize);

        static void setDesignSizeFixedEdge(const cocos2d::Size& designSize);
        static cocos2d::Vec2 getVisibleOrigin(void);
        static cocos2d::Size getVisibleSize(void);

        /// align type defination
        enum AlignType {
            ALIGN_NONE = 1 >> 1,
            ALIGN_XCENTER = 1,
            ALIGN_YCENTER = 1 << 1,
            ALIGN_LEFT = 1 << 2,
            ALIGN_RIGHT = 1 << 3,
            ALIGN_TOP = 1 << 4,
            ALIGN_BOTTOM = 1 << 5,

            ALIGN_CENTER = ALIGN_XCENTER | ALIGN_YCENTER,
            ALIGN_CT = ALIGN_XCENTER | ALIGN_TOP,    // center top
            ALIGN_CB = ALIGN_XCENTER | ALIGN_BOTTOM, // center bottom

            ALIGN_LC = ALIGN_LEFT | ALIGN_YCENTER,   // left center
            ALIGN_LT = ALIGN_LEFT | ALIGN_TOP,       // left top
            ALIGN_LB = ALIGN_LEFT | ALIGN_BOTTOM,    // left bottom

            ALIGN_RC = ALIGN_RIGHT | ALIGN_YCENTER,  // right center
            ALIGN_RT = ALIGN_RIGHT | ALIGN_TOP,      // right top
            ALIGN_RB = ALIGN_RIGHT | ALIGN_BOTTOM,   // right bottom
        };

#define isIgnoreX(align)  (align != ALIGN_LEFT) && (align != ALIGN_RIGHT)
#define isIgnoreY(align)  (align != ALIGN_TOP) && (align != ALIGN_BOTTOM)
#define isIgnoreXY(align) (align == ALIGN_CENTER)

        static const cocos2d::Point  ANCHOR_CENTER; // center
        static const cocos2d::Point  ANCHOR_LB;  // left bottom,
        static const cocos2d::Point  ANCHOR_LT;  // left top
        static const cocos2d::Point  ANCHOR_RB;  // rigth bottom
        static const cocos2d::Point  ANCHOR_RT;  // rigth top

        static cocos2d::Vec2    getScale2D(Node* pNode) { return cocos2d::Vec2(pNode->getScaleX(), pNode->getScaleY()); }

        static float getNodeLeftX(Node* pNode) {
            return pNode->getPositionX() - pNode->getAnchorPoint().x * pNode->getContentSize().width * pNode->getScaleX();
        }

        static float getNodeRightX(Node* pNode) {
            return pNode->getPositionX() + (1 - pNode->getAnchorPoint().x) * pNode->getContentSize().width * pNode->getScaleX();
        }

        static float getNodeTopY(Node* pNode) {
            return pNode->getPositionY() + (1 - pNode->getAnchorPoint().y) * pNode->getContentSize().height * pNode->getScaleY();
        }

        static float getNodeBottomY(Node* pNode) {
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
        static void setNodePosition(Node* pNode, const int align, float x, float y)
        {
            const AlignType alignType = (const AlignType)align;
            if (alignType == ALIGN_NONE) {
                pNode->setPosition(x, y);
                return;
            }
            if (alignType & ALIGN_XCENTER) {
                centerNodeX(pNode);
            }
            if (alignType & ALIGN_YCENTER) {
                centerNodeY(pNode);
            }
            if (alignType & ALIGN_LEFT) {
                setNodeLeft(pNode, x);
            }
            if (alignType & ALIGN_RIGHT) {
                setNodeRight(pNode, x);
            }
            if (alignType & ALIGN_TOP) {
                setNodeTop(pNode, y);
            }
            if (alignType & ALIGN_BOTTOM) {
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
        static void setNodePosition(Node* pNode, const cocos2d::Point& anchorPoint, const int align, float x, float y)
        {
            pNode->setAnchorPoint(anchorPoint);
            setNodePosition(pNode, align, x, y);
        }

        static void setNodePosition(Node* pNode, const int align, float value = 0.0f)
        { // ignore x or y
            setNodePosition(pNode, align, value, value);
        }

        static void setNodePosition(Node* pNode, const cocos2d::Point& anchorPoint, const int align, float value = 0.0f)
        { // ignore x or y
            setNodePosition(pNode, anchorPoint, align, value, value);
        }

        // @version 1
        static void centerNodeX(Node* pNode)
        {
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                nodes_layout::centerNodeX(pNode, pNodeParent->getContentSize());
            }
        }

        static void centerNodeY(Node* pNode)
        {
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                nodes_layout::centerNodeY(pNode, pNodeParent->getContentSize());
            }
        }

        static void centerNode(Node* pNode)
        {
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                nodes_layout::centerNode(pNode, pNodeParent->getContentSize());
            }
        }

        // @version 2
        static void centerNodeX(Node* pNode, const cocos2d::Size& parentSize)
        {
            CC_ASSERT(pNode);

            cocos2d::Size size = pNode->getContentSize() * getScale2D(pNode);
            float achorX = 0.0f;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorX = pNode->getAnchorPoint().x;
            }
            pNode->setPositionX(center_coord(parentSize.width, size.width, achorX)/*parentSize.width * 0.5f*/);
        }



        static void centerNodeY(Node* pNode, const cocos2d::Size& parentSize)
        {
            CC_ASSERT(pNode);

            cocos2d::Size size = pNode->getContentSize() * getScale2D(pNode);
            float achorY = 0.0f;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorY = pNode->getAnchorPoint().y;
            }
            pNode->setPositionY(center_coord(parentSize.height, size.height, achorY));
        }

        static void centerNode(Node* pNode, const cocos2d::Size& parentSize)
        {
            CC_ASSERT(pNode);

            cocos2d::Size size = pNode->getContentSize() * getScale2D(pNode);
            cocos2d::Point achor = Vec2::ZERO;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achor = pNode->getAnchorPoint();
            }
            pNode->setPosition(cocos2d::Vec2(center_coord(parentSize.width, size.width, achor.x),
                center_coord(parentSize.height, size.height, achor.y)));
        }

        // @version 1
        static void setNodeLeft(Node* pNode, float left, float anchor = 0.0f)
        {
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                setNodeLeft(pNode, pNodeParent->getContentSize(), left, anchor);
            }
        }

        static float getNodeLeft(Node* pNode, float anchor = 0.0f)
        {
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                return getNodeLeft(pNode, pNodeParent->getContentSize(), anchor);
            }
            return 0.0f;
        }

        static void setNodeTop(Node* pNode, float top, float anchor = 0.0f)
        {
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                nodes_layout::setNodeTop(pNode, pNodeParent->getContentSize(), top, anchor);
            }
        }

        static float getNodeTop(Node* pNode, float anchor = 0.0f)
        {
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                return getNodeTop(pNode, pNodeParent->getContentSize(), anchor);
            }
            return 0.0f;
        }

        static void setNodeRight(Node* pNode, float right)
        {
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                nodes_layout::setNodeRight(pNode, pNodeParent->getContentSize(), right);
            }
        }
        static float getNodeRight(Node* pNode, float anchor = 0.0f)
        {
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                return getNodeRight(pNode, pNodeParent->getContentSize(), anchor);
            }
            return 0.0f;
        }

        static void setNodeBottom(Node* pNode, float bottom, float anchor = .0f)
        {
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                nodes_layout::setNodeBottom(pNode, pNodeParent->getContentSize(), bottom, anchor);
            }
        }

        static float getNodeBottom(Node* pNode, float anchor = 0.0f)
        {
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                return getNodeBottom(pNode, pNodeParent->getContentSize(), anchor);
            }
            return 0.0f;
        }

        static void setNodeLB(Node* pNode, const cocos2d::Point& p)
        { // left bottom
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                nodes_layout::setNodeLB(pNode, pNodeParent->getContentSize(), p);
            }
        }

        static void setNodeRB(Node* pNode, const cocos2d::Point& p)
        { // right bottom
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                nodes_layout::setNodeRB(pNode, pNodeParent->getContentSize(), p);
            }
        }

        static void setNodeLT(Node* pNode, const cocos2d::Point& p)
        { // left top
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                nodes_layout::setNodeLT(pNode, pNodeParent->getContentSize(), p);
            }
        }

        static void setNodeRT(Node* pNode, const cocos2d::Point& p)
        { // right top
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                nodes_layout::setNodeRT(pNode, pNodeParent->getContentSize(), p);
            }
        }

        // @version 2 used as internal interfaces
        static void setNodeLeft(Node* pNode, const cocos2d::Size& parentSize, float left, float anchor = 0.0f)
        {
            CC_ASSERT(pNode);

            cocos2d::Size size = pNode->getContentSize() * getScale2D(pNode);
            float achorX = 0.0f;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorX = pNode->getAnchorPoint().x;
            }
            pNode->setPositionX(adjust_coord(size.width, achorX, left) - size.width * anchor);
        }
        static float getNodeLeft(Node* pNode, const cocos2d::Size& parentSize, float anchor = 0.0f)
        {
            CC_ASSERT(pNode);

            Size size = pNode->getContentSize() * getScale2D(pNode);
            float achorX = 0.0f;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorX = pNode->getAnchorPoint().x;
            }
            return adjust_coord_r(size.width, achorX, pNode->getPositionX()) + anchor * size.width;
        }

        static void setNodeTop(Node* pNode, const cocos2d::Size& parentSize, float top, float anchor = 0.0f)
        {
            CC_ASSERT(pNode);

            cocos2d::Size size = pNode->getContentSize() * getScale2D(pNode);
            float achorY = 0.0f;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorY = pNode->getAnchorPoint().y;
            }
            pNode->setPositionY(adjust_coord_neg(parentSize.height, size.height, achorY, top) - size.height * anchor);
        }
        static float getNodeTop(Node* pNode, const cocos2d::Size& parentSize, float anchor = 0.0f)
        {
            CC_ASSERT(pNode);

            Size size = pNode->getContentSize() * getScale2D(pNode);
            float achorY = 0.0f;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorY = pNode->getAnchorPoint().y;
            }
            return adjust_coord_neg_r(parentSize.height, size.height, achorY, pNode->getPositionY()) + size.height * anchor;
        }

        static void setNodeRight(Node* pNode, const cocos2d::Size& parentSize, float right)
        {
            CC_ASSERT(pNode);

            cocos2d::Size size = pNode->getContentSize() * getScale2D(pNode);
            float achorX = 0.0f;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorX = pNode->getAnchorPoint().x;
            }
            pNode->setPositionX(adjust_coord_neg(parentSize.width, size.width, achorX, right));
        }

        static float getNodeRight(Node* pNode, const cocos2d::Size& parentSize, float anchor = 0.0f)
        {
            CC_ASSERT(pNode);

            Size size = pNode->getContentSize() * getScale2D(pNode);
            float achorX = 0.0f;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorX = pNode->getAnchorPoint().x;
            }
            return adjust_coord_neg_r(parentSize.width, size.width, achorX, pNode->getPositionX()) + anchor * size.width;
        }

        static void setNodeBottom(Node* pNode, const cocos2d::Size& parentSize, float bottom, float anchor = 0.0f)
        {
            CC_ASSERT(pNode);

            Size size = pNode->getContentSize() * getScale2D(pNode);
            float achorY = 0.0f;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorY = pNode->getAnchorPoint().y;
            }
            pNode->setPositionY(adjust_coord(size.height, achorY, bottom) - anchor * size.height);
        }

        static float getNodeBottom(Node* pNode, const cocos2d::Size& parentSize, float anchor = 0.f)
        {
            CC_ASSERT(pNode);

            Size size = pNode->getContentSize() * getScale2D(pNode);
            float achorY = 0.0f;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorY = pNode->getAnchorPoint().y;
            }
            return adjust_coord_r(size.height, achorY, pNode->getPositionY()) + size.height * anchor;
        }

        static void setNodeLB(Node* pNode, const cocos2d::Size& parentSize, const cocos2d::Point& p)
        { // left bottom
            CC_ASSERT(pNode);

            Size size = pNode->getContentSize() * getScale2D(pNode);
            cocos2d::Point achorPoint = Vec2::ZERO;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorPoint = pNode->getAnchorPoint();
            }
            pNode->setPosition(cocos2d::Vec2(
                adjust_coord(size.width, achorPoint.x, p.x),
                adjust_coord(size.height, achorPoint.y, p.y)));
        }

        static void setNodeRB(Node* pNode, const cocos2d::Size& parentSize, const cocos2d::Point& p)
        { // right bottom
            CC_ASSERT(pNode);

            Size size = pNode->getContentSize() * getScale2D(pNode);
            cocos2d::Point achorPoint = Vec2::ZERO;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorPoint = pNode->getAnchorPoint();
            }
            pNode->setPosition(cocos2d::Vec2(
                adjust_coord_neg(parentSize.width, size.width, achorPoint.x, p.x),
                adjust_coord(size.height, achorPoint.y, p.y)));
        }

        static void setNodeLT(Node* pNode, const cocos2d::Size& parentSize, const cocos2d::Point& p)
        { // left top
            CC_ASSERT(pNode);

            Size size = pNode->getContentSize() * getScale2D(pNode);
            cocos2d::Point achorPoint = Vec2::ZERO;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorPoint = pNode->getAnchorPoint();
            }
            pNode->setPosition(cocos2d::Vec2(
                adjust_coord(size.width, achorPoint.x, p.x),
                adjust_coord_neg(parentSize.height, size.height, achorPoint.y, p.y)));
        }

        static void setNodeRT(Node* pNode, const cocos2d::Size& parentSize, const cocos2d::Point& p)
        { // right top
            CC_ASSERT(pNode);

            Size size = pNode->getContentSize() * getScale2D(pNode);
            cocos2d::Point achorPoint = Vec2::ZERO;
            if (!pNode->isIgnoreAnchorPointForPosition())
            {
                achorPoint = pNode->getAnchorPoint();
            }
            pNode->setPosition(cocos2d::Vec2(
                adjust_coord_neg(parentSize.width, size.width, achorPoint.x, p.x),
                adjust_coord_neg(parentSize.height, size.height, achorPoint.y, p.y)));
        }

        /* set node position as normalized: @version 1 */
        static void setNodeNormalizedPositionX(cocos2d::Node* pNode, float ratio)
        {
            cocos2d::Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                setNodeNormalizedPositionX(pNode, pNodeParent->getContentSize(), ratio);
            }
        }

        static void setNodeNormalizedPositionY(cocos2d::Node* pNode, float ratio)
        {
            cocos2d::Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                setNodeNormalizedPositionY(pNode, pNodeParent->getContentSize(), ratio);
            }
        }
        static void setNodeNormalizedPosition(Node* pNode, const cocos2d::Point& ratio)
        {
            Node* pNodeParent = pNode->getParent();
            if (_IsNotNull(pNodeParent))
            {
                setNodeNormalizedPosition(pNode, pNodeParent->getContentSize(), ratio);
            }
        }

        /* set node position as normalized: @version 2 */
        static void setNodeNormalizedPositionX(cocos2d::Node* pNode, const cocos2d::Size& parentSize, float ratio)
        {
            CC_ASSERT(pNode);

            pNode->setPositionX(parentSize.width * ratio);
        }

        static void setNodeNormalizedPositionY(cocos2d::Node* pNode, const cocos2d::Size& parentSize, float ratio)
        {
            CC_ASSERT(pNode);

            pNode->setPositionY(parentSize.height * ratio);
        }
        static void setNodeNormalizedPosition(Node* pNode, const cocos2d::Size& parentSize, const cocos2d::Point& ratio)
        {
            CC_ASSERT(pNode);

            pNode->setPosition(cocos2d::Point(parentSize.width * ratio.x, parentSize.height * ratio.y));
        }

        ///// visible screen location methods //////
        class CC_STUDIO_DLL scr {
            PLIB_STATICIZE_CLASS(scr);

            static void setNodePosition(Node* pNode, const cocos2d::Point& p)
            {
                setNodeNormalizedPosition(pNode, cocos2d::Vec2(p.x / designSize.width * nodes_layout::adjustedScale, p.y / designSize.height * nodes_layout::adjustedScale));
            }
            static void centerNode(Node* pNode)
            {
                setNodeNormalizedPosition(pNode, cocos2d::Vec2(.5f, .5f));
            }
            static void centerNodeX(Node* pNode)
            {
                setNodeNormalizedPositionX(pNode, .5f);
            }
            static void centerNodeY(Node* pNode)
            {
                setNodeNormalizedPositionY(pNode, .5f);
            }
            static void setNodeLeft(Node* pNode, float left);
            static void setNodeTop(Node* pNode, float top);
            static void setNodeRight(Node* pNode, float right);
            static void setNodeBottom(Node* pNode, float bottom);

            static float getNodeLeft(Node* pNode);
            static float getNodeTop(Node* pNode);
            static float getNodeRight(Node* pNode);
            static float getNodeBottom(Node* pNode);

            static void setNodeLT(Node* pNode, const cocos2d::Point& p);
            static void setNodeRT(Node* pNode, const cocos2d::Point& p);
            static void setNodeLB(Node* pNode, const cocos2d::Point& p);
            static void setNodeRB(Node* pNode, const cocos2d::Point& p);

            /// ratio position
            static void setNodeNormalizedLT(Node* pNode, const cocos2d::Point& ratio);
            static void setNodeNormalizedRT(Node* pNode, const cocos2d::Point& ratio);
            static void setNodeNormalizedLB(Node* pNode, const cocos2d::Point& ratio);
            static void setNodeNormalizedRB(Node* pNode, const cocos2d::Point& ratio);

            static void setNodeNormalizedTop(Node* pNode, const float ratioTop);

            static void setNodeNormalizedPositionX(cocos2d::Node* pNode, float ratio);

            static void setNodeNormalizedPositionY(cocos2d::Node* pNode, float ratio);
            static void setNodeNormalizedPosition(Node* pNode, const cocos2d::Point& ratio);
        };


        /// Get node group size
        static Size getNodeGroupSize(const std::vector<Node*>& nodes);

        static Size getNodeGroupScaledSize(const std::vector<Node*>& nodes);

        /// Set nodes group size
        static void   setNodeGroupSize(const std::vector<Node*>& nodes, const cocos2d::Size& newSize);

        /// Get Node group left
        static float  getNodeGroupLeft(const std::vector<Node*>& nodes);

        /// Get node group top
        static float  getNodeGroupTop(const std::vector<Node*>& nodes);

        /// Get node group right
        static float  getNodeGroupRight(const std::vector<Node*>& nodes);

        /// Get node group bottom
        static float  getNodeGroupBottom(const std::vector<Node*>& nodes);

        /*
        ** setNodeGroupLeft
        **
        */
        static void setNodeGroupLeft(const std::vector<Node*>& nodes, float left);

        /*
        ** setNodeGroupLeft
        **
        */
        static void setNodeGroupTop(const std::vector<Node*>& nodes, float top);

        static void setNodeGroupLT(const std::vector<Node*>& nodes, const cocos2d::Vec2& p);

        /*
        ** setNodeGroupRight
        **
        */
        static void setNodeGroupRight(const std::vector<Node*>& nodes, float right);

        /*
        ** setNodeGroupRight
        **
        */
        static void setNodeGroupBottom(const std::vector<Node*>& nodes, float bottom);

        //// move node group, use UI direction
        static void moveNodeGroupHorizontally(const std::vector<Node*>& nodes, float delta);
        static void moveNodeGroupVertically(const std::vector<Node*>& nodes, float delta);

        /* @brief: group layout and alignment
        ** @remark:
        */
        /// <summary>
        /// 水平居中
        /// </summary>
        /// <param name="nodes"></param>
        static void centerHorizontally(const std::vector<Node*>& nodes);

        /// <summary>
        /// 垂直居中
        /// </summary>
        /// <param name="nodes"></param>
        static void centerVertically(const std::vector<Node*>& nodes);

        /// <summary>
        /// 居中
        /// </summary>
        /// <param name="nodes"></param>
        static void centerHoriVerti(const std::vector<Node*>& nodes);

        /// <summary>
        /// 左对齐
        /// </summary>
        /// <param name="nodes"></param>
        static void alignLefts(const std::vector<Node*>& nodes);

        /// <summary>
        /// 右对齐
        /// </summary>
        /// <param name="nodes"></param>
        static void alignRights(const std::vector<Node*>& nodes);

        /// <summary>
        /// 顶端对齐
        /// </summary>
        /// <param name="nodes"></param>
        static void alignTops(const std::vector<Node*>& nodes);

        /// <summary>
        /// 底端对齐
        /// </summary>
        /// <param name="nodes"></param>
        static void alignBottoms(const std::vector<Node*>& nodes);

        /// <summary>
        /// 水平对齐
        /// </summary>
        /// <param name="nodes"></param>
        static void alignHorizontals(const std::vector<Node*>& nodes);

        /// <summary>
        /// 垂直对齐
        /// </summary>
        /// <param name="nodes"></param>
        static void alignVerticals(const std::vector<Node*>& nodes);

        /// <summary>
        /// 中心原点对齐
        /// </summary>
        /// <param name="nodes"></param>
        static void alignCenters(const std::vector<Node*>& nodes);

        /// <summary>
        /// 使宽度相等
        /// </summary>
        /// <param name="nodes"></param>
        static void makeSameWidth(const std::vector<Node*>& nodes);

        /// <summary>
        /// 使高度相等
        /// </summary>
        /// <param name="nodes"></param>
        static void makeSameHeight(const std::vector<Node*>& nodes);

        /// <summary>
        /// 使得大小相等
        /// </summary>
        /// <param name="nodes"></param>
        static void makeSameSize(const std::vector<Node*>& nodes);

        /// <summary>
        /// 使水平间距相等
        /// </summary>
        /// <param name="nodes"></param>
        static void makeHorizontalSpacingEqual(std::vector<Node*>& nodes);

        /// <summary>
        /// 使垂直间距相等
        /// </summary>
        /// <param name="nodes"></param>
        static void makeVerticalSpacingEqual(std::vector<Node*>& nodes);

        /// <summary>
        /// 增加水平间距
        /// </summary>
        /// <param name="nodes"></param>
        static void increaseHorizontalSpacing(std::vector<Node*>& nodes, float theSpacing);

        /// <summary>
        /// 增加垂直间距
        /// </summary>
        /// <param name="nodes"></param>
        static void increaseVerticalSpacing(std::vector<Node*>& nodes, float theSpacing);

        /// <summary>
        /// 减少水平间距
        /// </summary>
        /// <param name="nodes"></param>
        static void decreaseHorizontalSpacing(std::vector<Node*>& nodes, float theSpacing);

        /// <summary>
        /// 减少垂直间距
        /// </summary>
        /// <param name="nodes"></param>
        static void decreaseVerticalSpacing(std::vector<Node*>& nodes, float theSpacing);

        /// <summary>
        /// 移除水平间距
        /// </summary>
        /// <param name="nodes"></param>
        static void removeHorizontalSpacing(const std::vector<Node*>& nodes);

        /// <summary>
        /// 移除垂直间距
        /// </summary>
        /// <param name="nodes"></param>
        static void removeVerticalSpacing(const std::vector<Node*>& nodes);

        /// <summary>
        /// 使水平间距相等 maybe for internal use
        /// </summary>
        /// <param name="nodes"></param>
        static void makeHorizontalSpacingEqual(const std::vector<Node*>& nodes, float theSpacing);

        /// <summary>
        /// 使垂直间距相等 maybe for internal use
        /// </summary>
        /// <param name="nodes"></param>
        static void makeVerticalSpacingEqual(const std::vector<Node*>& nodes, float theSpacing);


    }; /* STATIC CLASS nodes_layout */

}

using namespace purelib;

#endif

