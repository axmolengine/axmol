#include "InputProcessor.h"
#include "GComponent.h"
#include "InputEvent.h"
#include "GRoot.h"
#include "GRichTextField.h"
#include "GTextInput.h"
#include "axmol/base/WeakPtr.h"

NS_FGUI_BEGIN
using namespace ax;

InputProcessor* InputProcessor::_activeProcessor = nullptr;
bool InputProcessor::_touchOnUI = false;
unsigned int InputProcessor::_touchOnUIFlagFrameId = 0;

class TouchInfo
{
public:
    TouchInfo();
    ~TouchInfo();

    void reset();

    ax::PointerEvent* event;
    intptr_t pointerId;
    ax::Vec2 pos;
    int clickCount;
    int mouseWheelDelta;
    int button;
    ax::Vec2 downPos;
    bool began;
    bool clickCancelled;
    clock_t lastClickTime;
    ax::WeakPtr<GObject> lastRollOver;
    std::vector<ax::WeakPtr<GObject>> downTargets;
    std::vector<ax::WeakPtr<GObject>> touchMonitors;
};

InputProcessor::InputProcessor(GComponent* owner) :
    _keyModifiers(0),
    _pointerListener(nullptr),
    _keyboardListener(nullptr)
{
    _owner = owner;
    _recentInput._inputProcessor = this;

    _pointerListener = PointerEventListener::create();
    AX_SAFE_RETAIN(_pointerListener);
    _pointerListener->onPointerDown   = AX_CALLBACK_1(InputProcessor::onPointerDown, this);
    _pointerListener->onPointerMove   = AX_CALLBACK_1(InputProcessor::onPointerMove, this);
    _pointerListener->onPointerUp     = AX_CALLBACK_1(InputProcessor::onPointerUp, this);
    _pointerListener->onPointerCancel = AX_CALLBACK_1(InputProcessor::onPointerCancel, this);
    _pointerListener->onPointerScroll = AX_CALLBACK_1(InputProcessor::onPointerScroll, this);
    _owner->displayObject()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_pointerListener,
                                                                                          _owner->displayObject());

    _keyboardListener = KeyboardEventListener::create();
    AX_SAFE_RETAIN(_keyboardListener);
    _keyboardListener->onKeyPressed = AX_CALLBACK_1(InputProcessor::onKeyDown, this);
    _keyboardListener->onKeyReleased = AX_CALLBACK_1(InputProcessor::onKeyUp, this);
    _owner->displayObject()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_keyboardListener, _owner->displayObject());
}

InputProcessor::~InputProcessor()
{
    _owner->displayObject()->getEventDispatcher()->removeEventListener(_pointerListener);
    _owner->displayObject()->getEventDispatcher()->removeEventListener(_keyboardListener);
    AX_SAFE_RELEASE_NULL(_pointerListener);
    AX_SAFE_RELEASE_NULL(_keyboardListener);

    for (auto &ti : _touches)
        delete ti;
}

ax::Vec2 InputProcessor::getTouchPosition(intptr_t touchId)
{
    for (auto &ti : _touches)
    {
        if (ti->pointerId == touchId)
            return ti->pos;
    }
    return _recentInput.getPosition();
}

TouchInfo* InputProcessor::getTouch(int touchId, bool createIfNotExisits)
{
    TouchInfo* ret = nullptr;
    for (auto &ti : _touches)
    {
        if (ti->pointerId == touchId)
            return ti;
        else if (ti->pointerId == -1)
            ret = ti;
    }

    if (!ret)
    {
        if (!createIfNotExisits)
            return nullptr;

        ret = new TouchInfo();
        _touches.push_back(ret);
    }
    ret->pointerId = touchId;
    return ret;
}

void InputProcessor::updateRecentInput(TouchInfo* ti, GObject* target)
{
    _recentInput._pos.x = (int)ti->pos.x;
    _recentInput._pos.y = (int)ti->pos.y;
    _recentInput._target = target;
    _recentInput._clickCount = ti->clickCount;
    _recentInput._button = ti->button;
    _recentInput._mouseWheelDelta = ti->mouseWheelDelta;
    _recentInput._pointerId       = ti->event ? ti->event->getPointerId() : -1;

    int curFrame = Director::getInstance()->getTotalFrames();
    bool flag = target != _owner;
    if (curFrame != _touchOnUIFlagFrameId)
        _touchOnUI = flag;
    else if (flag)
        _touchOnUI = true;
    _touchOnUIFlagFrameId = curFrame;
}

