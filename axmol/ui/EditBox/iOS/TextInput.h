/****************************************************************************
 Copyright (c) 2015 Mazyad Alabduljaleel
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

static const int AX_EDIT_BOX_PADDING = 5;

/**
    This protocol provides a common interface for consolidating text input method calls
 */
@protocol AxmolTextInput <NSObject>

@property(nonatomic, retain, setter=axui_setText:) NSString* axui_text;
@property(nonatomic, retain, setter=axui_setPlaceholder:) NSString* axui_placeholder;
@property(nonatomic, retain, setter=axui_setTextColor:) UIColor* axui_textColor;
@property(nonatomic, retain, setter=axui_setFont:) UIFont* axui_font;
@property(nonatomic, retain, setter=axui_setPlaceholderTextColor:) UIColor* axui_placeholderTextColor;
@property(nonatomic, retain, setter=axui_setPlaceholderFont:) UIFont* axui_placeholderFont;
@property(nonatomic, assign, setter=axui_setSecureTextEntry:) BOOL axui_secureTextEntry;
@property(nonatomic, assign, setter=axui_setTextHorizontalAlignment:) NSTextAlignment axui_alignment;

/* can only set. and not get, since the getter will only return one delegate implementation */
- (void)axui_setDelegate:(id<UITextFieldDelegate, UITextViewDelegate>)delegate;

@end
