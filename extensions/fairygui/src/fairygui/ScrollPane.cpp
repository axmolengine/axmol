#include "ScrollPane.h"
#include "GList.h"
#include "GScrollBar.h"
#include "UIConfig.h"
#include "UIPackage.h"
#include "event/InputProcessor.h"
#include "tween/GTween.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

ScrollPane* ScrollPane::_draggingPane = nullptr;
int ScrollPane::_gestureFlag = 0;

static const float TWEEN_TIME_GO = 0.5f;      //tween time for SetPos(ani)
static const float TWEEN_TIME_DEFAULT = 0.3f; //min tween time for inertial scroll
static const float PULL_RATIO = 0.5f;         //pull down/up ratio

static inline float sp_getField(const Vec2& pt, int axis) { return axis == 0 ? pt.x : pt.y; }
static void sp_setField(Vec2& pt, int axis, float value)
{
    if (axis == 0)
        pt.x = value;
    else
        pt.y = value;
}
static void sp_incField(Vec2& pt, int axis, float value)
{
    if (axis == 0)
        pt.x += value;
    else
        pt.y += value;
}

static inline float sp_EaseFunc(float t, float d)
{
    t = t / d - 1;
    return t * t * t + 1; //cubicOut
}

// clang-format off
ScrollPane::ScrollPane(GComponent* owner)
    : _vtScrollBar(nullptr),
      _hzScrollBar(nullptr),
      _header(nullptr),
      _footer(nullptr),
      _pageController(nullptr),
      _needRefresh(false),
      _refreshBarAxis(0),
      _aniFlag(0),
      _loop(0),
      _headerLockedSize(0),
      _footerLockedSize(0),
      _vScrollNone(false),
      _hScrollNone(false),
      _tweening(0),
      _xPos(0),
      _yPos(0),
      _floating(false),
      _dontClipMargin(false),
      _mouseWheelEnabled(true),
      _hover(false),
      _dragged(false),
      _owner(owner),
      _scrollStep(UIConfig::defaultScrollStep),
      _mouseWheelStep(UIConfig::defaultScrollStep * 2),
      _decelerationRate(UIConfig::defaultScrollDecelerationRate),
      _touchEffect(UIConfig::defaultScrollTouchEffect),
      _bouncebackEffect(UIConfig::defaultScrollBounceEffect),
      _pageSize(Vec2::ONE)
{
    _maskContainer = FUIContainer::create();
    _maskContainer->setCascadeOpacityEnabled(true);
    _owner->displayObject()->addChild(_maskContainer);

    _container = (FUIInnerContainer*)_owner->displayObject()->getChildren().at(0);
    _container->setPosition2(0, 0);
    _container->removeFromParent();
    _maskContainer->addChild(_container, 1);

    _owner->addEventListener(UIEventType::MouseWheel, AX_CALLBACK_1(ScrollPane::onMouseWheel, this));
    _owner->addEventListener(UIEventType::TouchBegin, AX_CALLBACK_1(ScrollPane::onTouchBegin, this));
    _owner->addEventListener(UIEventType::TouchMove, AX_CALLBACK_1(ScrollPane::onTouchMove, this));
    _owner->addEventListener(UIEventType::TouchEnd, AX_CALLBACK_1(ScrollPane::onTouchEnd, this));
    _owner->addEventListener(UIEventType::Exit, [this](EventContext*) {
        if (_draggingPane == this)
            _draggingPane = nullptr;
    });
}
// clang-format on

ScrollPane::~ScrollPane()
{
    CALL_PER_FRAME_CANCEL(ScrollPane, tweenUpdate);
    CALL_LATER_CANCEL(ScrollPane, refresh);

    if (_hzScrollBar)
        _hzScrollBar->release();
    if (_vtScrollBar)
        _vtScrollBar->release();
    if (_header)
        _header->release();
    if (_footer)
        _footer->release();

    if (_draggingPane == this)
        _draggingPane = nullptr;
}

void ScrollPane::setup(ByteBuffer* buffer)
{
    _scrollType = (ScrollType)buffer->readByte();
    ScrollBarDisplayType scrollBarDisplay = (ScrollBarDisplayType)buffer->readByte();
    int flags = buffer->readInt();

    if (buffer->readBool())
    {
        _scrollBarMargin.top = buffer->readInt();
        _scrollBarMargin.bottom = buffer->readInt();
        _scrollBarMargin.left = buffer->readInt();
        _scrollBarMargin.right = buffer->readInt();
    }

    const std::string& vtScrollBarRes = buffer->readS();
    const std::string& hzScrollBarRes = buffer->readS();
    const std::string& headerRes = buffer->readS();
    const std::string& footerRes = buffer->readS();

    _displayOnLeft = (flags & 1) != 0;
    _snapToItem = (flags & 2) != 0;
    _displayInDemand = (flags & 4) != 0;
    _pageMode = (flags & 8) != 0;
    if ((flags & 16) != 0)
        _touchEffect = true;
    else if ((flags & 32) != 0)
        _touchEffect = false;
    if ((flags & 64) != 0)
        _bouncebackEffect = true;
    else if ((flags & 128) != 0)
        _bouncebackEffect = false;
    _inertiaDisabled = (flags & 256) != 0;
    _maskContainer->setClippingEnabled((flags & 512) == 0);
    _floating = (flags & 1024) != 0;
    _dontClipMargin = (flags & 2048) != 0;

    if (scrollBarDisplay == ScrollBarDisplayType::DEFAULT)
    {
#ifdef AX_PLATFORM_PC
        scrollBarDisplay = UIConfig::defaultScrollBarDisplay;
#else
        scrollBarDisplay = ScrollBarDisplayType::AUTO;
#endif
    }

    if (scrollBarDisplay != ScrollBarDisplayType::HIDDEN)
    {
        if (_scrollType == ScrollType::BOTH || _scrollType == ScrollType::VERTICAL)
        {
            const std::string& res = vtScrollBarRes.size() == 0 ? UIConfig::verticalScrollBar : vtScrollBarRes;
            if (res.length() > 0)
            {
                _vtScrollBar = dynamic_cast<GScrollBar*>(UIPackage::createObjectFromURL(res));
                if (_vtScrollBar == nullptr)
                    AXLOGW("FairyGUI: cannot create scrollbar from {}", res);
                else
                {
                    _vtScrollBar->retain();
                    _vtScrollBar->setScrollPane(this, true);
                    _vtScrollBar->_alignToBL = true;
                    _owner->displayObject()->addChild(_vtScrollBar->displayObject());
                }
            }
        }
        if (_scrollType == ScrollType::BOTH || _scrollType == ScrollType::HORIZONTAL)
        {
            const std::string& res = hzScrollBarRes.length() == 0 ? UIConfig::horizontalScrollBar : hzScrollBarRes;
            if (res.length() > 0)
            {
                _hzScrollBar = dynamic_cast<GScrollBar*>(UIPackage::createObjectFromURL(res));
                if (_hzScrollBar == nullptr)
                    AXLOGW("FairyGUI: cannot create scrollbar from {}", res);
                else
                {
                    _hzScrollBar->retain();
                    _hzScrollBar->setScrollPane(this, false);
                    _hzScrollBar->_alignToBL = true;
                    _owner->displayObject()->addChild(_hzScrollBar->displayObject());
                }
            }
        }

        _scrollBarDisplayAuto = scrollBarDisplay == ScrollBarDisplayType::AUTO;
        if (_scrollBarDisplayAuto)
        {
            if (_vtScrollBar != nullptr)
                _vtScrollBar->setVisible(false);
            if (_hzScrollBar != nullptr)
                _hzScrollBar->setVisible(false);

            _owner->addEventListener(UIEventType::RollOver, AX_CALLBACK_1(ScrollPane::onRollOver, this));
            _owner->addEventListener(UIEventType::RollOut, AX_CALLBACK_1(ScrollPane::onRollOut, this));
        }
    }
    else
        _mouseWheelEnabled = false;

    if (headerRes.length() > 0)
    {
        _header = dynamic_cast<GComponent*>(UIPackage::createObjectFromURL(headerRes));
        if (_header == nullptr)
            AXLOGW("FairyGUI: cannot create scrollPane header from {}", headerRes);
        else
        {
            _header->retain();
            _header->setVisible(false);
            _header->_alignToBL = true;
            _owner->displayObject()->addChild(_header->displayObject());
        }
    }

    if (footerRes.length() > 0)
    {
        _footer = dynamic_cast<GComponent*>(UIPackage::createObjectFromURL(footerRes));
        if (_footer == nullptr)
            AXLOGW("FairyGUI: cannot create scrollPane footer from {}", footerRes);
        else
        {
            _footer->retain();
            _footer->setVisible(false);
            _footer->_alignToBL = true;
            _owner->displayObject()->addChild(_footer->displayObject());
        }
    }

    if (_header != nullptr || _footer != nullptr)
        _refreshBarAxis = (_scrollType == ScrollType::BOTH || _scrollType == ScrollType::VERTICAL) ? 1 : 0;

    setSize(_owner->getWidth(), _owner->getHeight());
}

