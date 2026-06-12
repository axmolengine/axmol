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

#pragma once

#include <list>
#include <map>
#include <vector>
#include "axmol/base/InputDelegate.h"
#include "axmol/base/PointerEvent.h"
#include "axmol/base/KeyboardEvent.h"

/**
 * @addtogroup base
 * @{
 */
namespace ax
{

class RenderViewCore;
class RenderView;
class Node;
class Event;
class EventDispatcher;

/**
@brief    Input Method Edit Message Dispatcher.
*/
class AX_DLL InputSystem
{
    using DelegateList = std::list<InputDelegate*>;
    using DelegateIter = std::list<InputDelegate*>::iterator;
    friend class InputDelegate;
    friend class RenderViewCore;
    friend class RenderView;

public:
    /**
     * @lua NA
     */
    ~InputSystem();

    /**
     * @brief Returns the shared InputSystem object for the system.
     * @lua NA
     */
    static InputSystem* getInstance();
    static void destroyInstance();
    /**
     * @brief Unified bridging utility to extract a node's UI bounding rect in native OS window coordinates.
     */
    Rect getNodeNativeWindowRect(Node* node) const;

    Vec2 screenToNative(const Vec2& point) const;
    Vec2 nativeToScreen(const Vec2& point) const;

    /////////////////////////////////////////////////////////////////////////////
    // pointer(mouse/touch) /keyboard event
    /////////////////////////////////////////////////////////////////////////////

    // handle key event
    void handleKeyEvent(KeyboardEvent::KeyCode keyCode, InputPhase phase);

    // pointer input handling helpers migrated from RenderView
    void handlePointerDown(Vec2 point, const PointerInputState& state);
    void handlePointerMove(Vec2 point, const PointerInputState& state);
    void handlePointerUp(Vec2 point, const PointerInputState& state);
    void handlePointerCancel(Vec2 point, const PointerInputState& state);

    void handlePointerScroll(Vec2 point, Vec2 scrollDelat, const PointerInputState& state);

    /**
     * Enable or disable pointer interactions.
     * When disabled, active pointers are canceled and new pointer events are ignored.
     */
    void setInteractive(bool interactive);
    bool isInteractive() const { return _interactive; }

    /**
     * @brief Dispatch a request to perform the given edit action.
     *
     * Called from the platform view on the main/UI thread. If no delegate is attached,
     * this call is a no-op.
     */
    void dispatchPerformEditAction(EditAction action);

    /**
     * @brief Returns the delegate attached to the IME, or nullptr if no delegate is attached.
     */
    bool hasAttachedDelegate() const;

    /**
     * @brief Returns the delegate attached to the IME, or nullptr if no delegate is attached.
     * @lua NA
     */
    bool dispatchHitTestWithIME(const Vec2& location);

    /*
     * @brief Dispatches the update-preedit-text message from IME.
     * @lua NA
     */
    void dispatchUpdatePreedit(std::string_view preeditText, int caret);

    /**
     * @brief Dispatches the input text from IME.
     * @lua NA
     */
    void dispatchInsertText(std::string_view text);

    /**
     * @brief Dispatches the delete-backward operation.
     * @lua NA
     */
    void dispatchDeleteBackward(unsigned int numChars);

    /**
     * @brief Dispatches the press control key operation.
     * @lua NA
     */
    void dispatchControlKey(KeyboardEvent::KeyCode keyCode);

    //////////////////////////////////////////////////////////////////////////
    // dispatch keyboard notification
    //////////////////////////////////////////////////////////////////////////
    /**
     * @lua NA
     */
    void dispatchKeyboardWillShow(IMEKeyboardNotificationInfo& info);
    /**
     * @lua NA
     */
    void dispatchKeyboardDidShow(IMEKeyboardNotificationInfo& info);
    /**
     * @lua NA
     */
    void dispatchKeyboardWillHide(IMEKeyboardNotificationInfo& info);
    /**
     * @lua NA
     */
    void dispatchKeyboardDidHide(IMEKeyboardNotificationInfo& info);

    /**
     * @brief Centralized event dispatch. InputSystem will apply input scaling
     *        to mouse/touch events before forwarding to EventDispatcher.
     */
    void dispatchEvent(Event* event, bool immediate = false);

    void onPlatformKeyboardWillShow(float rawX, float rawY, float rawWidth, float rawHeight, float duration);
    void onPlatformKeyboardDidShow();
    void onPlatformKeyboardWillHide(float duration);
    void onPlatformKeyboardDidHide();

    /**
     * @brief Returns the last recorded pointer position.
     *
     * This value is expressed in the platform's raw screen/canvas coordinate system
     * (native pixels as reported by the platform input layer). It represents the
     * most recent pointer location prior to the current update and is shared for
     * mouse, touch and pen input.
     *
     * @return Vec2 The last pointer position in screen (canvas) pixels.
     *
     * @note This position is in the platform/native screen coordinate space and
     *       has not been transformed by any world or UI layout conversions.
     *       If your code expects scaled or world coordinates, convert using the
     *       appropriate helper (for example, nativeToScreen / screenToWorld).
     */
    Vec2 getLastPointerPosition() const { return _lastPointerPosition; }

    void resetInput();

protected:
    InputSystem();

    void setInputScale(float scale);

    DelegateIter findDelegate(InputDelegate* delegate);

    /**
     *@brief Add delegate to receive IME messages.
     *@param delegate A instance implements InputDelegate delegate.
     */
    void addDelegate(InputDelegate* delegate);

    /**
     *@brief Attach the Delegate to the IME.
     *@param delegate A instance implements InputDelegate delegate.
     *@return If the old delegate can detach from the IME, and the new delegate
     *       can attach to the IME, return true, otherwise false.
     */
    bool attachDelegateWithIME(InputDelegate* delegate);

    /**
     * Detach the delegate to the IME
     *@see `attachDelegateWithIME(InputDelegate*)`
     *@param delegate  A instance implements InputDelegate delegate.
     *@return Whether the IME is detached or not.
     */
    bool detachDelegateWithIME(InputDelegate* delegate);

    /**
     *@brief Remove the delegate from the delegates which receive IME messages.
     *@param delegate A instance implements the InputDelegate delegate.
     */
    void removeDelegate(InputDelegate* delegate);

    PointerEvent* findPointerEvent(intptr_t pointerId) const;
    PointerEvent* fetchPointerEvent(intptr_t pointerId);
    void removePointerEvent(intptr_t pointerId);
    void dispatchPointerEvent(InputPhase phase, Vec2 point, const PointerInputState& state);

    // cached mouse position with inputScale applied
    Vec2 _lastPointerPosition;  // the original pointer position
    std::vector<PointerEvent*> _pointerEvents;

    PointerEvent _isolatedMoveEvent{};
    PointerEvent _scrollEvent{};

    bool _interactive{true};

    ax::IMEKeyboardNotificationInfo _cachedKeyboardNotifInfo;

    EventDispatcher* _eventDispatcher{nullptr};

    // Input scale factor:
    // - Always 1.0 on platforms with a 1:1 mapping between screen coordinates
    //   and physical pixels.
    // - On other platforms, matches _renderScale to account for DPI scaling.
    float _inputScale{1.0f};

    DelegateList _delegateList{};
    InputDelegate* _delegateWithIme{nullptr};

    static InputSystem* _instance;
};

}  // namespace ax
// end of base group
/// @}