void InputProcessor::handleRollOver(TouchInfo* touch, GObject* target)
{
    if (touch->lastRollOver == target)
        return;

    std::vector<ax::WeakPtr<GObject>> rollOutChain;
    std::vector<ax::WeakPtr<GObject>> rollOverChain;
    GObject* element = touch->lastRollOver.get();
    while (element != nullptr)
    {
        rollOutChain.emplace_back(element);
        element = element->findParent();
    }

    element = target;
    while (element != nullptr)
    {
        auto iter = std::find(rollOutChain.cbegin(), rollOutChain.cend(), element);
        if (iter != rollOutChain.cend())
        {
            rollOutChain.resize(iter - rollOutChain.cbegin());
            break;
        }
        rollOverChain.emplace_back(element);

        element = element->findParent();
    }

    touch->lastRollOver = target;

    for (auto &wptr : rollOutChain)
    {
        element = wptr.get();
        if (element && element->onStage())
            element->dispatchEvent(UIEventType::RollOut);
    }

    for (auto &wptr : rollOverChain)
    {
        element = wptr.get();
        if (element && element->onStage())
            element->dispatchEvent(UIEventType::RollOver);
    }
}

void InputProcessor::addTouchMonitor(int touchId, GObject * target)
{
    TouchInfo* ti = getTouch(touchId, false);
    if (!ti)
        return;

    auto it = std::find(ti->touchMonitors.cbegin(), ti->touchMonitors.cend(), target);
    if (it == ti->touchMonitors.cend())
        ti->touchMonitors.emplace_back(target);
}

void InputProcessor::removeTouchMonitor(GObject * target)
{
    for (auto it = _touches.cbegin(); it != _touches.cend(); ++it)
    {
        auto it2 = std::find((*it)->touchMonitors.begin(), (*it)->touchMonitors.end(), target);
        if (it2 != (*it)->touchMonitors.end())
            *it2 = nullptr;
    }
}

void InputProcessor::cancelClick(int touchId)
{
    TouchInfo* ti = getTouch(touchId, false);
    if (ti)
        ti->clickCancelled = true;
}

void InputProcessor::simulateClick(GObject* target, int touchId)
{
    _activeProcessor = this;

    Vec2 pt = target->localToGlobal(Vec2::ZERO);
    _recentInput._pos.x = pt.x;
    _recentInput._pos.y = pt.y;
    _recentInput._target = target;
    _recentInput._clickCount = 1;
    _recentInput._button = ax::InputButton::Left;
    _recentInput._pointerEvent = nullptr;
    _recentInput._pointerId = touchId;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchBegin);

    ax::WeakPtr<GObject> wptr(target);
    target->bubbleEvent(UIEventType::TouchBegin);

    target = wptr.get();
    if (target)
    {
        target->bubbleEvent(UIEventType::TouchEnd);

        target = wptr.get();
        if (target)
            target->bubbleEvent(UIEventType::Click);
    }

    _activeProcessor = nullptr;
}

void InputProcessor::setBegin(TouchInfo* touch, GObject* target)
{
    touch->began = true;
    touch->clickCancelled = false;
    touch->downPos = touch->pos;

    touch->downTargets.clear();
    GObject* obj = target;
    while (obj != nullptr)
    {
        touch->downTargets.emplace_back(obj);
        obj = obj->findParent();
    }
}

void InputProcessor::setEnd(TouchInfo* touch, GObject* target)
{
    touch->began = false;

    auto now = ::clock();
    float elapsed = (now - touch->lastClickTime) / (double)CLOCKS_PER_SEC;

    if (elapsed < 0.45f)
    {
        if (touch->clickCount == 2)
            touch->clickCount = 1;
        else
            touch->clickCount++;
    }
    else
        touch->clickCount = 1;
    touch->lastClickTime = now;
}

GObject* InputProcessor::clickTest(TouchInfo* touch, GObject* target)
{
    if (touch->downTargets.empty()
        || touch->clickCancelled
        || std::abs(touch->pos.x - touch->downPos.x) > 50 || std::abs(touch->pos.y - touch->downPos.y) > 50)
        return nullptr;

    GObject* obj = touch->downTargets[0].get();
    if (obj && obj->onStage())
        return obj;

    obj = target;
    while (obj != nullptr)
    {
        auto it = std::find(touch->downTargets.cbegin(), touch->downTargets.cend(), obj);
        if (it != touch->downTargets.cend())
        {
            obj = it->get();
            if (obj && obj->onStage())
                break;
        }

        obj = obj->findParent();
    }

    return obj;
}