void ScrollPane::setScrollStep(float value)
{
    _scrollStep = value;
    if (_scrollStep == 0)
        _scrollStep = UIConfig::defaultScrollStep;
    _mouseWheelStep = _scrollStep * 2;
}

void ScrollPane::setPosX(float value, bool ani)
{
    _owner->ensureBoundsCorrect();

    if (_loop == 1)
        loopCheckingNewPos(value, 0);

    value = clampf(value, 0, _overlapSize.width);
    if (value != _xPos)
    {
        _xPos = value;
        posChanged(ani);
    }
}

void ScrollPane::setPosY(float value, bool ani)
{
    _owner->ensureBoundsCorrect();

    if (_loop == 2)
        loopCheckingNewPos(value, 1);

    value = clampf(value, 0, _overlapSize.height);
    if (value != _yPos)
    {
        _yPos = value;
        posChanged(ani);
    }
}

float ScrollPane::getPercX() const
{
    return _overlapSize.width == 0 ? 0 : _xPos / _overlapSize.width;
}

void ScrollPane::setPercX(float value, bool ani)
{
    _owner->ensureBoundsCorrect();
    setPosX(_overlapSize.width * clampf(value, 0, 1), ani);
}

float ScrollPane::getPercY() const
{
    return _overlapSize.height == 0 ? 0 : _yPos / _overlapSize.height;
}

void ScrollPane::setPercY(float value, bool ani)
{
    _owner->ensureBoundsCorrect();
    setPosY(_overlapSize.height * clampf(value, 0, 1), ani);
}

bool ScrollPane::isBottomMost() const
{
    return _yPos == _overlapSize.height || _overlapSize.height == 0;
}

bool ScrollPane::isRightMost() const
{
    return _xPos == _overlapSize.width || _overlapSize.width == 0;
}

void ScrollPane::scrollLeft(float ratio, bool ani)
{
    if (_pageMode)
        setPosX(_xPos - _pageSize.width * ratio, ani);
    else
        setPosX(_xPos - _scrollStep * ratio, ani);
}

void ScrollPane::scrollRight(float ratio, bool ani)
{
    if (_pageMode)
        setPosX(_xPos + _pageSize.width * ratio, ani);
    else
        setPosX(_xPos + _scrollStep * ratio, ani);
}

void ScrollPane::scrollUp(float ratio, bool ani)
{
    if (_pageMode)
        setPosY(_yPos - _pageSize.height * ratio, ani);
    else
        setPosY(_yPos - _scrollStep * ratio, ani);
}

void ScrollPane::scrollDown(float ratio, bool ani)
{
    if (_pageMode)
        setPosY(_yPos + _pageSize.height * ratio, ani);
    else
        setPosY(_yPos + _scrollStep * ratio, ani);
}

void ScrollPane::scrollTop(bool ani)
{
    setPercY(0, ani);
}

void ScrollPane::scrollBottom(bool ani)
{
    setPercY(1, ani);
}

void ScrollPane::scrollToView(GObject* obj, bool ani, bool setFirst)
{
    _owner->ensureBoundsCorrect();
    if (_needRefresh)
        refresh();

    Rect rect = Rect(obj->getX(), obj->getY(), obj->getWidth(), obj->getHeight());
    if (obj->getParent() != _owner)
        rect = obj->getParent()->transformRect(rect, _owner);
    scrollToView(rect, ani, setFirst);
}

void ScrollPane::scrollToView(const ax::Rect& rect, bool ani, bool setFirst)
{
    _owner->ensureBoundsCorrect();
    if (_needRefresh)
        refresh();

    if (_overlapSize.height > 0)
    {
        float bottom = _yPos + _viewSize.height;
        if (setFirst || rect.origin.y <= _yPos || rect.size.height >= _viewSize.height)
        {
            if (_pageMode)
                setPosY(floor(rect.origin.y / _pageSize.height) * _pageSize.height, ani);
            else
                setPosY(rect.origin.y, ani);
        }
        else if (rect.getMaxY() > bottom)
        {
            if (_pageMode)
                setPosY(floor(rect.origin.y / _pageSize.height) * _pageSize.height, ani);
            else if (rect.size.height <= _viewSize.height / 2)
                setPosY(rect.origin.y + rect.size.height * 2 - _viewSize.height, ani);
            else
                setPosY(rect.getMaxY() - _viewSize.height, ani);
        }
    }
    if (_overlapSize.width > 0)
    {
        float right = _xPos + _viewSize.width;
        if (setFirst || rect.origin.x <= _xPos || rect.size.width >= _viewSize.width)
        {
            if (_pageMode)
                setPosX(floor(rect.origin.x / _pageSize.width) * _pageSize.width, ani);
            setPosX(rect.origin.x, ani);
        }
        else if (rect.getMaxX() > right)
        {
            if (_pageMode)
                setPosX(floor(rect.origin.x / _pageSize.width) * _pageSize.width, ani);
            else if (rect.size.width <= _viewSize.width / 2)
                setPosX(rect.origin.x + rect.size.width * 2 - _viewSize.width, ani);
            else
                setPosX(rect.getMaxX() - _viewSize.width, ani);
        }
    }

    if (!ani && _needRefresh)
        refresh();
}

