/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#import <UIKit/UIKit.h>

@interface AxmolViewController : UIViewController {
}
- (BOOL)prefersStatusBarHidden;

@property(nonatomic, strong) UIView* renderHostView;

@end
