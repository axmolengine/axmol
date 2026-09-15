/****************************************************************************
 Copyright (c) 2016-2017 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/base/Logging.h"

#include <functional>

#define QUEUEBUFFER_NUM       (3)
#define QUEUEBUFFER_TIME_STEP (0.05f)

#define QUOTEME_(x)           #x
#define QUOTEME(x)            QUOTEME_(x)

#if defined(_AX_DEBUG) && _AX_DEBUG > 0
#    define CHECK_AL_ERROR_DEBUG()                                                                        \
        do                                                                                                \
        {                                                                                                 \
            ALenum __error = alGetError();                                                                \
            if (__error)                                                                                  \
            {                                                                                             \
                AXLOGE("OpenAL error 0x{:04X} in {} {} {}\n", __error, __FILE__, __FUNCTION__, __LINE__); \
            }                                                                                             \
        } while (false)
#else
#    define CHECK_AL_ERROR_DEBUG()
#endif

#define BREAK_IF(condition) \
    if (!!(condition))      \
    {                       \
        break;              \
    }

#define BREAK_IF_ERR_LOG(condition, fmt, ...)                                    \
    if (!!(condition))                                                           \
    {                                                                            \
        AXLOGE("(" QUOTEME(condition) ") failed, message: " fmt, ##__VA_ARGS__); \
        break;                                                                   \
    }

namespace ax
{
using AudioId = int;
}
