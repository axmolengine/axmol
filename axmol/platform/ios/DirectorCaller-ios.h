/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#import <Foundation/Foundation.h>
#import <QuartzCore/CADisplayLink.h>

@interface CCDirectorCaller : NSObject {
    CADisplayLink* displayLink;
    double interval;
    int framesPerSecond;
    BOOL isAppActive;
    CFTimeInterval lastDisplayTime;
}
@property(readwrite) double interval;
@property(readwrite) int framesPerSecond;
- (void)startMainLoop;
- (void)stopMainLoop;
- (void)doCaller:(id)sender;
- (void)setAnimationInterval:(double)interval;
+ (id)sharedDirectorCaller;
+ (void)destroy;
@end
