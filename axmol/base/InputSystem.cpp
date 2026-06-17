/****************************************************************************
Copyright (c) 2010      cocos2d-x.org
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

#include "axmol/base/InputSystem.h"

#include "axmol/base/EventDispatcher.h"

#include "axmol/base/PointerEvent.h"
#include "axmol/scene/Camera.h"

#include <list>
#include <cfloat>
#include <map>

// Touch bookkeeping was migrated into InputSystem private members.

namespace ax
{

using EventDispatcher = EventDispatcher;  // ensure symbol visibility

InputSystem* InputSystem::_instance = nullptr;

//////////////////////////////////////////////////////////////////////////
// add/remove delegate in InputDelegate Cons/Destructor
//////////////////////////////////////////////////////////////////////////

InputDelegate::InputDelegate()
{
    InputSystem::getInstance()->addDelegate(this);
}

InputDelegate::~InputDelegate()
{
    InputSystem::getInstance()->removeDelegate(this);
}

bool InputDelegate::attachWithIME()
{
    return InputSystem::getInstance()->attachDelegateWithIME(this);
}

bool InputDelegate::detachWithIME()
{
    return InputSystem::getInstance()->detachDelegateWithIME(this);
}

//////////////////////////////////////////////////////////////////////////
// Cons/Destructor
//////////////////////////////////////////////////////////////////////////

InputSystem::InputSystem()
{
    _pointerEvents.reserve(4);
    _eventDispatcher = Director::getInstance()->getEventDispatcher();
}

InputSystem::~InputSystem()
{
    for (auto* event : _pointerEvents)
    {
        AX_SAFE_RELEASE(event);
    }
    _pointerEvents.clear();
}

Rect InputSystem::getNodeNativeWindowRect(Node* node) const
{
    // 1. Fetch the currently active running camera
    auto camera = Camera::getVisitingCamera();
    if (!camera)
        camera = Camera::getDefaultCamera();

    // 2. Transform local bounds of the node directly into 3D World Space coordinates
    auto worldLeftBottom = node->convertToWorldSpace(Vec2::zero);
    auto worldRightTop   = node->convertToWorldSpace(node->getContentSize());

    // 3. Project world positions straight to absolute Top-Left screen pixels using our new robust Camera API
    // (This automatically handles viewScale, window stretching, DPI scaling, and black bars offset)
    Vec2 screenLeftBottom = camera->projectWorldToScreen(Vec3(worldLeftBottom.x, worldLeftBottom.y, 0.0f));
    Vec2 screenRightTop   = camera->projectWorldToScreen(Vec3(worldRightTop.x, worldRightTop.y, 0.0f));

    // 4. Construct the standard Top-Left Rect
    // Note: Since screenSpace Y grows downwards, the "top" of the UI rect corresponds to the higher world position
    // (screenRightTop.y)
    float uiLeft   = screenLeftBottom.x;
    float uiTop    = screenRightTop.y;
    float uiWidth  = screenRightTop.x - screenLeftBottom.x;
    float uiHeight = screenLeftBottom.y - screenRightTop.y;

    if (_inputScale != 0.0f && _inputScale != 1.0f)
    {
        uiLeft /= _inputScale;
        uiTop /= _inputScale;
        uiWidth /= _inputScale;
        uiHeight /= _inputScale;
    }

    return Rect(uiLeft, uiTop, uiWidth, uiHeight);
}

void InputSystem::setInputScale(float scale)
{
    _inputScale = std::clamp(scale, 1.0f, 100.0f);
}

Vec2 InputSystem::screenToNative(const Vec2& point) const
{
    return Vec2{point.x / _inputScale, point.y / _inputScale};
}

Vec2 InputSystem::nativeToScreen(const Vec2& point) const
{
    return Vec2{point.x * _inputScale, point.y * _inputScale};
}

/////////////////////////////////////////////////////////////////////////
// touch/mouse/keyboard event

//////////////////////////////////////////////////////////////////////////
// clipboard operations
//////////////////////////////////////////////////////////////////////////
void InputSystem::dispatchPerformEditAction(EditAction action)
{
    if (_delegateWithIme)
        _delegateWithIme->performEditAction(action);
}

bool InputSystem::hasAttachedDelegate() const
{
    return _delegateWithIme != nullptr;
}

bool InputSystem::dispatchHitTestWithIME(const Vec2& location)
{
    if (_delegateWithIme)
        return _delegateWithIme->hitTestWithIME(location);
    return false;
}

//////////////////////////////////////////////////////////////////////////
// Add/Attach/Remove InputDelegate
//////////////////////////////////////////////////////////////////////////

InputSystem::DelegateIter InputSystem::findDelegate(InputDelegate* delegate)
{
    DelegateIter end = _delegateList.end();
    for (DelegateIter iter = _delegateList.begin(); iter != end; ++iter)
    {
        if (delegate == *iter)
        {
            return iter;
        }
    }
    return end;
}

void InputSystem::addDelegate(InputDelegate* delegate)
{
    if (!delegate)
    {
        return;
    }
    if (_delegateList.end() != findDelegate(delegate))
    {
        // pDelegate already in list
        return;
    }
    _delegateList.push_front(delegate);
}

bool InputSystem::attachDelegateWithIME(InputDelegate* delegate)
{
    bool ret = false;
    do
    {
        AX_BREAK_IF(!delegate);

        DelegateIter end  = _delegateList.end();
        DelegateIter iter = findDelegate(delegate);

        // if pDelegate is not in delegate list, return
        AX_BREAK_IF(end == iter);

        if (_delegateWithIme)
        {
            if (_delegateWithIme != delegate)
            {
                // if old delegate canDetachWithIME return false
                // or pDelegate canAttachWithIME return false,
                // do nothing.
                AX_BREAK_IF(!_delegateWithIme->canDetachWithIME() || !delegate->canAttachWithIME());

                // detach first
                InputDelegate* oldDelegate = _delegateWithIme;
                _delegateWithIme           = 0;
                oldDelegate->didDetachWithIME();

                _delegateWithIme = *iter;
                delegate->didAttachWithIME();
            }
            ret = true;
            break;
        }

        // delegate hasn't attached to IME yet
        AX_BREAK_IF(!delegate->canAttachWithIME());

        _delegateWithIme = *iter;
        delegate->didAttachWithIME();
        ret = true;
    } while (0);
    return ret;
}

bool InputSystem::detachDelegateWithIME(InputDelegate* delegate)
{
    bool ret = false;
    do
    {
        AX_BREAK_IF(!delegate || delegate != _delegateWithIme);

        AX_BREAK_IF(!delegate->canDetachWithIME());

        _delegateWithIme = 0;
        delegate->didDetachWithIME();
        ret = true;
    } while (0);
    return ret;
}

void InputSystem::removeDelegate(InputDelegate* delegate)
{
    do
    {
        AX_BREAK_IF(!delegate);

        DelegateIter iter = findDelegate(delegate);
        DelegateIter end  = _delegateList.end();
        AX_BREAK_IF(end == iter);

        if (_delegateWithIme)

            if (*iter == _delegateWithIme)
            {
                _delegateWithIme = 0;
            }
        _delegateList.erase(iter);
    } while (0);
}

void InputSystem::dispatchUpdatePreedit(std::string_view preeditText, int caret)
{
    if (_delegateWithIme)
        _delegateWithIme->updatePreeditText(preeditText, caret);
}

//////////////////////////////////////////////////////////////////////////
// dispatch text message
//////////////////////////////////////////////////////////////////////////

void InputSystem::dispatchInsertText(std::string_view text)
{
    if (_delegateWithIme && !text.empty())
        _delegateWithIme->insertText(text);
}

void InputSystem::dispatchDeleteBackward(unsigned int numChars)
{
    if (_delegateWithIme)
        _delegateWithIme->deleteBackward(numChars);
}

void InputSystem::dispatchControlKey(KeyboardEvent::KeyCode keyCode)
{
    if (_delegateWithIme)
        _delegateWithIme->controlKey(keyCode);
}

//////////////////////////////////////////////////////////////////////////
// dispatch keyboard message
//////////////////////////////////////////////////////////////////////////

void InputSystem::dispatchKeyboardWillShow(IMEKeyboardNotificationInfo& info)
{
    InputDelegate* delegate = nullptr;
    DelegateIter last       = _delegateList.end();
    for (DelegateIter first = _delegateList.begin(); first != last; ++first)
    {
        delegate = *(first);
        if (delegate)
        {
            delegate->keyboardWillShow(info);
        }
    }
}

void InputSystem::dispatchKeyboardDidShow(IMEKeyboardNotificationInfo& info)
{

    InputDelegate* delegate = nullptr;
    DelegateIter last       = _delegateList.end();
    for (DelegateIter first = _delegateList.begin(); first != last; ++first)
    {
        delegate = *(first);
        if (delegate)
        {
            delegate->keyboardDidShow(info);
        }
    }
}

void InputSystem::dispatchKeyboardWillHide(IMEKeyboardNotificationInfo& info)
{

    InputDelegate* delegate = nullptr;
    DelegateIter last       = _delegateList.end();
    for (DelegateIter first = _delegateList.begin(); first != last; ++first)
    {
        delegate = *(first);
        if (delegate)
        {
            delegate->keyboardWillHide(info);
        }
    }
}

void InputSystem::dispatchKeyboardDidHide(IMEKeyboardNotificationInfo& info)
{

    InputDelegate* delegate = nullptr;
    DelegateIter last       = _delegateList.end();
    for (DelegateIter first = _delegateList.begin(); first != last; ++first)
    {
        delegate = *(first);
        if (delegate)
        {
            delegate->keyboardDidHide(info);
        }
    }
}

void InputSystem::dispatchEvent(Event* event, bool immediate)
{
    if (!event)
        return;

    _eventDispatcher->dispatchEvent(event, immediate);
}

// Platform-facing unified input handlers ---------------------------------

void InputSystem::handleKeyEvent(KeyboardEvent::KeyCode keyCode, InputPhase phase)
{
    KeyboardEvent event(keyCode, phase);
    dispatchEvent(&event);
    bool stopped = event.isStopped();

    if (phase != InputPhase::KeyUp && !stopped)
    {
        switch (keyCode)
        {
        case KeyboardEvent::KeyCode::KEY_BACKSPACE:
            dispatchDeleteBackward(1);
            break;
        case KeyboardEvent::KeyCode::KEY_HOME:
        case KeyboardEvent::KeyCode::KEY_KP_HOME:
        case KeyboardEvent::KeyCode::KEY_DELETE:
        case KeyboardEvent::KeyCode::KEY_KP_DELETE:
        case KeyboardEvent::KeyCode::KEY_END:
        case KeyboardEvent::KeyCode::KEY_LEFT_ARROW:
        case KeyboardEvent::KeyCode::KEY_RIGHT_ARROW:
        case KeyboardEvent::KeyCode::KEY_ESCAPE:
            dispatchControlKey(keyCode);
            break;
        case KeyboardEvent::KeyCode::KEY_ENTER:
        case KeyboardEvent::KeyCode::KEY_KP_ENTER:
            dispatchInsertText("\n"sv);
            break;
        default:
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Touch handling migrated from RenderView
//////////////////////////////////////////////////////////////////////////

void InputSystem::handlePointerDown(Vec2 point, const PointerInputState& state)
{
    if (!_interactive)
        return;

    dispatchPointerEvent(InputPhase::PointerDown, point, state);
}

void InputSystem::handlePointerMove(Vec2 point, const PointerInputState& state)
{
    if (!_interactive)
        return;

    dispatchPointerEvent(InputPhase::PointerMove, point, state);
}

void InputSystem::handlePointerUp(Vec2 point, const PointerInputState& state)
{
    if (!_interactive)
        return;

    dispatchPointerEvent(InputPhase::PointerUp, point, state);

    auto remainingButtons = state.pressedButtons;
    if (state.button >= 0)
        remainingButtons &= ~(1u << state.button);
    if (state.type == PointerType::Touch || remainingButtons == 0)
        removePointerEvent(state.id);
}

void InputSystem::handlePointerCancel(Vec2 point, const PointerInputState& state)
{
    if (!_interactive)
        return;

    dispatchPointerEvent(InputPhase::PointerCancel, point, state);
    removePointerEvent(state.id);
}

void InputSystem::handlePointerScroll(Vec2 point, Vec2 scollDelat, const PointerInputState& state)
{
    if (!_interactive)
        return;

    _lastPointerPosition = point;

    _scrollEvent.setPointerInfo(InputPhase::PointerScroll, nativeToScreen(point), state);
    _scrollEvent.setScrollData(scollDelat);
    dispatchEvent(&_scrollEvent);
}

void InputSystem::setInteractive(bool interactive)
{
    if (_interactive == interactive)
        return;

    _interactive = interactive;

    if (_interactive)
        return;

    for (auto* event : _pointerEvents)
    {
        if (!event)
            continue;

        PointerInputState state;
        state.id             = event->getPointerId();
        state.pressure       = event->getPressure();
        state.button         = event->getButton();
        state.pressedButtons = event->getPressedButtons();
        state.type           = event->getPointerType();

        event->setPointerInfo(InputPhase::PointerCancel, event->getScreenLocation(), state);
        dispatchEvent(event);
        AX_SAFE_RELEASE(event);
    }
    _pointerEvents.clear();
}

PointerEvent* InputSystem::findPointerEvent(intptr_t pointerId) const
{
    auto iter = std::find_if(_pointerEvents.begin(), _pointerEvents.end(), [pointerId](const PointerEvent* event) {
        return event && event->getPointerId() == pointerId;
    });
    return iter != _pointerEvents.end() ? *iter : nullptr;
}

PointerEvent* InputSystem::fetchPointerEvent(intptr_t pointerId)
{
    if (auto event = findPointerEvent(pointerId))
        return event;

    auto* event = new PointerEvent();
    _pointerEvents.emplace_back(event);
    return event;
}

void InputSystem::removePointerEvent(intptr_t pointerId)
{
    auto iter = std::find_if(_pointerEvents.begin(), _pointerEvents.end(), [pointerId](const PointerEvent* event) {
        return event && event->getPointerId() == pointerId;
    });
    if (iter == _pointerEvents.end())
        return;

    AX_SAFE_RELEASE(*iter);
    _pointerEvents.erase(iter);
}

void InputSystem::dispatchPointerEvent(InputPhase phase, Vec2 point, const PointerInputState& state)
{
    _lastPointerPosition = point;

    PointerEvent* event = nullptr;

    if (phase == InputPhase::PointerDown)
    {
        if (state.type == PointerType::Touch)
        {
            auto tearEvent = findPointerEvent(state.id);
            // ============================================================================
            // PASSIVE RECOVERY: Handles OS touch interceptions (e.g. 3-Finger Screenshot)
            // ============================================================================
            if (tearEvent)
            {
                AXLOGW("[InputSystem] Touch mismatch: Tear pointer down caught for ID={}. Self-healing triggered.",
                       state.id);
                resetInput();
            }
        }

        bool isPrimary = state.type == PointerType::Mouse || _pointerEvents.empty();
        event          = fetchPointerEvent(state.id);
        event->setPrimary(isPrimary);
    }
    else
    {
        event = findPointerEvent(state.id);
        if (!event)
        {
            if (phase == InputPhase::PointerMove)
            {
                if (state.type == PointerType::Touch)
                {
                    AXLOGW("[InputSystem] Isolated touch move discarded: missing preceding down (id={}).", state.id);
                    return;
                }

                // Mouse hover / pen hover.
                event = &_isolatedMoveEvent;
            }
            else
            {
                // Terminal event without active pointer stream.
                // This can happen when the OS/window manager intercepts input, e.g.
                // double-click title bar to maximize, system gestures, focus changes.
                AXLOGD("[InputSystem] Orphan pointer terminal event discarded: phase={}, id={}, type={}.",
                       static_cast<int>(phase), state.id, static_cast<int>(state.type));
                return;
            }
        }
    }

    event->setPointerInfo(phase, nativeToScreen(point), state);
    dispatchEvent(event);
}

void InputSystem::resetInput()
{
    if (_pointerEvents.empty())
        return;

    AXLOGW("[InputSystem] Emergency flush: Purging {} stuck pointer events.", _pointerEvents.size());

    // Note: Make sure EventDispatcher has a corresponding reset/clear method if needed,
    // or we synthesize cancel events here and dispatch them.
    for (auto* event : _pointerEvents)
    {
        if (event && event->getPhase() != InputPhase::PointerUp && event->getPhase() != InputPhase::PointerCancel)
        {
            // Repurpose the event as a Cancel signal to release UI/Virtual Joysticks
            event->setPhase(InputPhase::PointerCancel);
            _eventDispatcher->dispatchEvent(event);
        }
    }

    // 2. Safely release memory and clear our local tracking cache
    for (auto event : _pointerEvents)
    {
        delete event;
    }
    _pointerEvents.clear();
}

//////////////////////////////////////////////////////////////////////////
// protected member function
//////////////////////////////////////////////////////////////////////////

void InputSystem::onPlatformKeyboardWillShow(float rawX, float rawY, float rawWidth, float rawHeight, float duration)
{
    auto director = ax::Director::getInstance();

    // Lambda to convert native platform pixels (Top-Left origin) into aligned World Space (Bottom-Left origin)
    auto convertToAlignedWorldRect = [&](float rx, float ry, float rw, float rh) -> ax::Rect {
        ax::Vec2 keyboardSize{rw, rh};
        ax::Vec2 keyboardPos{rx, ry};

        // Convert raw physical pixels/points to screen logic pixels via the input gateway
        keyboardSize = nativeToScreen(keyboardSize);
        keyboardPos  = nativeToScreen(keyboardPos);

        // Transform the relative screen size vector into World Space dimensions
        ax::Vec2 worldSize = director->screenToWorld(keyboardSize) - director->screenToWorld(ax::Vec2::zero);
        float worldW       = std::abs(worldSize.x);
        float worldH       = std::abs(worldSize.y);

        // Transform the screen position to World Space and shift from Top-Left to Bottom-Left orientation
        ax::Vec2 worldPos = director->screenToWorld(keyboardPos);
        float worldX      = worldPos.x;
        float worldY      = worldPos.y - worldH;

        return ax::Rect(worldX, worldY, worldW, worldH);
    };

    _cachedKeyboardNotifInfo.duration = duration;
    // Map and cache the fully-expanded keyboard geometry frame
    _cachedKeyboardNotifInfo.keyboardFrame = convertToAlignedWorldRect(rawX, rawY, rawWidth, rawHeight);

    this->dispatchKeyboardWillShow(_cachedKeyboardNotifInfo);
}

void InputSystem::onPlatformKeyboardDidShow()
{
    // Asynchronously dispatch the fully shown state snapshot
    this->dispatchKeyboardDidShow(_cachedKeyboardNotifInfo);
}

void InputSystem::onPlatformKeyboardWillHide(float duration)
{
    _cachedKeyboardNotifInfo.duration = duration;

    // Extract geometry layout metrics from the current cached visible frame
    float oldX = _cachedKeyboardNotifInfo.keyboardFrame.origin.x;
    float oldY = _cachedKeyboardNotifInfo.keyboardFrame.origin.y;
    float oldW = _cachedKeyboardNotifInfo.keyboardFrame.size.width;
    float oldH = _cachedKeyboardNotifInfo.keyboardFrame.size.height;

    // Advance state machine: Shift target frame downwards by its height to represent the fully hidden state
    _cachedKeyboardNotifInfo.keyboardFrame = ax::Rect(oldX, oldY - oldH, oldW, oldH);

    this->dispatchKeyboardWillHide(_cachedKeyboardNotifInfo);
}

void InputSystem::onPlatformKeyboardDidHide()
{
    // Close the current IME lifecycle loop by dispatching the final hidden state token
    this->dispatchKeyboardDidHide(_cachedKeyboardNotifInfo);
}

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////

InputSystem* InputSystem::getInstance()
{
    if (!_instance) [[unlikely]]
        _instance = new InputSystem();
    return _instance;
}

void InputSystem::destroyInstance()
{
    AX_SAFE_DELETE(_instance);
}

}  // namespace ax
