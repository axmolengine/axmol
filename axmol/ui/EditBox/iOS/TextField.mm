/****************************************************************************
 Copyright (c) 2015 Mazyad Alabduljaleel
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#import "axmol/ui/EditBox/iOS/TextField.h"

@implementation UITextField (AxmolTextInput)

- (NSString*)axui_text
{
    return self.text;
}

- (void)axui_setText:(NSString*)axui_text
{
    self.text = axui_text;
}

- (NSString*)axui_placeholder
{
    return self.placeholder;
}

- (void)axui_setPlaceholder:(NSString*)axui_placeholder
{
    self.placeholder = axui_placeholder;
}

- (UIColor*)axui_textColor
{
    return self.textColor;
}

- (void)axui_setTextColor:(UIColor*)axui_textColor
{
    self.textColor = axui_textColor;
}

- (UIFont*)axui_font
{
    return self.font;
}

- (void)axui_setFont:(UIFont*)axui_font
{
    self.font = axui_font;
}

- (NSTextAlignment)axui_alignment
{
    return self.textAlignment;
}

- (void)axui_setTextHorizontalAlignment:(NSTextAlignment)axui_alignment
{
    self.textAlignment = axui_alignment;
}

- (UIColor*)axui_placeholderTextColor
{
    SEL selector = NSSelectorFromString(@"placeholderTextColor");
    if ([self respondsToSelector:selector])
    {
        return [self performSelector:selector];
    }
    return nil;
}

- (void)axui_setPlaceholderTextColor:(UIColor*)axui_placeholderTextColor
{
    SEL selector = NSSelectorFromString(@"setPlaceholderTextColor:");
    if ([self respondsToSelector:selector])
    {
        [self performSelector:selector withObject:axui_placeholderTextColor];
    }
}

- (UIFont*)axui_placeholderFont
{
    SEL selector = NSSelectorFromString(@"placeholderFont");
    if ([self respondsToSelector:selector])
    {
        return [self performSelector:selector];
    }
    return nil;
}

- (void)axui_setPlaceholderFont:(UIFont*)axui_placeholderFont
{
    SEL selector = NSSelectorFromString(@"setPlaceholderFont:");
    if ([self respondsToSelector:selector])
    {
        [self performSelector:selector withObject:axui_placeholderFont];
    }
}

- (BOOL)axui_secureTextEntry
{
    return self.secureTextEntry;
}

- (void)axui_setSecureTextEntry:(BOOL)axui_secureTextEntry
{
    self.secureTextEntry = axui_secureTextEntry;
}

- (void)axui_setDelegate:(id<UITextFieldDelegate, UITextViewDelegate>)delegate
{
    self.delegate = delegate;
}

@end
