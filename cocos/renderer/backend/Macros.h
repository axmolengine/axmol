/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

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

#define CC_BACKEND_BEGIN  namespace cocos2d{ namespace backend{
#define CC_BACKEND_END    }}

#define MAX_COLOR_ATTCHMENT 4

#define MAX_INFLIGHT_BUFFER 3

// compatibility with non-clang compilers...
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

 /*
  * helps the compiler's optimizer predicting branches
  */
#if __has_builtin(__builtin_expect)
#   ifdef __cplusplus
#      define UTILS_LIKELY( exp )    (__builtin_expect( !!(exp), true ))
#      define UTILS_UNLIKELY( exp )  (__builtin_expect( !!(exp), false ))
#   else
#      define UTILS_LIKELY( exp )    (__builtin_expect( !!(exp), 1 ))
#      define UTILS_UNLIKELY( exp )  (__builtin_expect( !!(exp), 0 ))
#   endif
#else
#   define UTILS_LIKELY( exp )    (!!(exp))
#   define UTILS_UNLIKELY( exp )  (!!(exp))
#endif

#define CC_ARRAYSIZE(A) (sizeof(A) / sizeof((A)[0]))
