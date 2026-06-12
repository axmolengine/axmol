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
#pragma once

#include "axmol/platform/PlatformConfig.h"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT)

#    include "axmol/platform/StdC.h"
#    include "axmol/platform/Common.h"
#    include "axmol/platform/ApplicationCore.h"
#    include <string>
#    include <functional>
#    include <condition_variable>
#    include <mutex>
#    include <atomic>
#    include <chrono>

#    include <winrt/Windows.Foundation.h>
#    include <winrt/Windows.Graphics.Display.h>
#    include <winrt/Windows.UI.Core.h>
#    include <winrt/Windows.UI.Xaml.Controls.h>

namespace ax
{

class RenderView;

class AX_DLL Application : public ApplicationCore
{
public:
    Application();
    virtual ~Application();

    /**
    @brief    Run the message loop.
    */
    int run();

    void boot(winrt::Windows::UI::Xaml::Controls::SwapChainPanel const& panel);
    void shutdown();
    void suspend();
    void resume();
    void requestQuit();

    /* override functions */
    void setAnimationInterval(float interval) override;

    LanguageType getCurrentLanguage() override;
    const char* getCurrentLanguageCode() override;

    /**
     @brief Get target platform
     */
    Platform getTargetPlatform() override;

    /**
     @brief Get application version
     */
    std::string getVersion() override;

    /**
     @brief Open url in default browser
     @param String with url to open.
     @return true if the resource located by the URL was successfully opened; otherwise false.
     */
    virtual bool openURL(std::string_view url);

protected:
    friend class RenderView;

    void postBoundaryTaskSignal() override;

    void onPause();
    void onResume();
    void onDeviceLost();

    winrt::agile_ref<winrt::Windows::UI::Xaml::Controls::SwapChainPanel> getPanel() const { return _panel; }
    winrt::agile_ref<winrt::Windows::UI::Core::CoreDispatcher> getDispatcher() const { return _dispatcher; }
    winrt::Windows::Graphics::Display::DisplayOrientations getOrientation() const { return _orientation; }

    LARGE_INTEGER _animationInterval;
    LARGE_INTEGER _freq;
    LARGE_INTEGER _last;
    LARGE_INTEGER _now;

    std::string _resourceRootPath;

    std::function<void(const winrt::hstring&)> _openURLDelegate;

    winrt::agile_ref<winrt::Windows::UI::Xaml::Controls::SwapChainPanel> _panel;
    winrt::agile_ref<winrt::Windows::UI::Core::CoreDispatcher> _dispatcher;
    winrt::Windows::Graphics::Display::DisplayOrientations _orientation{
        winrt::Windows::Graphics::Display::DisplayOrientations::Landscape};
    winrt::Windows::Foundation::IAsyncAction _renderLoopWorker{nullptr};
    RenderView* _renderView{nullptr};

    std::mutex _loopMutex;
    std::condition_variable _loopCondition;
    bool _suspended{false};
    bool _deviceLost{false};
    std::atomic_bool _appShouldExit{false};
    bool _requestedTerminate{false};
    bool _boundaryTaskPending{false};
    std::atomic_bool _renderLoopRunning{false};

    bool isRenderLoopRunning() const;
};

}  // namespace ax

#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
