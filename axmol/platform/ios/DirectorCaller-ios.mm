/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#import "axmol/platform/ios/DirectorCaller-ios.h"
#import "axmol/platform/ios/RenderHostView-ios.h"
#import "axmol/base/Director.h"

#include <mach/mach_time.h>

#if AX_ENABLE_GL
#    import <OpenGLES/EAGL.h>
#endif

#include <span>
#include <cmath>
#include <algorithm>

// refer to:
// https://developer.apple.com/documentation/quartzcore/optimizing-iphone-and-ipad-apps-to-support-promotion-displays?language=objc
static constexpr int kAllowedFpsArray[]               = {30, 60, 120};
static constexpr std::span<const int> kAllowedFpsSpan = kAllowedFpsArray;

static int normalizeInterval(double& interval)
{
    int value           = static_cast<int>(1 / interval);
    int framesPerSecond = *std::min_element(kAllowedFpsSpan.begin(), kAllowedFpsSpan.end(), [value](int a, int b) {
        return std::abs(value - a) < std::abs(value - b);
    });

    const auto maxFPS = [[UIScreen mainScreen] maximumFramesPerSecond];
    if (framesPerSecond > maxFPS)
    {
        framesPerSecond = static_cast<int>(maxFPS);
    }

    interval = 1 / framesPerSecond;

    return framesPerSecond;
}

static id s_sharedDirectorCaller;

@interface NSObject (CADisplayLink)
+ (id)displayLinkWithTarget:(id)arg1 selector:(SEL)arg2;
- (void)addToRunLoop:(id)arg1 forMode:(id)arg2;
- (void)setFrameInterval:(NSInteger)interval;
- (void)invalidate;
@end

@implementation CCDirectorCaller

@synthesize interval;
@synthesize framesPerSecond;

+ (id)sharedDirectorCaller
{
    if (s_sharedDirectorCaller == nil)
    {
        s_sharedDirectorCaller = [[CCDirectorCaller alloc] init];
    }

    return s_sharedDirectorCaller;
}

+ (void)destroy
{
    [s_sharedDirectorCaller stopMainLoop];
    [s_sharedDirectorCaller release];
    s_sharedDirectorCaller = nil;
}

- (instancetype)init
{
    if (self = [super init])
    {
        isAppActive              = [UIApplication sharedApplication].applicationState == UIApplicationStateActive;
        NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
        [nc addObserver:self
               selector:@selector(appDidBecomeActive)
                   name:UIApplicationDidBecomeActiveNotification
                 object:nil];
        [nc addObserver:self
               selector:@selector(appDidBecomeInactive)
                   name:UIApplicationWillResignActiveNotification
                 object:nil];

        self.interval = 1 / 60.f;
    }
    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [self stopMainLoop];
    [super dealloc];
}

- (void)appDidBecomeActive
{
    // initialize initLastDisplayTime, or the dt will be invalid when
    // - the app is lauched
    // - the app resumes from background
    [self updateLastDisplayTime];

    isAppActive = YES;
}

- (void)appDidBecomeInactive
{
    isAppActive = NO;
}

- (void)startMainLoop
{
    // Director::setAnimationInterval() is called, we should invalidate it first
    [self stopMainLoop];

    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(doCaller:)];

    if (@available(iOS 15.0, *))
    {
        displayLink.preferredFrameRateRange = (CAFrameRateRange){.minimum = static_cast<float>(kAllowedFpsSpan.front()),
                                                                 .maximum = static_cast<float>(kAllowedFpsSpan.back()),
                                                                 .preferred = static_cast<float>(self.framesPerSecond)};
    }
    else
    {
        displayLink.preferredFramesPerSecond = self.framesPerSecond;
    }

    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)stopMainLoop
{
    if (displayLink != nil)
    {
        [displayLink invalidate];
        displayLink = nil;
    }
}

- (void)setAnimationInterval:(double)intervalNew
{
    self.framesPerSecond = normalizeInterval(intervalNew);
    self.interval        = intervalNew;

    [self startMainLoop];
}

- (void)doCaller:(id)sender
{
    if (isAppActive)
    {
        ax::Director* director = ax::Director::getInstance();
#if AX_GLES_PROFILE
        EAGLContext* context = [(__bridge RenderHostView*)director->getRenderView()->getNativeDisplay() context];
        if (context != [EAGLContext currentContext])
            glFlush();

        [EAGLContext setCurrentContext:context];
#endif
        CFTimeInterval dt = ((CADisplayLink*)displayLink).timestamp - lastDisplayTime;
        lastDisplayTime   = ((CADisplayLink*)displayLink).timestamp;
        director->stepFrame(dt);
    }
}

- (void)updateLastDisplayTime
{
    struct mach_timebase_info timeBaseInfo;
    mach_timebase_info(&timeBaseInfo);
    CGFloat clockFrequency = (CGFloat)timeBaseInfo.denom / (CGFloat)timeBaseInfo.numer;
    clockFrequency *= 1000000000.0;
    // convert absolute time to seconds and should minus one frame time interval
    lastDisplayTime = (mach_absolute_time() / clockFrequency) - self.interval;
}

@end
