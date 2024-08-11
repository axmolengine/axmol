/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 zilongshanren
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

#ifndef AXUITextInput_h
#define AXUITextInput_h

/**
 This protocol provides a common interface for consolidating text input method calls
 */
@protocol AXUITextInput <NSObject>

@property(nonatomic, retain, setter=axui_setText:) NSString* axui_text;
@property(nonatomic, retain, setter=axui_setTextColor:) NSColor* axui_textColor;
@property(nonatomic, retain, setter=axui_setFont:) NSFont* axui_font;
@property(nonatomic, assign, setter=axui_setMaxLength:) int axui_maxLength;
@property(nonatomic, retain, setter=axui_setPlaceholder:) NSString* axui_placeholder;
@property(nonatomic, retain, setter=axui_setPlaceholderColor:) NSColor* axui_placeholderColor;
@property(nonatomic, retain, setter=axui_setPlaceholderFont:) NSFont* axui_placeholderFont;
@property(nonatomic, assign, setter=axui_setTextHorizontalAlignment:) NSTextAlignment axui_alignment;

/* can only set. and not get, since the getter will only return one delegate implementation */
- (void)axui_setDelegate:(id<NSTextFieldDelegate, NSTextViewDelegate>)delegate;

@end

#endif /* AXUITextInput_h */
