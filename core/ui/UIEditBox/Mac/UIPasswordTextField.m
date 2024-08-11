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

#import "ui/UIEditBox/Mac/UIPasswordTextField.h"
#include "ui/UIEditBox/Mac/UITextFieldFormatter.h"

@interface RSVerticallyCenteredSecureTextFieldCell : NSSecureTextFieldCell
{
    BOOL mIsEditingOrSelecting;
}

@end

@implementation RSVerticallyCenteredSecureTextFieldCell

- (NSRect)drawingRectForBounds:(NSRect)theRect
{
    // Get the parent's idea of where we should draw
    NSRect newRect = [super drawingRectForBounds:theRect];

    // When the text field is being
    // edited or selected, we have to turn off the magic because it screws up
    // the configuration of the field editor.  We sneak around this by
    // intercepting selectWithFrame and editWithFrame and sneaking a
    // reduced, centered rect in at the last minute.
    if (mIsEditingOrSelecting == NO)
    {
        // Get our ideal size for current text
        NSSize textSize = [self cellSizeForBounds:theRect];

        // Center that in the proposed rect
        float heightDelta = newRect.size.height - textSize.height;
        if (heightDelta > 0)
        {
            newRect.size.height -= heightDelta;
            newRect.origin.y += (heightDelta / 2);
        }
    }

    return newRect;
}

- (void)selectWithFrame:(NSRect)aRect
                 inView:(NSView *)controlView
                 editor:(NSText *)textObj
               delegate:(id)anObject
                  start:(NSInteger)selStart
                 length:(NSInteger)selLength
{
    aRect = [self drawingRectForBounds:aRect];
    mIsEditingOrSelecting = YES;
    [super selectWithFrame:aRect
                    inView:controlView
                    editor:textObj
                  delegate:anObject
                     start:selStart
                    length:selLength];
    mIsEditingOrSelecting = NO;
}

- (void)editWithFrame:(NSRect)aRect
               inView:(NSView *)controlView
               editor:(NSText *)textObj
             delegate:(id)anObject
                event:(NSEvent *)theEvent
{
    aRect = [self drawingRectForBounds:aRect];
    mIsEditingOrSelecting = YES;
    [super editWithFrame:aRect
                  inView:controlView
                  editor:textObj
                delegate:anObject
                   event:theEvent];
    mIsEditingOrSelecting = NO;
}

@end

@interface CCUIPasswordTextField()
{

}

@end

@implementation CCUIPasswordTextField

-(id) initWithFrame:(NSRect)frameRect
{
    if (self = [super initWithFrame:frameRect]) {
        [self setLineBreakMode:NSLineBreakByTruncatingTail];
    }
    
    return self;
}

-(void)dealloc
{
    [super dealloc];
}

+(void)load
{
//    [self setCellClass:[RSVerticallyCenteredSecureTextFieldCell class]];
    //Verwijderd na upgrade Xcode 13.1 (ios 15/monterey)

}


-(void)axui_setPlaceholderFont:(NSFont *)font
{
    //TODO:
}

-(NSString*)axui_placeholder
{
    return self.placeholderString;
}

-(NSFont*)axui_placeholderFont
{
    return [NSFont systemFontOfSize:self.bounds.size.height * 3.0 / 2.0];
}

-(NSColor*)axui_placeholderColor
{
    return [NSColor whiteColor];
}

-(void)axui_setPlaceholder:(NSString *)text
{
    //TODO:
}

-(void)axui_setPlaceholderColor:(NSColor *)color
{
    //TODO;
}

#pragma mark - AXUITextInput
- (NSString *)axui_text
{
    return self.stringValue;
}

- (void)axui_setText:(NSString *)axui_text
{
    self.stringValue = axui_text;
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

- (void)axui_setMaxLength:(int)length
{
    id formater =  [[[CCUITextFieldFormatter alloc]init] autorelease];
    [formater setMaximumLength:length];
    [self setFormatter:formater];
}

- (int)axui_maxLength
{
    return [self.formatter maximumLength];
}
@end
