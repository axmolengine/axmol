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
#import "axmol/base/PointerEvent.h"
#import "axmol/base/InputSystem.h"
#import "axmol/platform/ios/InputView-ios.h"
#import "axmol/platform/ios/RenderView-ios.h"

#if AX_ENABLE_MTL
#    import <Metal/Metal.h>
#    import "axmol/rhi/metal/DriverMTL.h"
#    import "axmol/rhi/metal/UtilsMTL.h"
#endif
#if AX_ENABLE_GL
#    import "axmol/platform/ios/ES3Renderer-ios.h"
#    import "axmol/platform/ios/OpenGL_Internal-ios.h"
#endif

#include "axmol/rhi/DriverContext.h"

using namespace ax::rhi;

// CLASS IMPLEMENTATIONS:

#define IOS_MAX_TOUCHES_COUNT 10

@interface RenderHostView ()
@property(nonatomic) InputHostView* inputHost;
@property(nonatomic, readwrite, assign) BOOL isKeyboardShown;
@property(nonatomic, copy) NSNotification* keyboardShowNotification;
@end

@implementation RenderHostView

@synthesize pixelFormat = pixelformat_, depthFormat = depthFormat_;
#if AX_ENABLE_GL
@synthesize context = context_;
#endif
@synthesize multiSampling            = multiSampling_;
@synthesize keyboardShowNotification = keyboardShowNotification_;
@synthesize isKeyboardShown          = isKeyboardShown_;

static ax::Rect convertKeyboardRectToViewport(CGRect rect, CGSize viewSize)
{
    float flippedY = viewSize.height - rect.origin.y - rect.size.height;
    return ax::Rect(rect.origin.x, flippedY, rect.size.width, rect.size.height);
}

+ (Class)layerClass
{
#if AX_ENABLE_MTL && !AX_ENABLE_GL
    return [CAMetalLayer class];
#elif !AX_ENABLE_MTL && AX_ENABLE_GL
    return [CAEAGLLayer class];
#else
    return DriverContext::isMetal() ? [CAMetalLayer class] : [CAEAGLLayer class];
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
    DriverContext::makeCurrentDriver();

    if ((self = [super initWithFrame:frame]))
    {
        self.inputHost = [[InputHostView alloc] initWithFrame:frame];

        self.keyboardShowNotification = nil;
        if ([self respondsToSelector:@selector(setContentScaleFactor:)])
        {
            self.contentScaleFactor = [[UIScreen mainScreen] scale];
        }

        if (DriverContext::isMetal())
        {
            AX_UNUSED_PARAM(format);
            AX_UNUSED_PARAM(depth);
            AX_UNUSED_PARAM(sharegroup);
        }
        else
        {
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
            DriverContext::activateCurrentDriver();
        }
    }

    return self;
}

- (id)initWithCoder:(NSCoder*)aDecoder
{
    if ((self = [super initWithCoder:aDecoder]))
    {
        self.inputHost = [[InputHostView alloc] initWithCoder:aDecoder];
#if AX_ENABLE_GL
        if (DriverContext::isOpenGL())
        {
            CAEAGLLayer* eaglLayer = (CAEAGLLayer*)[self layer];

            pixelformat_      = (int)ax::PixelFormat::RGB565;
            depthFormat_      = (int)ax::PixelFormat::D24S8;
            multiSampling_    = NO;
            requestedSamples_ = 0;
            if (![self setupSurfaceWithSharegroup:nil])
            {
                [self release];
                return nil;
            }
        }
#endif
    }

    return self;
}

- (BOOL)setupSurfaceWithSharegroup:(void*)sharegroup
{
#if AX_ENABLE_GL
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

#endif

    return YES;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];  // remove keyboard notification
#if AX_ENABLE_GL
    if (DriverContext::isOpenGL())
        [renderer_ release];
#endif
    [self.inputHost release];
    [super dealloc];
}

- (void)layoutSubviews
{
    auto director = ax::Director::getInstance();
    if (!director->isValid())
        return;

    auto bounds           = [self bounds];
    self.inputHost.bounds = bounds;

// 2. Handle GL context resize if necessary
#if AX_ENABLE_GL
    if (DriverContext::isOpenGL())
    {
        [renderer_ resizeFromLayer:(CAEAGLLayer*)self.layer];
    }
#endif

    auto renderView = static_cast<ax::RenderView*>(director->getRenderView());
    if (renderView)
    {
        auto& viewSize = bounds.size;
        renderView->updateSurfaceMetrics(
            ax::Vec2(static_cast<float>(viewSize.width), static_cast<float>(viewSize.height)), self.contentScaleFactor,
            true);
    }

    // Avoid flicker. Issue #350
    if ([NSThread isMainThread])
        director->stepFrame();
}

