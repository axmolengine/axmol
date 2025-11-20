/*
 * Copyright (c) 2010-2014 - cocos2d-x community
 * Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 * Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
 *
 * https://axmol.dev/
 *
 * Portions Copyright (c) Microsoft Open Technologies, Inc.
 * All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */
#include "axmol/platform/winrt/xaml/SwapChainPage.h"
#include "SwapChainPage.g.cpp"

#include "axmol/platform/winrt/RenderViewImpl-winrt.h"
#include "axmol/platform/Application.h"

#include "yasio/wtimer_hres.hpp"

#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.UI.Input.Core.h>

using namespace winrt;

using namespace ax;
using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::Input::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

#if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP) || _MSC_VER >= 1900
using namespace Windows::UI::Input;
#endif

namespace winrt::AxmolAppWinRT::implementation
{
SwapChainPage::SwapChainPage()
    : m_coreInput(nullptr)
    , m_dpi(0.0f)
    , m_deviceLost(false)
    , m_cursorVisible(true)
    , m_visible(false)
    , m_orientation(DisplayOrientations::Landscape)
{
#if AX_RENDER_API == AX_RENDER_API_GL
    m_eglSurfaceProvider = new EGLSurfaceProvider();
#endif
    InitializeComponent();

    Windows::UI::Core::CoreWindow window = Windows::UI::Xaml::Window::Current().CoreWindow();

    window.VisibilityChanged({this, &SwapChainPage::OnVisibilityChanged});

    window.KeyDown({this, &SwapChainPage::OnKeyPressed});

    window.KeyUp({this, &SwapChainPage::OnKeyReleased});

    window.CharacterReceived({this, &SwapChainPage::_OnCharacterReceived});

    DisplayInformation currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation.OrientationChanged({this, &SwapChainPage::OnOrientationChanged});

    m_orientation = currentDisplayInformation.CurrentOrientation();

    this->Loaded({this, &SwapChainPage::OnPageLoaded});
    swapChainPanel().SizeChanged({this, &SwapChainPage::OnPanelSizeChanged});

    // If we have a phone contract, hide the status bar
    Window::Current().SetTitleBar(nullptr);

    if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent(L"Windows.Phone.UI.Input.HardwareButtons"))
    {
        SystemNavigationManager::GetForCurrentView().BackRequested({this, &SwapChainPage::OnBackButtonPressed});
    }

    CreateInput();
}

