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

#pragma once
/// @cond DO_NOT_SHOW

#include "platform/PlatformConfig.h"

#include <string.h>
#include <assert.h>

#if defined(__ANDROID__)
#    include <android/log.h>
#endif

#define AX_ASSERT(cond)              assert(cond)
#define AX_UNUSED_PARAM(unusedparam) (void)unusedparam

/* Define NULL pointer value */
#ifndef NULL
#    ifdef __cplusplus
#        define NULL 0
#    else
#        define NULL ((void*)0)
#    endif
#endif

/* AX_API */
#if defined(_WIN32)
#    if defined(AX_DLLEXPORT)
#        define AX_API __declspec(dllexport)
#    elif defined(AX_DLLIMPORT)
#        define AX_API __declspec(dllimport)
#    endif
#elif defined(AX_DLLEXPORT) || defined(AX_DLLIMPORT)
#    define AX_API __attribute__((visibility("default")))
#endif
#ifndef AX_API
#    define AX_API
#endif

#define AX_DLL AX_API
