/*

Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

=====================

File: RenderHostView.m
Abstract: Convenience class that wraps the CAEAGLLayer from CoreAnimation into a
UIView subclass.

Version: 1.3

Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc.
("Apple") in consideration of your agreement to the following terms, and your
use, installation, modification or redistribution of this Apple software
constitutes acceptance of these terms.  If you do not agree with these terms,
please do not use, install, modify or redistribute this Apple software.

In consideration of your agreement to abide by the following terms, and subject
to these terms, Apple grants you a personal, non-exclusive license, under
Apple's copyrights in this original Apple software (the "Apple Software"), to
use, reproduce, modify and redistribute the Apple Software, with or without
modifications, in source and/or binary forms; provided that if you redistribute
the Apple Software in its entirety and without modifications, you must retain
this notice and the following text and disclaimers in all such redistributions
of the Apple Software.
Neither the name, trademarks, service marks or logos of Apple Inc. may be used
to endorse or promote products derived from the Apple Software without specific
prior written permission from Apple.  Except as expressly stated in this notice,
no other rights or licenses, express or implied, are granted by Apple herein,
including but not limited to any patent rights that may be infringed by your
derivative works or by other works in which the Apple Software may be
incorporated.

The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
COMBINATION WITH YOUR PRODUCTS.

IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR
DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF
CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF
APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Copyright (C) 2008 Apple Inc. All Rights Reserved.

*/
#import "axmol/platform/ios/RenderHostView-ios.h"

#import <QuartzCore/QuartzCore.h>

#import "axmol/base/Director.h"
#import "axmol/base/Touch.h"
#import "axmol/base/IMEDispatcher.h"
#import "axmol/platform/ios/InputView-ios.h"

#if AX_RENDER_API == AX_RENDER_API_MTL
#    import <Metal/Metal.h>
#    import "axmol/rhi/metal/DriverMTL.h"
#    import "axmol/rhi/metal/UtilsMTL.h"
#else
#    import "axmol/platform/ios/RenderViewImpl-ios.h"
#    import "axmol/platform/ios/ES3Renderer-ios.h"
#    import "axmol/platform/ios/OpenGL_Internal-ios.h"
#endif

// CLASS IMPLEMENTATIONS:

#define IOS_MAX_TOUCHES_COUNT 10

@interface RenderHostView ()
@property(nonatomic) TextInputView* textInputView;
@property(nonatomic, readwrite, assign) BOOL isKeyboardShown;
@property(nonatomic, copy) NSNotification* keyboardShowNotification;
@property(nonatomic, assign) CGRect savedBounds;
@end

@implementation RenderHostView

@synthesize backingSize = backingSize_;
@synthesize pixelFormat = pixelformat_, depthFormat = depthFormat_;
#if AX_RENDER_API == AX_RENDER_API_GL
@synthesize context = context_;
#endif
@synthesize multiSampling            = multiSampling_;
@synthesize keyboardShowNotification = keyboardShowNotification_;
@synthesize isKeyboardShown          = isKeyboardShown_;
@synthesize savedBounds              = savedBounds_;

static ax::Rect convertKeyboardRectToViewport(CGRect rect, CGSize viewSize)
{
    float flippedY = viewSize.height - rect.origin.y - rect.size.height;
    return ax::Rect(rect.origin.x, flippedY, rect.size.width, rect.size.height);
}

+ (Class)layerClass
{
#if AX_RENDER_API == AX_RENDER_API_MTL
    return [CAMetalLayer class];
#else
    return [CAEAGLLayer class];
#endif
}

+ (id)viewWithFrame:(CGRect)frame
{
    return [[[self alloc] initWithFrame:frame] autorelease];
}

+ (id)viewWithFrame:(CGRect)frame pixelFormat:(int)format
{
    return [[[self alloc] initWithFrame:frame pixelFormat:format] autorelease];
}

+ (id)viewWithFrame:(CGRect)frame pixelFormat:(int)format depthFormat:(int)depth
{
    return [[[self alloc] initWithFrame:frame
                            pixelFormat:format
                            depthFormat:depth
                     preserveBackbuffer:NO
                             sharegroup:nil
                          multiSampling:NO
                        numberOfSamples:0] autorelease];
}

+ (id)viewWithFrame:(CGRect)frame
           pixelFormat:(int)format
           depthFormat:(int)depth
    preserveBackbuffer:(BOOL)retained
            sharegroup:(void*)sharegroup
         multiSampling:(BOOL)multisampling
       numberOfSamples:(unsigned int)samples
{
    return [[[self alloc] initWithFrame:frame
                            pixelFormat:format
                            depthFormat:depth
                     preserveBackbuffer:retained
                             sharegroup:sharegroup
                          multiSampling:multisampling
                        numberOfSamples:samples] autorelease];
}

