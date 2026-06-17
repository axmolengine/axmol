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

#include "axmol/platform/winrt/RenderView-winrt.h"
#include "axmol/base/Macros.h"
#include "axmol/base/Director.h"
#include "axmol/base/PointerEvent.h"
#include "axmol/base/InputSystem.h"
#include "axmol/platform/Device.h"
#include "axmol/platform/winrt/Application-winrt.h"
#include "axmol/platform/winrt/WinRTUtils.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/rhi/DriverContext.h"
#include <future>

#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Popups.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>

#include <winrt/Windows.System.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.UI.Input.Core.h>

#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.ApplicationModel.Core.h>

namespace ax
{

using namespace Windows::System;
using namespace Windows::System::Threading;
using namespace Windows::Devices::Input;
using namespace Windows::Graphics::Display;
using namespace Windows::Foundation::Metadata;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Input::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;

struct keyCodeItem
{
    int key;
    KeyboardEvent::KeyCode keyCode;
};

// http://www.kbdedit.com/manual/low_level_vk_list.html
// https://msdn.microsoft.com/library/windows/apps/windows.system.virtualkey.aspx

static constexpr keyCodeItem g_keyCodeStructArray[] = {
    /* The unknown key */
    {(int)VirtualKey::None, KeyboardEvent::KeyCode::KEY_NONE},
    /* Printable keys */
    {(int)VirtualKey::Space, KeyboardEvent::KeyCode::KEY_SPACE},
    {(int)VK_OEM_7, KeyboardEvent::KeyCode::KEY_APOSTROPHE},
    {(int)VK_OEM_COMMA, KeyboardEvent::KeyCode::KEY_COMMA},
    {(int)VK_OEM_MINUS, KeyboardEvent::KeyCode::KEY_MINUS},
    {(int)VK_OEM_PERIOD, KeyboardEvent::KeyCode::KEY_PERIOD},
    {(int)VK_OEM_2, KeyboardEvent::KeyCode::KEY_SLASH},
    {(int)VK_OEM_3, KeyboardEvent::KeyCode::KEY_TILDE},

    {(int)VirtualKey::Number0, KeyboardEvent::KeyCode::KEY_0},
    {(int)VirtualKey::Number1, KeyboardEvent::KeyCode::KEY_1},
    {(int)VirtualKey::Number2, KeyboardEvent::KeyCode::KEY_2},
    {(int)VirtualKey::Number3, KeyboardEvent::KeyCode::KEY_3},
    {(int)VirtualKey::Number4, KeyboardEvent::KeyCode::KEY_4},
    {(int)VirtualKey::Number5, KeyboardEvent::KeyCode::KEY_5},
    {(int)VirtualKey::Number6, KeyboardEvent::KeyCode::KEY_6},
    {(int)VirtualKey::Number7, KeyboardEvent::KeyCode::KEY_7},
    {(int)VirtualKey::Number8, KeyboardEvent::KeyCode::KEY_8},
    {(int)VirtualKey::Number9, KeyboardEvent::KeyCode::KEY_9},
    {(int)VK_OEM_1, KeyboardEvent::KeyCode::KEY_SEMICOLON},
    {(int)VK_OEM_PLUS, KeyboardEvent::KeyCode::KEY_EQUAL},
    {(int)VirtualKey::A, KeyboardEvent::KeyCode::KEY_A},
    {(int)VirtualKey::B, KeyboardEvent::KeyCode::KEY_B},
    {(int)VirtualKey::C, KeyboardEvent::KeyCode::KEY_C},
    {(int)VirtualKey::D, KeyboardEvent::KeyCode::KEY_D},
    {(int)VirtualKey::E, KeyboardEvent::KeyCode::KEY_E},
    {(int)VirtualKey::F, KeyboardEvent::KeyCode::KEY_F},
    {(int)VirtualKey::G, KeyboardEvent::KeyCode::KEY_G},
    {(int)VirtualKey::H, KeyboardEvent::KeyCode::KEY_H},
    {(int)VirtualKey::I, KeyboardEvent::KeyCode::KEY_I},
    {(int)VirtualKey::J, KeyboardEvent::KeyCode::KEY_J},
    {(int)VirtualKey::K, KeyboardEvent::KeyCode::KEY_K},
    {(int)VirtualKey::L, KeyboardEvent::KeyCode::KEY_L},
    {(int)VirtualKey::M, KeyboardEvent::KeyCode::KEY_M},
    {(int)VirtualKey::N, KeyboardEvent::KeyCode::KEY_N},
    {(int)VirtualKey::O, KeyboardEvent::KeyCode::KEY_O},
    {(int)VirtualKey::P, KeyboardEvent::KeyCode::KEY_P},
    {(int)VirtualKey::Q, KeyboardEvent::KeyCode::KEY_Q},
    {(int)VirtualKey::R, KeyboardEvent::KeyCode::KEY_R},
    {(int)VirtualKey::S, KeyboardEvent::KeyCode::KEY_S},
    {(int)VirtualKey::T, KeyboardEvent::KeyCode::KEY_T},
    {(int)VirtualKey::U, KeyboardEvent::KeyCode::KEY_U},
    {(int)VirtualKey::V, KeyboardEvent::KeyCode::KEY_V},
    {(int)VirtualKey::W, KeyboardEvent::KeyCode::KEY_W},
    {(int)VirtualKey::X, KeyboardEvent::KeyCode::KEY_X},
    {(int)VirtualKey::Y, KeyboardEvent::KeyCode::KEY_Y},
    {(int)VirtualKey::Z, KeyboardEvent::KeyCode::KEY_Z},
    {VK_OEM_4, KeyboardEvent::KeyCode::KEY_LEFT_BRACKET},
    {VK_OEM_5, KeyboardEvent::KeyCode::KEY_BACK_SLASH},
    {VK_OEM_6, KeyboardEvent::KeyCode::KEY_RIGHT_BRACKET},
    //	{ GLFW_KEY_GRAVE_ACCENT			, KeyboardEvent::KeyCode::KEY_GRAVE },

    /* Function keys */
    {(int)VirtualKey::Escape, KeyboardEvent::KeyCode::KEY_ESCAPE},
    {(int)VirtualKey::Enter, KeyboardEvent::KeyCode::KEY_ENTER},
    {(int)VirtualKey::Tab, KeyboardEvent::KeyCode::KEY_TAB},
    {(int)VirtualKey::Back, KeyboardEvent::KeyCode::KEY_BACKSPACE},
    {(int)VirtualKey::Insert, KeyboardEvent::KeyCode::KEY_INSERT},
    {(int)VirtualKey::Delete, KeyboardEvent::KeyCode::KEY_DELETE},
    {(int)VirtualKey::Right, KeyboardEvent::KeyCode::KEY_RIGHT_ARROW},
    {(int)VirtualKey::Left, KeyboardEvent::KeyCode::KEY_LEFT_ARROW},
    {(int)VirtualKey::Down, KeyboardEvent::KeyCode::KEY_DOWN_ARROW},
    {(int)VirtualKey::Up, KeyboardEvent::KeyCode::KEY_UP_ARROW},
    {VK_PRIOR, KeyboardEvent::KeyCode::KEY_PG_UP},
    {VK_NEXT, KeyboardEvent::KeyCode::KEY_PG_DOWN},
    {VK_HOME, KeyboardEvent::KeyCode::KEY_HOME},
    {VK_END, KeyboardEvent::KeyCode::KEY_END},
    {VK_CAPITAL, KeyboardEvent::KeyCode::KEY_CAPS_LOCK},
    {VK_SCROLL, KeyboardEvent::KeyCode::KEY_SCROLL_LOCK},
    {VK_NUMLOCK, KeyboardEvent::KeyCode::KEY_NUM_LOCK},
    {VK_SNAPSHOT, KeyboardEvent::KeyCode::KEY_PRINT},
    {VK_PAUSE, KeyboardEvent::KeyCode::KEY_PAUSE},
    {(int)VirtualKey::F1, KeyboardEvent::KeyCode::KEY_F1},
    {(int)VirtualKey::F2, KeyboardEvent::KeyCode::KEY_F2},
    {(int)VirtualKey::F3, KeyboardEvent::KeyCode::KEY_F3},
    {(int)VirtualKey::F4, KeyboardEvent::KeyCode::KEY_F4},
    {(int)VirtualKey::F5, KeyboardEvent::KeyCode::KEY_F5},
    {(int)VirtualKey::F6, KeyboardEvent::KeyCode::KEY_F6},
    {(int)VirtualKey::F7, KeyboardEvent::KeyCode::KEY_F7},
    {(int)VirtualKey::F8, KeyboardEvent::KeyCode::KEY_F8},
    {(int)VirtualKey::F9, KeyboardEvent::KeyCode::KEY_F9},
    {(int)VirtualKey::F10, KeyboardEvent::KeyCode::KEY_F10},
    {(int)VirtualKey::F11, KeyboardEvent::KeyCode::KEY_F11},
    {(int)VirtualKey::F12, KeyboardEvent::KeyCode::KEY_F12},
    {(int)VirtualKey::F13, KeyboardEvent::KeyCode::KEY_NONE},
    {(int)VirtualKey::F14, KeyboardEvent::KeyCode::KEY_NONE},
    {(int)VirtualKey::F15, KeyboardEvent::KeyCode::KEY_NONE},
    {(int)VirtualKey::F16, KeyboardEvent::KeyCode::KEY_NONE},
    {(int)VirtualKey::F17, KeyboardEvent::KeyCode::KEY_NONE},
    {(int)VirtualKey::F18, KeyboardEvent::KeyCode::KEY_NONE},
    {(int)VirtualKey::F19, KeyboardEvent::KeyCode::KEY_NONE},
    {(int)VirtualKey::F20, KeyboardEvent::KeyCode::KEY_NONE},
    {(int)VirtualKey::F21, KeyboardEvent::KeyCode::KEY_NONE},
    {(int)VirtualKey::F22, KeyboardEvent::KeyCode::KEY_NONE},
    {(int)VirtualKey::F23, KeyboardEvent::KeyCode::KEY_NONE},
    {(int)VirtualKey::F24, KeyboardEvent::KeyCode::KEY_NONE},

    {(int)VirtualKey::NumberPad0, KeyboardEvent::KeyCode::KEY_0},
    {(int)VirtualKey::NumberPad1, KeyboardEvent::KeyCode::KEY_1},
    {(int)VirtualKey::NumberPad2, KeyboardEvent::KeyCode::KEY_2},
    {(int)VirtualKey::NumberPad3, KeyboardEvent::KeyCode::KEY_3},
    {(int)VirtualKey::NumberPad4, KeyboardEvent::KeyCode::KEY_4},
    {(int)VirtualKey::NumberPad5, KeyboardEvent::KeyCode::KEY_5},
    {(int)VirtualKey::NumberPad6, KeyboardEvent::KeyCode::KEY_6},
    {(int)VirtualKey::NumberPad7, KeyboardEvent::KeyCode::KEY_7},
    {(int)VirtualKey::NumberPad8, KeyboardEvent::KeyCode::KEY_8},
    {(int)VirtualKey::NumberPad9, KeyboardEvent::KeyCode::KEY_9},
#if 0
    { GLFW_KEY_KP_1, KeyboardEvent::KeyCode::KEY_1 },
    { GLFW_KEY_KP_2, KeyboardEvent::KeyCode::KEY_2 },
    { GLFW_KEY_KP_3, KeyboardEvent::KeyCode::KEY_3 },
    { GLFW_KEY_KP_4, KeyboardEvent::KeyCode::KEY_4 },
    { GLFW_KEY_KP_5, KeyboardEvent::KeyCode::KEY_5 },
    { GLFW_KEY_KP_6, KeyboardEvent::KeyCode::KEY_6 },
    { GLFW_KEY_KP_7, KeyboardEvent::KeyCode::KEY_7 },
    { GLFW_KEY_KP_8, KeyboardEvent::KeyCode::KEY_8 },
    { GLFW_KEY_KP_9, KeyboardEvent::KeyCode::KEY_9 },
#endif
    {(int)VirtualKey::Decimal, KeyboardEvent::KeyCode::KEY_PERIOD},
    {(int)VirtualKey::Divide, KeyboardEvent::KeyCode::KEY_KP_DIVIDE},
    {(int)VirtualKey::Multiply, KeyboardEvent::KeyCode::KEY_KP_MULTIPLY},
    {(int)VirtualKey::Subtract, KeyboardEvent::KeyCode::KEY_KP_MINUS},
    {(int)VirtualKey::Add, KeyboardEvent::KeyCode::KEY_KP_PLUS},
    //{ GLFW_KEY_KP_ENTER        , KeyboardEvent::KeyCode::KEY_KP_ENTER },
    //{ GLFW_KEY_KP_EQUAL        , KeyboardEvent::KeyCode::KEY_EQUAL },
    {(int)VirtualKey::Shift, KeyboardEvent::KeyCode::KEY_LEFT_SHIFT},
    {(int)VirtualKey::Control, KeyboardEvent::KeyCode::KEY_LEFT_CTRL},
    {VK_LMENU, KeyboardEvent::KeyCode::KEY_LEFT_ALT},
    {(int)VirtualKey::LeftWindows, KeyboardEvent::KeyCode::KEY_HYPER},
    {(int)VirtualKey::RightShift, KeyboardEvent::KeyCode::KEY_RIGHT_SHIFT},
    {(int)VirtualKey::RightControl, KeyboardEvent::KeyCode::KEY_RIGHT_CTRL},
    {VK_RMENU, KeyboardEvent::KeyCode::KEY_RIGHT_ALT},
    {(int)VirtualKey::RightWindows, KeyboardEvent::KeyCode::KEY_HYPER},
    {(int)VirtualKey::Menu, KeyboardEvent::KeyCode::KEY_MENU},
    {(int)VirtualKey::LeftMenu, KeyboardEvent::KeyCode::KEY_MENU},
    {(int)VirtualKey::RightMenu, KeyboardEvent::KeyCode::KEY_MENU}};

RenderView* RenderView::s_renderView = nullptr;

namespace
{
constexpr float WHEEL_DELTA_UNIT = 120.0f;

PointerType toAxPointerType(PointerDeviceType type)
{
    switch (type)
    {
    case PointerDeviceType::Touch:
        return PointerType::Touch;
    case PointerDeviceType::Pen:
        return PointerType::Pen;
    case PointerDeviceType::Mouse:
    default:
        return PointerType::Mouse;
    }
}

int32_t toAxButton(PointerPointProperties const& properties)
{
    switch (properties.PointerUpdateKind())
    {
    case PointerUpdateKind::LeftButtonPressed:
    case PointerUpdateKind::LeftButtonReleased:
        return InputButton::Left;
    case PointerUpdateKind::RightButtonPressed:
    case PointerUpdateKind::RightButtonReleased:
        return InputButton::Right;
    case PointerUpdateKind::MiddleButtonPressed:
    case PointerUpdateKind::MiddleButtonReleased:
        return InputButton::Middle;
    case PointerUpdateKind::XButton1Pressed:
    case PointerUpdateKind::XButton1Released:
        return 3;
    case PointerUpdateKind::XButton2Pressed:
    case PointerUpdateKind::XButton2Released:
        return 4;
    default:
        return InputButton::None;
    }
}

uint32_t toAxPressedButtons(PointerPointProperties const& properties)
{
    uint32_t buttons = 0;
    if (properties.IsLeftButtonPressed())
        buttons |= 1u << InputButton::Left;
    if (properties.IsRightButtonPressed())
        buttons |= 1u << InputButton::Right;
    if (properties.IsMiddleButtonPressed())
        buttons |= 1u << InputButton::Middle;
    if (properties.IsXButton1Pressed())
        buttons |= 1u << 3;
    if (properties.IsXButton2Pressed())
        buttons |= 1u << 4;
    return buttons;
}
}  // namespace

RenderView* RenderView::create(std::string_view viewName)
{
    return createWithRect(viewName, Rect::zero);
}

RenderView* RenderView::createWithRect(std::string_view viewName,
                                       const Rect& rect,
                                       float frameZoomFactor,
                                       bool /*resizable*/)
{
    auto ret = new RenderView();
    if (ret && ret->initWithRect(viewName, rect, frameZoomFactor))
    {
        ret->autorelease();
        return ret;
    }

    AX_SAFE_DELETE(ret);
    return nullptr;
}

RenderView* RenderView::createWithFullscreen(std::string_view viewName)
{
    return create(viewName);
}

RenderView::RenderView()
    : _isCursorVisible(true)
    , _initialized(false)
    , _width(0)
    , _height(0)
    , _dpi(0)
    , _orientation(Windows::Graphics::Display::DisplayOrientations::Landscape)
{
    s_renderView = this;
    _viewName    = "axmol3";

    _keyCodeMap.clear();
    for (auto& item : g_keyCodeStructArray)
    {
        _keyCodeMap[item.key] = item.keyCode;
    }
}

RenderView::~RenderView()
{
    if (_coreInput)
        _coreInput.Dispatcher().StopProcessEvents();
    if (_inputLoopWorker)
        _inputLoopWorker.Cancel();
    destroyRenderSurface();

    AX_ASSERT(this == s_renderView);
    s_renderView = nullptr;
}

bool RenderView::initWithRect(std::string_view viewName, const Rect& rect, float /*frameZoomFactor*/)
{
    auto application = Application::getInstance();
    _panel           = application->getPanel();
    _dispatcher      = application->getDispatcher();
    _dpi             = static_cast<float>(Device::getDPI());
    _orientation     = application->getOrientation();
    _width           = rect.size.width;
    _height          = rect.size.height;
    _initialized     = true;

    updateRenderScale();
    if (_width > 0 && _height > 0)
    {
        updateRenderSurface(_width, _height, SurfaceUpdateFlag::WindowSizeChanged | SurfaceUpdateFlag::SilentUpdate);
        updateRenderSurface(_width * _renderScale, _height * _renderScale,
                            SurfaceUpdateFlag::RenderSizeChanged | SurfaceUpdateFlag::SilentUpdate);
    }

    setViewName(viewName);

    return true;
}

void RenderView::setViewName(std::string_view viewName)
{
    RenderViewCore::setViewName(viewName);

    _dispatcher.get().RunAsync(CoreDispatcherPriority::Normal, [this]() {
        using namespace Windows::UI::ViewManagement;

        ApplicationView appView = ApplicationView::GetForCurrentView();
        appView.Title(winrt::to_hstring(_viewName));
    });
}

void RenderView::setCursorVisible(bool isVisible)
{
    _isCursorVisible = isVisible;
}

void RenderView::registerEventHandlers()
{
    if (!_dispatcher || !_panel)
        return;

    _dispatcher.get().RunAsync(CoreDispatcherPriority::Normal, [this]() {
        auto window = Window::Current().CoreWindow();
        window.VisibilityChanged({this, &RenderView::onVisibilityChanged});
        window.KeyDown({this, &RenderView::onKeyPressed});
        window.KeyUp({this, &RenderView::onKeyReleased});
        window.CharacterReceived({this, &RenderView::onCharacterReceived});

        auto display = DisplayInformation::GetForCurrentView();
        display.OrientationChanged({this, &RenderView::onOrientationChanged});
        display.DpiChanged({this, &RenderView::onDpiChanged});

        _panel.get().SizeChanged({this, &RenderView::onPanelSizeChanged});
        Window::Current().SetTitleBar(nullptr);

        if (ApiInformation::IsTypePresent(L"Windows.Phone.UI.Input.HardwareButtons"))
            SystemNavigationManager::GetForCurrentView().BackRequested({this, &RenderView::onBackButtonPressed});

        registerInput();
    });
}

void RenderView::registerInput()
{
    auto panel = _panel;
    if (!panel)
        return;

    if (_coreInput)
        _coreInput.Dispatcher().StopProcessEvents();
    if (_inputLoopWorker)
        _inputLoopWorker.Cancel();

    auto workItemHandler = [this, panel](Windows::Foundation::IAsyncAction const&) {
        _coreInput = panel.get().CreateCoreIndependentInputSource(
            CoreInputDeviceTypes::Mouse | CoreInputDeviceTypes::Touch | CoreInputDeviceTypes::Pen);

        _coreInput.PointerPressed({this, &RenderView::onPointerPressed});
        _coreInput.PointerMoved({this, &RenderView::onPointerMoved});
        _coreInput.PointerReleased({this, &RenderView::onPointerReleased});
        _coreInput.PointerWheelChanged({this, &RenderView::onPointerWheelChanged});

        if (!isCursorVisible())
            _coreInput.PointerCursor(nullptr);

        _coreInput.Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
    };

    _inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

void RenderView::syncCursorVisibility()
{
    if (_cursorVisible == isCursorVisible())
        return;

    registerInput();
    _cursorVisible = isCursorVisible();
}

void* RenderView::getNativeWindow() const
{
    return winrt::get_abi(_panel.get());
}

SurfaceHandle RenderView::getNativeDisplay() const
{
    return winrt::get_abi(_panel.get());
}

void RenderView::createRenderSurface()
{
#if AX_ENABLE_GL
    if (!rhi::DriverContext::isOpenGL())
        return;

    if (!_eglSurfaceProvider)
        _eglSurfaceProvider = std::make_unique<EGLSurfaceProvider>();

    if (_eglSurface == EGL_NO_SURFACE)
    {
        // The app can configure the SwapChainPanel which may boost performance.
        // By default, this template uses the default configuration.
        _eglSurface = _eglSurfaceProvider->createSurface(_panel.get(), nullptr, nullptr);
    }
#endif
}

void RenderView::destroyRenderSurface()
{
#if AX_ENABLE_GL
    if (!rhi::DriverContext::isOpenGL())
        return;

    if (_eglSurfaceProvider)
        _eglSurfaceProvider->destroySurface(_eglSurface);

    _eglSurface = EGL_NO_SURFACE;
#endif
}

void RenderView::recoverFromLostDevice()
{
#if AX_ENABLE_GL
    if (rhi::DriverContext::isOpenGL())
    {
        Concurrency::critical_section::scoped_lock lock(_eglSurfaceCriticalSection);
        destroyRenderSurface();
        if (_eglSurfaceProvider)
            _eglSurfaceProvider->reset();
        createRenderSurface();
    }
#endif
}

void RenderView::terminateApp()
{
#if AX_ENABLE_GL
    if (rhi::DriverContext::isOpenGL())
    {
        Concurrency::critical_section::scoped_lock lock(_eglSurfaceCriticalSection);
        destroyRenderSurface();
        if (_eglSurfaceProvider)
            _eglSurfaceProvider->cleanup();
    }
#endif
    Windows::UI::Xaml::Application::Current().Exit();
}

void RenderView::makeSurfaceCurrent()
{
#if AX_ENABLE_GL
    if (rhi::DriverContext::isOpenGL() && _eglSurfaceProvider && _eglSurface != EGL_NO_SURFACE)
        _eglSurfaceProvider->makeCurrent(_eglSurface);
#endif
}

bool RenderView::swapSurfaceBuffers()
{
#if AX_ENABLE_GL
    if (rhi::DriverContext::isOpenGL())
    {
        EGLBoolean result = GL_FALSE;
        {
            Concurrency::critical_section::scoped_lock lock(_eglSurfaceCriticalSection);
            if (_eglSurfaceProvider)
                result = _eglSurfaceProvider->swapBuffers(_eglSurface);
        }

        return result == GL_TRUE;
    }
#endif
    return true;
}

void RenderView::setIMEKeyboardState(bool bOpen)
{
    setIMEKeyboardState(bOpen, "");
}

AlertResult RenderView::showAlertDialog(const winrt::hstring& title, const winrt::hstring& message, AlertStyle style)
{
    using namespace winrt::Windows::UI::Core;
    using namespace winrt::Windows::UI::Popups;

    if (!_dispatcher)
        return AlertResult::No;

    bool isOnMainUIThread = _dispatcher.get().HasThreadAccess();
    bool canPromise       = !isOnMainUIThread && bitmask::any(style, AlertStyle::RequireSync);

    auto promisePtr = std::make_shared<std::promise<AlertResult>>();
    auto future     = promisePtr->get_future();

    auto addCommand = [canPromise](MessageDialog& dlg, std::wstring_view btnTitle, AlertResult ret,
                                   std::shared_ptr<std::promise<AlertResult>> promisePtr) {
        dlg.Commands().Append(UICommand(btnTitle, [promisePtr, ret, canPromise](auto&&) {
            if (canPromise)
            {
                try
                {
                    promisePtr->set_value(ret);
                }
                catch (...)
                {}
            }
        }));
    };

    auto showDialogAsync = [title, message, style, addCommand, promisePtr]() mutable {
        MessageDialog dlg(message, title);
        dlg.CancelCommandIndex(1);

        if (bitmask::any(style, AlertStyle::OkCancel))
        {
            addCommand(dlg, L"OK", AlertResult::Ok, promisePtr);
            addCommand(dlg, L"Cancel", AlertResult::Cancel, promisePtr);
        }
        else if (bitmask::any(style, AlertStyle::YesNo))
        {
            addCommand(dlg, L"Yes", AlertResult::Yes, promisePtr);
            addCommand(dlg, L"No", AlertResult::No, promisePtr);
        }
        else if (bitmask::any(style, AlertStyle::YesNoCancel))
        {
            addCommand(dlg, L"Yes", AlertResult::Yes, promisePtr);
            addCommand(dlg, L"No", AlertResult::No, promisePtr);
            addCommand(dlg, L"Cancel", AlertResult::Cancel, promisePtr);
        }
        else
        {
            addCommand(dlg, L"OK", AlertResult::Ok, promisePtr);
        }

        dlg.ShowAsync();
    };

    if (!isOnMainUIThread)
    {
        _dispatcher.get().RunAsync(CoreDispatcherPriority::Normal, showDialogAsync);
    }
    else
    {
        showDialogAsync();
    }

    return canPromise ? future.get() : AlertResult::None;
}

void RenderView::setIMEKeyboardState(bool bOpen, std::string_view str)
{
    if (bOpen)
    {
        showKeyboard(PlatformStringFromString(str));
    }
    else
    {
        hideKeyboard(PlatformStringFromString(str));
    }
}

void RenderView::swapBuffers() {}

bool RenderView::isGfxContextReady()
{
    return true;
}

void RenderView::end()
{
    Application::getInstance()->requestQuit();
}

void RenderView::onVisibilityChanged(Windows::UI::Core::CoreWindow const& /*sender*/,
                                     Windows::UI::Core::VisibilityChangedEventArgs const& args)
{
    if (args.Visible())
        Application::getInstance()->resume();
    else
        Application::getInstance()->suspend();
}

void RenderView::onWindowClosed(Windows::UI::Core::CoreWindow const& /*sender*/,
                                Windows::UI::Core::CoreWindowEventArgs const& /*args*/)
{
    Application::getInstance()->shutdown();
}

RenderView* RenderView::sharedRenderView()
{
    return s_renderView;
}

void RenderView::updateOrientation(Windows::Graphics::Display::DisplayOrientations orientation)
{
    if (_orientation != orientation)
    {
        _orientation = orientation;
        handleWindowResized();
    }
}

void RenderView::updateWindowSize(float width, float height)
{
    if (width != _width || height != _height)
    {
        _width  = width;
        _height = height;
        handleWindowResized();
    }
}

void RenderView::setDPI(float dpi)
{
    bool inital = _dpi == 0;
    if (_dpi != dpi)
    {
        Device::setDPI(dpi);
        _dpi = dpi;
        updateRenderScale();
        if (!inital)
        {
            updateRenderSurface(_width * _renderScale, _height * _renderScale, SurfaceUpdateFlag::RenderSizeChanged);
        }
    }
}

void RenderView::handleWindowResized()
{
    updateRenderSurface(_width, _height, SurfaceUpdateFlag::WindowSizeChanged);
    updateRenderSurface(_width * _renderScale, _height * _renderScale, SurfaceUpdateFlag::RenderSizeChanged);

    Size size(_width, _height);
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(RenderView::EVENT_WINDOW_RESIZED, &size);
}

void RenderView::updateRenderScale()
{
    if (!rhi::DriverContext::isOpenGL())
        _renderScale = Application::getContextAttrs().renderScaleMode == RenderScaleMode::Physical
                           ? (_dpi > 0 ? _dpi / 96.0f /* 96.0f: Standard DPI baseline */ : 1.0f)
                           : 1.0f;
    else
        _renderScale = 1.0f;

    InputSystem::getInstance()->setInputScale(_renderScale);
}

void RenderView::onPanelSizeChanged(Windows::Foundation::IInspectable const& /*sender*/,
                                    Windows::UI::Xaml::RoutedEventArgs const& /*args*/)
{
    if (_updateScheduled)
        return;

    _updateScheduled = true;
    auto panel       = _panel.get();
    auto width       = static_cast<float>(panel.ActualWidth());
    auto height      = static_cast<float>(panel.ActualHeight());
    Director::getInstance()->postTask([this, width, height]() {
        updateWindowSize(width, height);
        _updateScheduled = false;
    }, Director::TaskTiming::FrameBoundary);
}

void RenderView::onPointerPressed(Windows::Foundation::IInspectable const& /*sender*/, PointerEventArgs const& args)
{
    handlePointerEvent(InputPhase::PointerDown, args);
}

void RenderView::onPointerMoved(Windows::Foundation::IInspectable const& /*sender*/, PointerEventArgs const& args)
{
    handlePointerEvent(InputPhase::PointerMove, args);
}

void RenderView::onPointerReleased(Windows::Foundation::IInspectable const& /*sender*/, PointerEventArgs const& args)
{
    handlePointerEvent(InputPhase::PointerUp, args);
}

void RenderView::onPointerWheelChanged(Windows::Foundation::IInspectable const& /*sender*/,
                                       PointerEventArgs const& args)
{
    handlePointerEvent(InputPhase::PointerScroll, args);
}

void RenderView::onKeyPressed(CoreWindow const& /*sender*/, KeyEventArgs const& args)
{
    handleKeyboardEvent(ax::InputPhase::KeyDown, args);
}

void RenderView::onKeyReleased(CoreWindow const& /*sender*/, KeyEventArgs const& args)
{
    handleKeyboardEvent(ax::InputPhase::KeyUp, args);
}

void RenderView::onCharacterReceived(CoreWindow const& /*sender*/, CharacterReceivedEventArgs const& args)
{
    if (_textBox != nullptr)
        return;

    auto codepoint = args.KeyCode();
    if (codepoint < 0x20 || codepoint == 0x7f)
        return;

    std::wstring text;
    if (codepoint <= 0xffff)
    {
        text.push_back(static_cast<wchar_t>(codepoint));
    }
    else
    {
        codepoint -= 0x10000;
        text.push_back(static_cast<wchar_t>(0xd800 + (codepoint >> 10)));
        text.push_back(static_cast<wchar_t>(0xdc00 + (codepoint & 0x3ff)));
    }

    auto inputText = PlatformStringToString(winrt::hstring{text});
    Director::getInstance()->postTask([inputText = std::move(inputText)]() {
        InputSystem::getInstance()->dispatchInsertText(inputText);
    }, Director::TaskTiming::FrameBoundary);
}

void RenderView::onOrientationChanged(DisplayInformation const& sender,
                                      Windows::Foundation::IInspectable const& /*args*/)
{
    auto orientation = sender.CurrentOrientation();
    Director::getInstance()->postTask([this, orientation]() { updateOrientation(orientation); },
                                      Director::TaskTiming::FrameBoundary);
}

void RenderView::onDpiChanged(DisplayInformation const& sender, Windows::Foundation::IInspectable const& /*args*/)
{
    auto dpi = sender.LogicalDpi();
    Director::getInstance()->postTask([this, dpi]() { setDPI(dpi); }, Director::TaskTiming::FrameBoundary);
}

#if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP) || _MSC_VER >= 1900
void RenderView::onBackButtonPressed(Windows::Foundation::IInspectable const& /*sender*/,
                                     BackRequestedEventArgs const& args)
{
    Director::getInstance()->postTask([]() {
        InputSystem::getInstance()->handleKeyEvent(KeyboardEvent::KeyCode::KEY_ESCAPE, InputPhase::KeyUp);
    }, Director::TaskTiming::FrameBoundary);
    args.Handled(true);
}
#endif

void RenderView::showKeyboard(winrt::hstring const& text)
{
    auto panel      = _panel;
    auto dispatcher = _dispatcher;

    if (dispatcher && panel)
    {
        // run on main UI thread
        dispatcher.get().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this, text, panel]() {
            if (_textBox == nullptr)
            {
                _useInputMethod = false;
                _textBox        = TextBox();
                _textBox.Opacity(0.0);
                _textBox.Width(1);
                _textBox.Height(1);
                _textBox.TextChanged(TextChangedEventHandler(this, &RenderView::onTextChanged));
                _textBox.TextCompositionStarted(Windows::Foundation::TypedEventHandler<
                                                Windows::UI::Xaml::Controls::TextBox,
                                                Windows::UI::Xaml::Controls::TextCompositionStartedEventArgs>(
                    this, &RenderView::onTextCompositionStarted));

                _textBox.TextCompositionEnded(
                    Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::TextBox,
                                                           Windows::UI::Xaml::Controls::TextCompositionEndedEventArgs>(
                        this, &RenderView::onTextCompositionEnded));
                panel.get().Children().Append(_textBox);
            }
            _textBox.SelectionLength(0);
            _textBox.SelectionStart(32768);
            _textBox.Focus(FocusState::Programmatic);
        });
    }
}

