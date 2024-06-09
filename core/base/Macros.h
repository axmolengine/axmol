/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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

#ifndef __BASE_CCMACROS_H__
#define __BASE_CCMACROS_H__

#ifndef _USE_MATH_DEFINES
#    define _USE_MATH_DEFINES
#endif

#include "base/Logging.h"
#include "platform/StdC.h"

#ifndef AXASSERT
#    if _AX_DEBUG > 0
#        if AX_ENABLE_SCRIPT_BINDING
extern bool AX_DLL cc_assert_script_compatible(const char* msg);
#            define AXASSERT(cond, msg)                                 \
                do                                                      \
                {                                                       \
                    if (!(cond))                                        \
                    {                                                   \
                        const char* m = (msg);                          \
                        if (m && *m && !cc_assert_script_compatible(m)) \
                            AXLOGE("Assert failed: {}", m);             \
                        AX_ASSERT(cond);                                \
                    }                                                   \
                } while (0)
#        else
#            define AXASSERT(cond, msg) AX_ASSERT(cond)
#        endif
#    else
#        define AXASSERT(cond, msg)
#    endif

#    define GP_ASSERT(cond) AXASSERT(cond, "")

// FIXME:: Backward compatible
#    define CCAssert AXASSERT
#endif  // AXASSERT

#include "base/Config.h"

#include "base/Random.h"

#define AX_HALF_PI   (M_PI * 0.5f)

#define AX_DOUBLE_PI (M_PI * 2)

/** @def AXRANDOM_MINUS1_1
 returns a random float between -1 and 1
 */
#define AXRANDOM_MINUS1_1() ax::rand_minus1_1()

/** @def AXRANDOM_0_1
 returns a random float between 0 and 1
 */
#define AXRANDOM_0_1() ax::rand_0_1()

/** @def AX_DEGREES_TO_RADIANS
 converts degrees to radians
 */
#define AX_DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) * 0.01745329252f)  // PI / 180

/** @def AX_RADIANS_TO_DEGREES
 converts radians to degrees
 */
#define AX_RADIANS_TO_DEGREES(__ANGLE__) ((__ANGLE__) * 57.29577951f)  // PI * 180

#define AX_REPEAT_FOREVER                (UINT_MAX - 1)
#define kRepeatForever                   AX_REPEAT_FOREVER

/** @def AX_BLEND_SRC
default gl blend src function. Compatible with premultiplied alpha images.
*/
#define AX_BLEND_SRC ax::backend::BlendFactor::ONE
#define AX_BLEND_DST ax::backend::BlendFactor::ONE_MINUS_SRC_ALPHA

/** @def AX_NODE_DRAW_SETUP [DEPRECATED]
 Helpful macro that setups the GL server state, the correct GL program and sets the Model View Projection matrix
 @since v2.0
 */
#define AX_NODE_DRAW_SETUP()

/** @def AX_DIRECTOR_END
 Stops and removes the director from memory.
 Removes the GLView from its parent

 @since v0.99.4
 */
#define AX_DIRECTOR_END()                              \
    do                                                 \
    {                                                  \
        auto __director = ax::Director::getInstance(); \
        __director->end();                             \
    } while (0)

/** @def AX_CONTENT_SCALE_FACTOR
On Mac it returns 1;
On iPhone it returns 2 if RetinaDisplay is On. Otherwise it returns 1
*/
#define AX_CONTENT_SCALE_FACTOR() ax::Director::getInstance()->getContentScaleFactor()

/****************************/
/** RETINA DISPLAY ENABLED **/
/****************************/

/** @def AX_RECT_PIXELS_TO_POINTS
 Converts a rect in pixels to points
 */
#define AX_RECT_PIXELS_TO_POINTS(__rect_in_pixels__)                      \
    ax::Rect((__rect_in_pixels__).origin.x / AX_CONTENT_SCALE_FACTOR(),   \
             (__rect_in_pixels__).origin.y / AX_CONTENT_SCALE_FACTOR(),   \
             (__rect_in_pixels__).size.width / AX_CONTENT_SCALE_FACTOR(), \
             (__rect_in_pixels__).size.height / AX_CONTENT_SCALE_FACTOR())

/** @def AX_RECT_POINTS_TO_PIXELS
 Converts a rect in points to pixels
 */
#define AX_RECT_POINTS_TO_PIXELS(__rect_in_points_points__)                     \
    ax::Rect((__rect_in_points_points__).origin.x* AX_CONTENT_SCALE_FACTOR(),   \
             (__rect_in_points_points__).origin.y* AX_CONTENT_SCALE_FACTOR(),   \
             (__rect_in_points_points__).size.width* AX_CONTENT_SCALE_FACTOR(), \
             (__rect_in_points_points__).size.height* AX_CONTENT_SCALE_FACTOR())

/** @def AX_POINT_PIXELS_TO_POINTS
 Converts a rect in pixels to points
 */