- (id)initWithFrame:(CGRect)frame
{
    return [self initWithFrame:frame
                   pixelFormat:(int)ax::PixelFormat::RGB565
                   depthFormat:0
            preserveBackbuffer:NO
                    sharegroup:nil
                 multiSampling:NO
               numberOfSamples:0];
}

- (id)initWithFrame:(CGRect)frame pixelFormat:(int)format
{
    return [self initWithFrame:frame
                   pixelFormat:format
                   depthFormat:0
            preserveBackbuffer:NO
                    sharegroup:nil
                 multiSampling:NO
               numberOfSamples:0];
}

- (id)initWithFrame:(CGRect)frame
           pixelFormat:(int)format
           depthFormat:(int)depth
    preserveBackbuffer:(BOOL)retained
            sharegroup:(void*)sharegroup

         multiSampling:(BOOL)sampling
       numberOfSamples:(unsigned int)nSamples
{
    if ((self = [super initWithFrame:frame]))
    {
        self.textInputView = [[TextInputView alloc] initWithFrame:frame];

        savedBounds_                  = [self bounds];
        self.keyboardShowNotification = nil;
        if ([self respondsToSelector:@selector(setContentScaleFactor:)])
        {
            self.contentScaleFactor = [[UIScreen mainScreen] scale];
        }

#if AX_RENDER_API == AX_RENDER_API_MTL
        AX_UNUSED_PARAM(format);
        AX_UNUSED_PARAM(depth);
        AX_UNUSED_PARAM(sharegroup);
#else
        pixelformat_        = format;
        depthFormat_        = depth;
        multiSampling_      = sampling;
        requestedSamples_   = nSamples;
        preserveBackbuffer_ = retained;
        if (![self setupSurfaceWithSharegroup:sharegroup])
        {
            [self release];
            return nil;
        }
#endif
    }

    return self;
}

- (id)initWithCoder:(NSCoder*)aDecoder
{
    if ((self = [super initWithCoder:aDecoder]))
    {
        self.textInputView = [[TextInputView alloc] initWithCoder:aDecoder];
#if AX_RENDER_API == AX_RENDER_API_MTL
        backingSize_ = [self bounds].size;
#else
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*)[self layer];

        pixelformat_      = (int)ax::PixelFormat::RGB565;
        depthFormat_      = (int)ax::PixelFormat::D24S8;
        multiSampling_    = NO;
        requestedSamples_ = 0;
        backingSize_      = [eaglLayer bounds].size;

        if (![self setupSurfaceWithSharegroup:nil])
        {
            [self release];
            return nil;
        }
#endif
    }

    return self;
}

#if AX_RENDER_API == AX_RENDER_API_GL
- (BOOL)setupSurfaceWithSharegroup:(void*)sharegroup
{
    CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;

    NSString* platformPF =
        pixelformat_ == (int)ax::PixelFormat::RGB565 ? kEAGLColorFormatRGB565 : kEAGLColorFormatRGBA8;

    eaglLayer.opaque = YES;
    eaglLayer.drawableProperties =
        [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:preserveBackbuffer_],
                                                   kEAGLDrawablePropertyRetainedBacking, platformPF,
                                                   kEAGLDrawablePropertyColorFormat, nil];

    auto depth = depthFormat_ == (int)ax::PixelFormat::D24S8 ? GL_DEPTH24_STENCIL8 : 0;
    auto pixel = pixelformat_ == (int)ax::PixelFormat::RGB565 ? GL_RGB565 : GL_RGBA8_OES;
    renderer_  = [[ES3Renderer alloc] initWithDepthFormat:depth
                                         withPixelFormat:pixel
                                          withSharegroup:(EAGLSharegroup*)sharegroup
                                       withMultiSampling:multiSampling_
                                     withNumberOfSamples:requestedSamples_];

    NSAssert(renderer_, @"OpenGL ES 2.O is required.");
    if (!renderer_)
        return NO;

    context_ = [renderer_ context];

#    if GL_EXT_discard_framebuffer == 1
    discardFramebufferSupported_ = YES;
#    else
    discardFramebufferSupported_ = NO;
#    endif

    CHECK_GL_ERROR();

    return YES;
}
#endif

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];  // remove keyboard notification
#if AX_RENDER_API == AX_RENDER_API_GL
    [renderer_ release];