bool ScrollPane::isChildInView(GObject* obj) const
{
    if (_overlapSize.height > 0)
    {
        float dist = obj->getY() + _container->getPositionY2();
        if (dist <= -obj->getHeight() || dist >= _viewSize.height)
            return false;
    }
    if (_overlapSize.width > 0)
    {
        float dist = obj->getX() + _container->getPositionX();
        if (dist <= -obj->getWidth() || dist >= _viewSize.width)
            return false;
    }

    return true;
}

int ScrollPane::getPageX() const
{
    if (!_pageMode)
        return 0;

    int page = floor(_xPos / _pageSize.width);
    if (_xPos - page * _pageSize.width > _pageSize.width * 0.5f)
        page++;

    return page;
}

void ScrollPane::setPageX(int value, bool ani)
{
    if (!_pageMode)
        return;

    _owner->ensureBoundsCorrect();

    if (_overlapSize.width > 0)
        setPosX(value * _pageSize.width, ani);
}

int ScrollPane::getPageY() const
{
    if (!_pageMode)
        return 0;

    int page = floor(_yPos / _pageSize.height);
    if (_yPos - page * _pageSize.height > _pageSize.height * 0.5f)
        page++;

    return page;
}

void ScrollPane::setPageY(int value, bool ani)
{
    if (!_pageMode)
        return;

    _owner->ensureBoundsCorrect();

    if (_overlapSize.height > 0)
        setPosY(value * _pageSize.height, ani);
}

float ScrollPane::getScrollingPosX() const
{
    return clampf(-_container->getPositionX(), 0, _overlapSize.width);
}

float ScrollPane::getScrollingPosY() const
{
    return clampf(-_container->getPositionY2(), 0, _overlapSize.height);
}

void ScrollPane::setViewWidth(float value)
{
    value = value + _owner->_margin.left + _owner->_margin.right;
    if (_vtScrollBar != nullptr && !_floating)
        value += _vtScrollBar->getWidth();
    _owner->setWidth(value);
}

void ScrollPane::setViewHeight(float value)
{
    value = value + _owner->_margin.top + _owner->_margin.bottom;
    if (_hzScrollBar != nullptr && !_floating)
        value += _hzScrollBar->getHeight();
    _owner->setHeight(value);
}

void ScrollPane::lockHeader(int size)
{
    if (_headerLockedSize == size)
        return;

    Vec2 cpos = _container->getPosition2();

    _headerLockedSize = size;
    if (!_owner->isDispatchingEvent(UIEventType::PullDownRelease) && sp_getField(cpos, _refreshBarAxis) >= 0)
    {
        _tweenStart = cpos;
        _tweenChange.setZero();
        sp_setField(_tweenChange, _refreshBarAxis, _headerLockedSize - sp_getField(_tweenStart, _refreshBarAxis));
        _tweenDuration.set(TWEEN_TIME_DEFAULT, TWEEN_TIME_DEFAULT);
        startTween(2);

        CALL_PER_FRAME(ScrollPane, tweenUpdate);
    }
}

void ScrollPane::lockFooter(int size)
{
    if (_footerLockedSize == size)
        return;

    Vec2 cpos = _container->getPosition2();

    _footerLockedSize = size;
    if (!_owner->isDispatchingEvent(UIEventType::PullUpRelease) && sp_getField(cpos, _refreshBarAxis) >= 0)
    {
        _tweenStart = cpos;
        _tweenChange.setZero();
        float max = sp_getField(_overlapSize, _refreshBarAxis);
        if (max == 0)
            max = MAX(sp_getField(_contentSize, _refreshBarAxis) + _footerLockedSize - sp_getField(_viewSize, _refreshBarAxis), 0);
        else
            max += _footerLockedSize;
        sp_setField(_tweenChange, _refreshBarAxis, -max - sp_getField(_tweenStart, _refreshBarAxis));
        _tweenDuration.set(TWEEN_TIME_DEFAULT, TWEEN_TIME_DEFAULT);
        startTween(2);

        CALL_PER_FRAME(ScrollPane, tweenUpdate);
    }
}

void ScrollPane::cancelDragging()
{
    if (_draggingPane == this)
        _draggingPane = nullptr;

    _gestureFlag = 0;
    _dragged = false;
}

void ScrollPane::handleControllerChanged(GController* c)
{
    if (_pageController == c)
    {
        if (_scrollType == ScrollType::HORIZONTAL)
            setPageX(c->getSelectedIndex(), true);
        else
            setPageY(c->getSelectedIndex(), true);
    }
}

void ScrollPane::updatePageController()
{
    if (_pageController != nullptr && !_pageController->changing)
    {
        int index;
        if (_scrollType == ScrollType::HORIZONTAL)
            index = getPageX();
        else
            index = getPageY();
        if (index < _pageController->getPageCount())
        {
            GController* c = _pageController;
            _pageController = nullptr; //avoid calling handleControllerChanged
            c->setSelectedIndex(index);
            _pageController = c;
        }
    }
}

void ScrollPane::adjustMaskContainer()
{
    float mx, my;
    if (_displayOnLeft && _vtScrollBar != nullptr && !_floating)
        mx = floor(_owner->_margin.left + _vtScrollBar->getWidth());
    else
        mx = floor(_owner->_margin.left);
    my = floor(_owner->_margin.top);
    mx += _owner->_alignOffset.x;
    my += _owner->_alignOffset.y;

    _maskContainer->setPosition(Vec2(mx, _owner->getHeight() - _viewSize.height - my));
}

void ScrollPane::onOwnerSizeChanged()
{
    setSize(_owner->getWidth(), _owner->getHeight());
    posChanged(false);
}

void ScrollPane::setSize(float wv, float hv)
{
    if (_hzScrollBar != nullptr)
    {
        _hzScrollBar->setY(hv - _hzScrollBar->getHeight());
        if (_vtScrollBar != nullptr)
        {
            _hzScrollBar->setWidth(wv - _vtScrollBar->getWidth() - _scrollBarMargin.left - _scrollBarMargin.right);
            if (_displayOnLeft)
                _hzScrollBar->setX(_scrollBarMargin.left + _vtScrollBar->getWidth());
            else
                _hzScrollBar->setX(_scrollBarMargin.left);
        }
        else
        {
            _hzScrollBar->setWidth(wv - _scrollBarMargin.left - _scrollBarMargin.right);
            _hzScrollBar->setX(_scrollBarMargin.left);
        }
    }
    if (_vtScrollBar != nullptr)
    {
        if (!_displayOnLeft)
            _vtScrollBar->setX(wv - _vtScrollBar->getWidth());
        if (_hzScrollBar != nullptr)
            _vtScrollBar->setHeight(hv - _hzScrollBar->getHeight() - _scrollBarMargin.top - _scrollBarMargin.bottom);
        else
            _vtScrollBar->setHeight(hv - _scrollBarMargin.top - _scrollBarMargin.bottom);
        _vtScrollBar->setY(_scrollBarMargin.top);
    }

    _viewSize.width = wv;
    _viewSize.height = hv;
    if (_hzScrollBar != nullptr && !_floating)
        _viewSize.height -= _hzScrollBar->getHeight();
    if (_vtScrollBar != nullptr && !_floating)
        _viewSize.width -= _vtScrollBar->getWidth();
    _viewSize.width -= (_owner->_margin.left + _owner->_margin.right);
    _viewSize.height -= (_owner->_margin.top + _owner->_margin.bottom);

    _viewSize.width = MAX(1, _viewSize.width);
    _viewSize.height = MAX(1, _viewSize.height);
    _pageSize = _viewSize;

    adjustMaskContainer();
    handleSizeChanged();
}

