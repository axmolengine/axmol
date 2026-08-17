/****************************************************************************
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

#include "AppDelegate.h"
#include "MainScene.h"

#define USE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE
#    include "audio/AudioEngine.h"
#endif

#if _AX_TESTS
#   include "doctest/doctest.h"
#endif

using namespace ax;

static ax::Size designResolutionSize = ax::Size(1280, 720);

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

// if you want a different context, modify the value of gfxContextAttrs
// it will affect all platforms
void AppDelegate::initGfxContextAttrs()
{
    // set graphics context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GfxContextAttrs gfxContextAttrs = {8, 8, 8, 8, 24, 8, 0};
    // since axmol-2.2 vsync was enabled in engine by default
    // gfxContextAttrs.vsync = false;

    RenderView::setGfxContextAttrs(gfxContextAttrs);
}

void AppDelegate::initRenderContext()
{
    auto director = Director::getInstance();
    auto renderView   = director->getRenderView();
    if (!renderView)
    {
#if (AX_TARGET_PLATFORM != AX_PLATFORM_ANDROID) && (AX_TARGET_PLATFORM != AX_PLATFORM_IOS)
        renderView = RenderViewImpl::createWithRect(
            "Dummy", ax::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        renderView = RenderViewImpl::create("Dummy");
#endif
        director->setRenderView(renderView);
    }
    
    // Set the design resolution
    renderView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
                                    ResolutionPolicy::SHOW_ALL);   
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director = Director::getInstance();
    
    auto renderView = director->getRenderView();
    if (!renderView)
    {
        initRenderContext();
    }

    // turn on display FPS
    director->setStatsDisplay(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

#if !_AX_TESTS
    // create a scene. it's an autorelease object
    auto scene = utils::createInstance<MainScene>();

    // run
    director->runWithScene(scene);
#endif

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#endif
}

void AppDelegate::applicationWillQuit() {}

#if _AX_TESTS
int AppDelegate::run(int argc, char** argv) {
    AXLOGI("Running unit tests...\n");
    fflush(stdout);
    AXLOGI("Default resource path: {}\n", FileUtils::getInstance()->getDefaultResourceRootPath());
    AXLOGI("Writable path: {}\n", FileUtils::getInstance()->getWritablePath());
    {
        for (auto& path : FileUtils::getInstance()->getSearchPaths())
            AXLOGI("Search path: {}\n", path);
    }
    fflush(stdout);

    ax::Director::getInstance()->init();

    doctest::Context context;

    //context.addFilter("test-case-exclude", "*math*"); // exclude test cases with "math" in their name
    //context.setOption("abort-after", 5);              // stop test execution after 5 failed assertions

    //context.setOption("order-by", "name");            // sort the test cases by their name

    context.applyCommandLine(argc, argv);

    // overrides
    context.setOption("no-breaks", true);             // don't break in the debugger when assertions fail

    int res = context.run(); // run
    return res;
}
#endif
