/****************************************************************************
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

#ifndef __TEXT_INPUT_TEST_H__
#define __TEXT_INPUT_TEST_H__

#include "../BaseTest.h"

class KeyboardNotificationLayer;

DEFINE_TEST_SUITE(TextInputTests);

//////////////////////////////////////////////////////////////////////////
// KeyboardNotificationLayer for test IME keyboard notification.
//////////////////////////////////////////////////////////////////////////

class KeyboardNotificationLayer : public TestCase, public ax::IMEDelegate
{
public:
    KeyboardNotificationLayer();
    virtual std::string title() const override;
    virtual void onClickTrackNode(bool bClicked, const ax::Vec2& touchPos) = 0;

    virtual void keyboardWillShow(ax::IMEKeyboardNotificationInfo& info) override;

    bool onTouchBegan(ax::Touch* touch, ax::Event* event);
    void onTouchEnded(ax::Touch* touch, ax::Event* event);

protected:
    ax::Node* _trackNode;
    ax::Vec2 _beginPos;
};

//////////////////////////////////////////////////////////////////////////
// TextFieldTTFDefaultTest for test TextFieldTTF default behavior.
//////////////////////////////////////////////////////////////////////////

class TextFieldTTFDefaultTest : public KeyboardNotificationLayer
{
public:
    CREATE_FUNC(TextFieldTTFDefaultTest);
    // KeyboardNotificationLayer
    virtual std::string subtitle() const override;
    virtual void onClickTrackNode(bool bClicked, const ax::Vec2& touchPos) override;

    // Layer
    virtual void onEnter() override;
};

//////////////////////////////////////////////////////////////////////////
// TextFieldTTFActionTest
//////////////////////////////////////////////////////////////////////////

class TextFieldTTFActionTest : public KeyboardNotificationLayer, public ax::TextFieldDelegate
{
    ax::TextFieldTTF* _textField;
    ax::Action* _textFieldAction;
    bool _action;
    size_t _charLimit;  // the textfield max char limit

public:
    CREATE_FUNC(TextFieldTTFActionTest);
    void callbackRemoveNodeWhenDidAction(Node* node);

    // KeyboardNotificationLayer
    virtual std::string subtitle() const override;
    virtual void onClickTrackNode(bool bClicked, const ax::Vec2& touchPos) override;

    // Layer
    virtual void onEnter() override;
    virtual void onExit() override;

    // TextFieldDelegate
    virtual bool onTextFieldAttachWithIME(ax::TextFieldTTF* sender) override;
    virtual bool onTextFieldDetachWithIME(ax::TextFieldTTF* sender) override;
    virtual bool onTextFieldInsertText(ax::TextFieldTTF* sender, const char* text, size_t nLen) override;
    virtual bool onTextFieldDeleteBackward(ax::TextFieldTTF* sender, const char* delText, size_t nLen) override;
    virtual bool onDraw(ax::TextFieldTTF* sender);
};

//////////////////////////////////////////////////////////////////////////
// TextFieldTTFSecureTextEntryTest for test TextFieldTTF SecureTextEntry.
//////////////////////////////////////////////////////////////////////////

class TextFieldTTFSecureTextEntryTest : public TextFieldTTFDefaultTest
{
public:
    CREATE_FUNC(TextFieldTTFSecureTextEntryTest);

    virtual std::string subtitle() const override;
    // Layer
    virtual void onEnter() override;
};

//////////////////////////////////////////////////////////////////////////
// TextFieldTTSetCursorFromPoint for test TextFieldTTF setCursorFromPoint.
//////////////////////////////////////////////////////////////////////////

class TextFieldTTSetCursorFromPoint : public KeyboardNotificationLayer
{
public:
    CREATE_FUNC(TextFieldTTSetCursorFromPoint);
    // KeyboardNotificationLayer
    virtual std::string subtitle() const override;
    virtual void onClickTrackNode(bool bClicked, const ax::Vec2& touchPos) override;

    // Layer
    virtual void onEnter() override;
};
#endif  // __TEXT_INPUT_TEST_H__
