/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 James Chen
 Copyright (c) 2013-2015 zilongshanren
 Copyright (c) 2015 Mazyad Alabduljaleel
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#import <UIKit/UIKit.h>
#import "axmol/ui/EditBox/iOS/TextInput.h"
#include "axmol/ui/EditBox/EditBoxImpl-ios.h"

@interface UIEditBoxImplIOS_objc : NSObject <UITextFieldDelegate, UITextViewDelegate>

@property(nonatomic, retain) UIView<UITextInput, AxmolTextInput>* textInput;
@property(nonatomic, assign) void* editBox;
@property(nonatomic, assign) NSString* text;
@property(nonatomic, assign) CGRect frameRect;
@property(nonatomic, assign) ax::ui::EditBox::InputFlag dataInputMode;
@property(nonatomic, assign) ax::ui::EditBox::KeyboardReturnType keyboardReturnType;
@property(nonatomic, readonly, getter=isEditState) BOOL editState;
@property(nonatomic, readwrite) BOOL returnPressed;

- (instancetype)initWithFrame:(CGRect)frameRect editBox:(void*)editBox;
- (void)doAnimationWhenKeyboardMoveWithDuration:(float)duration distance:(float)distance;

- (NSString*)getDefaultFontName;
- (ax::ui::EditBoxDelegate::EditBoxEndAction)getEndAction;

- (void)setInputMode:(ax::ui::EditBox::InputMode)inputMode;
- (void)setInputFlag:(ax::ui::EditBox::InputFlag)flag;
- (void)setReturnType:(ax::ui::EditBox::KeyboardReturnType)returnType;
- (void)setTextHorizontalAlignment:(ax::TextHAlignment)alignment;

- (void)setPlaceHolder:(NSString*)text;
- (void)setPlaceholderFont:(UIFont*)font;
- (void)setPlaceholderTextColor:(UIColor*)color;
- (void)setVisible:(BOOL)visible;
- (void)setTextColor:(UIColor*)color;
- (void)setFont:(UIFont*)font;
- (void)updateFrame:(CGRect)rect;

- (void)openKeyboard;
- (void)closeKeyboard;

@end
