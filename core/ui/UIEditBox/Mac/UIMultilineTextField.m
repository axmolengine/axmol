/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 zilongshanren
 
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

#import "ui/UIEditBox/Mac/UIMultilineTextField.h"

@interface CCUIMultilineTextField()
@property(nonatomic, copy)NSString* placeHolder;
@end

@implementation CCUIMultilineTextField
{
}

@synthesize placeHolder = _placeHolder;

-(void)dealloc
{
    self.placeHolder = nil;
    
    [super dealloc];
}

-(void)axui_setPlaceholderFont:(NSFont *)font
{
    //TODO
}

-(NSFont*)axui_placeholderFont
{
    return NULL;
}

-(void)axui_setPlaceholder:(NSString *)text
{
    self.placeHolder = text;
}

-(NSString*)axui_placeholder
{
    return self.placeHolder;
}

-(void)axui_setPlaceholderColor:(NSColor *)color
{
    //TODO
}

-(NSColor*)axui_placeholderColor
{
    return NULL;
}


#pragma mark - AXUITextInput
- (NSString *)axui_text
{
    return self.string;
}

- (void)axui_setText:(NSString *)axui_text
{
    self.string = axui_text;
}

- (NSColor *)axui_textColor
{
    return self.textColor;
}

- (void)axui_setTextColor:(NSColor *)axui_textColor
{
    self.textColor = axui_textColor;
}

- (NSFont *)axui_font
{
    return self.font;
}

- (void)axui_setFont:(NSFont *)axui_font
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


- (void)axui_setDelegate:(id<NSTextFieldDelegate,NSTextViewDelegate>)delegate
{
    self.delegate = delegate;
}

- (void)axui_setMaxLength:(int)axui_maxLength
{
    //noop
}
-(int)axui_maxLength
{
    return 0;
}
@end
