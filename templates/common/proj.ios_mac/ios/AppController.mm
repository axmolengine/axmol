/****************************************************************************
 Copyright (c) 2010-2013 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmolengine.github.io/

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

#import "AppController.h"
#import "axmol.h"
#import "AppDelegate.h"
#import "RootViewController.h"

@implementation AppController

@synthesize window;

#pragma mark -
#pragma mark Application lifecycle

// axmol application instance
static AppDelegate s_sharedApplication;

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{

    ax::Application* app = ax::Application::getInstance();

    // Initialize the GLView attributes
    app->initGLContextAttrs();
    ax::GLViewImpl::convertAttrs();

    // Override point for customization after application launch.

    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

    // Use RootViewController to manage EAGLView
    _viewController                       = [[RootViewController alloc] init];
#if !defined(AX_TARGET_OS_TVOS)
    _viewController.wantsFullScreenLayout = YES;
#endif

    // Set RootViewController to window
    if ([[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview:_viewController.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:_viewController];
    }

    [window makeKeyAndVisible];

#if !defined(AX_TARGET_OS_TVOS)
    [[UIApplication sharedApplication] setStatusBarHidden:true];
#endif

    // Launching the app with the arguments -NSAllowsDefaultLineBreakStrategy NO to force back to the old behavior.
    if ([[UIDevice currentDevice].systemVersion floatValue] >= 13.0f)
    {
        [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"NSAllowsDefaultLineBreakStrategy"];
    }

    // IMPORTANT: Setting the GLView should be done after creating the RootViewController
    ax::GLView* glView = ax::GLViewImpl::createWithEAGLView((__bridge void*)_viewController.view);
    ax::Director::getInstance()->setGLView(glView);

    // run the cocos2d-x game scene
    app->run();

    return YES;
}

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
    ax::Application::getInstance()->applicationDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication*)application
{
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made
     on entering the background.
     */
    ax::Application::getInstance()->applicationWillEnterForeground();
}

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

#if __has_feature(objc_arc)
#else
- (void)dealloc
{
    [window release];
    [_viewController release];
    [super dealloc];
}
#endif

@end