#endif
    [self.textInputView release];
    [super dealloc];
}

- (void)layoutSubviews
{
    auto director = ax::Director::getInstance();
    if (!director->isValid())
        return;

    savedBounds_              = [self bounds];
    self.textInputView.bounds = savedBounds_;
#if AX_RENDER_API == AX_RENDER_API_MTL
    backingSize_ = savedBounds_.size;
    backingSize_.width *= self.contentScaleFactor;
    backingSize_.height *= self.contentScaleFactor;
#else
    [renderer_ resizeFromLayer:(CAEAGLLayer*)self.layer];
    backingSize_ = [renderer_ backingSize];
#endif

    auto renderView = director->getRenderView();
    if (renderView)
        renderView->updateRenderSurface(backingSize_.width, backingSize_.height, ax::RenderView::AllUpdates);

    // Avoid flicker. Issue #350
    if ([NSThread isMainThread])
        director->drawScene();
}

- (void)swapBuffers
{
#if AX_RENDER_API == AX_RENDER_API_GL
    // IMPORTANT:
    // - preconditions
    //    -> context_ MUST be the OpenGL context
    //    -> renderbuffer_ must be the RENDER BUFFER

#    ifdef __IPHONE_4_0

    if (multiSampling_)
    {
        /* Resolve from msaaFramebuffer to resolveFramebuffer */
        // __state.disableStencilTest();
        glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, [renderer_ msaaFrameBuffer]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, [renderer_ defaultFrameBuffer]);
        glResolveMultisampleFramebufferAPPLE();
    }

    if (discardFramebufferSupported_)
    {
        if (multiSampling_)
        {
            if (depthFormat_)
            {
                GLenum attachments[] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
                glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 2, attachments);
            }
            else
            {
                GLenum attachments[] = {GL_COLOR_ATTACHMENT0};
                glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 1, attachments);
            }

            glBindRenderbuffer(GL_RENDERBUFFER, [renderer_ colorRenderBuffer]);
        }

        // not MSAA
        else if (depthFormat_)
        {
            GLenum attachments[] = {GL_DEPTH_ATTACHMENT};
            glDiscardFramebufferEXT(GL_FRAMEBUFFER, 1, attachments);
        }
    }

#    endif  // __IPHONE_4_0

    if (![context_ presentRenderbuffer:GL_RENDERBUFFER])
    {
        //         AXLOGD(@"Failed to swap renderbuffer in {}\n", __FUNCTION__);
    }

#    if _AX_DEBUG
    CHECK_GL_ERROR();
#    endif

    // We can safely re-bind the framebuffer here, since this will be the
    // 1st instruction of the new main loop
    if (multiSampling_)
        glBindFramebuffer(GL_FRAMEBUFFER, [renderer_ msaaFrameBuffer]);
#endif
}

#pragma mark RenderHostView - Point conversion

- (CGPoint)convertPointFromViewToSurface:(CGPoint)point
{
    CGRect bounds = [self bounds];

    CGPoint ret;
    ret.x = (point.x - bounds.origin.x) / bounds.size.width * backingSize_.width;
    ret.y = (point.y - bounds.origin.y) / bounds.size.height * backingSize_.height;

    return ret;
}

- (CGRect)convertRectFromViewToSurface:(CGRect)rect
{
    CGRect bounds = [self bounds];

    CGRect ret;
    ret.origin.x    = (rect.origin.x - bounds.origin.x) / bounds.size.width * backingSize_.width;
    ret.origin.y    = (rect.origin.y - bounds.origin.y) / bounds.size.height * backingSize_.height;
    ret.size.width  = rect.size.width / bounds.size.width * backingSize_.width;
    ret.size.height = rect.size.height / bounds.size.height * backingSize_.height;

    return ret;
}

