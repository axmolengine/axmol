/****************************************************************************
Copyright (c) 2010 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include <string>
#include <string_view>
#include "axmol/math/Math.h"
#include "axmol/base/KeyboardEvent.h"

/**
 * @addtogroup base
 * @{
 */
namespace ax
{

/**
 * A static global empty std::string install.
 */
extern const std::string AX_DLL STD_STRING_EMPTY;

/**
 * @enum EditAction
 * @brief Edit actions that the platform view may query or request.
 *
 * @note Keep values stable across builds; new actions can be appended.
 */
enum class EditAction : int
{
    Copy = 0, /**< Copy the current selection to the clipboard. */
    Cut,      /**< Copy the current selection to the clipboard and delete it. */
    Paste,    /**< Insert clipboard contents at the caret. */
    SelectAll /**< Select all editable content in the current field. */
};

/**
 * Keyboard notification event type.
 */
struct IMEKeyboardNotificationInfo
{
    // The soft keyboard geometry frame transformed into World Space (Origin at Bottom-Left)
    ax::Rect keyboardFrame;

    // The soft keyboard animation duration in seconds
    float duration;
};

/**
 *@brief Input delegate.
 */
class AX_DLL InputDelegate
{
public:
    /**
     * Default constructor.
     * @lua NA
     */
    virtual ~InputDelegate();

    /**
     * Default destructor.
     * @lua NA
     */
    virtual bool attachWithIME();

    /**
     * Determine whether the IME is detached or not.
     * @lua NA
     */
    virtual bool detachWithIME();

protected:
    friend class InputSystem;

    /**
     * @brief IME hit-test.
     * @param location Touch point in axmol world coordinates.
     * @return true to keep the IME active; false otherwise.
     */
    virtual bool hitTestWithIME(const Vec2& location) { return false; }

    /**
    @brief    Decide if the delegate instance is ready to receive an IME message.

    Called by InputSystem.
    * @lua NA
    */
    virtual bool canAttachWithIME() const { return false; }
    /**
    @brief    When the delegate detaches from the IME, this method is called by InputSystem.
    * @lua NA
    */
    virtual void didAttachWithIME() {}

    /**
    @brief    Decide if the delegate instance can stop receiving IME messages.
    * @lua NA
    */
    virtual bool canDetachWithIME() const { return false; }

    /**
    @brief    When the delegate detaches from the IME, this method is called by InputSystem.
    * @lua NA
    */
    virtual void didDetachWithIME() {}

    /**
    @brief    Called by InputSystem when text input received from the IME.
    * @lua NA
    */
    virtual void insertText(std::string_view /*text*/) {}

    /**
    @brief    Called by InputSystem when the preedit text is updated.
    * @lua NA
    */
    virtual void updatePreeditText(std::string_view /*text*/, int /*caretPos*/) {}

    /**
    @brief    Called by InputSystem after the user clicks the backward key.
    * @lua NA
    */
    virtual void deleteBackward(unsigned int numChars) {}

    /**
    @brief    Called by InputSystem after the user press control key.
    * @lua NA
    */
    virtual void controlKey(KeyboardEvent::KeyCode /*keyCode*/) {}

    //////////////////////////////////////////////////////////////////////////
    // keyboard show/hide notification
    //////////////////////////////////////////////////////////////////////////
    /**
     * @lua NA
     */
    virtual void keyboardWillShow(IMEKeyboardNotificationInfo& /*info*/) {}
    /**
     * @lua NA
     */
    virtual void keyboardDidShow(IMEKeyboardNotificationInfo& /*info*/) {}
    /**
     * @lua NA
     */
    virtual void keyboardWillHide(IMEKeyboardNotificationInfo& /*info*/) {}
    /**
     * @lua NA
     */
    virtual void keyboardDidHide(IMEKeyboardNotificationInfo& /*info*/) {}

    //////////////////////////////////////////////////////////////////////////
    // clipboard / edit actions
    //////////////////////////////////////////////////////////////////////////

    /**
     * @brief Execute the requested edit action.
     *
     * Called on the main/UI thread when the user selects a menu item. The delegate
     * should perform the actual behavior (clipboard I/O, model updates, deletion,
     * insertion, selection changes, etc.). Implementations that need to do heavy
     * work should quickly schedule asynchronous work and return to avoid blocking
     * the UI.
     *
     * @param action The edit action to perform.
     */
    virtual void performEditAction(EditAction action) {}

protected:
    /**
     * @lua NA
     */
    InputDelegate();
};

}  // namespace ax
// end of base group
/// @}
