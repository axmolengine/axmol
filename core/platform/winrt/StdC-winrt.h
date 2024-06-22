/****************************************************************************
Copyright (c) 2010 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include <BaseTsd.h>
#ifndef __SSIZE_T
#    define __SSIZE_T
typedef SSIZE_T ssize_t;
#endif  // __SSIZE_T

#include "platform/PlatformMacros.h"
#include <float.h>

// for math.h on win32 platform
#    if !defined(_USE_MATH_DEFINES)
#        define _USE_MATH_DEFINES  // make M_PI can be use
#    endif

#    if _MSC_VER < 1800
#        if !defined(isnan)
#            define isnan _isnan
#        endif
#    endif

#    if _MSC_VER < 1900
#        ifndef snprintf
#            define snprintf _snprintf
#        endif
#    endif

#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef M_PI
#    define M_PI 3.14159265358
#endif
#ifndef M_PI_2
#    define M_PI_2 1.57079632679
#endif

#ifndef MIN
#    define MIN(x, y) (((x) > (y)) ? (y) : (x))
#endif  // MIN

#ifndef MAX
#    define MAX(x, y) (((x) < (y)) ? (y) : (x))
#endif  // MAX

# include <stdint.h>

#ifndef NOMINMAX
#    define NOMINMAX
#endif

#    include <WinSock2.h>
// Structure timeval has define in winsock.h, include windows.h for it.
#    include <Windows.h>

NS_AX_BEGIN

struct timezone
{
    int tz_minuteswest;
    int tz_dsttime;
};

int AX_DLL gettimeofday(struct timeval*, struct timezone*);

NS_AX_END

// Conflicted with ParticleSystem::PositionType::RELATIVE, so we need to undef it.
#ifdef RELATIVE
#    undef RELATIVE
#endif

// Conflicted with CCBReader::SizeType::RELATIVE and CCBReader::ScaleType::RELATIVE, so we need to undef it.
#ifdef ABSOLUTE
#    undef ABSOLUTE
#endif

// Conflicted with HttpRequest::Type::DELETE, so we need to undef it.
#ifdef DELETE
#    undef DELETE
#endif

#undef min
#undef max
