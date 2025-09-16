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

#define USE_VR_RENDERER  0
#define USE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE
#    include "axmol/audio/AudioEngine.h"
#endif

#if USE_VR_RENDERER && defined(AX_ENABLE_VR)
#    include "axmol/vr/VRGenericRenderer.h"
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
    // powerPreference only affect when RHI backend is D3D
    GfxContextAttrs gfxContextAttrs = {.powerPreference = PowerPreference::HighPerformance};

    // V-Sync is enabled by default since axmol 2.2.
    // Uncomment to disable V-Sync and unlock FPS.
    // gfxContextAttrs.vsync = false;

    // Enable high-DPI scaling support (non-Windows platforms only)
    // Note: cpp-tests keep the default render mode to ensure consistent performance benchmarks
#if AX_TARGET_PLATFORM != AX_PLATFORM_WIN32
    gfxContextAttrs.renderScaleMode = RenderScaleMode::Physical;
#endif
    RenderView::setGfxContextAttrs(gfxContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director   = Director::getInstance();
    auto renderView = director->getRenderView();
    if (!renderView)
    {
#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC) || \
    (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
        renderView = RenderViewImpl::createWithRect(
            "Dummy", ax::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        renderView = RenderViewImpl::create("Dummy");
#endif
        director->setRenderView(renderView);
    }
#if USE_VR_RENDERER && defined(AX_ENABLE_VR)
    auto vrRenderer = std::make_unique<VRGenericRenderer>();
    // On Android/iOS emulator devices, uncomment to visualize the left/right eye VR rendering output.
    // Useful for debugging stereo rendering without a physical headset.
    // vrRenderer->setDebugIgnoreHeadTracker(true);
    renderView->setVR(std::move(vrRenderer));
#endif

    // turn on display FPS
    director->setStatsDisplay(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    renderView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height,
                                        ResolutionPolicy::SHOW_ALL);

    // create a scene. it's an autorelease object
    auto scene = utils::createInstance<MainScene>();

    // run
    director->runWithScene(scene);

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
