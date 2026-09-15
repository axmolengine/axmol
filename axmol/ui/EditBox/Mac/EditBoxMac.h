/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 zilongshanren
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#include "axmol/ui/EditBox/EditBoxImpl-mac.h"
#include "axmol/ui/EditBox/Mac/TextInput.h"

#pragma mark - UIEditBox mac implementation

@class NSFont;

@interface UIEditBoxImplMac : NSObject <NSTextFieldDelegate, NSTextViewDelegate> {
    BOOL _editState;
    NSView<AxmolTextInput>* _textInput;
    void* _editBox;
}

@property(nonatomic, retain) NSView<AxmolTextInput>* textInput;
@property(nonatomic, readonly) NSWindow* window;

@property(nonatomic, readonly, getter=isEditState) BOOL editState;
@property(nonatomic, assign) void* editBox;
@property(nonatomic, assign) NSRect frameRect;
@property(nonatomic, assign) ax::ui::EditBox::InputFlag dataInputMode;
@property(nonatomic, assign) ax::ui::EditBox::KeyboardReturnType keyboardReturnType;

- (instancetype)initWithFrame:(NSRect)frameRect editBox:(void*)editBox;
- (void)setMaxLength:(int)maxLength;
- (void)updateFrame:(CGRect)rect;

- (void)openKeyboard;
- (void)closeKeyboard;

- (NSString*)getDefaultFontName;
- (ax::ui::EditBoxDelegate::EditBoxEndAction)getEndAction:(NSNotification*)notification;

- (void)setInputMode:(ax::ui::EditBox::InputMode)inputMode;
- (void)setInputFlag:(ax::ui::EditBox::InputFlag)inputFlag;
- (void)setReturnType:(ax::ui::EditBox::KeyboardReturnType)returnType;
- (void)setTextHorizontalAlignment:(ax::TextHAlignment)alignment;
- (void)setPlaceHolder:(NSString*)text;
- (void)setVisible:(BOOL)visible;
- (void)setTextColor:(NSColor*)color;
- (void)setFont:(NSFont*)font;
- (void)setPlaceholderFontColor:(NSColor*)color;
- (void)setPlaceholderFont:(NSFont*)font;
- (void)setText:(NSString*)text;
- (NSString*)getText;

@end
