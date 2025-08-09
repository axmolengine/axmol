/****************************************************************************
 Copyright (c) 2013      cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#include <string>
#include "AppDelegate.h"

#include "axmol.h"
#include "controller.h"
#include "BaseTest.h"
#include "extensions/axmol-ext.h"

#include <charconv>
#include <system_error>

using namespace ax;

AppDelegate::AppDelegate() : _testController(nullptr) {}

AppDelegate::~AppDelegate()
{

    AXLOGI("AppDelegate::~AppDelegate");
    // SimpleAudioEngine::end();
    // TODO: minggo
    //  cocostudio::ArmatureDataManager::destroyInstance();
}

// if you want a different context, modify the value of gfxContextAttrs
// it will affect all platforms
void AppDelegate::initGfxContextAttrs()
{
    // set graphics context attributes: red,green,blue,alpha,depth,stencil
    GfxContextAttrs gfxContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    RenderView::setGfxContextAttrs(gfxContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // Enable logging output colored text style and prefix timestamp
    ax::setLogFmtFlag(ax::LogFmtFlag::Full);

    // whether enable global SDF font render support, since axmol-2.0.1
    FontFreeType::setShareDistanceFieldEnabled(true);

    // As an example, load config file
    // FIXME:: This should be loaded before the Director is initialized,
    // FIXME:: but at this point, the director is already initialized
    Configuration::getInstance()->loadConfigFile("configs/config-example.plist");

    // initialize director
    auto director = Director::getInstance();
    auto renderView   = director->getRenderView();
    if (!renderView)
    {
        std::string title = "Cpp Tests";
#ifndef NDEBUG
        title += " *Debug*",
#endif
#ifdef AX_PLATFORM_PC
        renderView = RenderViewImpl::createWithRect(title, Rect(0, 0, g_resourceSize.width, g_resourceSize.height), 1.0F, true);
#else
        renderView = RenderViewImpl::createWithRect(title, Rect(0, 0, g_resourceSize.width, g_resourceSize.height));
#endif
        director->setRenderView(renderView);
    }

    const char* const autotest_capture =
        std::getenv("AXMOL_AUTOTEST_CAPTURE_DIR");
    director->setStatsDisplay(!autotest_capture || !autotest_capture[0]);

#ifdef AX_PLATFORM_PC
    director->setAnimationInterval(1.0f / glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate);
#else
    director->setAnimationInterval(1.0f / 60);
#endif

    auto screenSize = renderView->getFrameSize();

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
    fileUtils->setSearchPaths(searchPaths);

    renderView->setDesignResolutionSize(g_designSize.width, g_designSize.height, ResolutionPolicy::SHOW_ALL);

    director->setClearColor(Color32{0x36, 0x3B, 0x44, 0xFF});

    // Enable Remote Console
    auto console = director->getConsole();
    console->listenOnTCP(5678);

    _testController = TestController::getInstance();

    const char* const autotest_env = std::getenv("AXMOL_START_AUTOTEST");
    int autotest = 0;
    if (autotest_env)
    {
      const std::from_chars_result r =
          std::from_chars
          (autotest_env, autotest_env + std::strlen(autotest_env), autotest);
      if (r.ec != std::errc{})
        AXLOGW("Could not parse AXMOL_START_AUTOTEST: {}.",
               std::make_error_code(r.ec).message());

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
