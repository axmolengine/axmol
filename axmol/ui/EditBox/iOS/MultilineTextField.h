/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 James Chen
 Copyright (c) 2015 Mazyad Alabduljaleel
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#import <UIKit/UIKit.h>
#import "axmol/ui/EditBox/iOS/TextView.h"

#pragma mark - AxmolMultilineTextField implementation

@interface AxmolMultilineTextField : UITextView

@property(nonatomic, assign) NSString* placeholder;
@property(nonatomic, retain) UILabel* placeHolderLabel;

@end
