/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#import "axmol/platform/ios/AxmolAppController.h"
#import "axmol/platform/ios/AxmolSceneDelegate.h"

@implementation AxmolAppController

#pragma mark - UISceneSession Lifecycle

- (UISceneConfiguration*)application:(UIApplication*)application
    configurationForConnectingSceneSession:(UISceneSession*)connectingSceneSession
                                   options:(UISceneConnectionOptions*)options API_AVAILABLE(ios(13.0))
{
    // Attempt to read the configuration from the project Info.plist
    if ([connectingSceneSession.configuration.name isEqualToString:@"Default Configuration"])
    {
        return connectingSceneSession.configuration;
    }

    // Fallback is to create the configuration programmatically
    UISceneConfiguration* config = [[UISceneConfiguration alloc] initWithName:@"Default Configuration"
                                                                  sessionRole:connectingSceneSession.role];

    config.delegateClass = [AxmolSceneDelegate class];

    return config;
}

- (void)application:(UIApplication*)application
    didDiscardSceneSessions:(NSSet<UISceneSession*>*)sceneSessions API_AVAILABLE(ios(13.0))
{
    // Handle resource cleanup for discarded scene sessions here
}

#pragma mark -
#pragma mark Application lifecycle

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    return YES;
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

#if !__has_feature(objc_arc)
- (void)dealloc
{
    [super dealloc];
}
#endif

@end
