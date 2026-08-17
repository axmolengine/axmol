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

#import "AxmolAppController.h"
#import "AxmolViewController.h"
#import "AxmolSceneDelegate.h"

#include "base/Director.h"
#include "platform/Application.h"
#include "platform/ios/RenderViewImpl-ios.h"

using namespace ax;

@implementation AxmolAppController

#pragma mark - UISceneSession Lifecycle

- (UISceneConfiguration *)application:(UIApplication *)application
configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession
                              options:(UISceneConnectionOptions *)options API_AVAILABLE(ios(13.0)) {
    
    // Attempt to read the configuration from the project Info.plist
    if ([connectingSceneSession.configuration.name isEqualToString:@"Default Configuration"]) {
        return connectingSceneSession.configuration;
    }
    
    // Fallback is to create the configuration programmatically
    UISceneConfiguration *config = [[UISceneConfiguration alloc] initWithName:@"AxmolDefaultConfiguration"
                                                                  sessionRole:connectingSceneSession.role];
    
    config.delegateClass = [AxmolSceneDelegate class];
    
    return config;
}

- (void)application:(UIApplication *)application didDiscardSceneSessions:(NSSet<UISceneSession *> *)sceneSessions API_AVAILABLE(ios(13.0)) {
    // Handle resource cleanup for discarded scene sessions here
}

#pragma mark -
#pragma mark Application lifecycle

- (UIViewController*)createRootViewController {
    return [[AxmolViewController alloc] initWithNibName:nil bundle:nil];
}

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    if (@available(iOS 13.0, *))
    {
        // do nothing here for iOS13+
    }
    else
    {
        auto axmolApp = Application::getInstance();
        
        // Initialize the RenderView attributes
        axmolApp->initGfxContextAttrs();
        
        // Override point for customization after application launch.
        
        auto renderView = ax::RenderViewImpl::createWithFullScreen("axmol2");
        _viewController = [self createRootViewController];
        
        renderView->showWindow(_viewController);
        
        // IMPORTANT: Setting the RenderView should be done after creating the RootViewController
        Director::getInstance()->setRenderView(renderView);
        
        // run the axmol game scene
        axmolApp->run();
    }
    return YES;
}

#if defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED < 130000
- (void)applicationWillResignActive:(UIApplication*)application
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

- (void)applicationDidBecomeActive:(UIApplication*)application
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was
     previously in the background, optionally refresh the user interface.
     */
    // We don't need to call this method any more. It will interrupt user defined game pause&resume logic
    /* ax::Director::getInstance()->resume(); */
}

- (void)applicationDidEnterBackground:(UIApplication*)application
{
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state
     information to restore your application to its current state in case it is terminated later. If your application
     supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    if (@available(iOS 13.0, *))
    {
        // do nothing here for iOS13+
    }
    else
    {
        ax::Application::getInstance()->applicationDidEnterBackground();
    }
}

- (void)applicationWillEnterForeground:(UIApplication*)application
{
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made
     on entering the background.
     */
    if (@available(iOS 13.0, *))
    {
        // do nothing here for iOS13+
    }
    else
    {
        ax::Application::getInstance()->applicationWillEnterForeground();
    }
}
#endif

- (void)applicationWillTerminate:(UIApplication*)application
{
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}

#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication*)application
{
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk)
     later.
     */
}

#if !__has_feature(objc_arc)
- (void)dealloc
{
    [_viewController release];
    [super dealloc];
}
#endif

@end
