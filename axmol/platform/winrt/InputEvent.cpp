/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.
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

#include "axmol/platform/winrt/InputEvent.h"
#include "axmol/platform/winrt/WinRTUtils.h"
#include "axmol/platform/winrt/RenderViewImpl-winrt.h"
#include "axmol/base/EventAcceleration.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/IMEDispatcher.h"

namespace ax
{

AccelerometerEvent::AccelerometerEvent(const Acceleration& event) : m_event(event) {}

void AccelerometerEvent::execute()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    ax::EventAcceleration accEvent(m_event);
    dispatcher->dispatchEvent(&accEvent);
}

PointerEvent::PointerEvent(PointerEventType type, const Windows::UI::Core::PointerEventArgs& args)
    : m_type(type), m_args(args)
{}

void PointerEvent::execute()
{
    switch (m_type)
    {
    case PointerEventType::PointerPressed:
        RenderViewImpl::sharedRenderView()->OnPointerPressed(m_args);
        break;
    case PointerEventType::PointerMoved:
        RenderViewImpl::sharedRenderView()->OnPointerMoved(m_args);
        break;
    case PointerEventType::PointerReleased:
        RenderViewImpl::sharedRenderView()->OnPointerReleased(m_args);
        break;
    case ax::MousePressed:
        RenderViewImpl::sharedRenderView()->OnMousePressed(m_args);
        break;
    case ax::MouseMoved:
        RenderViewImpl::sharedRenderView()->OnMouseMoved(m_args);
        break;
    case ax::MouseReleased:
        RenderViewImpl::sharedRenderView()->OnMouseReleased(m_args);
        break;
    case ax::MouseWheelChanged:
        RenderViewImpl::sharedRenderView()->OnMouseWheelChanged(m_args);
        break;
    }
}

KeyboardEvent::KeyboardEvent(AxmolKeyEvent type) : m_type(type), m_text() {}

KeyboardEvent::KeyboardEvent(AxmolKeyEvent type, const winrt::hstring& text) : m_type(type), m_text(text) {}

void KeyboardEvent::execute()
{
    switch (m_type)
    {
    case AxmolKeyEvent::Text:
    {
        std::string utf8String = PlatformStringToString(m_text);
        IMEDispatcher::sharedDispatcher()->dispatchInsertText(utf8String.c_str(), utf8String.size());
        break;
    }

    default:
        switch (m_type)
        {
        case AxmolKeyEvent::Escape:
            // Director::getInstance()()->getKeypadDispatcher()->dispatchKeypadMSG(kTypeBackClicked);
            break;
        case AxmolKeyEvent::Back:
            IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward(1);
            break;
        case AxmolKeyEvent::Enter:
            IMEDispatcher::sharedDispatcher()->dispatchInsertText("\n", 1);
            break;
        default:
            break;
        }
        break;
    }
}

WinRTKeyboardEvent::WinRTKeyboardEvent(WinRTKeyboardEventType type, const Windows::UI::Core::KeyEventArgs& args)
    : m_type(type), m_key(args)
{}

void WinRTKeyboardEvent::execute()
{
    RenderViewImpl::sharedRenderView()->OnWinRTKeyboardEvent(m_type, m_key);
}

BackButtonEvent::BackButtonEvent() {}

void BackButtonEvent::execute()
{
    RenderViewImpl::sharedRenderView()->OnBackKeyPress();
}

CustomInputEvent::CustomInputEvent(const std::function<void()>& fun) : m_fun(fun) {}

void CustomInputEvent::execute()
{
    m_fun();
}

UIEditBoxEvent::UIEditBoxEvent(
    const Windows::Foundation::IInspectable& sender,
    const winrt::hstring& text,
    const winrt::delegate<Windows::Foundation::IInspectable const&, winrt::hstring const&>& handle)
    : m_sender(sender), m_text(text), m_handler(handle)
{}

void UIEditBoxEvent::execute()
{
    if (m_handler)
    {
        m_handler(m_sender, m_text);
    }
}

UIEditBoxEndEvent::UIEditBoxEndEvent(
    const Windows::Foundation::IInspectable& sender,
    const winrt::hstring& text,
    int action,
    winrt::delegate<Windows::Foundation::IInspectable const&, EndEventArgs const&>& handle)
    : m_sender(sender), m_text(text), m_action(action), m_handler(handle)
{}

void UIEditBoxEndEvent::execute()
{
    if (m_handler)
    {
        EndEventArgs args(m_action, m_text);
        m_handler(m_sender, args);
    }
}

}  // namespace ax
