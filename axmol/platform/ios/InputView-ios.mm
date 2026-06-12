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
#import "axmol/platform/ios/InputView-ios.h"
#import "axmol/base/InputSystem.h"
#import "axmol/base/Director.h"

struct InputState
{
    bool hasText;
    bool hasSelection;
    bool readOnly;
};

@interface InputHostView ()

@property(nonatomic) NSString* markedText;
@property(nonatomic) struct InputState inputState;

@end

@implementation InputHostView

@synthesize autocorrectionType;

- (instancetype)initWithFrame:(CGRect)frame
{
    if (self = [super initWithFrame:frame])
    {
        self.contentScaleFactor = [[UIScreen mainScreen] scale];
        self.markedText         = nil;
        self.autocorrectionType = UITextAutocorrectionTypeNo;
    }

    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];  // remove keyboard notification
    [self.markedText release];
    [self removeFromSuperview];
    [super dealloc];
}

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

- (BOOL)pointInside:(CGPoint)point withEvent:(UIEvent*)event
{
    auto inputDisp = ax::InputSystem::getInstance();
    if (inputDisp->hasAttachedDelegate())
    {
        ax::Vec2 pt{static_cast<float>(point.x * [self contentScaleFactor]),
                    static_cast<float>(point.y * [self contentScaleFactor])};
        auto director   = ax::Director::getInstance();
        auto renderView = director->getRenderView();
        // convert axmol screen to world coordinate
        pt = director->screenToWorld(pt);

        bool keep = inputDisp->dispatchHitTestWithIME(pt);
        if (keep)
            return NO;
    }
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
    if (nil != self.markedText)
    {
        [self.markedText release];
        self.markedText = nil;
    }
    ax::InputSystem::getInstance()->dispatchDeleteBackward(1u);
}

- (void)insertText:(nonnull NSString*)text
{
    if (nil != self.markedText)
    {
        [self.markedText release];
        self.markedText = nil;
    }
    const char* pszText = [text cStringUsingEncoding:NSUTF8StringEncoding];
    ax::InputSystem::getInstance()->dispatchInsertText(std::string_view{pszText, strlen(pszText)});
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
    if (markedText == self.markedText)
    {
        return;
    }
    if (nil != self.markedText)
    {
        [self.markedText release];
    }
    self.markedText = markedText;
    [self.markedText retain];
}

- (UITextRange*)markedTextRange
{
    AXLOGD("markedTextRange");
    if (nil != self.markedText)
    {
        return [[[UITextRange alloc] init] autorelease];
    }
    return nil;  // Nil if no marked text.
}

- (nullable NSString*)textInRange:(nonnull UITextRange*)range
{
    AXLOGD("textInRange");
    if (nil != self.markedText)
    {
        return self.markedText;
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
    if (nil == self.markedText)
    {
        return;
    }
    const char* pszText = [self.markedText cStringUsingEncoding:NSUTF8StringEncoding];
    ax::InputSystem::getInstance()->dispatchInsertText(std::string_view{pszText, strlen(pszText)});
    [self.markedText release];
    self.markedText = nil;
}

- (void)encodeWithCoder:(nonnull NSCoder*)coder
{}

#pragma mark - System edit menu (dispatch to engine)

#if TARGET_OS_IOS

// Show the system edit menu at the given UIKit point (points, not pixels).
// This view does NOT perform clipboard read/write itself; it dispatches actions to the engine.
// Insert into InputHostView implementation

- (void)showContextMenu:(CGPoint)point hasText:(BOOL)hasText hasSelection:(BOOL)hasSelection readOnly:(BOOL)readOnly
{
    if (![self isFirstResponder])
    {
        [self becomeFirstResponder];
    }

    _inputState.hasSelection = hasSelection;
    _inputState.hasText      = hasText;
    _inputState.readOnly     = readOnly;

    UIMenuController* menu = [UIMenuController sharedMenuController];
    CGRect targetRect      = CGRectMake(point.x, point.y, 1.0f, 1.0f);

    // Ensure UIKit calls on main thread
    dispatch_async(dispatch_get_main_queue(), ^{
      if (@available(iOS 13.0, *))
      {
          // iOS 13+ recommended API
          [menu showMenuFromView:self rect:targetRect];
      }
      else
      {
          // Fallback for older iOS
          [menu setTargetRect:targetRect inView:self];
          [menu setMenuVisible:YES animated:YES];
      }
    });
}

- (void)hideContextMenu
{
    UIMenuController* menu = [UIMenuController sharedMenuController];
    dispatch_async(dispatch_get_main_queue(), ^{
      if (@available(iOS 13.0, *))
      {
          [menu hideMenuFromView:self];
      }
      else
      {
          [menu setMenuVisible:NO animated:YES];
      }
    });
}

// Decide which actions are enabled. We query the engine synchronously for selection existence.
- (BOOL)canPerformAction:(SEL)action withSender:(id)sender
{
    if (action == @selector(copy:))
    {
        return _inputState.hasSelection || _inputState.hasText;
    }
    else if (action == @selector(cut:))
    {
        return _inputState.hasSelection && !_inputState.readOnly;
    }
    else if (action == @selector(selectAll:))
    {
        return _inputState.hasText;
    }
    if (action == @selector(paste:))
    {
        // Enable paste if UIPasteboard has a string OR let engine decide (we enable if pasteboard non-empty).
        return !_inputState.readOnly && ([UIPasteboard generalPasteboard].string != nil);
    }
    return [super canPerformAction:action withSender:sender];
}

#    pragma mark - Menu actions (dispatch to engine)

// Copy: tell engine to perform copy (engine should obtain selection and write to UIPasteboard)
- (void)copy:(id)sender
{
    // Dispatch a copy request; engine will handle reading selection and writing to system clipboard.
    ax::InputSystem::getInstance()->dispatchPerformEditAction(ax::EditAction::Copy);
    [[UIMenuController sharedMenuController] setMenuVisible:NO animated:YES];
}

// Paste: ask engine to perform paste. We can either read UIPasteboard here and pass text,
// or let engine read the UIPasteboard itself. Here we dispatch a paste request so engine controls behavior.
- (void)paste:(id)sender
{
    ax::InputSystem::getInstance()->dispatchPerformEditAction(ax::EditAction::Paste);
    [[UIMenuController sharedMenuController] setMenuVisible:NO animated:YES];
}

// Cut: tell engine to perform cut (engine should copy selection to clipboard and delete it)
- (void)cut:(id)sender
{
    ax::InputSystem::getInstance()->dispatchPerformEditAction(ax::EditAction::Cut);
    [[UIMenuController sharedMenuController] setMenuVisible:NO animated:YES];
}

- (void)selectAll:(id)sender
{
    ax::InputSystem::getInstance()->dispatchPerformEditAction(ax::EditAction::SelectAll);
    [[UIMenuController sharedMenuController] setMenuVisible:NO animated:YES];
}

#endif

@synthesize endOfDocument;

@synthesize hasText;

@synthesize markedTextStyle;

@synthesize tokenizer;

@synthesize beginningOfDocument;

@end