void RenderView::hideKeyboard(winrt::hstring const& text)
{
    auto panel      = _panel;
    auto dispatcher = _dispatcher;

    if (dispatcher && panel)
    {
        // run on main UI thread
        dispatcher.get().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
                                  DispatchedHandler([this, text, panel]() {
            if (_textBox != nullptr)
            {
                unsigned int index;
                if (panel.get().Children().IndexOf(_textBox, index))
                {
                    panel.get().Children().RemoveAt(index);
                }
            }
            _textBox = nullptr;
        }));
    }
}

void RenderView::handlePointerEvent(ax::InputPhase phase, PointerEventArgs const& args)
{
    auto point      = args.CurrentPoint();
    auto properties = point.Properties();
    auto position   = point.Position();

    Vec2 pos{static_cast<float>(position.X), static_cast<float>(position.Y)};
    PointerInputState state{.id             = static_cast<intptr_t>(point.PointerId()),
                            .pressure       = properties.Pressure(),
                            .button         = phase == InputPhase::PointerMove || phase == InputPhase::PointerScroll
                                                  ? InputButton::None
                                                  : toAxButton(properties),
                            .pressedButtons = toAxPressedButtons(properties),
                            .type           = toAxPointerType(point.PointerDevice().PointerDeviceType())};

    if (phase == InputPhase::PointerScroll)
    {
        Vec2 scrollDelta{0.0f, static_cast<float>(properties.MouseWheelDelta()) / WHEEL_DELTA_UNIT};
        Director::getInstance()->postTask([pos, scrollDelta, state]() {
            InputSystem::getInstance()->handlePointerScroll(pos, scrollDelta, state);
        }, Director::TaskTiming::FrameBoundary);
        return;
    }

    Director::getInstance()->postTask([phase, pos, state]() {
        auto inputSystem = InputSystem::getInstance();
        switch (phase)
        {
        case InputPhase::PointerDown:
            inputSystem->handlePointerDown(pos, state);
            break;
        case InputPhase::PointerMove:
            inputSystem->handlePointerMove(pos, state);
            break;
        case InputPhase::PointerUp:
            inputSystem->handlePointerUp(pos, state);
            break;
        default:
            break;
        }
    }, Director::TaskTiming::FrameBoundary);
}

