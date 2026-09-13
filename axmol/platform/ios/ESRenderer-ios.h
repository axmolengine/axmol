/****************************************************************************
 Copyright (c) 2010      Ricardo Quesada
 Copyright (c) 2010-2012 cocos2d-x.org
 Corpyight (c) 2011      Zynga Inc.
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

// Only compile this code on iOS. These files should NOT be included on your Mac project.
// But in case they are included, it won't be compiled.

#pragma once

#include "axmol/platform/PlatformConfig.h"
#if AX_TARGET_PLATFORM == AX_PLATFORM_IOS

// #    include "platform/PlatformMacros.h"
#    import <QuartzCore/QuartzCore.h>

#    import <OpenGLES/EAGL.h>
#    import <OpenGLES/EAGLDrawable.h>

@protocol ESRenderer <NSObject>

- (id)initWithDepthFormat:(unsigned int)depthFormat
          withPixelFormat:(unsigned int)pixelFormat
           withSharegroup:(EAGLSharegroup*)sharegroup
        withMultiSampling:(BOOL)multiSampling
      withNumberOfSamples:(unsigned int)requestedSamples;

- (BOOL)resizeFromLayer:(CAEAGLLayer*)layer;

- (EAGLContext*)context;
- (CGSize)backingSize;

- (unsigned int)colorRenderBuffer;
- (unsigned int)defaultFrameBuffer;
- (unsigned int)msaaFrameBuffer;
- (unsigned int)msaaColorBuffer;
@end

#endif  // AX_PLATFORM_IOS
