/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "axmol/ui/Widget.h"
#include "axmol/ui/LayoutGroup.h"
#include "axmol/ui/UIHelper.h"
#include "axmol/base/PointerEventListener.h"
#include "axmol/base/KeyboardEventListener.h"
#include "axmol/base/Director.h"
#include "axmol/base/FocusEvent.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/ui/LayoutComponent.h"
#include "axmol/renderer/Shaders.h"
#include "axmol/scene/Camera.h"
#include "axmol/2d/Sprite.h"
#include "axmol/ui/Scale9Sprite.h"

namespace ax
{

namespace ui
{

class Widget::FocusNavigationController
{
    void enableFocusNavigation(bool flag);

    FocusNavigationController()
        : _keyboardListener(nullptr)
        , _firstFocusedWidget(nullptr)
        , _enableFocusNavigation(false)
        , _keyboardEventPriority(1)
    {
        // no-op
    }
    ~FocusNavigationController();

protected:
    void setFirstFocusedWidget(Widget* widget);

    void onKeypadKeyPressed(KeyboardEvent* event);

    void addKeyboardEventListener();
    void removeKeyboardEventListener();

    friend class Widget;

private:
    KeyboardEventListener* _keyboardListener;
    Widget* _firstFocusedWidget;
    bool _enableFocusNavigation;
    const int _keyboardEventPriority;
};

Widget::FocusNavigationController::~FocusNavigationController()
{
    this->removeKeyboardEventListener();
}

void Widget::FocusNavigationController::onKeypadKeyPressed(KeyboardEvent* event)
{
    if (_enableFocusNavigation && _firstFocusedWidget)
    {
        auto keyCode = event->getKeyCode();
        if (keyCode == KeyboardEvent::KeyCode::KEY_DPAD_DOWN)
        {
            _firstFocusedWidget =
                _firstFocusedWidget->findNextFocusedWidget(Widget::FocusDirection::DOWN, _firstFocusedWidget);
        }
        if (keyCode == KeyboardEvent::KeyCode::KEY_DPAD_UP)
        {
            _firstFocusedWidget =
                _firstFocusedWidget->findNextFocusedWidget(Widget::FocusDirection::UP, _firstFocusedWidget);
        }
        if (keyCode == KeyboardEvent::KeyCode::KEY_DPAD_LEFT)
        {
            _firstFocusedWidget =
                _firstFocusedWidget->findNextFocusedWidget(Widget::FocusDirection::LEFT, _firstFocusedWidget);
        }
        if (keyCode == KeyboardEvent::KeyCode::KEY_DPAD_RIGHT)
        {
            _firstFocusedWidget =
                _firstFocusedWidget->findNextFocusedWidget(Widget::FocusDirection::RIGHT, _firstFocusedWidget);
        }
    }
}

void Widget::FocusNavigationController::enableFocusNavigation(bool flag)
{
    if (_enableFocusNavigation == flag)
        return;

    _enableFocusNavigation = flag;

    if (flag)
        this->addKeyboardEventListener();
    else
        this->removeKeyboardEventListener();
}

void Widget::FocusNavigationController::setFirstFocusedWidget(Widget* widget)
{
    _firstFocusedWidget = widget;
}

void Widget::FocusNavigationController::addKeyboardEventListener()
{
    if (nullptr == _keyboardListener)
    {
        _keyboardListener                = KeyboardEventListener::create();
        _keyboardListener->onKeyReleased = AX_CALLBACK_1(Widget::FocusNavigationController::onKeypadKeyPressed, this);
        EventDispatcher* dispatcher      = Director::getInstance()->getEventDispatcher();
        dispatcher->addEventListenerWithFixedPriority(_keyboardListener, _keyboardEventPriority);
    }
}

void Widget::FocusNavigationController::removeKeyboardEventListener()
{
    if (nullptr != _keyboardListener)
    {
        EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeEventListener(_keyboardListener);
        _keyboardListener = nullptr;
    }
}

Widget* Widget::_focusedWidget                                        = nullptr;
Widget::FocusNavigationController* Widget::_focusNavigationController = nullptr;

Widget::Widget()
    : _usingLayoutComponent(false)
    , _autoSize(true)
    , _enabled(true)
    , _bright(true)
    , _pointerEnabled(false)
    , _highlight(false)
    , _hovered(false)
    , _affectByClipping(false)
    , _propagatePointerEvents(true)
    , _brightStyle(BrightStyle::NONE)
    , _sizeType(SizeType::ABSOLUTE)
    , _positionType(PositionType::ABSOLUTE)
    , _actionTag(0)
    , _customSize(Vec2::ZERO)
    , _hitted(false)
    , _hittedByCamera(nullptr)
    , _hoveredByCamera(nullptr)
    , _pointerEventListener(nullptr)
    , _flippedX(false)
    , _flippedY(false)
    , _layoutParameterType(LayoutParameter::Type::NONE)
    , _focused(false)
    , _focusEnabled(true)
    , _callbackType("")
    , _callbackName("")
{}

Widget::~Widget()
{
    this->cleanupWidget();
}

void Widget::cleanupWidget()
{
    // clean up _pointerEventListener
    _eventDispatcher->removeEventListener(_pointerEventListener);
    AX_SAFE_RELEASE_NULL(_pointerEventListener);

    // cleanup focused widget and focus navigation controller
    if (_focusedWidget == this)
    {
        // delete
        AX_SAFE_DELETE(_focusNavigationController);
        _focusedWidget = nullptr;
    }
}

Widget* Widget::create()
{
    Widget* widget = new Widget();
    if (widget->init())
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

bool Widget::init()
{
    if (ProtectedNode::init())
    {
        initRenderNode();
        setBright(true);
        onFocusChanged      = AX_CALLBACK_2(Widget::onFocusChange, this);
        onNextFocusedWidget = nullptr;
        this->setAnchorPoint(Vec2(0.5f, 0.5f));

        setAutoSize(true);

        return true;
    }
    return false;
}

void Widget::onEnter()
{
    if (!_usingLayoutComponent)
        updateSizeAndPosition();
    ProtectedNode::onEnter();
}

void Widget::onExit()
{
    unscheduleUpdate();
    ProtectedNode::onExit();
}

void Widget::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (_visible)
    {
        updateLayout();
        ProtectedNode::visit(renderer, parentTransform, parentFlags);
    }
}

Widget* Widget::getWidgetParent()
{
    return dynamic_cast<Widget*>(getParent());
}

void Widget::setEnabled(bool enabled)
{
    _enabled = enabled;
    setBright(enabled);
}

void Widget::initRenderNode() {}

LayoutComponent* Widget::getOrCreateLayoutComponent()
{
    auto layoutComponent = this->getComponent(__LAYOUT_COMPONENT_NAME);
    if (nullptr == layoutComponent)
    {
        LayoutComponent* component = LayoutComponent::create();
        this->addComponent(component);
        layoutComponent = component;
    }

    return (LayoutComponent*)layoutComponent;
}

void Widget::setContentSize(const Vec2& contentSize)
{
    Vec2 previousSize = ProtectedNode::getContentSize();
    if (previousSize.equals(contentSize))
    {
        return;
    }

    _customSize = contentSize;

    ProtectedNode::setContentSize(_autoSize ? resolvePreferredSize(_customSize) : _customSize);

    if (!_usingLayoutComponent && _running)
    {
        Widget* widgetParent = getWidgetParent();
        Vec2 pSize;
        if (widgetParent)
        {
            pSize = widgetParent->getContentSize();
        }
        else
        {
            pSize = _parent->getContentSize();
        }
        float spx = 0.0f;
        float spy = 0.0f;
        if (pSize.width > 0.0f)
        {
            spx = _customSize.width / pSize.width;
        }
        if (pSize.height > 0.0f)
        {
            spy = _customSize.height / pSize.height;
        }
        _sizePercent.set(spx, spy);
    }
    onSizeChanged();
}

void Widget::setSizePercent(const Vec2& percent)
{
    if (_usingLayoutComponent)
    {
        auto component = this->getOrCreateLayoutComponent();
        component->setUsingPercentContentSize(true);
        component->setPercentContentSize(percent);
        component->refreshLayout();
    }
    else
    {
        _sizePercent = percent;
        Vec2 cSize   = _customSize;
        if (_running)
        {
            Widget* widgetParent = getWidgetParent();
            if (widgetParent)
            {
                cSize = Vec2(widgetParent->getContentSize().width * percent.x,
                             widgetParent->getContentSize().height * percent.y);
            }
            else
            {
                cSize = Vec2(_parent->getContentSize().width * percent.x, _parent->getContentSize().height * percent.y);
            }
        }
        if (_autoSize)
            setContentSize(resolvePreferredSize(cSize));
        else
            setContentSize(cSize);

        _customSize = cSize;
    }
}

void Widget::updateSizeAndPosition()
{
    Vec2 pSize = _parent->getContentSize();

    updateSizeAndPosition(pSize);
}

void Widget::updateSizeAndPosition(const Vec2& parentSize)
{
    switch (_sizeType)
    {
    case SizeType::ABSOLUTE:
    {
        if (_autoSize)
        {
            this->setContentSize(resolvePreferredSize(_customSize));
        }
        else
        {
            this->setContentSize(_customSize);
        }
        float spx = 0.0f;
        float spy = 0.0f;
        if (parentSize.width > 0.0f)
        {
            spx = _customSize.width / parentSize.width;
        }
        if (parentSize.height > 0.0f)
        {
            spy = _customSize.height / parentSize.height;
        }
        _sizePercent.set(spx, spy);
        break;
    }
    case SizeType::PERCENT:
    {
        Vec2 cSize = Vec2(parentSize.width * _sizePercent.x, parentSize.height * _sizePercent.y);
        if (_autoSize)
        {
            this->setContentSize(resolvePreferredSize(cSize));
        }
        else
        {
            this->setContentSize(cSize);
        }
        _customSize = cSize;
        break;
    }
    default:
        break;
    }

    // update position & position percent
    if (_usingNormalizedPosition)
    {
        Vec2 absPos = getNormalizedPosition();
        switch (_positionType)
        {
        case PositionType::ABSOLUTE:
        {
            if (parentSize.width <= 0.0f || parentSize.height <= 0.0f)
            {
                _positionPercent.setZero();
            }
            else
            {
                _positionPercent.set(absPos.x, absPos.y);
            }
            break;
        }
        case PositionType::PERCENT:
        {
            absPos.set(_positionPercent.x, _positionPercent.y);
            break;
        }
        default:
            break;
        }
        setPositionNormalized(absPos);
    }
    else
    {
        Vec2 absPos = getPosition();
        switch (_positionType)
        {
        case PositionType::ABSOLUTE:
        {
            if (parentSize.width <= 0.0f || parentSize.height <= 0.0f)
            {
                _positionPercent.setZero();
            }
            else
            {
                _positionPercent.set(absPos.x / parentSize.width, absPos.y / parentSize.height);
            }
            break;
        }
        case PositionType::PERCENT:
        {
            absPos.set(parentSize.width * _positionPercent.x, parentSize.height * _positionPercent.y);
            break;
        }
        default:
            break;
        }
        setPosition(absPos);
    }
}

void Widget::setSizeType(SizeType type)
{
    _sizeType = type;

    if (_usingLayoutComponent)
    {
        auto component = this->getOrCreateLayoutComponent();

        if (_sizeType == Widget::SizeType::PERCENT)
        {
            component->setUsingPercentContentSize(true);
        }
        else
        {
            component->setUsingPercentContentSize(false);
        }
    }
}
Widget::SizeType Widget::getSizeType() const
{
    return _sizeType;
}

const Vec2& Widget::getCustomSize() const
{
    return _customSize;
}

const Vec2& Widget::getSizePercent()
{
    if (_usingLayoutComponent)
    {
        auto component = this->getOrCreateLayoutComponent();
        _sizePercent   = component->getPercentContentSize();
    }

    return _sizePercent;
}

Node* Widget::getRenderNode()
{
    return this;
}

void Widget::onSizeChanged()
{
    if (!_usingLayoutComponent)
    {
        for (auto&& child : getChildren())
        {
            Widget* widgetChild = dynamic_cast<Widget*>(child);
            if (widgetChild)
            {
                widgetChild->updateSizeAndPosition();
            }
        }
    }
}

Vec2 Widget::resolvePreferredSize(const Vec2& sizeHint) const
{
    return sizeHint;
}

void Widget::updateContentSize()
{
    auto preferredSize = resolvePreferredSize(_customSize);

    if (_autoSize)
    {
        // Auto size mode: update to the preferred size based on content
        ProtectedNode::setContentSize(preferredSize);
    }
    else
    {
        // Fixed size mode: keep custom size unchanged
        ProtectedNode::setContentSize(_customSize);
    }
}

void Widget::setPointerEnabled(bool enable)
{
    if (enable == _pointerEnabled)
    {
        return;
    }
    _pointerEnabled = enable;
    if (_pointerEnabled)
    {
        _pointerEventListener = PointerEventListener::create();
        AX_SAFE_RETAIN(_pointerEventListener);
        _pointerEventListener->onPointerDown   = AX_CALLBACK_1(Widget::onPointerDown, this);
        _pointerEventListener->onPointerMove   = AX_CALLBACK_1(Widget::dispatchPointerMove, this);
        _pointerEventListener->onPointerUp     = AX_CALLBACK_1(Widget::onPointerUp, this);
        _pointerEventListener->onPointerCancel = AX_CALLBACK_1(Widget::onPointerCancel, this);
        _pointerEventListener->onPointerScroll = AX_CALLBACK_1(Widget::onPointerScroll, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(_pointerEventListener, this);
    }
    else
    {
        _eventDispatcher->removeEventListener(_pointerEventListener);
        AX_SAFE_RELEASE_NULL(_pointerEventListener);
    }
}

bool Widget::isPointerEnabled() const
{
    return _pointerEnabled;
}

bool Widget::isHighlighted() const
{
    return _highlight;
}

void Widget::setHighlighted(bool highlight)
{
    if (highlight == _highlight)
    {
        return;
    }

    _highlight = highlight;
    if (_bright)
    {
        if (_highlight)
        {
            setBrightStyle(BrightStyle::HIGHLIGHT);
        }
        else
        {
            setBrightStyle(BrightStyle::NORMAL);
        }
    }
    else
    {
        onPressStateChangedToDisabled();
    }
}

void Widget::setBright(bool bright)
{
    _bright = bright;
    if (_bright)
    {
        _brightStyle = BrightStyle::NONE;
        setBrightStyle(BrightStyle::NORMAL);
    }
    else
    {
        onPressStateChangedToDisabled();
    }
}

void Widget::setBrightStyle(BrightStyle style)
{
    if (_brightStyle == style)
    {
        return;
    }
    _brightStyle = style;
    switch (_brightStyle)
    {
    case BrightStyle::NORMAL:
        onPressStateChangedToNormal();
        break;
    case BrightStyle::HIGHLIGHT:
        onPressStateChangedToPressed();
        break;
    default:
        break;
    }
}

void Widget::onPressStateChangedToNormal() {}

void Widget::onPressStateChangedToPressed() {}

void Widget::onPressStateChangedToDisabled() {}

void Widget::updateChildrenDisplayedRGBA()
{
    this->setColor(this->getColor());
    this->setOpacity(this->getOpacity());
}

Widget* Widget::getAncestorWidget(Node* node)
{
    if (nullptr == node)
    {
        return nullptr;
    }

    Node* parent = node->getParent();
    if (nullptr == parent)
    {
        return nullptr;
    }
    Widget* parentWidget = dynamic_cast<Widget*>(parent);
    if (parentWidget)
    {
        return parentWidget;
    }
    else
    {
        return this->getAncestorWidget(parent);
    }
}

bool Widget::isAncestorsVisible(Node* node)
{
    if (nullptr == node)
    {
        return true;
    }
    Node* parent = node->getParent();

    if (parent && !parent->isVisible())
    {
        return false;
    }
    return this->isAncestorsVisible(parent);
}

bool Widget::isAncestorsEnabled()
{
    Widget* parentWidget = this->getAncestorWidget(this);
    if (parentWidget == nullptr)
    {
        return true;
    }
    if (!parentWidget->isEnabled())
    {
        return false;
    }

    return parentWidget->isAncestorsEnabled();
}

void Widget::setPropagatePointerEvents(bool isPropagate)
{
    _propagatePointerEvents = isPropagate;
}

bool Widget::isPropagatePointerEvents() const
{
    return _propagatePointerEvents;
}

bool Widget::onPointerDown(PointerEvent* event)
{
    if (!event || !event->isPrimaryPressed())
        return false;

    auto camera = event->getCamera();
    if (!camera)
        return false;

    RefPtr<Widget> guard(this);

    // Fallback for old/manual paths where onPointerHitTest() was not called.
    if (!_hitted || _hittedByCamera != camera)
    {
        _hitted              = false;
        _hittedByCamera      = nullptr;
        _pointerDownPosition = event->getLocation();

        if (isPointerInside(event, camera, nullptr))
        {
            _hittedByCamera = camera;
            _hitted         = true;
        }
    }

    if (!_hitted)
        return false;

    setHighlighted(true);

    if (_propagatePointerEvents)
    {
        this->propagatePointerEvent(this, event);
    }

    pushDownEvent();
    return true;
}

void Widget::propagatePointerEvent(ax::ui::Widget* sender, PointerEvent* event)
{
    Widget* widgetParent = getWidgetParent();
    if (widgetParent)
    {
        widgetParent->_hittedByCamera = _hittedByCamera;
        widgetParent->interceptPointerEvent(sender, event);
        widgetParent->_hittedByCamera = nullptr;
    }
}

void Widget::onPointerMove(PointerEvent* event)
{
    RefPtr<Widget> guard(this);

    _pointerMovePosition = event->getLocation();

    setHighlighted(isPointerInside(event, _hittedByCamera, nullptr));

    if (_propagatePointerEvents)
    {
        this->propagatePointerEvent(this, event);
    }

    moveEvent();
}

void Widget::dispatchPointerMove(PointerEvent* event)
{
    RefPtr<Widget> guard(this);

    if (event->isCaptured())
    {
        if (_hitted)
            onPointerMove(event);
        return;
    }

    auto camera = event->getCamera();
    if (!camera)
        return;

    const bool hit = isPointerInside(event, camera, nullptr);

    if (hit)
    {
        if (!_hovered)
        {
            _hovered         = true;
            _hoveredByCamera = camera;
            hoverEnterEvent();
        }
        else
        {
            _hoveredByCamera = camera;
        }

        hoverMoveEvent();
        event->stopPropagation();
    }
    else if (_hovered)
    {
        _hovered         = false;
        _hoveredByCamera = nullptr;
        hoverExitEvent();
    }
}

void Widget::onPointerUp(PointerEvent* event)
{
    RefPtr<Widget> guard(this);

    _pointerUpPosition = event->getLocation();

    if (_propagatePointerEvents)
    {
        this->propagatePointerEvent(this, event);
    }

    bool highlight = _highlight;
    setHighlighted(false);

    if (highlight)
    {
        releaseUpEvent();
    }
    else
    {
        cancelUpEvent();
    }

    _hitted = false;

    const Camera* hoverCamera = _hoveredByCamera ? _hoveredByCamera : _hittedByCamera;
    if (_hovered && !isPointerInside(event, hoverCamera, nullptr))
    {
        _hovered         = false;
        _hoveredByCamera = nullptr;
        hoverExitEvent();
    }

    _hittedByCamera = nullptr;
}

void Widget::onPointerCancel(PointerEvent* event)
{
    RefPtr<Widget> guard(this);

    if (_propagatePointerEvents)
    {
        this->propagatePointerEvent(this, event);
    }

    setHighlighted(false);
    cancelUpEvent();

    _hitted         = false;
    _hittedByCamera = nullptr;

    if (_hovered)
    {
        _hovered         = false;
        _hoveredByCamera = nullptr;
        hoverExitEvent();
    }
}

bool Widget::onPointerScroll(PointerEvent* pointerEvent)
{
    return false;
}

void Widget::pushDownEvent()
{
    RefPtr<Widget> guard(this);

    if (_pointerEventHandler)
    {
        _pointerEventHandler(this, PointerPhase::Down);
    }
}

void Widget::moveEvent()
{
    RefPtr<Widget> guard(this);

    if (_pointerEventHandler)
    {
        _pointerEventHandler(this, PointerPhase::Move);
    }
}

void Widget::hoverEnterEvent()
{
    RefPtr<Widget> guard(this);

    if (_hoverEventHandler)
    {
        _hoverEventHandler(this, HoverEventType::ENTER);
    }
}

void Widget::hoverMoveEvent()
{
    RefPtr<Widget> guard(this);

    if (_hoverEventHandler)
    {
        _hoverEventHandler(this, HoverEventType::MOVE);
    }
}

void Widget::hoverExitEvent()
{
    RefPtr<Widget> guard(this);

    if (_hoverEventHandler)
    {
        _hoverEventHandler(this, HoverEventType::EXIT);
    }
}

void Widget::releaseUpEvent()
{
    RefPtr<Widget> guard(this);

    if (isFocusEnabled())
    {
        requestFocus();
    }

    if (_pointerEventHandler)
    {
        _pointerEventHandler(this, PointerPhase::Up);
    }

    if (_clickEventHandler)
    {
        _clickEventHandler(this);
    }
}

void Widget::cancelUpEvent()
{
    RefPtr<Widget> guard(this);

    if (_pointerEventHandler)
    {
        _pointerEventHandler(this, PointerPhase::Cancel);
    }
}

void Widget::addTouchEventListener(const TouchEventHandler& handler)
{
    if (!handler)
    {
        _pointerEventHandler = nullptr;
        return;
    }

    _pointerEventHandler = [handler](Object* sender, PointerPhase phase) {
        TouchEventType type = TouchEventType::CANCELED;
        switch (phase)
        {
        case PointerPhase::Down:
            type = TouchEventType::BEGAN;
            break;
        case PointerPhase::Move:
            type = TouchEventType::MOVED;
            break;
        case PointerPhase::Up:
            type = TouchEventType::ENDED;
            break;
        case PointerPhase::Cancel:
            type = TouchEventType::CANCELED;
            break;
        }
        handler(sender, type);
    };
}

void Widget::addCCSEventListener(const WidgetEventCallback& callback)
{
    _customEventCallback = callback;
}

bool Widget::onPointerHitTest(PointerEvent* event, const Camera* camera, Vec3* outHitPoint)
{
    if (!event || !camera)
        return false;

    const auto phase = event->getPhase();

    // PointerDown: this is the real press hit-test.
    if (phase == InputPhase::PointerDown)
    {
        _hitted         = false;
        _hittedByCamera = nullptr;

        if (!event->isPrimaryPressed())
            return false;

        if (!isPointerInside(event, camera, outHitPoint))
            return false;

        // Cache the down hit so onPointerDown() does not repeat hitTestSelf().
        _pointerDownPosition = event->getLocation();
        _hittedByCamera      = camera;
        _hitted              = true;

        return true;
    }

    // Uncaptured move is used for hover.
    // Need to keep dispatching to the previous hovered widget so it can emit hover exit.
    if (phase == InputPhase::PointerMove && !event->isCaptured())
    {
        const bool hit = isPointerInside(event, camera, outHitPoint);
        return hit || (_hovered && _hoveredByCamera == camera);
    }

    // Scroll should only go to widgets under the pointer.
    if (phase == InputPhase::PointerScroll)
    {
        return isPointerInside(event, camera, outHitPoint);
    }

    return isPointerInside(event, camera, outHitPoint);
}

bool Widget::isPointerInside(PointerEvent* event, const Camera* camera, Vec3* outHitPoint)
{
    if (!event || !camera)
        return false;

    if (!isVisible() || !isEnabled() || !isAncestorsEnabled() || !isAncestorsVisible(this))
        return false;

    const Vec2 pt = event->getLocation();

    if (!hitTestSelf(pt, camera, outHitPoint))
        return false;

    if (!isClippingParentContainsPoint(pt, camera))
        return false;

    return true;
}

bool Widget::hitTestSelf(const Vec2& pt, const Camera* camera, Vec3* p) const
{
    if (!camera)
        return false;

    Rect rect;
    rect.size = getContentSize();
    return camera->isWorldPointInRect(pt, getWorldToNodeTransform(), rect, p);
}

bool Widget::isClippingParentContainsPoint(const Vec2& pt, const Camera* camera)
{
    _affectByClipping      = false;
    Node* parent           = getParent();
    Widget* clippingParent = nullptr;

    while (parent)
    {
        LayoutGroup* layoutParent = dynamic_cast<LayoutGroup*>(parent);
        if (layoutParent)
        {
            if (layoutParent->isClippingEnabled())
            {
                _affectByClipping = true;
                clippingParent    = layoutParent;
                break;
            }
        }

        parent = parent->getParent();
    }

    if (!_affectByClipping)
        return true;

    if (!clippingParent || !camera)
        return false;

    if (clippingParent->hitTestSelf(pt, camera, nullptr))
    {
        return clippingParent->isClippingParentContainsPoint(pt, camera);
    }

    return false;
}

void Widget::interceptPointerEvent(ax::ui::Widget* sender, PointerEvent* event)
{
    Widget* widgetParent = getWidgetParent();
    if (widgetParent)
    {
        widgetParent->_hittedByCamera = _hittedByCamera;
        widgetParent->interceptPointerEvent(sender, event);
        widgetParent->_hittedByCamera = nullptr;
    }
}

void Widget::setPosition(const Vec2& pos)
{
    if (!_usingLayoutComponent && _running)
    {
        Widget* widgetParent = getWidgetParent();
        if (widgetParent)
        {
            Vec2 pSize = widgetParent->getContentSize();
            if (pSize.width <= 0.0f || pSize.height <= 0.0f)
            {
                _positionPercent.setZero();
            }
            else
            {
                _positionPercent.set(pos.x / pSize.width, pos.y / pSize.height);
            }
        }
    }
    ProtectedNode::setPosition(pos);
}

void Widget::setPositionNormalized(const Vec2& position)
{
    if (!_usingLayoutComponent && _running)
    {
        Widget* widgetParent = getWidgetParent();
        if (widgetParent)
        {
            Vec2 pSize = widgetParent->getContentSize();
            if (pSize.width <= 0.0f || pSize.height <= 0.0f)
            {
                _positionPercent.setZero();
            }
            else
            {
                _positionPercent.set(position.x, position.y);
            }
        }
    }
    ProtectedNode::setPositionNormalized(position);
}

void Widget::setPositionPercent(const Vec2& percent)
{
    if (_usingLayoutComponent)
    {
        auto component = this->getOrCreateLayoutComponent();
        component->setPositionPercentX(percent.x);
        component->setPositionPercentY(percent.y);
        component->refreshLayout();
    }
    else
    {
        _positionPercent = percent;
        if (_running)
        {
            if (_usingNormalizedPosition)
            {
                setPositionNormalized(Vec2(_positionPercent.x, _positionPercent.y));
            }
            else
            {
                Widget* widgetParent = getWidgetParent();
                if (widgetParent)
                {
                    Vec2 parentSize = widgetParent->getContentSize();
                    Vec2 absPos(parentSize.width * _positionPercent.x, parentSize.height * _positionPercent.y);
                    setPosition(absPos);
                }
            }
        }
    }
}

const Vec2& Widget::getPositionPercent()
{
    if (_usingLayoutComponent)
    {
        auto component = this->getOrCreateLayoutComponent();
        float percentX = component->getPositionPercentX();
        float percentY = component->getPositionPercentY();

        _positionPercent.set(percentX, percentY);
    }
    return _positionPercent;
}

void Widget::setPositionType(PositionType type)
{
    _positionType = type;

    if (_usingLayoutComponent)
    {
        auto component = this->getOrCreateLayoutComponent();
        if (type == Widget::PositionType::ABSOLUTE)
        {
            component->setPositionPercentXEnabled(false);
            component->setPositionPercentYEnabled(false);
        }
        else
        {
            component->setPositionPercentXEnabled(true);
            component->setPositionPercentYEnabled(true);
        }
    }
}

Widget::PositionType Widget::getPositionType() const
{
    return _positionType;
}

bool Widget::isBright() const
{
    return _bright;
}

bool Widget::isEnabled() const
{
    return _enabled;
}

float Widget::getLeftBoundary() const
{
    return getBoundingBox().origin.x;
}

float Widget::getBottomBoundary() const
{
    return getBoundingBox().origin.y;
}

float Widget::getRightBoundary() const
{
    return getLeftBoundary() + getBoundingBox().size.width;
}

float Widget::getTopBoundary() const
{
    return getBottomBoundary() + getBoundingBox().size.height;
}

const Vec2& Widget::getTouchBeganPosition() const
{
    return _pointerDownPosition;
}

const Vec2& Widget::getTouchMovePosition() const
{
    return _pointerMovePosition;
}

const Vec2& Widget::getTouchEndPosition() const
{
    return _pointerUpPosition;
}

void Widget::setLayoutParameter(LayoutParameter* parameter)
{
    if (!parameter)
    {
        return;
    }
    _layoutParameterDictionary.insert((int)parameter->getLayoutType(), parameter);
    _layoutParameterType = parameter->getLayoutType();
}

LayoutParameter* Widget::getLayoutParameter() const
{
    return dynamic_cast<LayoutParameter*>(_layoutParameterDictionary.at((int)_layoutParameterType));
}

std::string Widget::getDescription() const
{
    return "Widget";
}

Widget* Widget::clone()
{
    Widget* clonedWidget = createCloneInstance();
    clonedWidget->copyProperties(this);
    clonedWidget->copyClonedWidgetChildren(this);
    return clonedWidget;
}

Widget* Widget::createCloneInstance()
{
    return Widget::create();
}

void Widget::copyClonedWidgetChildren(Widget* model)
{
    auto& modelChildren = model->getChildren();

    for (auto&& subWidget : modelChildren)
    {
        Widget* child = dynamic_cast<Widget*>(subWidget);
        if (child)
        {
            addChild(child->clone());
        }
    }
}

void Widget::copySpecialProperties(Widget* /*model*/) {}

void Widget::copyProperties(Widget* widget)
{
    setEnabled(widget->isEnabled());
    setVisible(widget->isVisible());
    setBright(widget->isBright());
    setPointerEnabled(widget->isPointerEnabled());
    setLocalZOrder(widget->getLocalZOrder());
    setTag(widget->getTag());
    setName(widget->getName());
    setActionTag(widget->getActionTag());
    _autoSize = widget->_autoSize;
    this->setContentSize(widget->_contentSize);
    _customSize      = widget->_customSize;
    _sizeType        = widget->getSizeType();
    _sizePercent     = widget->_sizePercent;
    _positionType    = widget->_positionType;
    _positionPercent = widget->_positionPercent;
    setPosition(widget->getPosition());
    setAnchorPoint(widget->getAnchorPoint());
    setScaleX(widget->getScaleX());
    setScaleY(widget->getScaleY());
    setRotation(widget->getRotation());
    setRotationSkewX(widget->getRotationSkewX());
    setRotationSkewY(widget->getRotationSkewY());
    setFlippedX(widget->isFlippedX());
    setFlippedY(widget->isFlippedY());
    setColor(widget->getColor());
    setOpacity(widget->getOpacity());
    setCascadeColorEnabled(widget->isCascadeColorEnabled());
    setCascadeOpacityEnabled(widget->isCascadeOpacityEnabled());
    _pointerEventHandler    = widget->_pointerEventHandler;
    _clickEventHandler      = widget->_clickEventHandler;
    _customEventCallback    = widget->_customEventCallback;
    _focused                = widget->_focused;
    _focusEnabled           = widget->_focusEnabled;
    _propagatePointerEvents = widget->_propagatePointerEvents;

    copySpecialProperties(widget);

    Map<int, LayoutParameter*>& layoutParameterDic = widget->_layoutParameterDictionary;
    for (auto&& iter : layoutParameterDic)
    {
        setLayoutParameter(iter.second->clone());
    }
}

void Widget::setFlippedX(bool flippedX)
{

    float realScale = this->getScaleX();
    _flippedX       = flippedX;
    this->setScaleX(realScale);
}

void Widget::setFlippedY(bool flippedY)
{
    float realScale = this->getScaleY();
    _flippedY       = flippedY;
    this->setScaleY(realScale);
}

void Widget::setScaleX(float scaleX)
{
    if (_flippedX)
    {
        scaleX = scaleX * -1;
    }
    Node::setScaleX(scaleX);
}

void Widget::setScaleY(float scaleY)
{
    if (_flippedY)
    {
        scaleY = scaleY * -1;
    }
    Node::setScaleY(scaleY);
}

void Widget::setScale(float scale)
{
    this->setScaleX(scale);
    this->setScaleY(scale);
    this->setScaleZ(scale);
}

void Widget::setScale(float scaleX, float scaleY)
{
    this->setScaleX(scaleX);
    this->setScaleY(scaleY);
}

float Widget::getScaleX() const
{
    float originalScale = Node::getScaleX();
    if (_flippedX)
    {
        originalScale = originalScale * -1.0f;
    }
    return originalScale;
}

float Widget::getScaleY() const
{
    float originalScale = Node::getScaleY();
    if (_flippedY)
    {
        originalScale = originalScale * -1.0f;
    }
    return originalScale;
}

float Widget::getScale() const
{
    AXASSERT(this->getScaleX() == this->getScaleY(), "scaleX should be equal to scaleY.");
    return this->getScaleX();
}

/*temp action*/
void Widget::setActionTag(int tag)
{
    _actionTag = tag;
}

int Widget::getActionTag() const
{
    return _actionTag;
}

void Widget::setFocused(bool focus)
{
    _focused = focus;

    // make sure there is only one focusedWidget
    if (focus)
    {
        _focusedWidget = this;
        if (_focusNavigationController)
        {
            _focusNavigationController->setFirstFocusedWidget(this);
        }
    }
    else if (_focusedWidget == this)
    {
        _focusedWidget = nullptr;
    }
}

bool Widget::isFocused() const
{
    return _focused;
}

void Widget::setFocusEnabled(bool enable)
{
    _focusEnabled = enable;
}

bool Widget::isFocusEnabled() const
{
    return _focusEnabled;
}

Widget* Widget::findNextFocusedWidget(FocusDirection direction, Widget* current)
{
    if (nullptr == onNextFocusedWidget || nullptr == onNextFocusedWidget(direction))
    {
        if (this->isFocused() || dynamic_cast<LayoutGroup*>(current))
        {
            Node* parent = this->getParent();

            LayoutGroup* layout = dynamic_cast<LayoutGroup*>(parent);
            if (nullptr == layout)
            {
                // the outer layout's default behaviour is : loop focus
                if (dynamic_cast<LayoutGroup*>(current))
                {
                    return current->findNextFocusedWidget(direction, current);
                }
                return current;
            }
            else
            {
                Widget* nextWidget = layout->findNextFocusedWidget(direction, current);
                return nextWidget;
            }
        }
        else
        {
            return current;
        }
    }
    else
    {
        Widget* getFocusWidget = onNextFocusedWidget(direction);
        this->dispatchFocusEvent(this, getFocusWidget);
        return getFocusWidget;
    }
}

void Widget::dispatchFocusEvent(ax::ui::Widget* widgetLoseFocus, ax::ui::Widget* widgetGetFocus)
{
    // if the widgetLoseFocus doesn't get focus, it will use the previous focused widget instead
    if (widgetLoseFocus && !widgetLoseFocus->isFocused())
    {
        widgetLoseFocus = _focusedWidget;
    }

    if (widgetGetFocus != widgetLoseFocus)
    {

        if (widgetGetFocus)
        {
            widgetGetFocus->onFocusChanged(widgetLoseFocus, widgetGetFocus);
        }

        if (widgetLoseFocus)
        {
            widgetLoseFocus->onFocusChanged(widgetLoseFocus, widgetGetFocus);
        }

        FocusEvent event(widgetLoseFocus, widgetGetFocus);
        auto dispatcher = _director->getEventDispatcher();
        dispatcher->dispatchEvent(&event);
    }
}

void Widget::requestFocus()
{
    if (this == _focusedWidget)
    {
        return;
    }

    this->dispatchFocusEvent(_focusedWidget, this);
}

void Widget::onFocusChange(Widget* widgetLostFocus, Widget* widgetGetFocus)
{
    // only change focus when there is indeed a get&lose happens
    if (widgetLostFocus)
    {
        widgetLostFocus->setFocused(false);
    }

    if (widgetGetFocus)
    {
        widgetGetFocus->setFocused(true);
    }
}

Widget* Widget::getCurrentFocusedWidget()
{
    return _focusedWidget;
}

void Widget::enableDpadNavigation(bool enable)
{
    if (enable)
    {
        if (nullptr == _focusNavigationController)
        {
            _focusNavigationController = new FocusNavigationController;
            if (_focusedWidget)
            {
                _focusNavigationController->setFirstFocusedWidget(_focusedWidget);
            }
        }
    }
    else
    {
        AX_SAFE_DELETE(_focusNavigationController);
    }

    if (nullptr != _focusNavigationController)
    {
        _focusNavigationController->enableFocusNavigation(enable);
    }
}

void Widget::setAutoSize(bool enable)
{
    if (_autoSize == enable)
        return;

    _autoSize = enable;

    if (!_autoSize)
    {
        // Switching to fixed size mode: use the explicit custom size when one
        // was provided while auto-size was active. If there is no custom size
        // yet, preserve the current auto-sized content size.
        if (_customSize.equals(Vec2::ZERO))
            _customSize = getContentSize();

        ProtectedNode::setContentSize(_customSize);
        onSizeChanged();
    }
    else
    {
        // Switching to auto size mode: update to content size immediately
        ProtectedNode::setContentSize(this->resolvePreferredSize(_customSize));

        onSizeChanged();
    }
}

void Widget::setLayoutComponentEnabled(bool enable)
{
    _usingLayoutComponent = enable;
}

bool Widget::isLayoutComponentEnabled() const
{
    return _usingLayoutComponent;
}

}  // namespace ui
}  // namespace ax
