/*

===== IMPORTANT =====

This is sample code demonstrating API, technology or techniques in development.
Although this sample code has been reviewed for technical accuracy, it is not
final. Apple is supplying this information to help you plan for the adoption of
the technologies and programming interfaces described herein. This information
is subject to change, and software implemented based on this sample code should
be tested with final operating system software and final documentation. Newer
versions of this sample code may be provided with future seeds of the API or
technology. For information about updates to this and other developer
documentation, view the New & Updated sidebars in subsequent documentation
seeds.

=====================

File: EARenderView.m
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
#import "platform/ios/EARenderView-ios.h"

#import <QuartzCore/QuartzCore.h>

#import "base/Director.h"
#import "base/Touch.h"
#import "base/IMEDispatcher.h"
#import "platform/ios/InputView-ios.h"
#import "platform/Application.h"

#if defined(AX_USE_METAL)
#    import <Metal/Metal.h>
#    import "renderer/backend/metal/DriverMTL.h"
#    import "renderer/backend/metal/UtilsMTL.h"
#else
#    import "platform/ios/RenderViewImpl-ios.h"
#    import "platform/ios/ES3Renderer-ios.h"
#    import "platform/ios/OpenGL_Internal-ios.h"
#endif

// CLASS IMPLEMENTATIONS:

#define IOS_MAX_TOUCHES_COUNT 10

@interface EARenderView ()
@property(nonatomic) TextInputView* textInputView;
@property(nonatomic, readwrite, assign) BOOL isKeyboardShown;
@property(nonatomic, copy) NSNotification* keyboardShowNotification;
@property(nonatomic, assign) CGRect originalRect;
@end

@implementation EARenderView

@synthesize surfaceSize = size_;
@synthesize pixelFormat = pixelformat_, depthFormat = depthFormat_;
#if !defined(AX_USE_METAL)
@synthesize context = context_;
#endif
@synthesize multiSampling            = multiSampling_;
@synthesize keyboardShowNotification = keyboardShowNotification_;
@synthesize isKeyboardShown;
@synthesize originalRect = originalRect_;

+ (Class)layerClass
{
#if defined(AX_USE_METAL)
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

        originalRect_                 = self.frame;
        self.keyboardShowNotification = nil;
        if ([self respondsToSelector:@selector(setContentScaleFactor:)])
        {
            self.contentScaleFactor = [[UIScreen mainScreen] scale];
        }

#if defined(AX_USE_METAL)
        AX_UNUSED_PARAM(format);
        AX_UNUSED_PARAM(depth);
        AX_UNUSED_PARAM(sharegroup);
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        if (!device)
        {
            AXLOGE("Doesn't support metal.");
            return nil;
        }
        CAMetalLayer* metalLayer   = (CAMetalLayer*)[self layer];
        metalLayer.device          = device;
        metalLayer.pixelFormat     = MTLPixelFormatBGRA8Unorm;
        metalLayer.framebufferOnly = YES;
        ax::backend::DriverMTL::setCAMetalLayer(metalLayer);
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
#if defined(AX_USE_METAL)
        size_ = [self bounds].size;
#else
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*)[self layer];

        pixelformat_      = (int)ax::PixelFormat::RGB565;
        depthFormat_      = (int)ax::PixelFormat::D24S8;
        multiSampling_    = NO;
        requestedSamples_ = 0;
        size_             = [eaglLayer bounds].size;

        if (![self setupSurfaceWithSharegroup:nil])
        {
            [self release];
            return nil;
        }
#endif
    }

    return self;
}

- (int)getWidth
{
    CGSize bound = [self bounds].size;
    return (int)bound.width * self.contentScaleFactor;
}

- (int)getHeight
{
    CGSize bound = [self bounds].size;
    return (int)bound.height * self.contentScaleFactor;
}

#if !defined(AX_USE_METAL)
- (BOOL)setupSurfaceWithSharegroup:(void*)sharegroup
{
    CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;

    NSString* platformPF = pixelformat_ == (int)ax::PixelFormat::RGB565 ? kEAGLColorFormatRGB565 : kEAGLColorFormatRGBA8;

    eaglLayer.opaque = YES;
    eaglLayer.drawableProperties =
        [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:preserveBackbuffer_],
                                                   kEAGLDrawablePropertyRetainedBacking, platformPF,
                                                   kEAGLDrawablePropertyColorFormat, nil];

    auto depth = depthFormat_ == (int)ax::PixelFormat::D24S8 ? GL_DEPTH24_STENCIL8 : 0;
    auto pixel = pixelformat_ == (int)ax::PixelFormat::RGB565 ? GL_RGB565 : GL_RGBA8_OES;
    renderer_ = [[ES3Renderer alloc] initWithDepthFormat:depth
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
#if !defined(AX_USE_METAL)
    [renderer_ release];
#endif
    [self.textInputView release];
    [super dealloc];
}

- (void)layoutSubviews
{
    if (!ax::Director::getInstance()->isValid())
        return;

    self.textInputView.bounds = originalRect_ = [self bounds];

#if defined(AX_USE_METAL)
    size_ = originalRect_.size;
    size_.width *= self.contentScaleFactor;
    size_.height *= self.contentScaleFactor;
    ax::backend::UtilsMTL::resizeDefaultAttachmentTexture(size_.width, size_.height);
#else
    [renderer_ resizeFromLayer:(CAEAGLLayer*)self.layer];
    size_ = [renderer_ backingSize];

    // Issue #914 #924
    //     Director *director = [Director sharedDirector];
    //     [director reshapeProjection:size_];
    ax::Size size;
    size.width  = size_.width;
    size.height = size_.height;
    // ax::Director::getInstance()->reshapeProjection(size);
#endif

    auto renderView = ax::Director::getInstance()->getRenderView();
    if (renderView)
    {
        auto&& designSize = renderView->getDesignResolutionSize();
        auto resolutionPolicy = renderView->getResolutionPolicy();
        renderView->setFrameSize(size_.width, size_.height);
        renderView->setDesignResolutionSize(designSize.width, designSize.height, resolutionPolicy);
    }

    ax::Application::getInstance()->applicationScreenSizeChanged(size_.width, size_.height);

    // Avoid flicker. Issue #350
    if ([NSThread isMainThread])
    {
        ax::Director::getInstance()->drawScene();
    }
}

#if defined(AX_USE_METAL)
- (void)swapBuffers
{}
#else
- (void)swapBuffers
{
    // IMPORTANT:
    // - preconditions
    //    -> context_ MUST be the OpenGL context
    //    -> renderbuffer_ must be the RENDER BUFFER

#    ifdef __IPHONE_4_0

    if (multiSampling_)
    {
        /* Resolve from msaaFramebuffer to resolveFramebuffer */
        // __gl.disableStencilTest();
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
}

