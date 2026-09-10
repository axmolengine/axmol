/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
 ****************************************************************************/
package dev.axmol.sample.LuaJavaBridgeTest;

import dev.axmol.lib.LuaBridge;

public class LuaJavaBridgeTest
{
	public static int addTwoNumbers(final int num1,final int num2){
		return num1 + num2;
	}

	public static void callbackLua(final String tipInfo,final int luaFunc){
        LuaBridge.callLuaFunctionWithString(luaFunc, "success");
        LuaBridge.releaseLuaFunction(luaFunc);
	}
}
