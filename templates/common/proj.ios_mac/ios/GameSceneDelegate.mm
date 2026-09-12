/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#import "GameSceneDelegate.h"
#import "GameViewController.h"

API_AVAILABLE(ios(13.0))
@implementation GameSceneDelegate

- (UIViewController*)createRootViewController
{
    GameViewController* viewController = [[GameViewController alloc] init];
    return viewController;
}

@end