bool InputProcessor::isTouchOnUI()
{
    return _touchOnUI;
}

void InputProcessor::disableDefaultTouchEvent()
{
    _owner->displayObject()->getEventDispatcher()->removeEventListener(_pointerListener);
}

bool InputProcessor::pointerDown(ax::PointerEvent *event)
{
    return onPointerDown(event);
}

void InputProcessor::pointerMove(ax::PointerEvent* event)
{
    onPointerMove(event);
}

void InputProcessor::pointerUp(ax::PointerEvent* event)
{
    onPointerUp(event);
}

bool InputProcessor::onPointerDown(ax::PointerEvent* event)
{
    if (!event->isPrimaryPressed())
        return false;

    if (!(_owner->isTouchable() && _owner->isVisible())) {
        return false;
    }
    
    auto camera = Camera::getVisitingCamera();
    Vec2 pt         = event->getLocation();
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    TouchInfo* ti    = getTouch(event->getPointerId());
    ti->pos = UIRoot->worldToRoot(pt);
    ti->button = ax::InputButton::Left;
    ti->event  = event;
    setBegin(ti, target);

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchBegin);

    ax::WeakPtr<GObject> wptr(target);
    target->bubbleEvent(UIEventType::TouchBegin);
    target = wptr.get();

    handleRollOver(ti, target);

    _activeProcessor = nullptr;

    return true;
}

void InputProcessor::onPointerMove(ax::PointerEvent* event)
{
    auto camera = Camera::getVisitingCamera();
    Vec2 pt         = event->getLocation();
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(event->getPointerId());
    ti->pos = UIRoot->worldToRoot(pt);
    ti->button    = ax::InputButton::Left;
    ti->event     = event;

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchMove);

    handleRollOver(ti, target);

    if (ti->began)
    {
        bool done = false;
        size_t cnt = ti->touchMonitors.size();
        if (cnt > 0)
        {
            for (size_t i = 0; i < cnt; i++)
            {
                GObject* mm = ti->touchMonitors[i].get();
                if (!mm)
                    continue;

                mm->dispatchEvent(UIEventType::TouchMove);
                if (mm == _owner)
                    done = true;
            }
        }
        if (!done)
            _owner->dispatchEvent(UIEventType::TouchMove);
    }

    _activeProcessor = nullptr;
}

void InputProcessor::onPointerUp(ax::PointerEvent* event)
{
    auto camera = Camera::getVisitingCamera();
    Vec2 pt         = event->getLocation();
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(event->getPointerId());
    ti->pos = UIRoot->worldToRoot(pt);
    ti->button = ax::InputButton::Left;
    ti->event     = event;
    setEnd(ti, target);

    updateRecentInput(ti, target);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchEnd);

    ax::WeakPtr<GObject> wptr(target);
    size_t cnt = ti->touchMonitors.size();
    if (cnt > 0)
    {
        for (size_t i = 0; i < cnt; i++)
        {
            GObject* mm = ti->touchMonitors[i].get();
            if (!mm)
                continue;

            if (mm != target
                && (!dynamic_cast<GComponent*>(mm) || !((GComponent*)mm)->isAncestorOf(target)))
                mm->dispatchEvent(UIEventType::TouchEnd);
        }
        ti->touchMonitors.clear();
        target = wptr.get();
    }
    if (target)
    {
        target->bubbleEvent(UIEventType::TouchEnd);
        target = wptr.get();
    }

    target = clickTest(ti, target);
    if (target)
    {
        wptr = target;
        updateRecentInput(ti, target);

        GRichTextField* tf = dynamic_cast<GRichTextField*>(target);
        if (tf)
        {
            const char* linkHref = dynamic_cast<FUIRichText*>(tf->displayObject())->hitTestLink(pt);
            if (linkHref)
            {
                tf->bubbleEvent(UIEventType::ClickLink, nullptr, Value(linkHref));
                target = wptr.get();
            }
        }

        target->bubbleEvent(UIEventType::Click);
        target = wptr.get();
    }

#ifndef AX_PLATFORM_PC
    //on mobile platform, trigger RollOut on up event, but not on PC
    handleRollOver(ti, nullptr);
#else
    handleRollOver(ti, target);
#endif
    ti->pointerId = -1;
    ti->button = ax::InputButton::None;

    _activeProcessor = nullptr;
}

