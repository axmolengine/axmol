/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2020 c4games.com.

 http://www.cocos2d-x.org

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
#pragma once

#if !defined(CC_USE_MOJOAL)
#define CC_USE_MOJOAL 0
#endif

#if !defined(__APPLE__)
#if !defined(CC_USE_ALSOFT) && !CC_USE_MOJOAL
#define CC_USE_ALSOFT 1
#endif
#endif

// if CC_USE_MOJOAL, force disable openal-soft
#if CC_USE_MOJOAL
#define CC_USE_ALSOFT 0
#endif

#if !CC_USE_ALSOFT && !CC_USE_MOJOAL
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#define MAX_AUDIOINSTANCES 24
#else
#define AL_ALEXT_PROTOTYPES 1
#include "AL/al.h"
#include "AL/alc.h"
#if !CC_USE_MOJOAL
#include "AL/alext.h"
#endif
#define MAX_AUDIOINSTANCES 32
#endif
