/****************************************************************************
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
#import "AxmolLauncher.h"
#import "AxmolViewController.h"
#import "axmol.h"
#import "RenderViewImpl-ios.h"

#include "base/Director.h"
#include "platform/Application.h"

namespace AxmolLauncher
{
void launchApp(UIViewController* viewController, UIWindow* windowHandle) 
{
    auto axmolApp = ax::Application::getInstance();
    axmolApp->initGfxContextAttrs();
    
    auto renderView = ax::RenderViewImpl::createWithFullScreen("axmol2");
    
    // Only set Window if a handle was provided (iOS13+ SceneDelegate path)
    if (windowHandle) 
        renderView->setEAWindow((__bridge void*)windowHandle);

    renderView->showWindow(viewController);
    
    // IMPORTANT: Must be done after creating RootViewController
    ax::Director::getInstance()->setRenderView(renderView);

    axmolApp->run();
}
}