void SwapChainPage::CreateInput()
{
    // Register our SwapChainPanel to get independent input pointer events
    auto workItemHandler = ([this](IAsyncAction const&) {
        // The CoreIndependentInputSource will raise pointer events for the specified device types on whichever thread
        // it's created on.
        m_coreInput = swapChainPanel().CreateCoreIndependentInputSource(Windows::UI::Core::CoreInputDeviceTypes::Mouse |
                                                                        Windows::UI::Core::CoreInputDeviceTypes::Touch |
                                                                        Windows::UI::Core::CoreInputDeviceTypes::Pen);

        // Register for pointer events, which will be raised on the background thread.
        m_coreInput.PointerPressed({this, &SwapChainPage::_OnPointerPressed});
        m_coreInput.PointerMoved({this, &SwapChainPage::_OnPointerMoved});
        m_coreInput.PointerReleased({this, &SwapChainPage::_OnPointerReleased});
        m_coreInput.PointerWheelChanged({this, &SwapChainPage::_OnPointerWheelChanged});

        if (RenderViewImpl::sharedRenderView() && !RenderViewImpl::sharedRenderView()->isCursorVisible())
        {
            m_coreInput.PointerCursor(nullptr);
        }

        // Begin processing input messages as they're delivered.
        m_coreInput.Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
    });

    // Run task on a dedicated high priority background thread.
    m_inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

SwapChainPage::~SwapChainPage()
{
    StopRenderLoop();
    DestroyRenderSurface();
}

void SwapChainPage::OnPageLoaded(Windows::Foundation::IInspectable const& /*sender*/,
                                 Windows::UI::Xaml::RoutedEventArgs const& e)
{
    // The SwapChainPanel has been created and arranged in the page layout, so we can start render.
    CreateRenderSurface();
    StartRenderLoop();

    m_visible = true;
}

void SwapChainPage::OnPanelSizeChanged(Windows::Foundation::IInspectable const& /*sender*/,
                                       Windows::UI::Xaml::RoutedEventArgs const& /*e*/)
{
    if (!m_updateScheduled)
    {
        m_updateScheduled = true;
        swapChainPanel().Dispatcher().RunAsync(CoreDispatcherPriority::Low, [this]() {
            m_updateScheduled = false;
            UpdatePanelSize();
        });
    }
}

void SwapChainPage::CreateRenderSurface()
{
    UpdatePanelSize();
#if AX_RENDER_API == AX_RENDER_API_GL
    if (m_eglSurfaceProvider && m_eglSurface == EGL_NO_SURFACE)
    {
        // The app can configure the SwapChainPanel which may boost performance.
        // By default, this template uses the default configuration.
        m_eglSurface = m_eglSurfaceProvider->CreateSurface(swapChainPanel(), nullptr, nullptr);

        // You can configure the SwapChainPanel to render at a lower resolution and be scaled up to
        // the swapchain panel size. This scaling is often free on mobile hardware.
        //
        // One way to configure the SwapChainPanel is to specify precisely which resolution it should render at.
        // Size custom_eglSurfaceSize = Size(800, 600);
        // m_eglSurface = m_eglSurfaceProvider->CreateSurface(swapChainPanel, &custom_eglSurfaceSize, nullptr);
        //
        // Another way is to tell the SwapChainPanel to render at a certain scale factor compared to its size.
        // e.g. if the SwapChainPanel is 1920x1280 then setting a factor of 0.5f will make the app render at 960x640
        // float customResolutionScale = 0.5f;
        // m_eglSurface = m_eglSurfaceProvider->CreateSurface(swapChainPanel, nullptr, &customResolutionScale);
        //
    }
#endif
}

void SwapChainPage::UpdatePanelSize()
{
    auto panel    = swapChainPanel();
    m_panelWidth  = panel.ActualWidth();
    m_panelHeight = panel.ActualHeight();
}

void SwapChainPage::DestroyRenderSurface()
{
#if AX_RENDER_API == AX_RENDER_API_GL
    if (m_eglSurfaceProvider)
    {
        m_eglSurfaceProvider->DestroySurface(m_eglSurface);
        delete m_eglSurfaceProvider;
        m_eglSurfaceProvider = nullptr;
    }

    m_eglSurface = EGL_NO_SURFACE;
#endif
}

void SwapChainPage::RecoverFromLostDevice()
{
#if AX_RENDER_API == AX_RENDER_API_GL
    critical_section::scoped_lock lock(m_eglSurfaceCriticalSection);
    DestroyRenderSurface();
    m_eglSurfaceProvider->Reset();
    CreateRenderSurface();
#endif

    std::unique_lock<std::mutex> locker(m_sleepMutex);
    m_deviceLost = false;
    m_sleepCondition.notify_one();
}

void SwapChainPage::TerminateApp()
{
#if AX_RENDER_API == AX_RENDER_API_GL
    {
        critical_section::scoped_lock lock(m_eglSurfaceCriticalSection);

        if (m_eglSurfaceProvider)
        {
            m_eglSurfaceProvider->DestroySurface(m_eglSurface);
            m_eglSurfaceProvider->Cleanup();
        }
    }
#endif
    Windows::UI::Xaml::Application::Current().Exit();
}

void SwapChainPage::ProcessOperations()
{
    std::function<void()> op;
    while (m_operations.try_pop(op))
        op();
}

void SwapChainPage::StartRenderLoop()
{
    // If the render loop is already running then do not start another thread.
    if (m_renderLoopWorker != nullptr && m_renderLoopWorker.Status() == Windows::Foundation::AsyncStatus::Started)
    {
        return;
    }

    DisplayInformation currentDisplayInformation = DisplayInformation::GetForCurrentView();
    m_dpi                                        = currentDisplayInformation.LogicalDpi();

    auto dispatcher = Windows::UI::Xaml::Window::Current().CoreWindow().Dispatcher();

    // Create a task for rendering that will be run on a background thread.
    auto renderFrame = ([this, dispatcher](Windows::Foundation::IAsyncAction const& action) {
        if (!m_renderer)
        {
            m_renderer = std::make_shared<AxmolRenderer>(m_panelWidth, m_panelHeight, m_dpi, m_orientation, dispatcher,
                                                         swapChainPanel());
        }

#if AX_RENDER_API == AX_RENDER_API_GL
        m_eglSurfaceProvider->MakeCurrent(m_eglSurface);
#endif

        // !!!Start the engine renderer on the render thread so that WICImageDecoder
        // initializes COM in multi-threaded apartment (MTA) mode.
        m_renderer->Resume();

        void* thiz = (void*)this;
        m_renderer->SetQueueOperationCb([thiz](ax::AsyncOperation op, void* param) {
            auto thisUnsafe = reinterpret_cast<SwapChainPage*>(thiz);
            thisUnsafe->m_operations.push([=]() { op(param); });
            thisUnsafe->m_sleepCondition.notify_one();
        });

        // the actual render frame function
        std::function<bool(void)> frameFunc = [&]() {
            if (!m_visible)
            {
                m_renderer->Pause();
            }

            // wait until app is visible again or thread is cancelled
            while (!m_visible)
            {
                std::unique_lock<std::mutex> lock(m_sleepMutex);
                m_sleepCondition.wait(lock);

                if (action.Status() != Windows::Foundation::AsyncStatus::Started)
                {
                    return false;  // thread was cancelled. Exit thread
                }

                if (m_visible)
                {
                    m_renderer->Resume();
                }
                else  // spurious wake up
                {
                    ProcessOperations();
                    continue;
                }
            }

            ProcessOperations();

            m_renderer->Draw(static_cast<size_t>(m_panelWidth), static_cast<size_t>(m_panelHeight), m_dpi,
                             m_orientation);

            // Recreate input dispatch
            if (RenderViewImpl::sharedRenderView() &&
                m_cursorVisible != RenderViewImpl::sharedRenderView()->isCursorVisible())
            {
                CreateInput();
                m_cursorVisible = RenderViewImpl::sharedRenderView()->isCursorVisible();
            }

            if (m_renderer->AppShouldExit())
            {
                // run on main UI thread
                auto thiz = this;
                swapChainPanel().Dispatcher().RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
                                                       ([thiz]() { thiz->TerminateApp(); }));

                return false;
            }

#if AX_RENDER_API == AX_RENDER_API_GL
            EGLBoolean result = GL_FALSE;
            {
                critical_section::scoped_lock lock(m_eglSurfaceCriticalSection);
                result = m_eglSurfaceProvider->SwapBuffers(m_eglSurface);
            }

            if (result != GL_TRUE)
            {
                // The call to eglSwapBuffers was not be successful (i.e. due to Device Lost)
                // If the call fails, then we must reinitialize EGL and the GL resources.
                m_renderer->Pause();
                m_deviceLost = true;

                // XAML objects like the SwapChainPanel must only be manipulated on the UI thread.
                auto thiz = this;
                swapChainPanel().Dispatcher().RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
                                                       ([thiz]() { thiz->RecoverFromLostDevice(); }));

                // wait until OpenGL is reset or thread is cancelled
                while (m_deviceLost)
                {
                    std::unique_lock<std::mutex> lock(m_sleepMutex);
                    m_sleepCondition.wait(lock);

                    if (action.Status() != Windows::Foundation::AsyncStatus::Started)
                    {
                        return false;  // thread was cancelled. Exit thread
                    }

                    if (!m_deviceLost)
                    {
                        m_eglSurfaceProvider->MakeCurrent(m_eglSurface);
                        m_renderer->DeviceLost();
                    }
                    else  // spurious wake up
                    {
                        continue;
                    }
                }
            }
#endif

            return true;
        };

        // Sets Sleep(aka NtDelayExecution) resolution to 1ms
        yasio::wtimer_hres __timer_hres_man;
        auto application = ax::Application::getInstance();
        while (action.Status() == Windows::Foundation::AsyncStatus::Started)
        {
            if (!application->frameStep(frameFunc))
                return;
        }
    });

    // Run task on a dedicated high priority background thread.
    m_renderLoopWorker = Windows::System::Threading::ThreadPool::RunAsync(
        renderFrame, Windows::System::Threading::WorkItemPriority::High,
        Windows::System::Threading::WorkItemOptions::TimeSliced);
}

