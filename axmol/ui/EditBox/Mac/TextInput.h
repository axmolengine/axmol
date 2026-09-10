/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 zilongshanren
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

/**
 This protocol provides a common interface for consolidating text input method calls
 */
@protocol AxmolTextInput <NSObject>

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
