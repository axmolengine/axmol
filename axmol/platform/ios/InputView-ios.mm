/****************************************************************************
Copyright (c) 2019 Xiamen Yaji Software Co., Ltd.

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
#import "platform/ios/InputView-ios.h"
#import "base/IMEDispatcher.h"
#import "base/Director.h"

@interface TextInputView ()

@property(nonatomic) NSString* myMarkedText;

@end

@implementation TextInputView

@synthesize myMarkedText;
@synthesize hasText;
@synthesize beginningOfDocument;
@synthesize endOfDocument;
@synthesize markedTextStyle;
@synthesize tokenizer;
@synthesize autocorrectionType;

- (instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        self.myMarkedText       = nil;
        self.autocorrectionType = UITextAutocorrectionTypeNo;
    }

    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];  // remove keyboard notification
    [self.myMarkedText release];
    [self removeFromSuperview];
    [super dealloc];
}

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    [self resignFirstResponder];
    [self removeFromSuperview];
}

#pragma TextInput protocol

- (id<UITextInputDelegate>)inputDelegate
{
    return nil;
}

- (void)setInputDelegate:(id<UITextInputDelegate>)inputDelegate
{}

- (void)setSelectedTextRange:(UITextRange*)aSelectedTextRange
{
    AXLOGD("UITextRange:setSelectedTextRange");
}

- (UITextRange*)selectedTextRange
{
    return [[[UITextRange alloc] init] autorelease];
}

- (void)deleteBackward
{
    if (nil != self.myMarkedText)
    {
        [self.myMarkedText release];
        self.myMarkedText = nil;
    }
    ax::IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward(1);
}

- (void)insertText:(nonnull NSString*)text
{
    if (nil != self.myMarkedText)
    {
        [self.myMarkedText release];
        self.myMarkedText = nil;
    }
    const char* pszText = [text cStringUsingEncoding:NSUTF8StringEncoding];
    ax::IMEDispatcher::sharedDispatcher()->dispatchInsertText(pszText, strlen(pszText));
}

- (NSWritingDirection)baseWritingDirectionForPosition:(nonnull UITextPosition*)position
                                          inDirection:(UITextStorageDirection)direction
{
    AXLOGD("baseWritingDirectionForPosition");
    return NSWritingDirectionLeftToRight;
}

- (CGRect)caretRectForPosition:(nonnull UITextPosition*)position
{
    AXLOGD("caretRectForPosition");
    return CGRectZero;
}

- (nullable UITextRange*)characterRangeAtPoint:(CGPoint)point
{
    AXLOGD("characterRangeAtPoint");
    return nil;
}

- (nullable UITextRange*)characterRangeByExtendingPosition:(nonnull UITextPosition*)position
                                               inDirection:(UITextLayoutDirection)direction
{
    AXLOGD("characterRangeByExtendingPosition");
    return nil;
}

- (nullable UITextPosition*)closestPositionToPoint:(CGPoint)point
{
    AXLOGD("closestPositionToPoint");
    return nil;
}

- (nullable UITextPosition*)closestPositionToPoint:(CGPoint)point withinRange:(nonnull UITextRange*)range
{
    AXLOGD("closestPositionToPoint");
    return nil;
}

- (NSComparisonResult)comparePosition:(nonnull UITextPosition*)position toPosition:(nonnull UITextPosition*)other
{
    AXLOGD("comparePosition");
    return (NSComparisonResult)0;
}

- (CGRect)firstRectForRange:(nonnull UITextRange*)range
{
    AXLOGD("firstRectForRange");
    return CGRectNull;
}

- (NSInteger)offsetFromPosition:(nonnull UITextPosition*)from toPosition:(nonnull UITextPosition*)toPosition
{
    AXLOGD("offsetFromPosition");
    return 0;
}

- (nullable UITextPosition*)positionFromPosition:(nonnull UITextPosition*)position
                                     inDirection:(UITextLayoutDirection)direction
                                          offset:(NSInteger)offset
{
    AXLOGD("positionFromPosition");
    return nil;
}

- (nullable UITextPosition*)positionFromPosition:(nonnull UITextPosition*)position offset:(NSInteger)offset
{
    AXLOGD("positionFromPosition");
    return nil;
}

- (nullable UITextPosition*)positionWithinRange:(nonnull UITextRange*)range
                            farthestInDirection:(UITextLayoutDirection)direction
{
    AXLOGD("positionWithinRange");
    return nil;
}

- (void)replaceRange:(nonnull UITextRange*)range withText:(nonnull NSString*)text
{}

- (nonnull NSArray<UITextSelectionRect*>*)selectionRectsForRange:(nonnull UITextRange*)range
{
    AXLOGD("selectionRectsForRange");
    return nil;
}

- (void)setBaseWritingDirection:(NSWritingDirection)writingDirection forRange:(nonnull UITextRange*)range
{}

- (void)setMarkedText:(nullable NSString*)markedText selectedRange:(NSRange)selectedRange
{
    AXLOGD("setMarkedText");
    if (markedText == self.myMarkedText)
    {
        return;
    }
    if (nil != self.myMarkedText)
    {
        [self.myMarkedText release];
    }
    self.myMarkedText = markedText;
    [self.myMarkedText retain];
}

- (UITextRange*)markedTextRange
{
    AXLOGD("markedTextRange");
    if (nil != self.myMarkedText)
    {
        return [[[UITextRange alloc] init] autorelease];
    }
    return nil;  // Nil if no marked text.
}

- (nullable NSString*)textInRange:(nonnull UITextRange*)range
{
    AXLOGD("textInRange");
    if (nil != self.myMarkedText)
    {
        return self.myMarkedText;
    }
    return nil;
}

- (nullable UITextRange*)textRangeFromPosition:(nonnull UITextPosition*)fromPosition
                                    toPosition:(nonnull UITextPosition*)toPosition
{
    AXLOGD("textRangeFromPosition");
    return nil;
}

- (void)unmarkText
{
    AXLOGD("unmarkText");
    if (nil == self.myMarkedText)
    {
        return;
    }
    const char* pszText = [self.myMarkedText cStringUsingEncoding:NSUTF8StringEncoding];
    ax::IMEDispatcher::sharedDispatcher()->dispatchInsertText(pszText, strlen(pszText));
    [self.myMarkedText release];
    self.myMarkedText = nil;
}

- (void)encodeWithCoder:(nonnull NSCoder*)coder
{}

@end