#define AX_POINT_PIXELS_TO_POINTS(__pixels__) \
    ax::Vec2((__pixels__).x / AX_CONTENT_SCALE_FACTOR(), (__pixels__).y / AX_CONTENT_SCALE_FACTOR())

/** @def AX_POINT_POINTS_TO_PIXELS
 Converts a rect in points to pixels
 */
#define AX_POINT_POINTS_TO_PIXELS(__points__) \
    ax::Vec2((__points__).x* AX_CONTENT_SCALE_FACTOR(), (__points__).y* AX_CONTENT_SCALE_FACTOR())

/** @def AX_POINT_PIXELS_TO_POINTS
 Converts a rect in pixels to points
 */
#define AX_SIZE_PIXELS_TO_POINTS(__size_in_pixels__)                 \
    ax::Vec2((__size_in_pixels__).width / AX_CONTENT_SCALE_FACTOR(), \
             (__size_in_pixels__).height / AX_CONTENT_SCALE_FACTOR())

/** @def AX_POINT_POINTS_TO_PIXELS
 Converts a rect in points to pixels
 */
#define AX_SIZE_POINTS_TO_PIXELS(__size_in_points__)                \
    ax::Vec2((__size_in_points__).width* AX_CONTENT_SCALE_FACTOR(), \
             (__size_in_points__).height* AX_CONTENT_SCALE_FACTOR())

#ifndef FLT_EPSILON
#    define FLT_EPSILON 1.192092896e-07F
#endif  // FLT_EPSILON

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);             \
    void operator=(const TypeName&)

/**
Helper macros which converts 4-byte little/big endian
integral number to the machine native number representation

It should work same as apples CFSwapInt32LittleToHost(..)
*/

/// when define returns true it means that our architecture uses big endian
#define AX_HOST_IS_BIG_ENDIAN           (bool)(*(unsigned short*)"\0\xff" < 0x100)
#define AX_SWAP32(i)                    ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define AX_SWAP16(i)                    ((i & 0x00ff) << 8 | (i & 0xff00) >> 8)
#define AX_SWAP_INT32_LITTLE_TO_HOST(i) ((AX_HOST_IS_BIG_ENDIAN == true) ? AX_SWAP32(i) : (i))
#define AX_SWAP_INT16_LITTLE_TO_HOST(i) ((AX_HOST_IS_BIG_ENDIAN == true) ? AX_SWAP16(i) : (i))
#define AX_SWAP_INT32_BIG_TO_HOST(i)    ((AX_HOST_IS_BIG_ENDIAN == true) ? (i) : AX_SWAP32(i))
#define AX_SWAP_INT16_BIG_TO_HOST(i)    ((AX_HOST_IS_BIG_ENDIAN == true) ? (i) : AX_SWAP16(i))

/**********************/
/** Profiling Macros **/
/**********************/
#if AX_ENABLE_PROFILERS

#    define AX_PROFILER_DISPLAY_TIMERS() NS_AX::Profiler::getInstance()->displayTimers()
#    define AX_PROFILER_PURGE_ALL()      NS_AX::Profiler::getInstance()->releaseAllTimers()

#    define AX_PROFILER_START(__name__)  NS_AX::ProfilingBeginTimingBlock(__name__)
#    define AX_PROFILER_STOP(__name__)   NS_AX::ProfilingEndTimingBlock(__name__)
#    define AX_PROFILER_RESET(__name__)  NS_AX::ProfilingResetTimingBlock(__name__)

#    define AX_PROFILER_START_CATEGORY(__cat__, __name__)   \
        do                                                  \
        {                                                   \
            if (__cat__)                                    \
                NS_AX::ProfilingBeginTimingBlock(__name__); \
        } while (0)
#    define AX_PROFILER_STOP_CATEGORY(__cat__, __name__)  \
        do                                                \
        {                                                 \
            if (__cat__)                                  \
                NS_AX::ProfilingEndTimingBlock(__name__); \
        } while (0)
#    define AX_PROFILER_RESET_CATEGORY(__cat__, __name__)   \
        do                                                  \
        {                                                   \
            if (__cat__)                                    \
                NS_AX::ProfilingResetTimingBlock(__name__); \
        } while (0)

#    define AX_PROFILER_START_INSTANCE(__id__, __name__)                                       \
        do                                                                                     \
        {                                                                                      \
            NS_AX::ProfilingBeginTimingBlock(                                                  \
                NS_AX::String::createWithFormat("%08X - %s", __id__, __name__)->getCString()); \
        } while (0)
#    define AX_PROFILER_STOP_INSTANCE(__id__, __name__)                                        \
        do                                                                                     \
        {                                                                                      \
            NS_AX::ProfilingEndTimingBlock(                                                    \
                NS_AX::String::createWithFormat("%08X - %s", __id__, __name__)->getCString()); \
        } while (0)