void ScrollPane::setContentSize(float wv, float hv)
{
    if (_contentSize.width == wv && _contentSize.height == hv)
        return;

    _contentSize.width = wv;
    _contentSize.height = hv;
    handleSizeChanged();
}

void ScrollPane::changeContentSizeOnScrolling(float deltaWidth, float deltaHeight, float deltaPosX, float deltaPosY)
{
    bool isRightmost = _xPos == _overlapSize.width;
    bool isBottom = _yPos == _overlapSize.height;

    _contentSize.width += deltaWidth;
    _contentSize.height += deltaHeight;
    handleSizeChanged();

    if (_tweening == 1)
    {
        if (deltaWidth != 0 && isRightmost && _tweenChange.x < 0)
        {
            _xPos = _overlapSize.width;
            _tweenChange.x = -_xPos - _tweenStart.x;
        }

        if (deltaHeight != 0 && isBottom && _tweenChange.y < 0)
        {
            _yPos = _overlapSize.height;
            _tweenChange.y = -_yPos - _tweenStart.y;
        }
    }
    else if (_tweening == 2)
    {
        if (deltaPosX != 0)
        {
            _container->setPositionX(_container->getPositionX() - deltaPosX);
            _tweenStart.x -= deltaPosX;
            _xPos = -_container->getPositionX();
        }
        if (deltaPosY != 0)
        {
            _container->setPositionY2(_container->getPositionY2() - deltaPosY);
            _tweenStart.y -= deltaPosY;
            _yPos = -_container->getPositionY2();
        }
    }
    else if (_dragged)
    {
        if (deltaPosX != 0)
        {
            _container->setPositionX(_container->getPositionX() - deltaPosX);
            _containerPos.x -= deltaPosX;
            _xPos = -_container->getPositionX();
        }
        if (deltaPosY != 0)
        {
            _container->setPositionY2(_container->getPositionY2() - deltaPosY);
            _containerPos.y -= deltaPosY;
            _yPos = -_container->getPositionY2();
        }
    }
    else
    {
        if (deltaWidth != 0 && isRightmost)
        {
            _xPos = _overlapSize.width;
            _container->setPositionX(-_xPos);
        }

        if (deltaHeight != 0 && isBottom)
        {
            _yPos = _overlapSize.height;
            _container->setPositionY2(-_yPos);
        }
    }

    if (_pageMode)
        updatePageController();
}

void ScrollPane::handleSizeChanged()
{
    if (_displayInDemand)
    {
        _vScrollNone = _contentSize.height <= _viewSize.height;
        _hScrollNone = _contentSize.width <= _viewSize.width;
    }

    if (_vtScrollBar != nullptr)
    {
        if (_contentSize.height == 0)
            _vtScrollBar->setDisplayPerc(0);
        else
            _vtScrollBar->setDisplayPerc(MIN(1, _viewSize.height / _contentSize.height));
    }
    if (_hzScrollBar != nullptr)
    {
        if (_contentSize.width == 0)
            _hzScrollBar->setDisplayPerc(0);
        else
            _hzScrollBar->setDisplayPerc(MIN(1, _viewSize.width / _contentSize.width));
    }

    updateScrollBarVisible();

    _maskContainer->setContentSize(_viewSize);
    Rect maskRect(Vec2(-_owner->_alignOffset.x, _owner->_alignOffset.y), _viewSize);
    if (_vScrollNone && _vtScrollBar != nullptr)
        maskRect.size.width += _vtScrollBar->getWidth();
    if (_hScrollNone && _hzScrollBar != nullptr)
        maskRect.size.height += _hzScrollBar->getHeight();
    if (_dontClipMargin)
    {
        maskRect.origin.x -= _owner->_margin.left;
        maskRect.size.width += _owner->_margin.left + _owner->_margin.right;
        maskRect.origin.y -= _owner->_margin.top;
        maskRect.size.height += _owner->_margin.top + _owner->_margin.bottom;
    }
    _maskContainer->setClippingRegion(maskRect);

    if (_vtScrollBar)
        _vtScrollBar->handlePositionChanged();
    if (_hzScrollBar)
        _hzScrollBar->handlePositionChanged();
    if (_header)
        _header->handlePositionChanged();
    if (_footer)
        _footer->handlePositionChanged();

    if (_scrollType == ScrollType::HORIZONTAL || _scrollType == ScrollType::BOTH)
        _overlapSize.width = ceil(MAX(0, _contentSize.width - _viewSize.width));
    else
        _overlapSize.width = 0;
    if (_scrollType == ScrollType::VERTICAL || _scrollType == ScrollType::BOTH)
        _overlapSize.height = ceil(MAX(0, _contentSize.height - _viewSize.height));
    else
        _overlapSize.height = 0;

    _xPos = clampf(_xPos, 0, _overlapSize.width);
    _yPos = clampf(_yPos, 0, _overlapSize.height);
    float max = sp_getField(_overlapSize, _refreshBarAxis);
    if (max == 0)
        max = MAX(sp_getField(_contentSize, _refreshBarAxis) + _footerLockedSize - sp_getField(_viewSize, _refreshBarAxis), 0);
    else
        max += _footerLockedSize;
    if (_refreshBarAxis == 0)
        _container->setPosition2(clampf(_container->getPositionX(), -max, _headerLockedSize),
                                 clampf(_container->getPositionY2(), -_overlapSize.height, 0));
    else
        _container->setPosition2(clampf(_container->getPositionX(), -_overlapSize.width, 0),
                                 clampf(_container->getPositionY2(), -max, _headerLockedSize));

    if (_header != nullptr)
    {
        if (_refreshBarAxis == 0)
            _header->setHeight(_viewSize.height);
        else
            _header->setWidth(_viewSize.width);
    }

    if (_footer != nullptr)
    {
        if (_refreshBarAxis == 0)
            _footer->setHeight(_viewSize.height);
        else
            _footer->setWidth(_viewSize.width);
    }

    updateScrollBarPos();
    if (_pageMode)
        updatePageController();
}

GObject* ScrollPane::hitTest(const ax::Vec2& pt, const ax::Camera* camera)
{
    GObject* target = nullptr;
    if (_vtScrollBar)
    {
        target = _vtScrollBar->hitTest(pt, camera);
        if (target)
            return target;
    }
    if (_hzScrollBar)
    {
        target = _hzScrollBar->hitTest(pt, camera);
        if (target)
            return target;
    }
    if (_header && _header->displayObject()->getParent() != nullptr)
    {
        target = _header->hitTest(pt, camera);
        if (target)
            return target;
    }
    if (_footer && _footer->displayObject()->getParent() != nullptr)
    {
        target = _footer->hitTest(pt, camera);
        if (target)
            return target;
    }
    if (_maskContainer->isClippingEnabled())
    {
        Vec2 localPoint = _maskContainer->convertToNodeSpace(pt);
        if (_maskContainer->getClippingRegion().containsPoint(localPoint))
            return _owner;
        else
            return nullptr;
    }
    else
        return _owner;
}