void RenderView::handleKeyboardEvent(ax::InputPhase phase, KeyEventArgs const& args)
{
    int key = static_cast<int>(args.VirtualKey());
    auto it = _keyCodeMap.find(key);
    if (it != _keyCodeMap.end())
    {
        KeyboardEvent::KeyCode keyCode = it->second;

        const auto isKeyDown = phase == ax::InputPhase::KeyDown;
        if (isKeyDown && args.KeyStatus().WasKeyDown)
            phase = ax::InputPhase::KeyRepeat;
        Director::getInstance()->postTask([keyCode, phase]() {
            InputSystem::getInstance()->handleKeyEvent(keyCode, phase);
        }, Director::TaskTiming::FrameBoundary);
    }
    else
    {
        AXLOGW("RenderView::onWinRTKeyboardEvent Virtual Key Code {} not supported", key);
    }
}

void RenderView::onTextChanged(const Windows::Foundation::IInspectable& sender, TextChangedEventArgs const& args)
{
    if (_useInputMethod)
    {
        return;
    }
    auto text = _textBox.Text();
    if (!text.empty())
    {
        auto inputText = PlatformStringToString(text);
        Director::getInstance()->postTask([inputText = std::move(inputText)]() {
            InputSystem::getInstance()->dispatchInsertText(inputText);
        }, Director::TaskTiming::FrameBoundary);
        _textBox.Text(L"");
    }
}

void RenderView::onTextCompositionStarted(Windows::UI::Xaml::Controls::TextBox,
                                          Windows::UI::Xaml::Controls::TextCompositionStartedEventArgs const& args)
{
    _useInputMethod = true;
}

void RenderView::onTextCompositionEnded(Windows::UI::Xaml::Controls::TextBox,
                                        Windows::UI::Xaml::Controls::TextCompositionEndedEventArgs const& args)
{
    _useInputMethod = false;
    auto text       = _textBox.Text();
    if (!text.empty())
    {
        auto inputText = PlatformStringToString(text);
        Director::getInstance()->postTask([inputText = std::move(inputText)]() {
            InputSystem::getInstance()->dispatchInsertText(inputText);
        }, Director::TaskTiming::FrameBoundary);
        _textBox.Text(L"");
    }
}

}  // namespace ax
