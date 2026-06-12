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
#include "axmol/platform/RenderViewCore.h"
#include "axmol/base/KeyboardEvent.h"
#include "axmol/base/InputSystem.h"

#include <concurrent_queue.h>
#include <string>
#include <memory>
#include <concrt.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.Core.h>

#if AX_ENABLE_GL
#    include "axmol/platform/winrt/EGLSurfaceProvider.h"
#endif

using namespace winrt;

namespace ax
{

class RenderView;
class Application;

class AX_DLL RenderView : public RenderViewCore
{
public:
    static const std::string_view EVENT_WINDOW_RESIZED;

    static RenderView* create(std::string_view viewName);
    static RenderView* createWithRect(std::string_view viewName,
                                      const Rect& rect,
                                      float zoomFactor = 1.0f,
                                      bool resizable   = false);
    static RenderView* createWithFullscreen(std::string_view viewName);

    /* override functions */
    bool isGfxContextReady() override;
    void end() override;
    void swapBuffers() override;

    Windows::Graphics::Display::DisplayOrientations getDeviceOrientation() { return _orientation; };
    Size getRenerTargetSize() const { return Size(_width, _height); }

    void setIMEKeyboardState(bool bOpen) override;
    void setIMEKeyboardState(bool bOpen, std::string_view str);

    /**
     * Hide or Show the mouse cursor if there is one.
     *
     * @param isVisible Hide or Show the mouse cursor if there is one.
     */
    void setCursorVisible(bool isVisible) override;

    bool isCursorVisible() { return _isCursorVisible; }

    winrt::agile_ref<Windows::UI::Core::CoreDispatcher> getDispatcher() const { return _dispatcher; }
    winrt::agile_ref<Windows::UI::Xaml::Controls::SwapChainPanel> getPanel() const { return _panel; }

    void registerEventHandlers();
    void registerInput();
    void syncCursorVisibility();

    void onVisibilityChanged(Windows::UI::Core::CoreWindow const& sender,
                             Windows::UI::Core::VisibilityChangedEventArgs const& args);
    void onWindowClosed(Windows::UI::Core::CoreWindow const& sender,
                        Windows::UI::Core::CoreWindowEventArgs const& args);

    void handlePointerEvent(ax::InputPhase phase, Windows::UI::Core::PointerEventArgs const& args);
    void handleKeyboardEvent(ax::InputPhase phase, Windows::UI::Core::KeyEventArgs const& args);

    AlertResult showAlertDialog(const winrt::hstring& title, const winrt::hstring& message, AlertStyle style);

    void updateOrientation(Windows::Graphics::Display::DisplayOrientations orientation);
    void updateWindowSize(float width, float height);

    void setDPI(float dpi);
    float getDPI() { return _dpi; }

    void createRenderSurface();
    void destroyRenderSurface();
    void recoverFromLostDevice();
    void terminateApp();
    void makeSurfaceCurrent();
    bool swapSurfaceBuffers();
    // static function
    /**
    @brief    get the shared main open gl window
    */
    static RenderView* sharedRenderView();

    void* getNativeWindow() const override;
    SurfaceHandle getNativeDisplay() const override;
    WindowPlatform getWindowPlatform() const override { return WindowPlatform::CoreWindow; }

    void setViewName(std::string_view viewName) override;

protected:
    RenderView();
    ~RenderView() override;

    void showKeyboard(const winrt::hstring& text);
    void hideKeyboard(const winrt::hstring& text);

    void onTextChanged(const Windows::Foundation::IInspectable& sender,
                       Windows::UI::Xaml::Controls::TextChangedEventArgs const& args);
    void onTextCompositionStarted(Windows::UI::Xaml::Controls::TextBox,
                                  Windows::UI::Xaml::Controls::TextCompositionStartedEventArgs const& args);
    void onTextCompositionEnded(Windows::UI::Xaml::Controls::TextBox,
                                Windows::UI::Xaml::Controls::TextCompositionEndedEventArgs const& args);

    AX_DISALLOW_COPY_AND_ASSIGN(RenderView);

    bool initWithRect(std::string_view viewName, const Rect& rect, float frameZoomFactor);

    void handleWindowResized();
    void updateRenderScale();

    void onPanelSizeChanged(Windows::Foundation::IInspectable const& sender,
                            Windows::UI::Xaml::RoutedEventArgs const& args);
    void onPointerPressed(Windows::Foundation::IInspectable const& sender,
                          Windows::UI::Core::PointerEventArgs const& args);
    void onPointerMoved(Windows::Foundation::IInspectable const& sender,
                        Windows::UI::Core::PointerEventArgs const& args);
    void onPointerReleased(Windows::Foundation::IInspectable const& sender,
                           Windows::UI::Core::PointerEventArgs const& args);
    void onPointerWheelChanged(Windows::Foundation::IInspectable const& sender,
                               Windows::UI::Core::PointerEventArgs const& args);
    void onKeyPressed(Windows::UI::Core::CoreWindow const& sender, Windows::UI::Core::KeyEventArgs const& args);
    void onKeyReleased(Windows::UI::Core::CoreWindow const& sender, Windows::UI::Core::KeyEventArgs const& args);
    void onCharacterReceived(Windows::UI::Core::CoreWindow const& sender,
                             Windows::UI::Core::CharacterReceivedEventArgs const& args);
    void onOrientationChanged(Windows::Graphics::Display::DisplayInformation const& sender,
                              Windows::Foundation::IInspectable const& args);
    void onDpiChanged(Windows::Graphics::Display::DisplayInformation const& sender,
                      Windows::Foundation::IInspectable const& args);
#if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP) || _MSC_VER >= 1900
    void onBackButtonPressed(Windows::Foundation::IInspectable const& sender,
                             Windows::UI::Core::BackRequestedEventArgs const& args);
#endif

    static RenderView* s_renderView;

    bool _isCursorVisible;

    float _width;
    float _height;
    float _dpi;
    Windows::Graphics::Display::DisplayOrientations _orientation;

    bool _initialized;
    winrt::agile_ref<Windows::UI::Core::CoreDispatcher> _dispatcher;
    winrt::agile_ref<Windows::UI::Xaml::Controls::SwapChainPanel> _panel;
    Windows::UI::Core::CoreIndependentInputSource _coreInput{nullptr};
    Windows::Foundation::IAsyncAction _inputLoopWorker{nullptr};

#if AX_ENABLE_GL
    std::unique_ptr<EGLSurfaceProvider> _eglSurfaceProvider;
    EGLSurface _eglSurface{EGL_NO_SURFACE};
    Concurrency::critical_section _eglSurfaceCriticalSection{};
#endif

    // keyboard support
    std::unordered_map<int, KeyboardEvent::KeyCode> _keyCodeMap;
    Windows::UI::Xaml::Controls::TextBox _textBox{nullptr};
    bool _useInputMethod{false};

    bool _updateScheduled{false};
    bool _cursorVisible{true};
};

}  // namespace ax