void ScrollPane::posChanged(bool ani)
{
    if (_aniFlag == 0)
        _aniFlag = ani ? 1 : -1;
    else if (_aniFlag == 1 && !ani)
        _aniFlag = -1;

    _needRefresh = true;
    CALL_LATER(ScrollPane, refresh);
}

void ScrollPane::refresh()
{
    CALL_LATER_CANCEL(ScrollPane, refresh);
    _needRefresh = false;

    if (_pageMode || _snapToItem)
    {
        Vec2 pos(-_xPos, -_yPos);
        alignPosition(pos, false);
        _xPos = -pos.x;
        _yPos = -pos.y;
    }

    refresh2();

    _owner->dispatchEvent(UIEventType::Scroll);
    if (_needRefresh) //pos may change in onScroll
    {
        _needRefresh = false;
        CALL_LATER_CANCEL(ScrollPane, refresh);

        refresh2();
    }

    updateScrollBarPos();
    _aniFlag = 0;
}

void ScrollPane::refresh2()
{
    if (_aniFlag == 1 && !_dragged)
    {
        Vec2 pos;

        if (_overlapSize.width > 0)
            pos.x = -(int)_xPos;
        else
        {
            if (_container->getPositionX() != 0)
                _container->setPositionX(0);
            pos.x = 0;
        }
        if (_overlapSize.height > 0)
            pos.y = -(int)_yPos;
        else
        {
            if (_container->getPositionY2() != 0)
                _container->setPositionY2(0);
            pos.y = 0;
        }

        if (pos.x != _container->getPositionX() || pos.y != _container->getPositionY2())
        {
            _tweenDuration.set(TWEEN_TIME_GO, TWEEN_TIME_GO);
            _tweenStart = _container->getPosition2();
            _tweenChange = pos - _tweenStart;
            startTween(1);
        }
        else if (_tweening != 0)
            killTween();
    }
    else
    {
        if (_tweening != 0)
            killTween();

        _container->setPosition2(Vec2((int)-_xPos, (int)-_yPos));

        loopCheckingCurrent();
    }

    if (_pageMode)
        updatePageController();
}

void ScrollPane::updateScrollBarPos()
{
    if (_vtScrollBar != nullptr)
        _vtScrollBar->setScrollPerc(_overlapSize.height == 0 ? 0 : clampf(-_container->getPositionY2(), 0, _overlapSize.height) / _overlapSize.height);

    if (_hzScrollBar != nullptr)
        _hzScrollBar->setScrollPerc(_overlapSize.width == 0 ? 0 : clampf(-_container->getPositionX(), 0, _overlapSize.width) / _overlapSize.width);

    checkRefreshBar();
}

void ScrollPane::updateScrollBarVisible()
{
    if (_vtScrollBar != nullptr)
    {
        if (_viewSize.height <= _vtScrollBar->getMinSize() || _vScrollNone)
            _vtScrollBar->setVisible(false);
        else
            updateScrollBarVisible2(_vtScrollBar);
    }

    if (_hzScrollBar != nullptr)
    {
        if (_viewSize.width <= _hzScrollBar->getMinSize() || _hScrollNone)
            _hzScrollBar->setVisible(false);
        else
            updateScrollBarVisible2(_hzScrollBar);
    }
}

void ScrollPane::updateScrollBarVisible2(GScrollBar* bar)
{
    if (_scrollBarDisplayAuto)
        GTween::kill(bar, TweenPropType::Alpha, false);

    if (_scrollBarDisplayAuto && !_hover && _tweening == 0 && !_dragged && !bar->_gripDragging)
    {
        if (bar->isVisible())
            GTween::to(1, 0, 0.5f)
                ->setDelay(0.5f)
                ->onComplete1(AX_CALLBACK_1(ScrollPane::onBarTweenComplete, this))
                ->setTarget(bar, TweenPropType::Alpha);
    }
    else
    {
        bar->setAlpha(1);
        bar->setVisible(true);
    }
}

void ScrollPane::onBarTweenComplete(GTweener* tweener)
{
    GObject* bar = (GObject*)tweener->getTarget();
    bar->setAlpha(1);
    bar->setVisible(false);
}

float ScrollPane::getLoopPartSize(float division, int axis)
{
    return (sp_getField(_contentSize, axis) + (axis == 0 ? ((GList*)_owner)->getColumnGap() : ((GList*)_owner)->getLineGap())) / division;
}

bool ScrollPane::loopCheckingCurrent()
{
    bool changed = false;
    if (_loop == 1 && _overlapSize.width > 0)
    {
        if (_xPos < 0.001f)
        {
            _xPos += getLoopPartSize(2, 0);
            changed = true;
        }
        else if (_xPos >= _overlapSize.width)
        {
            _xPos -= getLoopPartSize(2, 0);
            changed = true;
        }
    }
    else if (_loop == 2 && _overlapSize.height > 0)
    {
        if (_yPos < 0.001f)
        {
            _yPos += getLoopPartSize(2, 1);
            changed = true;
        }
        else if (_yPos >= _overlapSize.height)
        {
            _yPos -= getLoopPartSize(2, 1);
            changed = true;
        }
    }

    if (changed)
        _container->setPosition2(Vec2((int)-_xPos, (int)-_yPos));

    return changed;
}

void ScrollPane::loopCheckingTarget(ax::Vec2& endPos)
{
    if (_loop == 1)
        loopCheckingTarget(endPos, 0);

    if (_loop == 2)
        loopCheckingTarget(endPos, 1);
}

void ScrollPane::loopCheckingTarget(ax::Vec2& endPos, int axis)
{
    if (sp_getField(endPos, axis) > 0)
    {
        float halfSize = getLoopPartSize(2, axis);
        float tmp = sp_getField(_tweenStart, axis) - halfSize;
        if (tmp <= 0 && tmp >= -sp_getField(_overlapSize, axis))
        {
            sp_incField(endPos, axis, -halfSize);
            sp_setField(_tweenStart, axis, tmp);
        }
    }
    else if (sp_getField(endPos, axis) < -sp_getField(_overlapSize, axis))
    {
        float halfSize = getLoopPartSize(2, axis);
        float tmp = sp_getField(_tweenStart, axis) + halfSize;
        if (tmp <= 0 && tmp >= -sp_getField(_overlapSize, axis))
        {
            sp_incField(endPos, axis, halfSize);
            sp_setField(_tweenStart, axis, tmp);
        }
    }
}

void ScrollPane::loopCheckingNewPos(float& value, int axis)
{
    float overlapSize = sp_getField(_overlapSize, axis);
    if (overlapSize == 0)
        return;

    float pos = axis == 0 ? _xPos : _yPos;
    bool changed = false;
    if (value < 0.001f)
    {
        value += getLoopPartSize(2, axis);
        if (value > pos)
        {
            float v = getLoopPartSize(6, axis);
            v = ceil((value - pos) / v) * v;
            pos = clampf(pos + v, 0, overlapSize);
            changed = true;
        }
    }
    else if (value >= overlapSize)
    {
        value -= getLoopPartSize(2, axis);
        if (value < pos)
        {
            float v = getLoopPartSize(6, axis);
            v = ceil((pos - value) / v) * v;
            pos = clampf(pos - v, 0, overlapSize);
            changed = true;
        }
    }

    if (changed)
    {
        if (axis == 0)
            _container->setPositionX(-(int)pos);
        else
            _container->setPositionY2(-(int)pos);
    }
}