- (void)swapBuffers
{
#if AX_ENABLE_GL
    // IMPORTANT:
    // - preconditions
    //    -> context_ MUST be the OpenGL context
    //    -> renderbuffer_ must be the RENDER BUFFER

    if (DriverContext::isMetal())
        return;

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

// Pass the touches to the superview
#pragma mark RenderHostView - Touch Delegate

- (void)handlePlatformTouches:(NSSet*)touches
               dispatchAction:(void (^)(ax::InputSystem* sys, ax::Vec2 pt, ax::PointerInputState st))dispatchBlock
{
    auto inputSys = ax::InputSystem::getInstance();

    int i = 0;
    for (UITouch* touch in touches)
    {
        if (i >= IOS_MAX_TOUCHES_COUNT)
        {
            AXLOGW("warning: touches more than 10, should adjust IOS_MAX_TOUCHES_COUNT");
            break;
        }

        CGPoint nativePoint = [touch locationInView:self];

        ax::Vec2 point{static_cast<float>(nativePoint.x), static_cast<float>(nativePoint.y)};

        float pressure = 1.0f;
        if (touch.maximumPossibleForce > 0.0f)
        {
            pressure = static_cast<float>(touch.force / touch.maximumPossibleForce);
        }

        ax::PointerInputState state{.id = (intptr_t)touch, .pressure = pressure, .type = ax::PointerType::Touch};

        dispatchBlock(inputSys, point, state);

        ++i;
    }
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    if (self.isKeyboardShown)
        [self closeKeyboardOpenedByEditBox];

    [self handlePlatformTouches:touches
                 dispatchAction:^(ax::InputSystem* sys, ax::Vec2 pt, ax::PointerInputState st) {
                   sys->handlePointerDown(pt, st);
                 }];
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
    [self handlePlatformTouches:touches
                 dispatchAction:^(ax::InputSystem* sys, ax::Vec2 pt, ax::PointerInputState st) {
                   sys->handlePointerMove(pt, st);
                 }];
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
    [self handlePlatformTouches:touches
                 dispatchAction:^(ax::InputSystem* sys, ax::Vec2 pt, ax::PointerInputState st) {
                   sys->handlePointerUp(pt, st);
                 }];
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
    [self handlePlatformTouches:touches
                 dispatchAction:^(ax::InputSystem* sys, ax::Vec2 pt, ax::PointerInputState st) {
                   sys->handlePointerCancel(pt, st);
                 }];
}

- (void)showKeyboard
{
    [self addSubview:self.inputHost];
    [self.inputHost becomeFirstResponder];
}

- (void)hideKeyboard
{
    [self.inputHost resignFirstResponder];
    [self.inputHost removeFromSuperview];
}

- (void)doAnimationWhenKeyboardMoveWithDuration:(float)duration distance:(float)dis
{
    if (dis < 0.0f)
        dis = 0.0f;

    auto renderView = ax::Director::getInstance()->getRenderView();
    dis *= renderView->getScaleY();
    dis /= self.contentScaleFactor;

    CGRect newFrame = [self bounds];
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

    auto inputSys = ax::InputSystem::getInstance();
    if (!inputSys)
        return;

    // Extract animation hardware clock duration from the notification payload
    double aniDuration = [[info objectForKey:UIKeyboardAnimationDurationUserInfoKey] doubleValue];
    float duration     = static_cast<float>(aniDuration);

    // 1. Keyboard is about to slide up
    if (UIKeyboardWillShowNotification == type)
    {
        // Extract the target raw keyboard bounds (End Frame)
        CGRect end = [[info objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];

        // Convert frame coordinates to local view system (UIKit Points, Top-Left origin)
        end = [self convertRect:end fromView:nil];

        // Pipe raw properties directly to C++ core layer for centralized coordinate/resolution mapping
        inputSys->onPlatformKeyboardWillShow(static_cast<float>(end.origin.x), static_cast<float>(end.origin.y),
                                             static_cast<float>(end.size.width), static_cast<float>(end.size.height),
                                             duration);
    }
    // 2. Keyboard expansion animation completed
    else if (UIKeyboardDidShowNotification == type)
    {
        self.isKeyboardShown = YES;
        inputSys->onPlatformKeyboardDidShow();
    }
    // 3. Keyboard is about to slide down
    else if (UIKeyboardWillHideNotification == type)
    {
        // No layout tracking required; C++ layer derives the dismissal path via cached frame
        inputSys->onPlatformKeyboardWillHide(duration);
    }
    // 4. Keyboard dismissal animation completed
    else if (UIKeyboardDidHideNotification == type)
    {
        self.isKeyboardShown = NO;
        inputSys->onPlatformKeyboardDidHide();
    }
#endif /* !defined(AX_TARGET_OS_TVOS) */
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

- (void)showContextMenu:(CGPoint)point hasText:(BOOL)hasText hasSelection:(BOOL)hasSelection readOnly:(BOOL)readOnly
{
    void (^showMenuBlock)(void) = ^{
      auto renderView = ax::Director::getInstance()->getRenderView();

      CGPoint screenPointInPoints = CGPointMake(point.x, point.y);

      CGPoint hostPoint = [self.inputHost convertPoint:screenPointInPoints fromView:nil];

      if (![self.inputHost isFirstResponder])
      {
          [self.inputHost becomeFirstResponder];
      }
      [self.inputHost showContextMenu:hostPoint hasText:hasText hasSelection:hasSelection readOnly:readOnly];
    };

    if ([NSThread isMainThread])
    {
        showMenuBlock();
    }
    else
    {
        dispatch_async(dispatch_get_main_queue(), showMenuBlock);
    }
}

- (void)hideContextMenu
{
    [self.inputHost hideContextMenu];
}

@end
