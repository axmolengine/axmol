/****************************************************************************
 Copyright (c) 2015 Mazyad Alabduljaleel
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#import "ui/UIEditBox/iOS/UITextField+UITextInput.h"

@implementation UITextField (AXUITextInput)

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

void LoadUITextFieldAXUITextInputCategory()
{
    // noop
}