// Pass the touches to the superview
#pragma mark RenderHostView - Touch Delegate
- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    if (self.isKeyboardShown)
        [self closeKeyboardOpenedByEditBox];

    UITouch* ids[IOS_MAX_TOUCHES_COUNT] = {0};
    float xs[IOS_MAX_TOUCHES_COUNT]     = {0.0f};
    float ys[IOS_MAX_TOUCHES_COUNT]     = {0.0f};

    int i = 0;
    for (UITouch* touch in touches)
    {
        if (i >= IOS_MAX_TOUCHES_COUNT)
        {
            AXLOGW("warning: touches more than 10, should adjust IOS_MAX_TOUCHES_COUNT");
            break;
        }

        ids[i] = touch;
        xs[i]  = [touch locationInView:[touch view]].x * self.contentScaleFactor;
        ys[i]  = [touch locationInView:[touch view]].y * self.contentScaleFactor;
        ++i;
    }

    auto renderView = ax::Director::getInstance()->getRenderView();
    renderView->handleTouchesBegin(i, (intptr_t*)ids, xs, ys);
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
    UITouch* ids[IOS_MAX_TOUCHES_COUNT] = {0};
    float xs[IOS_MAX_TOUCHES_COUNT]     = {0.0f};
    float ys[IOS_MAX_TOUCHES_COUNT]     = {0.0f};
    float fs[IOS_MAX_TOUCHES_COUNT]     = {0.0f};
    float ms[IOS_MAX_TOUCHES_COUNT]     = {0.0f};

    int i = 0;
    for (UITouch* touch in touches)
    {
        if (i >= IOS_MAX_TOUCHES_COUNT)
        {
            AXLOGW("warning: touches more than 10, should adjust IOS_MAX_TOUCHES_COUNT");
            break;
        }

        ids[i] = touch;
        xs[i]  = [touch locationInView:[touch view]].x * self.contentScaleFactor;
        ys[i]  = [touch locationInView:[touch view]].y * self.contentScaleFactor;
#if defined(__IPHONE_9_0) && (__IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_9_0)
        // running on iOS 9.0 or higher version
        if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 9.0f)
        {
            fs[i] = touch.force;
            ms[i] = touch.maximumPossibleForce;
        }
#endif
        ++i;
    }

    auto renderView = ax::Director::getInstance()->getRenderView();
    renderView->handleTouchesMove(i, (intptr_t*)ids, xs, ys, fs, ms);
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
    UITouch* ids[IOS_MAX_TOUCHES_COUNT] = {0};
    float xs[IOS_MAX_TOUCHES_COUNT]     = {0.0f};
    float ys[IOS_MAX_TOUCHES_COUNT]     = {0.0f};

    int i = 0;
    for (UITouch* touch in touches)
    {
        if (i >= IOS_MAX_TOUCHES_COUNT)
        {
            AXLOGW("warning: touches more than 10, should adjust IOS_MAX_TOUCHES_COUNT");
            break;
        }

        ids[i] = touch;
        xs[i]  = [touch locationInView:[touch view]].x * self.contentScaleFactor;
        ys[i]  = [touch locationInView:[touch view]].y * self.contentScaleFactor;
        ++i;
    }

    auto renderView = ax::Director::getInstance()->getRenderView();
    renderView->handleTouchesEnd(i, (intptr_t*)ids, xs, ys);
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
    UITouch* ids[IOS_MAX_TOUCHES_COUNT] = {0};
    float xs[IOS_MAX_TOUCHES_COUNT]     = {0.0f};
    float ys[IOS_MAX_TOUCHES_COUNT]     = {0.0f};

    int i = 0;
    for (UITouch* touch in touches)
    {
        if (i >= IOS_MAX_TOUCHES_COUNT)
        {
            AXLOGW("warning: touches more than 10, should adjust IOS_MAX_TOUCHES_COUNT");
            break;
        }

        ids[i] = touch;
        xs[i]  = [touch locationInView:[touch view]].x * self.contentScaleFactor;
        ys[i]  = [touch locationInView:[touch view]].y * self.contentScaleFactor;
        ++i;
    }

    auto renderView = ax::Director::getInstance()->getRenderView();
    renderView->handleTouchesCancel(i, (intptr_t*)ids, xs, ys);
}

- (void)showKeyboard
{
    [self addSubview:self.textInputView];
    [self.textInputView becomeFirstResponder];
}

- (void)hideKeyboard
{
    [self.textInputView resignFirstResponder];
    [self.textInputView removeFromSuperview];
}

- (void)doAnimationWhenKeyboardMoveWithDuration:(float)duration distance:(float)dis
{
    if (dis < 0.0f)
        dis = 0.0f;

    auto renderView = ax::Director::getInstance()->getRenderView();
    dis *= renderView->getScaleY();
    dis /= self.contentScaleFactor;

    CGRect newFrame = savedBounds_;
    newFrame.origin.y -= dis;

    [UIView animateWithDuration:duration
                     animations:^{
                       self.frame = newFrame;
                     }];
}

- (void)doAnimationWhenAnotherEditBeClicked
{
    if (self.keyboardShowNotification != nil)
    {
        [[NSNotificationCenter defaultCenter] postNotification:self.keyboardShowNotification];
    }
}

