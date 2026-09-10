/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

/* AX_LUA_API */
#if defined(_WIN32)
#    if defined(AX_LUA_DLLEXPORT)
#        define AX_LUA_API __declspec(dllexport)
#    elif defined(AX_LUA_DLLIMPORT)
#        define AX_LUA_API __declspec(dllimport)
#    endif
#elif defined(AX_LUA_DLLEXPORT) || defined(AX_LUA_DLLIMPORT)
#    define AX_LUA_API __attribute__((visibility("default")))
#endif
#ifndef AX_LUA_API
#    define AX_LUA_API
#endif

#define AX_LUA_DLL AX_LUA_API
