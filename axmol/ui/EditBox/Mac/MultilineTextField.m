/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 zilongshanren
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#import "axmol/ui/EditBox/Mac/MultilineTextField.h"

@interface AxmolMultilineTextField ()
@property(nonatomic, copy) NSString* placeHolder;
@end

@implementation AxmolMultilineTextField {
}

@synthesize placeHolder = _placeHolder;

- (void)dealloc
{
    self.placeHolder = nil;

    [super dealloc];
}

- (void)axui_setPlaceholderFont:(NSFont*)font
{
    // TODO
}

- (NSFont*)axui_placeholderFont
{
    return NULL;
}

- (void)axui_setPlaceholder:(NSString*)text
{
    self.placeHolder = text;
}

- (NSString*)axui_placeholder
{
    return self.placeHolder;
}

- (void)axui_setPlaceholderColor:(NSColor*)color
{
    // TODO
}

- (NSColor*)axui_placeholderColor
{
    return NULL;
}

#pragma mark - AxmolTextInput
- (NSString*)axui_text
{
    return self.string;
}

- (void)axui_setText:(NSString*)axui_text
{
    self.string = axui_text;
}

- (NSColor*)axui_textColor
{
    return self.textColor;
}

- (void)axui_setTextColor:(NSColor*)axui_textColor
{
    self.textColor = axui_textColor;
}

- (NSFont*)axui_font
{
    return self.font;
}

- (void)axui_setFont:(NSFont*)axui_font
{
    self.font = axui_font;
}

- (NSTextAlignment)axui_alignment
{
    return self.alignment;
}

- (void)axui_setTextHorizontalAlignment:(NSTextAlignment)axui_alignment
{
    self.alignment = axui_alignment;
}

- (void)axui_setDelegate:(id<NSTextFieldDelegate, NSTextViewDelegate>)delegate
{
    self.delegate = delegate;
}

- (void)axui_setMaxLength:(int)axui_maxLength
{
    // noop
}
- (int)axui_maxLength
{
    return 0;
}
@end