void ScrollPane::alignPosition(Vec2& pos, bool inertialScrolling)
{
    if (_pageMode)
    {
        pos.x = alignByPage(pos.x, 0, inertialScrolling);
        pos.y = alignByPage(pos.y, 1, inertialScrolling);
    }
    else if (_snapToItem)
    {
        Vec2 tmp = _owner->getSnappingPosition(-pos);
        if (pos.x < 0 && pos.x > -_overlapSize.width)
            pos.x = -tmp.x;
        if (pos.y < 0 && pos.y > -_overlapSize.height)
            pos.y = -tmp.y;
    }
}

float ScrollPane::alignByPage(float pos, int axis, bool inertialScrolling)
{
    int page;
    float pageSize = sp_getField(_pageSize, axis);
    float overlapSize = sp_getField(_overlapSize, axis);
    float contentSize = sp_getField(_contentSize, axis);

    if (pos > 0)
        page = 0;
    else if (pos < -overlapSize)
        page = ceil(contentSize / pageSize) - 1;
    else
    {
        page = floor(-pos / pageSize);
        float change = inertialScrolling ? (pos - sp_getField(_containerPos, axis)) : (pos - sp_getField(_container->getPosition2(), axis));
        float testPageSize = MIN(pageSize, contentSize - (page + 1) * pageSize);
        float delta = -pos - page * pageSize;

        if (std::abs(change) > pageSize)
        {
            if (delta > testPageSize * 0.5f)
                page++;
        }
        else
        {
            if (delta > testPageSize * (change < 0 ? 0.3f : 0.7f))
                page++;
        }

        pos = -page * pageSize;
        if (pos < -overlapSize)
            pos = -overlapSize;
    }

    if (inertialScrolling)
    {
        float oldPos = sp_getField(_tweenStart, axis);
        int oldPage;
        if (oldPos > 0)
            oldPage = 0;
        else if (oldPos < -overlapSize)
            oldPage = ceil(contentSize / pageSize) - 1;
        else
            oldPage = floor(-oldPos / pageSize);
        int startPage = floor(-sp_getField(_containerPos, axis) / pageSize);
        if (abs(page - startPage) > 1 && abs(oldPage - startPage) <= 1)
        {
            if (page > startPage)
                page = startPage + 1;
            else
                page = startPage - 1;
            pos = -page * pageSize;
        }
    }

    return pos;
}

ax::Vec2 ScrollPane::updateTargetAndDuration(const ax::Vec2& orignPos)
{
    Vec2 ret(0, 0);
    ret.x = updateTargetAndDuration(orignPos.x, 0);
    ret.y = updateTargetAndDuration(orignPos.y, 1);
    return ret;
}

float ScrollPane::updateTargetAndDuration(float pos, int axis)
{
    float v = sp_getField(_velocity, axis);
    float duration = 0;

    if (pos > 0)
        pos = 0;
    else if (pos < -sp_getField(_overlapSize, axis))
        pos = -sp_getField(_overlapSize, axis);
    else
    {
        float v2 = std::abs(v) * _velocityScale;
        float ratio = 0;
#ifdef AX_PLATFORM_PC
        if (v2 > 500)
            ratio = pow((v2 - 500) / 500, 2);
#else
        const ax::Size& winSize = Director::getInstance()->getWinSizeInPixels();
        v2 *= 1136.0f / MAX(winSize.width, winSize.height);

        if (_pageMode)
        {
            if (v2 > 500)
                ratio = pow((v2 - 500) / 500, 2);
        }
        else
        {
            if (v2 > 1000)
                ratio = pow((v2 - 1000) / 1000, 2);
        }
#endif

        if (ratio != 0)
        {
            if (ratio > 1)
                ratio = 1;

            v2 *= ratio;
            v *= ratio;
            sp_setField(_velocity, axis, v);

            duration = ::log(60 / v2) / ::log(_decelerationRate) / 60;
            float change = (int)(v * duration * 0.4f);
            pos += change;
        }
    }

    if (duration < TWEEN_TIME_DEFAULT)
        duration = TWEEN_TIME_DEFAULT;
    sp_setField(_tweenDuration, axis, duration);

    return pos;
}

void ScrollPane::fixDuration(int axis, float oldChange)
{
    float tweenChange = sp_getField(_tweenChange, axis);
    if (tweenChange == 0 || std::abs(tweenChange) >= std::abs(oldChange))
        return;

    float newDuration = std::abs(tweenChange / oldChange) * sp_getField(_tweenDuration, axis);
    if (newDuration < TWEEN_TIME_DEFAULT)
        newDuration = TWEEN_TIME_DEFAULT;

    sp_setField(_tweenDuration, axis, newDuration);
}

void ScrollPane::startTween(int type)
{
    _tweenTime.setZero();
    _tweening = type;
    CALL_PER_FRAME(ScrollPane, tweenUpdate);
    updateScrollBarVisible();
}

void ScrollPane::killTween()
{
    if (_tweening == 1)
    {
        Vec2 t = _tweenStart + _tweenChange;
        _container->setPosition2(t);
        _owner->dispatchEvent(UIEventType::Scroll);
    }

    _tweening = 0;
    CALL_PER_FRAME_CANCEL(ScrollPane, tweenUpdate);
    _owner->dispatchEvent(UIEventType::ScrollEnd);
}

void ScrollPane::checkRefreshBar()
{
    if (_header == nullptr && _footer == nullptr)
        return;

    float pos = sp_getField(_container->getPosition2(), _refreshBarAxis);
    if (_header != nullptr)
    {
        if (pos > 0)
        {
            _header->setVisible(true);
            Vec2 vec;

            vec = _header->getSize();
            sp_setField(vec, _refreshBarAxis, pos);
            _header->setSize(vec.x, vec.y);
        }
        else
            _header->setVisible(false);
    }

    if (_footer != nullptr)
    {
        float max = sp_getField(_overlapSize, _refreshBarAxis);
        if (pos < -max || (max == 0 && _footerLockedSize > 0))
        {
            _footer->setVisible(true);

            Vec2 vec;

            vec = _footer->getPosition();
            if (max > 0)
                sp_setField(vec, _refreshBarAxis, pos + sp_getField(_contentSize, _refreshBarAxis));
            else
                sp_setField(vec, _refreshBarAxis, MAX(MIN(pos + sp_getField(_viewSize, _refreshBarAxis), sp_getField(_viewSize, _refreshBarAxis) - _footerLockedSize), sp_getField(_viewSize, _refreshBarAxis) - sp_getField(_contentSize, _refreshBarAxis)));
            _footer->setPosition(vec.x, vec.y);

            vec = _footer->getSize();
            if (max > 0)
                sp_setField(vec, _refreshBarAxis, -max - pos);
            else
                sp_setField(vec, _refreshBarAxis, sp_getField(_viewSize, _refreshBarAxis) - sp_getField(_footer->getPosition(), _refreshBarAxis));
            _footer->setSize(vec.x, vec.y);
        }
        else
            _footer->setVisible(false);
    }
}

