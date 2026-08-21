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
#import "AxmolSceneDelegate.h"
#import "RenderViewImpl-ios.h"
#import "AxmolViewController.h"
#import "AxmolLauncher.h"

#include "platform/Application.h"

using namespace ax;

API_AVAILABLE(ios(13.0))
@implementation AxmolSceneDelegate

- (UIViewController*)createRootViewController
{
    return [[AxmolViewController alloc] initWithNibName:nil bundle:nil];
}

- (void)scene:(UIScene *)scene willConnectToSession:(UISceneSession *)session options:(UISceneConnectionOptions *)connectionOptions {
    
    UIWindowScene *windowScene = (UIWindowScene *)scene;
    if (![windowScene isKindOfClass:[UIWindowScene class]])
        return;

    _window = [[UIWindow alloc] initWithWindowScene:windowScene];
    _viewController = [self createRootViewController];

    AxmolLauncher::launchApp(_viewController, _window);
}

- (void)sceneDidDisconnect:(UIScene *)scene
{}

- (void)sceneDidBecomeActive:(UIScene *)scene
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was
     previously in the background, optionally refresh the user interface.
     */
    // We don't need to call this method any more. It will interrupt user defined game pause&resume logic
    /* ax::Director::getInstance()->resume(); */
}

- (void)sceneWillResignActive:(UIScene *)scene
{
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of
     temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and
     it begins the transition to the background state. Use this method to pause ongoing tasks, disable timers, and
     throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    // We don't need to call this method any more. It will interrupt user defined game pause&resume logic
    /* ax::Director::getInstance()->pause(); */
}

- (void)sceneWillEnterForeground:(UIScene *)scene
{
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made
     on entering the background.
     */
    ax::Application::getInstance()->applicationWillEnterForeground();
}

- (void)sceneDidEnterBackground:(UIScene *)scene
{
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state
     information to restore your application to its current state in case it is terminated later. If your application
     supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    ax::Application::getInstance()->applicationDidEnterBackground();
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
    [_viewController release];
    [_window release];
    [super dealloc];
}
#endif


@end