void SwapChainPage::StopRenderLoop()
{
    if (m_renderLoopWorker)
    {
        m_renderLoopWorker.Cancel();
        std::unique_lock<std::mutex> locker(m_sleepMutex);
        m_sleepCondition.notify_one();
        m_renderLoopWorker = nullptr;
    }
}

void SwapChainPage::_OnPointerPressed(Windows::Foundation::IInspectable const& sender, PointerEventArgs const& args)
{
    bool isMouseEvent =
        args.CurrentPoint().PointerDevice().PointerDeviceType() == Windows::Devices::Input::PointerDeviceType::Mouse;
    if (m_renderer)
    {
        m_renderer->QueuePointerEvent(isMouseEvent ? PointerEventType::MousePressed : PointerEventType::PointerPressed,
                                      args);
    }
}

void SwapChainPage::_OnPointerMoved(Windows::Foundation::IInspectable const& sender, PointerEventArgs const& args)
{
    bool isMouseEvent =
        args.CurrentPoint().PointerDevice().PointerDeviceType() == Windows::Devices::Input::PointerDeviceType::Mouse;
    if (m_renderer)
    {
        m_renderer->QueuePointerEvent(isMouseEvent ? PointerEventType::MouseMoved : PointerEventType::PointerMoved,
                                      args);
    }
}