void ScrollPane::tweenUpdate(float dt)
{
    float nx = runTween(0, dt);
    float ny = runTween(1, dt);

    _container->setPosition2(nx, ny);

    if (_tweening == 2)
    {
        if (_overlapSize.width > 0)
            _xPos = clampf(-nx, 0, _overlapSize.width);
        if (_overlapSize.height > 0)
            _yPos = clampf(-ny, 0, _overlapSize.height);

        if (_pageMode)
            updatePageController();
    }

    if (_tweenChange.x == 0 && _tweenChange.y == 0)
    {
        _tweening = 0;
        CALL_PER_FRAME_CANCEL(ScrollPane, tweenUpdate);

        loopCheckingCurrent();

        updateScrollBarPos();
        updateScrollBarVisible();

        _owner->dispatchEvent(UIEventType::Scroll);
        _owner->dispatchEvent(UIEventType::ScrollEnd);
    }
    else
    {
        updateScrollBarPos();
        _owner->dispatchEvent(UIEventType::Scroll);
    }
}

float ScrollPane::runTween(int axis, float dt)
{
    float newValue;
    if (sp_getField(_tweenChange, axis) != 0)
    {
        sp_incField(_tweenTime, axis, dt);
        if (sp_getField(_tweenTime, axis) >= sp_getField(_tweenDuration, axis))
        {
            newValue = sp_getField(_tweenStart, axis) + sp_getField(_tweenChange, axis);
            sp_setField(_tweenChange, axis, 0);
        }
        else
        {
            float ratio = sp_EaseFunc(sp_getField(_tweenTime, axis), sp_getField(_tweenDuration, axis));
            newValue = sp_getField(_tweenStart, axis) + (int)(sp_getField(_tweenChange, axis) * ratio);
        }

        float threshold1 = 0;
        float threshold2 = -sp_getField(_overlapSize, axis);
        if (_headerLockedSize > 0 && _refreshBarAxis == axis)
            threshold1 = _headerLockedSize;
        if (_footerLockedSize > 0 && _refreshBarAxis == axis)
        {
            float max = sp_getField(_overlapSize, _refreshBarAxis);
            if (max == 0)
                max = MAX(sp_getField(_contentSize, _refreshBarAxis) + _footerLockedSize - sp_getField(_viewSize, _refreshBarAxis), 0);
            else
                max += _footerLockedSize;
            threshold2 = -max;
        }

        if (_tweening == 2 && _bouncebackEffect)
        {
            if ((newValue > 20 + threshold1 && sp_getField(_tweenChange, axis) > 0) || (newValue > threshold1 && sp_getField(_tweenChange, axis) == 0))
            {
                sp_setField(_tweenTime, axis, 0);
                sp_setField(_tweenDuration, axis, TWEEN_TIME_DEFAULT);
                sp_setField(_tweenChange, axis, -newValue + threshold1);
                sp_setField(_tweenStart, axis, newValue);
            }
            else if ((newValue < threshold2 - 20 && sp_getField(_tweenChange, axis) < 0) || (newValue < threshold2 && sp_getField(_tweenChange, axis) == 0))
            {
                sp_setField(_tweenTime, axis, 0);
                sp_setField(_tweenDuration, axis, TWEEN_TIME_DEFAULT);
                sp_setField(_tweenChange, axis, threshold2 - newValue);
                sp_setField(_tweenStart, axis, newValue);
            }
        }
        else
        {
            if (newValue > threshold1)
            {
                newValue = threshold1;
                sp_setField(_tweenChange, axis, 0);
            }
            else if (newValue < threshold2)
            {
                newValue = threshold2;
                sp_setField(_tweenChange, axis, 0);
            }
        }
    }
    else
        newValue = sp_getField(_container->getPosition2(), axis);

    return newValue;
}

void ScrollPane::onTouchBegin(EventContext* context)
{
    if (!_touchEffect)
        return;

    context->captureTouch();
    InputEvent* evt = context->getInput();
    Vec2 pt = _owner->globalToLocal(evt->getPosition());

    if (_tweening != 0)
    {
        killTween();
        evt->getProcessor()->cancelClick(evt->getTouchId());

        _dragged = true;
    }
    else
        _dragged = false;

    _containerPos = _container->getPosition2();
    _beginTouchPos = _lastTouchPos = pt;
    _lastTouchGlobalPos = evt->getPosition();
    _isHoldAreaDone = false;
    _velocity.setZero();
    _velocityScale = 1;
    _lastMoveTime = ::clock();
}