#    define AX_PROFILER_RESET_INSTANCE(__id__, __name__)                                       \
        do                                                                                     \
        {                                                                                      \
            NS_AX::ProfilingResetTimingBlock(                                                  \
                NS_AX::String::createWithFormat("%08X - %s", __id__, __name__)->getCString()); \
        } while (0)

#else

#    define AX_PROFILER_DISPLAY_TIMERS() \
        do                               \
        {                                \
        } while (0)
#    define AX_PROFILER_PURGE_ALL() \
        do                          \
        {                           \
        } while (0)

#    define AX_PROFILER_START(__name__) \
        do                              \
        {                               \
        } while (0)
#    define AX_PROFILER_STOP(__name__) \
        do                             \
        {                              \
        } while (0)
#    define AX_PROFILER_RESET(__name__) \
        do                              \
        {                               \
        } while (0)

#    define AX_PROFILER_START_CATEGORY(__cat__, __name__) \
        do                                                \
        {                                                 \
        } while (0)
#    define AX_PROFILER_STOP_CATEGORY(__cat__, __name__) \
        do                                               \
        {                                                \
        } while (0)
#    define AX_PROFILER_RESET_CATEGORY(__cat__, __name__) \
        do                                                \
        {                                                 \
        } while (0)

#    define AX_PROFILER_START_INSTANCE(__id__, __name__) \
        do                                               \
        {                                                \
        } while (0)
#    define AX_PROFILER_STOP_INSTANCE(__id__, __name__) \
        do                                              \
        {                                               \
        } while (0)
#    define AX_PROFILER_RESET_INSTANCE(__id__, __name__) \
        do                                               \
        {                                                \
        } while (0)

#endif

/*********************************/
/** 64bits Program Sense Macros **/
/*********************************/
#if defined(_M_X64) || defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(__x86_64) || \
    defined(__arm64__) || defined(__aarch64__)
#    define AX_64BITS 1
#else
#    define AX_64BITS 0
#endif

/******************************************************************************************/
/** LittleEndian Sense Macro, from google protobuf see:                                  **/
/** https://github.com/google/protobuf/blob/master/src/google/protobuf/io/coded_stream.h **/
/******************************************************************************************/
#ifdef _WIN32
// Assuming windows is always little-endian.
#    if !defined(AX_DISABLE_LITTLE_ENDIAN_OPT_FOR_TEST)
#        define AX_LITTLE_ENDIAN 1
#    endif
#    if defined(_MSC_VER) && _MSC_VER >= 1300 && !defined(__INTEL_COMPILER)
// If MSVC has "/RTCc" set, it will complain about truncating casts at
// runtime.  This file contains some intentional truncating casts.
#        pragma runtime_checks("c", off)
#    endif
#else
#    ifdef __APPLE__
#        include <machine/endian.h>  // __BYTE_ORDER
#    elif defined(__FreeBSD__)
#        include <sys/endian.h>  // __BYTE_ORDER
#    else
#        if !defined(__QNX__)
#            include <endian.h>  // __BYTE_ORDER
#        endif
#    endif
#    if ((defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)) ||    \
         (defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN)) && \
        !defined(AX_DISABLE_LITTLE_ENDIAN_OPT_FOR_TEST)
#        define AX_LITTLE_ENDIAN 1
#    endif
#endif

/** @def AX_INCREMENT_GL_DRAWS_BY_ONE
 Increments the GL Draws counts by one.
 The number of calls per frame are displayed on the screen when the Director's stats are enabled.
 */
#define AX_INCREMENT_GL_DRAWS(__n__) ax::Director::getInstance()->getRenderer()->addDrawnBatches(__n__)
#define AX_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(__drawcalls__, __vertices__) \
    do                                                                          \
    {                                                                           \
        auto __renderer__ = ax::Director::getInstance() -> getRenderer();       \
        __renderer__->addDrawnBatches(__drawcalls__);                           \
        __renderer__->addDrawnVertices(__vertices__);                           \
    } while (0)

/*******************/
/** Notifications **/
/*******************/
/** @def AnimationFrameDisplayedNotification
 Notification name when a SpriteFrame is displayed
 */
#define AnimationFrameDisplayedNotification "CCAnimationFrameDisplayedNotification"

/*******************/
/** Notifications **/
/*******************/
/** @def Animate3DDisplayedNotification
 Notification name when a frame in Animate3D is played
 */
#define Animate3DDisplayedNotification "CCAnimate3DDisplayedNotification"

// new callbacks based on C++11
#define AX_CALLBACK_0(__selector__, __target__, ...) std::bind(&__selector__, __target__, ##__VA_ARGS__)
#define AX_CALLBACK_1(__selector__, __target__, ...) \
    std::bind(&__selector__, __target__, std::placeholders::_1, ##__VA_ARGS__)
#define AX_CALLBACK_2(__selector__, __target__, ...) \
    std::bind(&__selector__, __target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define AX_CALLBACK_3(__selector__, __target__, ...)                                                          \
    std::bind(&__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, \
              ##__VA_ARGS__)

#endif  // __BASE_CCMACROS_H__
