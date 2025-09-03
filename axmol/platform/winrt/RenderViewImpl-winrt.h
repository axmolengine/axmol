/****************************************************************************
Copyright (c) 2010 cocos2d-x.org
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

#pragma once

#include "axmol/platform/winrt/StdC-winrt.h"
#include "axmol/platform/Common.h"
#include "axmol/platform/winrt/Keyboard-winrt.h"
#include "axmol/platform/RenderView.h"
#include "axmol/base/EventKeyboard.h"
#include "axmol/base/EventMouse.h"

#include <concurrent_queue.h>
#include <string>
#include <memory>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>

using namespace winrt;

class AxmolRenderer;

namespace ax
{

class RenderViewImpl;

class AX_DLL RenderViewImpl : public RenderView
{
    friend class ::AxmolRenderer;

public:
    static RenderViewImpl* create(std::string_view viewName);
    static RenderViewImpl* createWithRect(std::string_view viewName,
                                          const Rect& rect,
                                          float frameZoomFactor = 1.0f,
                                          bool resizable        = false);
    static RenderViewImpl* createWithFullScreen(std::string_view viewName);

    /* override functions */
    bool isGfxContextReady() override;
    void end() override;
    void swapBuffers() override;

    Windows::Graphics::Display::DisplayOrientations getDeviceOrientation() { return m_orientation; };
    Size getRenerTargetSize() const { return Size(m_width, m_height); }

    void setIMEKeyboardState(bool bOpen) override;
    void setIMEKeyboardState(bool bOpen, std::string_view str);

    /**
     * Hide or Show the mouse cursor if there is one.
     *
     * @param isVisible Hide or Show the mouse cursor if there is one.
     */
    void setCursorVisible(bool isVisible) override;

    bool isCursorVisible() { return _isCursorVisible; }

    void setDispatcher(winrt::agile_ref<Windows::UI::Core::CoreDispatcher> dispatcher);
    winrt::agile_ref<Windows::UI::Core::CoreDispatcher> getDispatcher() const { return m_dispatcher; }

    void setPanel(winrt::agile_ref<Windows::UI::Xaml::Controls::Panel> panel);
    winrt::agile_ref<Windows::UI::Xaml::Controls::Panel> getPanel() { return m_panel; }

    void OnPointerPressed(Windows::UI::Core::PointerEventArgs const& args);
    void OnPointerMoved(Windows::UI::Core::PointerEventArgs const& args);
    void OnPointerReleased(Windows::UI::Core::PointerEventArgs const& args);

    void OnMousePressed(Windows::UI::Core::PointerEventArgs const& args);
    void OnMouseMoved(Windows::UI::Core::PointerEventArgs const& args);
    void OnMouseReleased(Windows::UI::Core::PointerEventArgs const& args);
    void OnMouseWheelChanged(Windows::UI::Core::PointerEventArgs const& args);

    void OnWinRTKeyboardEvent(WinRTKeyboardEventType type, Windows::UI::Core::KeyEventArgs const& args);

    void OnPointerPressed(Windows::UI::Core::CoreWindow const& sender, Windows::UI::Core::PointerEventArgs const& args);
    void OnPointerWheelChanged(Windows::UI::Core::CoreWindow const&, Windows::UI::Core::PointerEventArgs const& args);
    void OnPointerMoved(Windows::UI::Core::CoreWindow const&, Windows::UI::Core::PointerEventArgs const& args);
    void OnPointerReleased(Windows::UI::Core::CoreWindow const& sender,
                           Windows::UI::Core::PointerEventArgs const& args);
    void OnVisibilityChanged(Windows::UI::Core::CoreWindow const& sender,
                             Windows::UI::Core::VisibilityChangedEventArgs const& args);
    void OnWindowClosed(Windows::UI::Core::CoreWindow const& sender,
                        Windows::UI::Core::CoreWindowEventArgs const& args);
    void OnResuming(Windows::Foundation::IInspectable const& sender);
    void OnSuspending(Windows::Foundation::IInspectable const& sender,
                      Windows::ApplicationModel::SuspendingEventArgs const& args);
    void OnBackKeyPress();
    bool AppShouldExit();
    void BackButtonListener(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);

    void QueueBackKeyPress();
    void QueuePointerEvent(PointerEventType type, Windows::UI::Core::PointerEventArgs const& args);
    void QueueWinRTKeyboardEvent(WinRTKeyboardEventType type, Windows::UI::Core::KeyEventArgs const& args);
    void QueueEvent(std::shared_ptr<InputEvent>& event);

    AlertResult ShowAlertDialog(const winrt::hstring& title, const winrt::hstring& message, AlertStyle style);

    int Run();
    void Render();

    float getFrameZoomFactor();
    void centerWindow();

    void UpdateOrientation(Windows::Graphics::Display::DisplayOrientations orientation);
    void UpdateForWindowSizeChange(float width, float height);

    void SetDPI(float dpi) { m_dpi = dpi; }
    float GetDPI() { return m_dpi; }
    // static function
    /**
    @brief    get the shared main open gl window
    */
    static RenderViewImpl* sharedRenderView();

    void ProcessEvents();

    void queueOperation(AsyncOperation op, void* param) override;

    void SetQueueOperationCb(std::function<void(AsyncOperation, void*)> cb);

    PresentTarget* getPresentTarget() const override { return &m_presentTarget; }

protected:
    RenderViewImpl();
    ~RenderViewImpl() override;

    bool initWithRect(std::string_view viewName, const Rect& rect, float frameZoomFactor);
    bool initWithFullScreen(std::string_view viewName);

    /*
     * Set zoom factor for frame. This method is for debugging big resolution (e.g.new ipad) app on desktop.
     */
    void setFrameZoomFactor(float zoomFactor);

    inline bool isRetina() { return _isRetina; };

    float _frameZoomFactor;
    bool _supportTouch;
    bool _isRetina;
    bool _isCursorVisible;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(RenderViewImpl);

    void OnRendering();
    void UpdateWindowSize();

    ax::Vec2 TransformToOrientation(Windows::Foundation::Point const& point);
    ax::Vec2 GetPoint(Windows::UI::Core::PointerEventArgs const& args);

    Windows::Foundation::Rect m_windowBounds;
    winrt::event_token m_eventToken;
    Windows::Foundation::Point m_lastPoint;

    float m_width;
    float m_height;
    float m_dpi;
    Windows::Graphics::Display::DisplayOrientations m_orientation;
    Windows::Foundation::Rect m_keyboardRect;

    bool m_lastPointValid;
    bool m_windowClosed;
    bool m_windowVisible;
    // PointerReleased for mouse not send button id, need save in PointerPressed last button
    EventMouse::MouseButton _lastMouseButtonPressed;

    bool m_running;
    bool m_initialized;
    bool m_appShouldExit;

    mutable PresentTarget m_presentTarget;

    Concurrency::concurrent_queue<std::shared_ptr<InputEvent>> mInputEvents;

    std::function<void(AsyncOperation, void*)> mQueueOperationCb;

    winrt::agile_ref<Windows::UI::Core::CoreDispatcher> m_dispatcher;
    winrt::agile_ref<Windows::UI::Xaml::Controls::Panel> m_panel;

    KeyBoardWinRT m_keyboard;

    ax::EventListenerKeyboard* m_backButtonListener;
};

}  // namespace ax
