/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once
/// @cond DO_NOT_SHOW

#include "axmol/platform/PlatformConfig.h"

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
