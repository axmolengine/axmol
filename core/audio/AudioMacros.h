/****************************************************************************
 Copyright (c) 2016-2017 Chukong Technologies Inc.
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

#include "base/Logging.h"

#include <functional>

#define QUEUEBUFFER_NUM (3)
#define QUEUEBUFFER_TIME_STEP (0.05f)

#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)

#if defined(_AX_DEBUG) && _AX_DEBUG > 0
#    define CHECK_AL_ERROR_DEBUG()                                                                     \
        do                                                                                             \
        {                                                                                              \
            ALenum __error = alGetError();                                                             \
            if (__error)                                                                               \
            {                                                                                          \
                AXLOGE("OpenAL error 0x{:04X} in {} {} {}\n", __error, __FILE__, __FUNCTION__, __LINE__); \
            }                                                                                          \
        } while (false)
#else
#    define CHECK_AL_ERROR_DEBUG()
#endif

#define BREAK_IF(condition) \
    if (!!(condition))      \
    {                       \
        break;              \
    }

#define BREAK_IF_ERR_LOG(condition, fmt, ...)                                   \
    if (!!(condition))                                                          \
    {                                                                           \
        AXLOGE("(" QUOTEME(condition) ") failed, message: " fmt, ##__VA_ARGS__); \
        break;                                                                  \
    }

#define AUDIO_ID int
