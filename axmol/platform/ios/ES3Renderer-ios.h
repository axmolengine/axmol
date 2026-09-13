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

#    import "axmol/platform/ios/ESRenderer-ios.h"

#    import <OpenGLES/ES3/gl.h>
#    import <OpenGLES/ES3/glext.h>

// #    import "platform/PlatformMacros.h"

@interface ES3Renderer : NSObject <ESRenderer> {
    // The pixel dimensions of the CAEAGLLayer
    GLint backingWidth_;
    GLint backingHeight_;

    unsigned int samplesToUse_;
    BOOL multiSampling_;

    unsigned int depthFormat_;
    unsigned int pixelFormat_;

    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
    GLuint defaultFramebuffer_;
    GLuint colorRenderbuffer_;
    GLuint depthBuffer_;

    // buffers for MSAA
    GLuint msaaFramebuffer_;
    GLuint msaaColorbuffer_;

    EAGLContext* context_;
}

/** Color Renderbuffer */
@property(nonatomic, readonly) GLuint colorRenderbuffer;

/** Default Renderbuffer */
@property(nonatomic, readonly) GLuint defaultFramebuffer;

/** MSAA Framebuffer */
@property(nonatomic, readonly) GLuint msaaFramebuffer;

/** MSAA Color Buffer */
@property(nonatomic, readonly) GLuint msaaColorbuffer;

/** EAGLContext */
@property(nonatomic, readonly) EAGLContext* context;

- (BOOL)resizeFromLayer:(CAEAGLLayer*)layer;
@end

#endif  // AX_PLATFORM_IOS
