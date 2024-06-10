/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.
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

#ifndef __WINRT_INPUT_EVENT__
#define __WINRT_INPUT_EVENT__

#include "platform/PlatformMacros.h"
#include "platform/winrt/InputEventTypes.h"
#include "base/Types.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
using namespace winrt;

NS_AX_BEGIN

enum PointerEventType
{
    PointerPressed,
    PointerMoved,
    PointerReleased,
    MousePressed,
    MouseMoved,
    MouseReleased,
    MouseWheelChanged,
};
enum MouseButton
{
    Left = 0,
    Right = 1,
    Middle = 2,
    None
};

class AX_DLL InputEvent
{
public:
    InputEvent() {};
    virtual ~InputEvent() {};
    virtual void execute() = 0;
};


class AX_DLL AccelerometerEvent : public InputEvent
{
public:
    AccelerometerEvent(const ax::Acceleration& event);
    virtual void execute();

private:
    ax::Acceleration m_event;
};

class AX_DLL PointerEvent : public InputEvent
{
public:
    PointerEvent(PointerEventType type, const Windows::UI::Core::PointerEventArgs& args);
    virtual void execute();


private:
    PointerEventType m_type;
    Windows::UI::Core::PointerEventArgs m_args;
};

class AX_DLL KeyboardEvent : public InputEvent

{
public:
    KeyboardEvent(AxmolKeyEvent type);
    KeyboardEvent(AxmolKeyEvent type, const winrt::hstring& text);
    virtual void execute();

private:
    AxmolKeyEvent m_type;
    winrt::hstring m_text;
};

enum WinRTKeyboardEventType
{
	KeyPressed,
	KeyReleased,
};

class AX_DLL WinRTKeyboardEvent : public InputEvent
{
public:
	WinRTKeyboardEvent(WinRTKeyboardEventType type, const Windows::UI::Core::KeyEventArgs& args);
	virtual void execute();

private:
	WinRTKeyboardEventType m_type;
    Windows::UI::Core::KeyEventArgs m_key;
};


class AX_DLL BackButtonEvent : public InputEvent
{
public:
    BackButtonEvent();
    virtual void execute();
};

class AX_DLL CustomInputEvent : public InputEvent
{
public:
    CustomInputEvent(const std::function<void()>&);
    virtual void execute();
private:
    std::function<void()> m_fun;
};

class UIEditBoxEvent : public ax::InputEvent
{
public:
    UIEditBoxEvent(const Windows::Foundation::IInspectable& sender,
                   const winrt::hstring& text,
                   const winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&>& handle);

    virtual void execute();

protected:
    Windows::Foundation::IInspectable m_sender;
    winrt::hstring m_text;
    winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&> m_handler;
};

struct EndEventArgs
{
public:
    EndEventArgs(int action, const winrt::hstring& text) : m_text(text), m_action(action) {}
    int GetAction() const { return m_action; }
    const winrt::hstring& GetText() const { return m_text; }

private:
    int m_action;
    winrt::hstring m_text;
};

class UIEditBoxEndEvent : public ax::InputEvent
{
public:
    UIEditBoxEndEvent(const Windows::Foundation::IInspectable& sender,
                      const winrt::hstring& text,
                      int action,
                      winrt::delegate<Windows::Foundation::IInspectable const&, EndEventArgs const&>& handle);
  virtual void execute();
protected:
  int m_action;
  Windows::Foundation::IInspectable m_sender;
  winrt::hstring m_text;
  winrt::delegate<Windows::Foundation::IInspectable const&, EndEventArgs const&> m_handler;
};

NS_AX_END

#endif // #ifndef __WINRT_INPUT_EVENT__

