/****************************************************************************
Copyright (c) 2010-2013 cocos2d-x.org
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
#include "axmol/platform/PlatformConfig.h"
#include "axmol/platform/winrt/RenderView-winrt.h"
#include "axmol/base/Director.h"
#include <algorithm>
#include "axmol/platform/FileUtils.h"
#include "axmol/platform/Device.h"
#include "axmol/platform/winrt/WinRTUtils.h"
#include "axmol/platform/Application.h"
#include "axmol/rhi/DriverContext.h"

#include "pugixml/pugixml.hpp"
#include "axmol/tlx/format.hpp"
#include "yasio/wtimer_hres.hpp"

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.System.UserProfile.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.System.Threading.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

#include <combaseapi.h>
#include <future>

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::System::Threading;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

/**
@brief    This function change the PVRFrame show/hide setting in register.
@param  bEnable If true show the PVRFrame window, otherwise hide.
*/

namespace ax
{

////////////////////////////////////////////////////////////////////////////////
// implement Application
////////////////////////////////////////////////////////////////////////////////

// sharedApplication pointer
Application* s_pSharedApplication = nullptr;

Application::Application() : _openURLDelegate(nullptr)
{
    _animationInterval.QuadPart = 0;
    _freq.QuadPart              = 0;
    _last.QuadPart              = 0;
    _now.QuadPart               = 0;
    AX_ASSERT(!s_axmolApp);
    s_axmolApp = this;
}

Application::~Application()
{
    AX_ASSERT(this == s_axmolApp);
    s_axmolApp = nullptr;
}

int Application::run()
{
    // On WinUWP/WinRT builds, the axmol rendering thread is not the UI main thread.
    // COM is expected to run in MTA (COINIT_MULTITHREADED) for this worker thread.
    // Unlike Win32, the rendering thread here does not own a native window handle.
    // Instead, it renders into a swap chain surface that is created and managed
    // by the UI main thread's CoreWindow. All UI-related COM components (e.g. WebView2)
    // must therefore be initialized and message-pumped on the main UI thread.
    std::ignore = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // Initialize instance and axmol.
    if (!applicationDidFinishLaunching())
    {
        return 0;
    }

    QueryPerformanceCounter(&_last);
    QueryPerformanceFrequency(&_freq);

    yasio::wtimer_hres timerResolution;
    auto director   = ax::Director::getInstance();
    auto dispatcher = _renderView->getDispatcher().get();

    LONGLONG interval = 0LL;
    LONG waitMS       = 0L;
    bool paused       = false;

    auto drainBoundaryTasks = [this, director]() {
        {
            std::unique_lock<std::mutex> lock(_loopMutex);
            _boundaryTaskPending = false;
        }
        director->performFrameBoundaryTasks();
    };

    while (isRenderLoopRunning())
    {
        drainBoundaryTasks();

        bool shouldSuspend = false;
        {
            std::unique_lock<std::mutex> lock(_loopMutex);
            shouldSuspend = _suspended;
        }

        if (shouldSuspend)
        {
            if (!paused)
            {
                onPause();
                paused = true;
            }

            while (isRenderLoopRunning())
            {
                bool shouldResume             = false;
                bool shouldDrainBoundaryTasks = false;
                {
                    std::unique_lock<std::mutex> lock(_loopMutex);
                    _loopCondition.wait(
                        lock, [this]() { return !isRenderLoopRunning() || !_suspended || _boundaryTaskPending; });

                    if (!isRenderLoopRunning())
                        break;

                    shouldResume             = !_suspended;
                    shouldDrainBoundaryTasks = _boundaryTaskPending;
                    if (shouldDrainBoundaryTasks)
                        _boundaryTaskPending = false;
                }

                if (shouldDrainBoundaryTasks)
                    director->performFrameBoundaryTasks();

                if (shouldResume)
                    break;
            }

            if (!isRenderLoopRunning())
                break;

            onResume();
            paused = false;
            QueryPerformanceCounter(&_last);
            continue;
        }

        QueryPerformanceCounter(&_now);

        interval = _now.QuadPart - _last.QuadPart;
        if (interval >= _animationInterval.QuadPart)
        {
            _last.QuadPart = _now.QuadPart;

            director->renderFrame();
            _renderView->syncCursorVisibility();

            if (_appShouldExit.load(std::memory_order_acquire))
            {
                _requestedTerminate = true;
                dispatcher.RunAsync(CoreDispatcherPriority::High, [this]() {
                    if (_renderView)
                        _renderView->terminateApp();
                });
                break;
            }

            if (!_renderView->swapSurfaceBuffers())
            {
                onPause();
                paused = true;
                {
                    std::unique_lock<std::mutex> lock(_loopMutex);
                    _deviceLost = true;
                }

                dispatcher.RunAsync(CoreDispatcherPriority::High, [this]() {
                    if (_renderView)
                        _renderView->recoverFromLostDevice();

                    std::unique_lock<std::mutex> lock(_loopMutex);
                    _deviceLost = false;
                    _loopCondition.notify_one();
                });

                while (isRenderLoopRunning())
                {
                    bool shouldDrainBoundaryTasks = false;
                    bool shouldRecover            = false;
                    {
                        std::unique_lock<std::mutex> lock(_loopMutex);
                        _loopCondition.wait(
                            lock, [this]() { return !isRenderLoopRunning() || !_deviceLost || _boundaryTaskPending; });

                        if (!isRenderLoopRunning())
                            break;

                        shouldRecover            = !_deviceLost;
                        shouldDrainBoundaryTasks = _boundaryTaskPending;
                        if (shouldDrainBoundaryTasks)
                            _boundaryTaskPending = false;
                    }

                    if (shouldDrainBoundaryTasks)
                        director->performFrameBoundaryTasks();

                    if (shouldRecover)
                        break;
                }

                if (!isRenderLoopRunning())
                    break;

                _renderView->makeSurfaceCurrent();
                onDeviceLost();
                paused = false;
                QueryPerformanceCounter(&_last);
            }
        }
        else
        {
            // The precision of timer on Windows is set to highest (1ms) by 'timeBeginPeriod' from above code,
            // but it's still not precise enough. For example, if the precision of timer is 1ms,
            // Sleep(3) may make a sleep of 2ms or 4ms. Therefore, we subtract 1ms here to make Sleep time shorter.
            // If 'waitMS' is equal or less than 1ms, don't sleep and run into next loop to
            // boost CPU to next frame accurately.
            waitMS = static_cast<LONG>((_animationInterval.QuadPart - interval) * 1000LL / _freq.QuadPart - 1L);
            if (waitMS > 1L)
            {
                std::unique_lock<std::mutex> lock(_loopMutex);
                _loopCondition.wait_for(lock, std::chrono::milliseconds(waitMS), [this]() {
                    return !isRenderLoopRunning() || _suspended || _boundaryTaskPending;
                });
            }
        }
    }

    return 0;
}

void Application::boot(SwapChainPanel const& panel)
{
    ax::Application::getInstance()->initContextAttrs();

    // Try to initialize a high-performance graphics driver first.
    // If any of the high-performance APIs (D3D11/D3D12/Vulkan/Metal) are enabled,
    // the runtime will attempt initialization in the default priority order.
    // If all attempts fail, OpenGL will then be explicitly selected as the fallback.
    ax::rhi::DriverContext::makeCurrentDriver();

    if (_renderLoopWorker != nullptr && _renderLoopWorker.Status() == AsyncStatus::Started)
        return;

    _panel = panel;
    resume();

    _dispatcher  = Window::Current().CoreWindow().Dispatcher();
    auto display = DisplayInformation::GetForCurrentView();
    Device::setDPI(display.LogicalDpi());
    _orientation     = display.CurrentOrientation();
    auto panelWidth  = panel.ActualWidth();
    auto panelHeight = panel.ActualHeight();

    auto axmolWinRTMain = [this, panelWidth, panelHeight](Windows::Foundation::IAsyncAction const& /*action*/) {
        auto dispatcher = _dispatcher.get();
        auto axmolApp   = ax::Application::getInstance();

        AX_ASSERT(!_renderView);
        auto director = ax::Director::getInstance();
        AX_ASSERT(!director->getRenderView());

        _renderLoopRunning = true;
        _appShouldExit.store(false, std::memory_order_release);
        _requestedTerminate = false;

        _renderView = RenderView::createWithRect(
            "axmol3", Rect(0, 0, static_cast<float>(panelWidth), static_cast<float>(panelHeight)));

        if (rhi::DriverContext::isOpenGL())
        {
            auto surfaceReady  = std::make_shared<std::promise<void>>();
            auto surfaceFuture = surfaceReady->get_future();
            dispatcher.RunAsync(CoreDispatcherPriority::High, [this, surfaceReady]() {
                try
                {
                    _renderView->createRenderSurface();
                    surfaceReady->set_value();
                }
                catch (...)
                {
                    surfaceReady->set_exception(std::current_exception());
                }
            });
            surfaceFuture.get();

            _renderView->makeSurfaceCurrent();
            rhi::DriverContext::activateCurrentDriver();
        }

        // must after egl surface created when not use d3d RHI
        director->setRenderView(_renderView);

        _renderView->registerEventHandlers();

        axmolApp->run();

        if (_renderView && !_requestedTerminate)
        {
            auto surfaceDestroyed = std::make_shared<std::promise<void>>();
            auto destroyFuture    = surfaceDestroyed->get_future();
            dispatcher.RunAsync(CoreDispatcherPriority::High, [this, surfaceDestroyed]() {
                if (_renderView)
                    _renderView->destroyRenderSurface();
                surfaceDestroyed->set_value();
            });
            destroyFuture.get();
        }

        _renderLoopRunning = false;
    };

    _renderLoopWorker = ThreadPool::RunAsync(axmolWinRTMain, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

void Application::shutdown()
{
    if (_renderLoopWorker)
    {
        _renderLoopWorker.Cancel();
        {
            std::unique_lock<std::mutex> lock(_loopMutex);
            _suspended           = false;
            _deviceLost          = false;
            _boundaryTaskPending = true;
            _renderLoopRunning   = false;
            _loopCondition.notify_all();
        }
        _renderLoopWorker = nullptr;
    }
}

void Application::suspend()
{
    std::unique_lock<std::mutex> lock(_loopMutex);
    _suspended = true;
    _loopCondition.notify_one();
}

void Application::resume()
{
    std::unique_lock<std::mutex> lock(_loopMutex);
    if (_suspended)
    {
        _suspended = false;
        _loopCondition.notify_one();
    }
}

void Application::requestQuit()
{
    std::unique_lock<std::mutex> lock(_loopMutex);
    _appShouldExit.store(true, std::memory_order_release);
    _loopCondition.notify_one();
}

bool Application::isRenderLoopRunning() const
{
    return _renderLoopRunning.load(std::memory_order_acquire);
}

void Application::postBoundaryTaskSignal()
{
    std::unique_lock<std::mutex> lock(_loopMutex);
    _boundaryTaskPending = true;
    _loopCondition.notify_one();
}

void Application::setAnimationInterval(float interval)
{
    QueryPerformanceFrequency(&_freq);
    _animationInterval.QuadPart = (LONGLONG)(interval * _freq.QuadPart);
}

const char* Application::getCurrentLanguageCode()
{
    static std::string code = "en";
    auto languages          = Windows::System::UserProfile::GlobalizationPreferences::Languages();
    code                    = PlatformStringToString(languages.GetAt(0));
    return code.c_str();
}

LanguageType Application::getCurrentLanguage()
{
    const char* code = getCurrentLanguageCode();

    return utils::getLanguageTypeByISO2(code);
}

Application::Platform Application::getTargetPlatform()
{
    return Platform::WinUWP;
}

std::string Application::getVersion()
{
    auto package   = winrt::Windows::ApplicationModel::Package::Current();
    auto packageId = package.Id();
    auto version   = packageId.Version();

    return fmt::format("{0}.{1}.{2}.{3}", version.Major, version.Minor, version.Build, version.Revision);
}

bool Application::openURL(std::string_view url)
{
    auto dispatcher = ax::RenderView::sharedRenderView()->getDispatcher();
    dispatcher.get().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, DispatchedHandler([url]() {
        auto uri = Windows::Foundation::Uri(PlatformStringFromString(url));
        Windows::System::Launcher::LaunchUriAsync(uri);
    }));
    return true;
}

void Application::onPause()
{
    applicationDidEnterBackground();
    ax::CustomEvent backgroundEvent(EVENT_COME_TO_BACKGROUND);
    ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&backgroundEvent, true);
}

void Application::onResume()
{
    auto director = ax::Director::getInstance();
    applicationWillEnterForeground();
    ax::CustomEvent foregroundEvent(EVENT_COME_TO_FOREGROUND);
    ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&foregroundEvent, true);
}

void Application::onDeviceLost()
{
    onPause();

    auto director = ax::Director::getInstance();

    axdrv->resetState();
    ax::CustomEvent recreatedEvent(EVENT_RENDERER_RECREATED);
    director->getEventDispatcher()->dispatchEvent(&recreatedEvent, true);
    director->setRenderDefaults();
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    ax::VolatileTextureMgr::reloadAllTextures();
#endif

    onResume();
}

}  // namespace ax