void SwapChainPage::_OnPointerReleased(Windows::Foundation::IInspectable const& sender, PointerEventArgs const& args)
{
    bool isMouseEvent =
        args.CurrentPoint().PointerDevice().PointerDeviceType() == Windows::Devices::Input::PointerDeviceType::Mouse;

    if (m_renderer)
    {
        m_renderer->QueuePointerEvent(
            isMouseEvent ? PointerEventType::MouseReleased : PointerEventType::PointerReleased, args);
    }
}

void SwapChainPage::_OnPointerWheelChanged(Windows::Foundation::IInspectable const& /*sender*/,
                                           PointerEventArgs const& args)
{
    bool isMouseEvent =
        args.CurrentPoint().PointerDevice().PointerDeviceType() == Windows::Devices::Input::PointerDeviceType::Mouse;
    if (m_renderer && isMouseEvent)
    {
        m_renderer->QueuePointerEvent(PointerEventType::MouseWheelChanged, args);
    }
}

void SwapChainPage::OnKeyPressed(CoreWindow const& sender, KeyEventArgs const& args)
{
    if (m_renderer)
    {
        m_renderer->QueueKeyboardEvent(WinRTKeyboardEventType::Down, args);
    }
}

void SwapChainPage::_OnCharacterReceived(CoreWindow const& /*sender*/, CharacterReceivedEventArgs const& /*args*/) {}

void SwapChainPage::OnKeyReleased(CoreWindow const& /*sender*/, KeyEventArgs const& args)
{
    if (m_renderer)
    {
        m_renderer->QueueKeyboardEvent(WinRTKeyboardEventType::Up, args);
    }
}

void SwapChainPage::OnOrientationChanged(DisplayInformation const& sender,
                                         Windows::Foundation::IInspectable const& /*args*/)
{
    m_orientation = sender.CurrentOrientation();
}

void SwapChainPage::SetVisibility(bool isVisible)
{
    if (isVisible)
    {
        if (!m_visible)
        {
            std::unique_lock<std::mutex> locker(m_sleepMutex);
            m_visible = true;
            m_sleepCondition.notify_one();
        }
    }
    else
    {
        m_visible = false;
    }
}

void SwapChainPage::OnVisibilityChanged(Windows::UI::Core::CoreWindow const& sender,
                                        Windows::UI::Core::VisibilityChangedEventArgs const& args)
{
    if (args.Visible())
    {
        SetVisibility(true);
    }
    else
    {
        SetVisibility(false);
    }
}

#if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP) || _MSC_VER >= 1900
/*
We set args->Handled = true to prevent the app from quitting when the back button is pressed.
This is because this back button event happens on the XAML UI thread and not the axmol UI thread.
We need to give the game developer a chance to decide to exit the app depending on where they
are in their game. They can receive the back button event by listening for the
EventKeyboard::KeyCode::KEY_ESCAPE event.

The default behavior is to exit the app if the  EventKeyboard::KeyCode::KEY_ESCAPE event
is not handled by the game.
*/
void SwapChainPage::OnBackButtonPressed(Windows::Foundation::IInspectable const& sender,
                                        BackRequestedEventArgs const& args)
{
    if (m_renderer)
    {
        m_renderer->QueueBackButtonEvent();
        args.Handled(true);
    }
}
#endif

}  // namespace winrt::AxmolAppWinRT::implementation
