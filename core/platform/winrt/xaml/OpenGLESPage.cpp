/*
 * cocos2d-x   https://axmol.dev/
 *
 * Copyright (c) 2010-2014 - cocos2d-x community
 * Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 * Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
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
#include "OpenGLESPage.h"
#include "OpenGLESPage.g.cpp"

#include "platform/winrt/GLViewImpl-winrt.h"
#include "platform/Application.h"

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
OpenGLESPage::OpenGLESPage() : OpenGLESPage(nullptr) {}
OpenGLESPage::OpenGLESPage(OpenGLES* openGLES)
    : mOpenGLES(openGLES)
    , mRenderSurface(EGL_NO_SURFACE)
    , mCoreInput(nullptr)
    , mDpi(0.0f)
    , mDeviceLost(false)
    , mCursorVisible(true)
    , mVisible(false)
    , mOrientation(DisplayOrientations::Landscape)
{
    InitializeComponent();

    Windows::UI::Core::CoreWindow window = Windows::UI::Xaml::Window::Current().CoreWindow();

    window.VisibilityChanged({this, &OpenGLESPage::OnVisibilityChanged});

    window.KeyDown({this, &OpenGLESPage::OnKeyPressed});

    window.KeyUp({this, &OpenGLESPage::OnKeyReleased});

    window.CharacterReceived({this, &OpenGLESPage::_OnCharacterReceived});

    DisplayInformation currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation.OrientationChanged({this, &OpenGLESPage::OnOrientationChanged});

    mOrientation = currentDisplayInformation.CurrentOrientation();

    this->Loaded({this, &OpenGLESPage::OnPageLoaded});

    // If we have a phone contract, hide the status bar
    Window::Current().SetTitleBar(nullptr);

    if (Windows::Foundation::Metadata::ApiInformation::IsTypePresent(L"Windows.Phone.UI.Input.HardwareButtons"))
    {
        SystemNavigationManager::GetForCurrentView().BackRequested({this, &OpenGLESPage::OnBackButtonPressed});
    }

    CreateInput();
}

void OpenGLESPage::CreateInput()
{
    // Register our SwapChainPanel to get independent input pointer events
    auto workItemHandler = ([this](IAsyncAction const&) {
        // The CoreIndependentInputSource will raise pointer events for the specified device types on whichever thread
        // it's created on.
        mCoreInput = swapChainPanel().CreateCoreIndependentInputSource(Windows::UI::Core::CoreInputDeviceTypes::Mouse |
                                                                       Windows::UI::Core::CoreInputDeviceTypes::Touch |
                                                                       Windows::UI::Core::CoreInputDeviceTypes::Pen);

        // Register for pointer events, which will be raised on the background thread.
        mCoreInput.PointerPressed({this, &OpenGLESPage::_OnPointerPressed});
        mCoreInput.PointerMoved({this, &OpenGLESPage::_OnPointerMoved});
        mCoreInput.PointerReleased({this, &OpenGLESPage::_OnPointerReleased});
        mCoreInput.PointerWheelChanged({this, &OpenGLESPage::_OnPointerWheelChanged});

        if (GLViewImpl::sharedGLView() && !GLViewImpl::sharedGLView()->isCursorVisible())
        {
            mCoreInput.PointerCursor(nullptr);
        }

        // Begin processing input messages as they're delivered.
        mCoreInput.Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit);
    });

    // Run task on a dedicated high priority background thread.
    mInputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

OpenGLESPage::~OpenGLESPage()
{
    StopRenderLoop();
    DestroyRenderSurface();
}

void OpenGLESPage::OnPageLoaded(Windows::Foundation::IInspectable const& sender,
                                Windows::UI::Xaml::RoutedEventArgs const& e)
{
    // The SwapChainPanel has been created and arranged in the page layout, so EGL can be initialized.
    CreateRenderSurface();
    StartRenderLoop();
    mVisible = true;
}

void OpenGLESPage::CreateRenderSurface()
{
    if (mOpenGLES && mRenderSurface == EGL_NO_SURFACE)
    {
        // The app can configure the SwapChainPanel which may boost performance.
        // By default, this template uses the default configuration.
        mRenderSurface = mOpenGLES->CreateSurface(swapChainPanel(), nullptr, nullptr);

        // You can configure the SwapChainPanel to render at a lower resolution and be scaled up to
        // the swapchain panel size. This scaling is often free on mobile hardware.
        //
        // One way to configure the SwapChainPanel is to specify precisely which resolution it should render at.
        // Size customRenderSurfaceSize = Size(800, 600);
        // mRenderSurface = mOpenGLES->CreateSurface(swapChainPanel, &customRenderSurfaceSize, nullptr);
        //
        // Another way is to tell the SwapChainPanel to render at a certain scale factor compared to its size.
        // e.g. if the SwapChainPanel is 1920x1280 then setting a factor of 0.5f will make the app render at 960x640
        // float customResolutionScale = 0.5f;
        // mRenderSurface = mOpenGLES->CreateSurface(swapChainPanel, nullptr, &customResolutionScale);
        //
    }
}

void OpenGLESPage::DestroyRenderSurface()
{
    if (mOpenGLES)
    {
        mOpenGLES->DestroySurface(mRenderSurface);
    }
    mRenderSurface = EGL_NO_SURFACE;
}

void OpenGLESPage::RecoverFromLostDevice()
{
    critical_section::scoped_lock lock(mRenderSurfaceCriticalSection);
    DestroyRenderSurface();
    mOpenGLES->Reset();
    CreateRenderSurface();
    std::unique_lock<std::mutex> locker(mSleepMutex);
    mDeviceLost = false;
    mSleepCondition.notify_one();
}

void OpenGLESPage::TerminateApp()
{
    {
        critical_section::scoped_lock lock(mRenderSurfaceCriticalSection);

        if (mOpenGLES)
        {
            mOpenGLES->DestroySurface(mRenderSurface);
            mOpenGLES->Cleanup();
        }
    }
    Windows::UI::Xaml::Application::Current().Exit();
}

void OpenGLESPage::ProcessOperations()
{
    std::function<void()> op;
    while (mOperations.try_pop(op))
        op();
}

void OpenGLESPage::StartRenderLoop()
{
    // If the render loop is already running then do not start another thread.
    if (mRenderLoopWorker != nullptr && mRenderLoopWorker.Status() == Windows::Foundation::AsyncStatus::Started)
    {
        return;
    }

    DisplayInformation currentDisplayInformation = DisplayInformation::GetForCurrentView();
    mDpi                                         = currentDisplayInformation.LogicalDpi();

    auto dispatcher = Windows::UI::Xaml::Window::Current().CoreWindow().Dispatcher();

    // Create a task for rendering that will be run on a background thread.
    auto workItemHandler = ([this, dispatcher](Windows::Foundation::IAsyncAction const& action) {
        mOpenGLES->MakeCurrent(mRenderSurface);

        GLsizei panelWidth  = 0;
        GLsizei panelHeight = 0;
        mOpenGLES->GetSurfaceDimensions(mRenderSurface, &panelWidth, &panelHeight);

        if (!mRenderer)
            mRenderer = std::make_shared<AxmolRenderer>(panelWidth, panelHeight, mDpi, mOrientation, dispatcher,
                                                        swapChainPanel());

        mRenderer->Resume();

        void* thiz = (void*)this;
        mRenderer->SetQueueOperationCb([thiz](ax::AsyncOperation op, void* param) {
            auto thisUnsafe = reinterpret_cast<OpenGLESPage*>(thiz);
            thisUnsafe->mOperations.push([=]() { op(param); });
            thisUnsafe->mSleepCondition.notify_one();
        });

        // the actual render frame function
        std::function<bool(void)> frameFunc = [&]() {

            if (!mVisible)
            {
                mRenderer->Pause();
            }

            // wait until app is visible again or thread is cancelled
            while (!mVisible)
            {
                std::unique_lock<std::mutex> lock(mSleepMutex);
                mSleepCondition.wait(lock);

                if (action.Status() != Windows::Foundation::AsyncStatus::Started)
                {
                    return false;  // thread was cancelled. Exit thread
                }

                if (mVisible)
                {
                    mRenderer->Resume();
                }
                else  // spurious wake up
                {
                    ProcessOperations();
                    continue;
                }
            }

            ProcessOperations();

            mOpenGLES->GetSurfaceDimensions(mRenderSurface, &panelWidth, &panelHeight);
            mRenderer->Draw(panelWidth, panelHeight, mDpi, mOrientation);

            // Recreate input dispatch
            if (GLViewImpl::sharedGLView() && mCursorVisible != GLViewImpl::sharedGLView()->isCursorVisible())
            {
                CreateInput();
                mCursorVisible = GLViewImpl::sharedGLView()->isCursorVisible();
            }

            if (mRenderer->AppShouldExit())
            {
                // run on main UI thread
                auto thiz = this;
                swapChainPanel().Dispatcher().RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
                                                       ([thiz]() { thiz->TerminateApp(); }));

                return false;
            }

            EGLBoolean result = GL_FALSE;
            {
                critical_section::scoped_lock lock(mRenderSurfaceCriticalSection);
                result = mOpenGLES->SwapBuffers(mRenderSurface);
            }

            if (result != GL_TRUE)
            {
                // The call to eglSwapBuffers was not be successful (i.e. due to Device Lost)
                // If the call fails, then we must reinitialize EGL and the GL resources.
                mRenderer->Pause();
                mDeviceLost = true;

                // XAML objects like the SwapChainPanel must only be manipulated on the UI thread.
                auto thiz = this;
                swapChainPanel().Dispatcher().RunAsync(Windows::UI::Core::CoreDispatcherPriority::High,
                                                       ([thiz]() { thiz->RecoverFromLostDevice(); }));

                // wait until OpenGL is reset or thread is cancelled
                while (mDeviceLost)
                {
                    std::unique_lock<std::mutex> lock(mSleepMutex);
                    mSleepCondition.wait(lock);

                    if (action.Status() != Windows::Foundation::AsyncStatus::Started)
                    {
                        return false;  // thread was cancelled. Exit thread
                    }

                    if (!mDeviceLost)
                    {
                        mOpenGLES->MakeCurrent(mRenderSurface);
                        mRenderer->DeviceLost();
                    }
                    else  // spurious wake up
                    {
                        continue;
                    }
                }
            }

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
    mRenderLoopWorker = Windows::System::Threading::ThreadPool::RunAsync(
        workItemHandler, Windows::System::Threading::WorkItemPriority::High,
        Windows::System::Threading::WorkItemOptions::TimeSliced);
}

void OpenGLESPage::StopRenderLoop()
{
    if (mRenderLoopWorker)
    {
        mRenderLoopWorker.Cancel();
        std::unique_lock<std::mutex> locker(mSleepMutex);
        mSleepCondition.notify_one();
        mRenderLoopWorker = nullptr;
    }
}

void OpenGLESPage::_OnPointerPressed(Windows::Foundation::IInspectable const& sender, PointerEventArgs const& args)
{
    bool isMouseEvent =
        args.CurrentPoint().PointerDevice().PointerDeviceType() == Windows::Devices::Input::PointerDeviceType::Mouse;
    if (mRenderer)
    {
        mRenderer->QueuePointerEvent(isMouseEvent ? PointerEventType::MousePressed : PointerEventType::PointerPressed,
                                     args);
    }
}

void OpenGLESPage::_OnPointerMoved(Windows::Foundation::IInspectable const& sender, PointerEventArgs const& args)
{
    bool isMouseEvent =
        args.CurrentPoint().PointerDevice().PointerDeviceType() == Windows::Devices::Input::PointerDeviceType::Mouse;
    if (mRenderer)
    {
        mRenderer->QueuePointerEvent(isMouseEvent ? PointerEventType::MouseMoved : PointerEventType::PointerMoved,
                                     args);
    }
}

void OpenGLESPage::_OnPointerReleased(Windows::Foundation::IInspectable const& sender, PointerEventArgs const& args)
{
    bool isMouseEvent =
        args.CurrentPoint().PointerDevice().PointerDeviceType() == Windows::Devices::Input::PointerDeviceType::Mouse;

    if (mRenderer)
    {
        mRenderer->QueuePointerEvent(isMouseEvent ? PointerEventType::MouseReleased : PointerEventType::PointerReleased,
                                     args);
    }
}

void OpenGLESPage::_OnPointerWheelChanged(Windows::Foundation::IInspectable const& /*sender*/,
                                          PointerEventArgs const& args)
{
    bool isMouseEvent =
        args.CurrentPoint().PointerDevice().PointerDeviceType() == Windows::Devices::Input::PointerDeviceType::Mouse;
    if (mRenderer && isMouseEvent)
    {
        mRenderer->QueuePointerEvent(PointerEventType::MouseWheelChanged, args);
    }
}

