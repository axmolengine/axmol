/****************************************************************************
Copyright (c) 2011-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#pragma once

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

extern int axlua_loader(lua_State* L);
}