- (unsigned int)convertPixelFormat:(int)pixelFormat
{
    return pixelFormat == (int)ax::PixelFormat::RGB565 ? GL_RGB565 : GL_RGBA8_OES;
}
#endif

#pragma mark EARenderView - Point conversion

- (CGPoint)convertPointFromViewToSurface:(CGPoint)point
{
    CGRect bounds = [self bounds];

    CGPoint ret;
    ret.x = (point.x - bounds.origin.x) / bounds.size.width * size_.width;
    ret.y = (point.y - bounds.origin.y) / bounds.size.height * size_.height;

    return ret;
}

- (CGRect)convertRectFromViewToSurface:(CGRect)rect
{
    CGRect bounds = [self bounds];

    CGRect ret;
    ret.origin.x    = (rect.origin.x - bounds.origin.x) / bounds.size.width * size_.width;
    ret.origin.y    = (rect.origin.y - bounds.origin.y) / bounds.size.height * size_.height;
    ret.size.width  = rect.size.width / bounds.size.width * size_.width;
    ret.size.height = rect.size.height / bounds.size.height * size_.height;

    return ret;
}

// Pass the touches to the superview
#pragma mark EARenderView - Touch Delegate
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
    if (dis < 0.0f) dis = 0.0f;

    auto renderView = ax::Director::getInstance()->getRenderView();
    dis *= renderView->getScaleY();
    dis /= self.contentScaleFactor;

    CGRect newFrame = originalRect_;
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
}
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
    NSString* type = notif.name;
    NSDictionary* info = [notif userInfo];

    CGRect begin = [[info objectForKey:UIKeyboardFrameBeginUserInfoKey] CGRectValue];
    CGRect end   = [[info objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];
    double aniDuration = [[info objectForKey:UIKeyboardAnimationDurationUserInfoKey] doubleValue];

    // Convert to current view's coordinate system
    begin = [self convertRect:begin fromView:nil];
    end   = [self convertRect:end fromView:nil];

    auto renderView  = ax::Director::getInstance()->getRenderView();
    float scaleX = renderView->getScaleX();
    float scaleY = renderView->getScaleY();

    // Convert to pixel coordinates
    begin = CGRectApplyAffineTransform(begin,
        CGAffineTransformScale(CGAffineTransformIdentity, self.contentScaleFactor, self.contentScaleFactor));
    end = CGRectApplyAffineTransform(end,
        CGAffineTransformScale(CGAffineTransformIdentity, self.contentScaleFactor, self.contentScaleFactor));

    // Adjust for viewport offset if needed
    float offestY = renderView->getViewPortRect().origin.y;
    if (offestY < 0.0f)
    {
        begin.origin.y += offestY;
        begin.size.height -= offestY;
        end.size.height -= offestY;
    }

    // Convert to design resolution coordinates
    begin = CGRectApplyAffineTransform(begin,
        CGAffineTransformScale(CGAffineTransformIdentity, 1.0f / scaleX, 1.0f / scaleY));
    end = CGRectApplyAffineTransform(end,
        CGAffineTransformScale(CGAffineTransformIdentity, 1.0f / scaleX, 1.0f / scaleY));

    // Fill notification info for Axmol IME dispatcher
    ax::IMEKeyboardNotificationInfo notiInfo;
    notiInfo.begin    = ax::Rect(begin.origin.x, begin.origin.y, begin.size.width, begin.size.height);
    notiInfo.end      = ax::Rect(end.origin.x, end.origin.y, end.size.width, end.size.height);
    notiInfo.duration = (float)aniDuration;

    ax::IMEDispatcher* dispatcher = ax::IMEDispatcher::sharedDispatcher();
    if (UIKeyboardWillShowNotification == type)
    {
        dispatcher->dispatchKeyboardWillShow(notiInfo);

        // Move the whole render view up by keyboard height
        CGFloat distance = end.size.height;
        [self doAnimationWhenKeyboardMoveWithDuration:aniDuration distance:distance];
    }
    else if (UIKeyboardDidShowNotification == type)
    {
        self.isKeyboardShown = YES;
        dispatcher->dispatchKeyboardDidShow(notiInfo);
    }
    else if (UIKeyboardWillHideNotification == type)
    {
        dispatcher->dispatchKeyboardWillHide(notiInfo);

        // Restore to original rect
        [self doAnimationWhenKeyboardMoveWithDuration:aniDuration distance:0.0f];
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
