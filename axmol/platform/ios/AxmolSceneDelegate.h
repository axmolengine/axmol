/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#import <UIKit/UIKit.h>

API_AVAILABLE(ios(13.0))
@interface AxmolSceneDelegate : UIResponder <UIWindowSceneDelegate>

@property(strong, nonatomic) UIWindow* window;

@property(nonatomic, readonly) UIViewController* viewController;

@end
