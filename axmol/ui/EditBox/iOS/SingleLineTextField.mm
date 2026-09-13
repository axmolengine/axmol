/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 James Chen
 Copyright (c) 2015 Mazyad Alabduljaleel
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#import "axmol/ui/EditBox/iOS/SingleLineTextField.h"
#import "axmol/ui/EditBox/iOS/TextInput.h"

#include "axmol/base/Director.h"

/**
 * http://stackoverflow.com/questions/18244790/changing-uitextfield-placeholder-font
 */

@implementation AxmolSingleLineTextField

#pragma mark - Init & Dealloc

- (void)dealloc
{
    [_placeholderFont release];
    [_placeholderTextColor release];

    [super dealloc];
}

#pragma mark - Properties

- (UIColor*)placeholderTextColor
{
    return _placeholderTextColor;
}

- (UIFont*)placeholderFont
{
    return _placeholderFont;
}

#pragma mark - Public methods

- (void)drawPlaceholderInRect:(CGRect)rect
{
    NSDictionary* attributes =
        @{NSForegroundColorAttributeName : _placeholderTextColor, NSFontAttributeName : _placeholderFont};

    // center vertically
    CGSize textSize = [self.placeholder sizeWithAttributes:attributes];
    CGFloat hdif    = rect.size.height - textSize.height;
    hdif            = MAX(0, hdif);
    rect.origin.y += ceil(hdif / 2.0);

    [[self placeholder] drawInRect:rect withAttributes:attributes];
}

- (CGRect)textRectForBounds:(CGRect)bounds
{
    auto renderView = ax::Director::getInstance()->getRenderView();

    float padding = AX_EDIT_BOX_PADDING * renderView->getScaleX() / renderView->getRenderScale();
    return CGRectInset(bounds, padding, padding);
}

- (CGRect)editingRectForBounds:(CGRect)bounds
{
    return [self textRectForBounds:bounds];
}

@end