void ScrollPane::onTouchMove(EventContext* context)
{
    if (!_touchEffect)
        return;

    if ((_draggingPane != nullptr && _draggingPane != this) || GObject::getDraggingObject() != nullptr)
        return;

    InputEvent* evt = context->getInput();
    Vec2 pt = _owner->globalToLocal(evt->getPosition());

    int sensitivity;
#ifdef AX_PLATFORM_PC
    sensitivity = 8;
#else
    sensitivity = UIConfig::touchScrollSensitivity;
#endif

    float diff;
    bool sv = false, sh = false;

    if (_scrollType == ScrollType::VERTICAL)
    {
        if (!_isHoldAreaDone)
        {
            _gestureFlag |= 1;

            diff = std::abs(_beginTouchPos.y - pt.y);
            if (diff < sensitivity)
                return;

            if ((_gestureFlag & 2) != 0)
            {
                float diff2 = std::abs(_beginTouchPos.x - pt.x);
                if (diff < diff2)
                    return;
            }
        }

        sv = true;
    }
    else if (_scrollType == ScrollType::HORIZONTAL)
    {
        if (!_isHoldAreaDone)
        {
            _gestureFlag |= 2;

            diff = std::abs(_beginTouchPos.x - pt.x);
            if (diff < sensitivity)
                return;

            if ((_gestureFlag & 1) != 0)
            {
                float diff2 = std::abs(_beginTouchPos.y - pt.y);
                if (diff < diff2)
                    return;
            }
        }

        sh = true;
    }
    else
    {
        _gestureFlag = 3;

        if (!_isHoldAreaDone)
        {
            diff = std::abs(_beginTouchPos.y - pt.y);
            if (diff < sensitivity)
            {
                diff = std::abs(_beginTouchPos.x - pt.x);
                if (diff < sensitivity)
                    return;
            }
        }

        sv = sh = true;
    }

    Vec2 newPos = _containerPos + pt - _beginTouchPos;
    newPos.x = (int)newPos.x;
    newPos.y = (int)newPos.y;

    if (sv)
    {
        if (newPos.y > 0)
        {
            if (!_bouncebackEffect)
                _container->setPositionY2(0);
            else if (_header != nullptr && _header->maxSize.height != 0)
                _container->setPositionY2(((int)MIN(newPos.y * 0.5f, _header->maxSize.height)));
            else
                _container->setPositionY2(((int)MIN(newPos.y * 0.5f, _viewSize.height * PULL_RATIO)));
        }
        else if (newPos.y < -_overlapSize.height)
        {
            if (!_bouncebackEffect)
                _container->setPositionY2(-_overlapSize.height);
            else if (_footer != nullptr && _footer->maxSize.height > 0)
                _container->setPositionY2(((int)MAX((newPos.y + _overlapSize.height) * 0.5f, -_footer->maxSize.height) - _overlapSize.height));
            else
                _container->setPositionY2(((int)MAX((newPos.y + _overlapSize.height) * 0.5f, -_viewSize.height * PULL_RATIO) - _overlapSize.height));
        }
        else
            _container->setPositionY2(newPos.y);
    }

    if (sh)
    {
        if (newPos.x > 0)
        {
            if (!_bouncebackEffect)
                _container->setPositionX(0);
            else if (_header != nullptr && _header->maxSize.width != 0)
                _container->setPositionX((int)MIN(newPos.x * 0.5f, _header->maxSize.width));
            else
                _container->setPositionX((int)MIN(newPos.x * 0.5f, _viewSize.width * PULL_RATIO));
        }
        else if (newPos.x < 0 - _overlapSize.width)
        {
            if (!_bouncebackEffect)
                _container->setPositionX(-_overlapSize.width);
            else if (_footer != nullptr && _footer->maxSize.width > 0)
                _container->setPositionX((int)MAX((newPos.x + _overlapSize.width) * 0.5f, -_footer->maxSize.width) - _overlapSize.width);
            else
                _container->setPositionX((int)MAX((newPos.x + _overlapSize.width) * 0.5f, -_viewSize.width * PULL_RATIO) - _overlapSize.width);
        }
        else
            _container->setPositionX(newPos.x);
    }

    auto deltaTime = Director::getInstance()->getDeltaTime();
    float elapsed = (::clock() - _lastMoveTime) / (double)CLOCKS_PER_SEC;
    elapsed = elapsed * 60 - 1;
    if (elapsed > 1)
        _velocity = _velocity * pow(0.833f, elapsed);
    Vec2 deltaPosition = pt - _lastTouchPos;
    if (!sh)
        deltaPosition.x = 0;
    if (!sv)
        deltaPosition.y = 0;
    _velocity = _velocity.lerp(deltaPosition / deltaTime, deltaTime * 10);

    Vec2 deltaGlobalPosition = _lastTouchGlobalPos - evt->getPosition();
    if (deltaPosition.x != 0)
        _velocityScale = std::abs(deltaGlobalPosition.x / deltaPosition.x);
    else if (deltaPosition.y != 0)
        _velocityScale = std::abs(deltaGlobalPosition.y / deltaPosition.y);

    _lastTouchPos = pt;
    _lastTouchGlobalPos = evt->getPosition();
    _lastMoveTime = ::clock();

    if (_overlapSize.width > 0)
        _xPos = clampf(-_container->getPositionX(), 0, _overlapSize.width);
    if (_overlapSize.height > 0)
        _yPos = clampf(-_container->getPositionY2(), 0, _overlapSize.height);

    if (_loop != 0)
    {
        newPos = _container->getPosition2();
        if (loopCheckingCurrent())
            _containerPos += _container->getPosition2() - newPos;
    }

    _draggingPane = this;
    _isHoldAreaDone = true;
    _dragged = true;

    updateScrollBarPos();
    updateScrollBarVisible();
    if (_pageMode)
        updatePageController();
    _owner->dispatchEvent(UIEventType::Scroll);
}

void ScrollPane::onTouchEnd(EventContext* context)
{
    if (_draggingPane == this)
        _draggingPane = nullptr;

    _gestureFlag = 0;

    if (!_dragged || !_touchEffect)
    {
        _dragged = false;
        return;
    }

    _dragged = false;
    _tweenStart = _container->getPosition2();

    Vec2 endPos = _tweenStart;
    bool flag = false;
    if (_container->getPositionX() > 0)
    {
        endPos.x = 0;
        flag = true;
    }
    else if (_container->getPositionX() < -_overlapSize.width)
    {
        endPos.x = -_overlapSize.width;
        flag = true;
    }
    if (_container->getPositionY2() > 0)
    {
        endPos.y = 0;
        flag = true;
    }
    else if (_container->getPositionY2() < -_overlapSize.height)
    {
        endPos.y = -_overlapSize.height;
        flag = true;
    }

    if (flag)
    {
        _tweenChange = endPos - _tweenStart;
        if (_tweenChange.x < -UIConfig::touchDragSensitivity || _tweenChange.y < -UIConfig::touchDragSensitivity)
            _owner->dispatchEvent(UIEventType::PullDownRelease);
        else if (_tweenChange.x > UIConfig::touchDragSensitivity || _tweenChange.y > UIConfig::touchDragSensitivity)
            _owner->dispatchEvent(UIEventType::PullUpRelease);

        if (_headerLockedSize > 0 && sp_getField(endPos, _refreshBarAxis) == 0)
        {
            sp_setField(endPos, _refreshBarAxis, _headerLockedSize);
            _tweenChange = endPos - _tweenStart;
        }
        else if (_footerLockedSize > 0 && sp_getField(endPos, _refreshBarAxis) == -sp_getField(_overlapSize, _refreshBarAxis))
        {
            float max = sp_getField(_overlapSize, _refreshBarAxis);
            if (max == 0)
                max = MAX(sp_getField(_contentSize, _refreshBarAxis) + _footerLockedSize - sp_getField(_viewSize, _refreshBarAxis), 0);
            else
                max += _footerLockedSize;
            sp_setField(endPos, _refreshBarAxis, -max);
            _tweenChange = endPos - _tweenStart;
        }

        _tweenDuration.set(TWEEN_TIME_DEFAULT, TWEEN_TIME_DEFAULT);
    }
    else
    {
        if (!_inertiaDisabled)
        {
            float elapsed = (::clock() - _lastMoveTime) / (double)CLOCKS_PER_SEC;
            elapsed = elapsed * 60 - 1;
            if (elapsed > 1)
                _velocity = _velocity * pow(0.833f, elapsed);

            endPos = updateTargetAndDuration(_tweenStart);
        }
        else
            _tweenDuration.set(TWEEN_TIME_DEFAULT, TWEEN_TIME_DEFAULT);
        Vec2 oldChange = endPos - _tweenStart;

        loopCheckingTarget(endPos);
        if (_pageMode || _snapToItem)
            alignPosition(endPos, true);

        _tweenChange = endPos - _tweenStart;
        if (_tweenChange.x == 0 && _tweenChange.y == 0)
        {
            updateScrollBarVisible();
            return;
        }

        if (_pageMode || _snapToItem)
        {
            fixDuration(0, oldChange.x);
            fixDuration(1, oldChange.y);
        }
    }

    startTween(2);
}

void ScrollPane::onMouseWheel(EventContext* context)
{
    if (!_mouseWheelEnabled)
        return;

    InputEvent* evt = context->getInput();
    int delta = evt->getMouseWheelDelta();
    delta = delta > 0 ? 1 : -1;
    if (_overlapSize.width > 0 && _overlapSize.height == 0)
    {
        if (_pageMode)
            setPosX(_xPos + _pageSize.width * delta, false);
        else
            setPosX(_xPos + _mouseWheelStep * delta, false);
    }
    else
    {
        if (_pageMode)
            setPosY(_yPos + _pageSize.height * delta, false);
        else
            setPosY(_yPos + _mouseWheelStep * delta, false);
    }
}

void ScrollPane::onRollOver(EventContext* context)
{
    _hover = true;
    updateScrollBarVisible();
}

void ScrollPane::onRollOut(EventContext* context)
{
    _hover = false;
    updateScrollBarVisible();
}

NS_FGUI_END
