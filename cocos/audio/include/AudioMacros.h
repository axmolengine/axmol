/****************************************************************************
 Copyright (c) 2016-2017 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2018 HALX99.

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

#include "platform/CCPlatformConfig.h"

#include <functional>

#define QUEUEBUFFER_NUM (3)
#define QUEUEBUFFER_TIME_STEP (0.05f)

#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)

// log, CCLOG aren't threadsafe, since we uses sub threads for parsing pcm data, threadsafe log output
// is needed. Define the following macros (ALOGV, ALOGD, ALOGI, ALOGW, ALOGE) for threadsafe log output.
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#include "base/ccUTF8.h" // for StringUtils::format
#define AUDIO_LOG(fmt, ...)  \
  OutputDebugStringA(StringUtils::format( (fmt "\r\n"),  ##__VA_ARGS__ ).c_str())
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <android/log.h>
#define AUDIO_LOG(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, "AudioEngine", fmt, ##__VA_ARGS__)
#else // other platforms
#define AUDIO_LOG(fmt,...) printf(fmt "\n", ##__VA_ARGS__)
#endif

#if defined(COCOS2D_DEBUG) && COCOS2D_DEBUG > 0
#define ALOGV(fmt, ...) AUDIO_LOG("V/" LOG_TAG " (" QUOTEME(__LINE__) "): " fmt "", ##__VA_ARGS__)
#else
#define ALOGV(fmt, ...) do {} while(false)
#endif
#define ALOGD(fmt, ...) AUDIO_LOG("D/" LOG_TAG " (" QUOTEME(__LINE__) "): " fmt "", ##__VA_ARGS__)
#define ALOGI(fmt, ...) AUDIO_LOG("I/" LOG_TAG " (" QUOTEME(__LINE__) "): " fmt "", ##__VA_ARGS__)
#define ALOGW(fmt, ...) AUDIO_LOG("W/" LOG_TAG " (" QUOTEME(__LINE__) "): " fmt "", ##__VA_ARGS__)
#define ALOGE(fmt, ...) AUDIO_LOG("E/" LOG_TAG " (" QUOTEME(__LINE__) "): " fmt "", ##__VA_ARGS__)

#if defined(COCOS2D_DEBUG) && COCOS2D_DEBUG > 0
#define CHECK_AL_ERROR_DEBUG() \
do { \
    ALenum __error = alGetError(); \
    if (__error) { \
        ALOGE("OpenAL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__); \
    } \
} while (false)
#else
#define CHECK_AL_ERROR_DEBUG()
#endif

#define BREAK_IF(condition) \
    if (!!(condition)) { \
        break; \
    }

#define BREAK_IF_ERR_LOG(condition, fmt, ...) \
    if (!!(condition)) { \
        ALOGE("(" QUOTEME(condition) ") failed, message: " fmt, ##__VA_ARGS__); \
        break; \
    }

#define AUDIO_ID int
#define AUDIO_ID_PRID "%d"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC) && !CC_USE_ALSOFT_ON_APPLE
#import <OpenAL/al.h>
#define MAX_AUDIOINSTANCES 24
#else
#ifdef OPENAL_PLAIN_INCLUDES
#include <al.h>
#include <alext.h>
#else
#include <AL/al.h>
#include <AL/alext.h>
#endif
#define MAX_AUDIOINSTANCES 32
#endif
