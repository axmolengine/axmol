#ifndef __SCROLLPANE_H__
#define __SCROLLPANE_H__

#include "FairyGUIMacros.h"
#include "Margin.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;
class GComponent;
class GScrollBar;
class FUIContainer;
class FUIInnerContainer;
class GController;
class EventContext;
class ByteBuffer;
class GTweener;

class ScrollPane : public ax::Object
{
public:
    ScrollPane(GComponent* owner);
    virtual ~ScrollPane();

    void setup(ByteBuffer* buffer);

    GComponent* getOwner() const { return _owner; }
    GComponent* getHeader() const { return _header; }
    GComponent* getFooter() const { return _footer; }
    GScrollBar* getVtScrollBar() const { return _vtScrollBar; }
    GScrollBar* getHzScrollBar() const { return _hzScrollBar; }

    bool isBouncebackEffect() const { return _bouncebackEffect; }
    void setBouncebackEffect(bool value) { _bouncebackEffect = value; }

    bool isTouchEffect() const { return _touchEffect; }
    void setTouchEffect(bool value) { _touchEffect = value; }

    bool isInertiaDisabled() const { return _inertiaDisabled; }
    void setInertiaDisabled(bool value) { _inertiaDisabled = value; }

    float getScrollStep() const { return _scrollStep; }
    void setScrollStep(float value);

    bool isSnapToItem() const { return _snapToItem; }
    void setSnapToItem(bool value) { _snapToItem = value; }

    bool isPageMode() const { return _pageMode; }
    void setPageMode(bool value) { _pageMode = value; }

    GController* getPageController() const { return _pageController; }
    void setPageController(GController* value) { _pageController = value; }

    bool isMouseWheelEnabled() const { return _mouseWheelEnabled; }
    void setMouseWheelEnabled(bool value) { _mouseWheelEnabled = value; }

    float getDecelerationRate() const { return _decelerationRate; }
    void setDecelerationRate(float value) { _decelerationRate = value; }

    float getPosX() const { return _xPos; }
    void setPosX(float value, bool ani = false);
    float getPosY() const { return _yPos; }
    void setPosY(float value, bool ani = false);

    float getPercX() const;
    void setPercX(float value, bool ani = false);
    float getPercY() const;
    void setPercY(float value, bool ani = false);

    bool isBottomMost() const;
    bool isRightMost() const;

    void scrollLeft(float ratio = 1, bool ani = false);
    void scrollRight(float ratio = 1, bool ani = false);
    void scrollUp(float ratio = 1, bool ani = false);
    void scrollDown(float ratio = 1, bool ani = false);
    void scrollTop(bool ani = false);
    void scrollBottom(bool ani = false);
    void scrollToView(GObject* obj, bool ani = false, bool setFirst = false);
    void scrollToView(const ax::Rect& rect, bool ani = false, bool setFirst = false);
    bool isChildInView(GObject* obj) const;

    int getPageX() const;
    void setPageX(int value, bool ani = false);
    int getPageY() const;
    void setPageY(int value, bool ani = false);

    float getScrollingPosX() const;
    float getScrollingPosY() const;

    const ax::Size& getContentSize() const { return _contentSize; }
    const ax::Size& getViewSize() const { return _viewSize; }

    void lockHeader(int size);
    void lockFooter(int size);

    void cancelDragging();
    static ScrollPane* getDraggingPane() { return _draggingPane; }

private:
    void onOwnerSizeChanged();
    void adjustMaskContainer();
    void setContentSize(float wv, float hv);
    void changeContentSizeOnScrolling(float deltaWidth, float deltaHeight, float deltaPosX, float deltaPosY);
    void setViewWidth(float value);
    void setViewHeight(float value);
    void setSize(float wv, float hv);
    void handleSizeChanged();

    void handleControllerChanged(GController* c);
    void updatePageController();

    GObject* hitTest(const ax::Vec2& pt, const ax::Camera* camera);

    void posChanged(bool ani);
    CALL_LATER_FUNC(ScrollPane, refresh);
    void refresh2();

    void updateScrollBarPos();
    void updateScrollBarVisible();
    void updateScrollBarVisible2(GScrollBar* bar);

    float getLoopPartSize(float division, int axis);
    bool loopCheckingCurrent();
    void loopCheckingTarget(ax::Vec2& endPos);
    void loopCheckingTarget(ax::Vec2& endPos, int axis);
    void loopCheckingNewPos(float& value, int axis);
    void alignPosition(ax::Vec2& pos, bool inertialScrolling);
    float alignByPage(float pos, int axis, bool inertialScrolling);
    ax::Vec2 updateTargetAndDuration(const ax::Vec2& orignPos);
    float updateTargetAndDuration(float pos, int axis);
    void fixDuration(int axis, float oldChange);
    void startTween(int type);
    void killTween();
    void tweenUpdate(float dt);
    float runTween(int axis, float dt);

    void checkRefreshBar();

    void onTouchBegin(EventContext* context);
    void onTouchMove(EventContext* context);
    void onTouchEnd(EventContext* context);
    void onMouseWheel(EventContext* context);
    void onRollOver(EventContext* context);
    void onRollOut(EventContext* context);
    void onBarTweenComplete(GTweener* tweener);

    ScrollType _scrollType;
    float _scrollStep;
    float _mouseWheelStep;
    Margin _scrollBarMargin;
    bool _bouncebackEffect;
    bool _touchEffect;
    bool _scrollBarDisplayAuto;
    bool _vScrollNone;
    bool _hScrollNone;
    bool _needRefresh;
    int _refreshBarAxis;
    bool _displayOnLeft;
    bool _snapToItem;
    bool _displayInDemand;
    bool _mouseWheelEnabled;
    bool _inertiaDisabled;
    float _decelerationRate;
    bool _pageMode;
    bool _floating;
    bool _dontClipMargin;

    float _xPos;
    float _yPos;

    ax::Size _viewSize;
    ax::Size _contentSize;
    ax::Size _overlapSize;
    ax::Size _pageSize;

    ax::Vec2 _containerPos;
    ax::Vec2 _beginTouchPos;
    ax::Vec2 _lastTouchPos;
    ax::Vec2 _lastTouchGlobalPos;
    ax::Vec2 _velocity;
    float _velocityScale;
    clock_t _lastMoveTime;
    bool _dragged;
    bool _isHoldAreaDone;
    int _aniFlag;
    int _loop;
    bool _hover;

    int _headerLockedSize;
    int _footerLockedSize;

    int _tweening;
    ax::Vec2 _tweenStart;
    ax::Vec2 _tweenChange;
    ax::Vec2 _tweenTime;
    ax::Vec2 _tweenDuration;

    GComponent* _owner;
    FUIContainer* _maskContainer;
    FUIInnerContainer* _container;
    GScrollBar* _hzScrollBar;
    GScrollBar* _vtScrollBar;
    GComponent* _header;
    GComponent* _footer;
    GController* _pageController;

    static int _gestureFlag;
    static ScrollPane* _draggingPane;

    friend class GComponent;
    friend class GList;
    friend class GScrollBar;
};

NS_FGUI_END

#endif