void OpenGLESPage::OnKeyPressed(CoreWindow const& sender, KeyEventArgs const& args)
{
    if (mRenderer)
    {
        mRenderer->QueueKeyboardEvent(WinRTKeyboardEventType::KeyPressed, args);
    }
}

void OpenGLESPage::_OnCharacterReceived(CoreWindow const& /*sender*/, CharacterReceivedEventArgs const& /*args*/)
{
}

void OpenGLESPage::OnKeyReleased(CoreWindow const& /*sender*/, KeyEventArgs const& args)
{
    if (mRenderer)
    {
        mRenderer->QueueKeyboardEvent(WinRTKeyboardEventType::KeyReleased, args);
    }
}

void OpenGLESPage::OnOrientationChanged(DisplayInformation const& sender,
                                        Windows::Foundation::IInspectable const& /*args*/)
{
    mOrientation = sender.CurrentOrientation();
}

void OpenGLESPage::SetVisibility(bool isVisible)
{
    if (isVisible && mRenderSurface != EGL_NO_SURFACE)
    {
        if (!mVisible)
        {
            std::unique_lock<std::mutex> locker(mSleepMutex);
            mVisible = true;
            mSleepCondition.notify_one();
        }
    }
    else
    {
        mVisible = false;
    }
}

void OpenGLESPage::OnVisibilityChanged(Windows::UI::Core::CoreWindow const& sender,
                                       Windows::UI::Core::VisibilityChangedEventArgs const& args)
{
    if (args.Visible() && mRenderSurface != EGL_NO_SURFACE)
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
void OpenGLESPage::OnBackButtonPressed(Windows::Foundation::IInspectable const& sender,
                                       BackRequestedEventArgs const& args)
{
    if (mRenderer)
    {
        mRenderer->QueueBackButtonEvent();
        args.Handled(true);
    }
}
#endif

}  // namespace winrt::AxmolAppWinRT::implementation