void InputProcessor::onPointerCancel(ax::PointerEvent* event)
{
    TouchInfo* ti = getTouch(event->getPointerId());
    if (ti == nullptr)
        return;

    ti->event = event;
    updateRecentInput(ti, _owner);
    _activeProcessor = this;

    if (_captureCallback)
        _captureCallback(UIEventType::TouchEnd);

    size_t cnt = ti->touchMonitors.size();
    if (cnt > 0)
    {
        for (size_t i = 0; i < cnt; i++)
        {
            GObject* mm = ti->touchMonitors[i].get();
            if (!mm)
                continue;

            if (mm != _owner)
                mm->dispatchEvent(UIEventType::TouchEnd);
        }
        ti->touchMonitors.clear();
    }
    _owner->dispatchEvent(UIEventType::TouchEnd);

    handleRollOver(ti, nullptr);

    ti->pointerId = -1;
    ti->button = ax::InputButton::None;
    _activeProcessor = nullptr;
}

bool InputProcessor::onPointerScroll(ax::PointerEvent* event)
{
    auto camera = Camera::getVisitingCamera();
    Vec2 pt = event->getLocation();
    GObject* target = _owner->hitTest(pt, camera);
    if (!target)
        target = _owner;

    TouchInfo* ti = getTouch(0);
    ti->pos = UIRoot->worldToRoot(pt);
    ti->event = nullptr;
    ti->mouseWheelDelta = MAX(event->getScrollX(), event->getScrollY());

    updateRecentInput(ti, target);
    _activeProcessor = this;

    target->bubbleEvent(UIEventType::MouseWheel);
    ti->mouseWheelDelta = 0;

    _activeProcessor = nullptr;

    return true;
}

void InputProcessor::onKeyDown(ax::KeyboardEvent* event)
{
    auto keyCode = event->getKeyCode();
    if (keyCode == KeyboardEvent::KeyCode::KEY_LEFT_CTRL || keyCode == KeyboardEvent::KeyCode::KEY_RIGHT_CTRL)
        _keyModifiers |= 1;
    else if (keyCode == KeyboardEvent::KeyCode::KEY_LEFT_ALT || keyCode == KeyboardEvent::KeyCode::KEY_RIGHT_ALT)
        _keyModifiers |= 2;
    else if (keyCode == KeyboardEvent::KeyCode::KEY_LEFT_SHIFT || keyCode == KeyboardEvent::KeyCode::KEY_RIGHT_SHIFT)
        _keyModifiers |= 4;

    _recentInput._keyCode = keyCode;
    _recentInput._target = _owner;

	_activeProcessor = this; // add by binxiaojiao
	_recentInput._target->dispatchEvent(UIEventType::KeyDown);
	_activeProcessor = nullptr; // add by binxiaojiao
}

void InputProcessor::onKeyUp(ax::KeyboardEvent* event)
{
    auto keyCode = event->getKeyCode();
    if (keyCode == KeyboardEvent::KeyCode::KEY_LEFT_CTRL || keyCode == KeyboardEvent::KeyCode::KEY_RIGHT_CTRL)
        _keyModifiers &= ~1;
    else if (keyCode == KeyboardEvent::KeyCode::KEY_LEFT_ALT || keyCode == KeyboardEvent::KeyCode::KEY_RIGHT_ALT)
        _keyModifiers &= ~2;
    else if (keyCode == KeyboardEvent::KeyCode::KEY_LEFT_SHIFT || keyCode == KeyboardEvent::KeyCode::KEY_RIGHT_SHIFT)
        _keyModifiers &= ~4;

    _recentInput._keyCode = keyCode;
    _recentInput._target = _owner;

	_activeProcessor = this; // add by binxiaojiao
    _recentInput._target->dispatchEvent(UIEventType::KeyUp);
	_activeProcessor = nullptr; // add by binxiaojiao
}

TouchInfo::TouchInfo() :
    event(nullptr),
    pointerId(-1),
    clickCount(0),
    mouseWheelDelta(0),
    button(ax::InputButton::None),
    began(false),
    lastClickTime(0),
    clickCancelled(false)
{
}

TouchInfo::~TouchInfo()
{
    downTargets.clear();
    touchMonitors.clear();
}

void TouchInfo::reset()
{
    pointerId       = -1;
    mouseWheelDelta = 0;
    button = ax::InputButton::None;
    event = nullptr;
    pos.setZero();
    downPos.setZero();
    clickCount = 0;
    lastClickTime = 0;
    began = false;
    downTargets.clear();
    lastRollOver = nullptr;
    clickCancelled = false;
    touchMonitors.clear();
}

NS_FGUI_END
