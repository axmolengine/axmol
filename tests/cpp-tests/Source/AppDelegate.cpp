/****************************************************************************
 Copyright (c) 2013      cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#include <string>
#include "AppDelegate.h"

#include "controller.h"
#include "BaseTest.h"
#include "extensions/axmol-ext.h"
#include "axmol/rhi/DriverContext.h"
#include "axmol/tlx/charconv.hpp"
#include "axmol/platform/CommandLineArgs.h"
#include <system_error>

using namespace ax;

AppDelegate::AppDelegate() : _testController(nullptr) {}

static DriverPreference parseDriverPreference(std::span<const std::string_view> args)
{
    for (int i = 1; i < args.size(); ++i)
    {
        std::string_view arg = args[i];
        if (arg.starts_with("--force-"))
        {
            std::string_view backend = arg.substr(8);
            if (backend == "opengl" || backend == "gles")
                return DriverPreference::OpenGL;
            if (backend == "d3d11")
                return DriverPreference::D3D11;
            if (backend == "d3d12")
                return DriverPreference::D3D12;
            if (backend == "vulkan")
                return DriverPreference::Vulkan;
            if (backend == "metal")
                return DriverPreference::Metal;
        }
    }
    return DriverPreference::Auto;
}

AppDelegate::~AppDelegate()
{
    AXLOGI("AppDelegate::~AppDelegate");
}

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 && defined(_UNICODE) && !defined(_CONSOLE)
int AppDelegate::launch(int argc, wchar_t** argv)
{
    CommandLineArgs args;
    args.buildFromWargv(argc, argv);
    return launch(args);
}
#else
int AppDelegate::launch(int argc, char** argv)
{
    CommandLineArgs args;
    args.buildViewsFromArgv(argc, argv);
    return launch(args);
}
#endif

int AppDelegate::launch(const ax::CommandLineArgs& args)
{
    _driverPreference = parseDriverPreference(args.views());

    return run();
}

// if you want a different context, modify the value of contextAttrs
// it will affect all platforms
void AppDelegate::initContextAttrs()
{
    // set vulkan min android api level, 31 for Android 12
    // refer: https://developer.android.com/tools/releases/platforms
    rhi::DriverContext::setVulkanMinAndroidApiLevel(31);

    // set app context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    // powerPreference only affect when RHI backend is D3D11, D3D12, Vulkan
    ContextAttrs contextAttrs = {.debugLayerEnabled = false, .powerPreference = PowerPreference::HighPerformance};

    // V-Sync is enabled by default since axmol 2.2.
    // Uncomment to disable V-Sync and unlock FPS.
    // contextAttrs.vsync = false;

    contextAttrs.driverPreference = _driverPreference;

    // Enable high-DPI scaling support (non-win32 platforms only)
    // Note: on win32, cpp-tests keep the default render mode to ensure consistent performance benchmarks
#if AX_TARGET_PLATFORM != AX_PLATFORM_WIN32
    contextAttrs.renderScaleMode = RenderScaleMode::Physical;
#endif
    setContextAttrs(contextAttrs);

    Device::setPreferredOrientation(Device::Orientation::SensorLandscape);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // Enable logging output colored text style and prefix timestamp
    ax::setLogFmtFlag(ax::LogFmtFlag::Full);

    // whether enable global SDF font render support, since axmol-2.0.1
    FontFreeType::setGlobalSDFEnabled(true);

    // As an example, load config file
    // FIXME:: This should be loaded before the Director is initialized,
    // FIXME:: but at this point, the director is already initialized
    Environment::getInstance()->loadConfigFile("configs/config-example.plist");

    // initialize director
    auto director   = Director::getInstance();
    auto renderView = director->getRenderView();
    if (!renderView)
    {
        std::string title = "Cpp Tests";
#ifndef NDEBUG
        title += " *Debug*";
#endif
#ifdef AX_PLATFORM_GLFW
        renderView =
            RenderViewImpl::createWithRect(title, Rect(0, 0, g_resourceSize.width, g_resourceSize.height), 1.0F, true);
#else
        renderView = RenderViewImpl::createWithRect(title, Rect(0, 0, g_resourceSize.width, g_resourceSize.height));
#endif
        director->setRenderView(renderView);

        title += fmt::format("({}@{})", axdrv->getVersion(), axdrv->getRenderer());
        renderView->setViewName(title);
    }

    const char* const autotest_capture = std::getenv("AXMOL_AUTOTEST_CAPTURE_DIR");
    director->setStatsDisplay(!autotest_capture || !autotest_capture[0]);

    director->setAnimationInterval(1.0f / Device::getDisplayRefreshRate());

    auto screenSize = renderView->getRenderSize();

    AXLOGI("AppDelegate::applicationDidFinishLaunching - Screen size: {} x {}", screenSize.width, screenSize.height);

    auto fileUtils = FileUtils::getInstance();
    std::vector<std::string> searchPaths;

    if (screenSize.height > 320)
    {
        searchPaths.emplace_back("hd");
        searchPaths.emplace_back("ccs-res/hd");
        searchPaths.emplace_back("ccs-res");
        searchPaths.emplace_back("Manifests");
        director->setContentScaleFactor(g_resourceSize.height / g_designSize.height);

        searchPaths.emplace_back("hd/ActionTimeline");
    }
    else
    {
        searchPaths.emplace_back("ccs-res");

        searchPaths.emplace_back("ActionTimeline");
    }

    auto&& oldSearchPaths = fileUtils->getSearchPaths();
    std::copy(oldSearchPaths.begin(), oldSearchPaths.end(), std::back_inserter(searchPaths));
    fileUtils->setSearchPaths(std::move(searchPaths));

    renderView->setDesignResolutionSize(g_designSize.width, g_designSize.height, ResolutionPolicy::SHOW_ALL);

    director->setClearColor(g_testsDefaultClearColor);

    // Enable Remote Console
    auto console = director->getConsole();
    console->listenOnTCP(5678);

    _testController = TestController::getInstance();

    const char* const autotest_env = std::getenv("AXMOL_START_AUTOTEST");
    int autotest                   = 0;
    if (autotest_env)
    {
        const tlx::from_chars_result r = tlx::from_chars(std::string_view{autotest_env}, autotest);
        if (r.ec != std::errc{})
            AXLOGW("Could not parse AXMOL_START_AUTOTEST: {}.", std::make_error_code(r.ec).message());
    }

    if (autotest != 0)
    {
        _testController->startAutoTest();
    }

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground()
{
    if (_testController)
    {
        //        _testController->onEnterBackground();
    }

    Director::getInstance()->stopAnimation();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    if (_testController)
    {
        //        _testController->onEnterForeground();
    }

    Director::getInstance()->startAnimation();
}

void AppDelegate::applicationWillQuit()
{
    TestController::destroyInstance();
}

void AppDelegate::applicationScreenSizeChanged(int newWidth, int newHeight)
{
    AXLOGI("AppDelegate::applicationScreenSizeChanged: ({},{})", newWidth, newHeight);
}