#pragma UIKeyboard notification

#if !defined(AX_TARGET_OS_TVOS)
namespace
{
UIInterfaceOrientation getFixedOrientation(UIInterfaceOrientation statusBarOrientation)
{
    if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0)
    {
        statusBarOrientation = UIInterfaceOrientationPortrait;
    }
    return statusBarOrientation;
}
}  // namespace
#endif

- (void)didMoveToWindow
{
#if !defined(AX_TARGET_OS_TVOS)
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onUIKeyboardNotification:)
                                                 name:UIKeyboardWillShowNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onUIKeyboardNotification:)
                                                 name:UIKeyboardDidShowNotification
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onUIKeyboardNotification:)
                                                 name:UIKeyboardWillHideNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(onUIKeyboardNotification:)
                                                 name:UIKeyboardDidHideNotification
                                               object:nil];
#endif
}

- (void)onUIKeyboardNotification:(NSNotification*)notif
{
#if !defined(AX_TARGET_OS_TVOS)
    NSString* type     = notif.name;
    NSDictionary* info = [notif userInfo];

    CGRect begin       = [[info objectForKey:UIKeyboardFrameBeginUserInfoKey] CGRectValue];
    CGRect end         = [[info objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];
    double aniDuration = [[info objectForKey:UIKeyboardAnimationDurationUserInfoKey] doubleValue];

    // Convert to current view's coordinate system
    begin = [self convertRect:begin fromView:nil];
    end   = [self convertRect:end fromView:nil];

    auto renderView = ax::Director::getInstance()->getRenderView();
    float scaleX    = renderView->getScaleX();
    float scaleY    = renderView->getScaleY();

    const auto backingScaleFactor = self.contentScaleFactor;

    // Convert to pixel coordinates
    begin = CGRectApplyAffineTransform(
        begin, CGAffineTransformScale(CGAffineTransformIdentity, backingScaleFactor, backingScaleFactor));
    end = CGRectApplyAffineTransform(
        end, CGAffineTransformScale(CGAffineTransformIdentity, backingScaleFactor, backingScaleFactor));

    float offestY = renderView->getViewportRect().origin.y;
    if (offestY < 0.0f)
    {
        begin.origin.y += offestY;
        begin.size.height -= offestY;
        end.size.height -= offestY;
    }

    // Convert to design resolution coordinates
    begin = CGRectApplyAffineTransform(begin,
                                       CGAffineTransformScale(CGAffineTransformIdentity, 1.0f / scaleX, 1.0f / scaleY));
    end   = CGRectApplyAffineTransform(end,
                                       CGAffineTransformScale(CGAffineTransformIdentity, 1.0f / scaleX, 1.0f / scaleY));

    // Fill notification info for Axmol IME dispatcher
    auto boundSize = savedBounds_.size;
    CGSize viewSize =
        CGSizeMake(boundSize.width * backingScaleFactor / scaleX, boundSize.height * backingScaleFactor / scaleY);

    ax::IMEKeyboardNotificationInfo notiInfo;
    notiInfo.begin    = convertKeyboardRectToViewport(begin, viewSize);
    notiInfo.end      = convertKeyboardRectToViewport(end, viewSize);
    notiInfo.duration = aniDuration;

    ax::IMEDispatcher* dispatcher = ax::IMEDispatcher::sharedDispatcher();
    if (UIKeyboardWillShowNotification == type)
    {
        dispatcher->dispatchKeyboardWillShow(notiInfo);
    }
    else if (UIKeyboardDidShowNotification == type)
    {
        self.isKeyboardShown = YES;
        dispatcher->dispatchKeyboardDidShow(notiInfo);
    }
    else if (UIKeyboardWillHideNotification == type)
    {
        dispatcher->dispatchKeyboardWillHide(notiInfo);
    }
    else if (UIKeyboardDidHideNotification == type)
    {
        self.isKeyboardShown = NO;
        dispatcher->dispatchKeyboardDidHide(notiInfo);
    }
#endif
}

// Close the keyboard opened by EditBox
- (void)closeKeyboardOpenedByEditBox
{
    NSArray* subviews = self.subviews;

    for (UIView* view in subviews)
    {
        if ([view isKindOfClass:NSClassFromString(@"UITextView")] ||
            [view isKindOfClass:NSClassFromString(@"UITextField")])
        {
            if ([view isFirstResponder])
            {
                [view resignFirstResponder];
                return;
            }
        }
    }
}

@end
