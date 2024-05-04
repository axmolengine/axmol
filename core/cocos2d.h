/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmolengine.github.io/

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

#ifndef __COCOS2D_H__
#define __COCOS2D_H__

#include "axmol.h"

#define COCOS2D_VERSION 0x00040000

NS_AX_BEGIN

AX_DLL const char* cocos2dVersion();

/** Backward compatibility with old axmol projects */

using Sprite3D              = MeshRenderer;
using Sprite3DMaterial      = MeshMaterial;
using Sprite3DCache         = MeshRendererCache;
using Sprite3DMaterialCache = MeshMaterialCache;
using Ref                   = Object;

#define setDisplayStats setStatsDisplay

NS_AX_END

namespace cocos2d = ax;

#define USING_NS_CC                  USING_NS_AX

#define CC_SAFE_RELEASE              AX_SAFE_RELEASE
#define CC_SAFE_RELEASE_NULL         AX_SAFE_RELEASE_NULL
#define CC_SAFE_DELETE               AX_SAFE_DELETE
#define CCASSERT                     AXASSERT
#define CC_ASSERT                    AX_ASSERT
#define CC_CONSTRUCTOR_ACCESS        public

#define CCLOG                        AXLOG
#define CCLOGINFO                    AXLOGINFO
#define CCLOGWARN                    AXLOGWARN
#define CCLOGERROR                   AXLOGERROR

#define CC_DEPRECATED_ATTRIBUTE      AX_DEPRECATED_ATTRIBUTE

#define CC_SAFE_RETAIN               AX_SAFE_RETAIN

#define CC_SYNTHESIZE_READONLY       AX_SYNTHESIZE_READONLY
#define CC_SYNTHESIZE                AX_SYNTHESIZE
#define CC_PROPERTY                  AX_PROPERTY

#define CC_USE_CULLING               AX_USE_CULLING

#define CC_CALLBACK_0                AX_CALLBACK_0
#define CC_CALLBACK_1                AX_CALLBACK_1
#define CC_CALLBACK_2                AX_CALLBACK_2

#define CC_SCHEDULE_SELECTOR         AX_SCHEDULE_SELECTOR

#define CC_INVALID_INDEX             AX_INVALID_INDEX

#define CC_TARGET_PLATFORM           AX_TARGET_PLATFORM
#define CC_PLATFORM_UNKNOWN          AX_PLATFORM_UNKNOWN
#define CC_PLATFORM_IOS              AX_PLATFORM_IOS
#define CC_PLATFORM_ANDROID          AX_PLATFORM_ANDROID
#define CC_PLATFORM_WIN32            AX_PLATFORM_WIN32
#define CC_PLATFORM_LINUX            AX_PLATFORM_LINUX
#define CC_PLATFORM_MAC              AX_PLATFORM_MAC

#define CC_REQUIRES_NULL_TERMINATION AX_REQUIRES_NULL_TERMINATION

#endif  // __COCOS2D_H__
