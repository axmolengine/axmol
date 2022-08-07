/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axys1.github.io/

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

#ifndef SCRIPT_LUA_PLATFORM_ANDROID_JNI_JAVA_ORG_COCOS2DX_LIB_AXLUA_JAVA_BRIDEG_H
#define SCRIPT_LUA_PLATFORM_ANDROID_JNI_JAVA_ORG_COCOS2DX_LIB_AXLUA_JAVA_BRIDEG_H
#if defined(__ANDROID__)

#    include <jni.h>

extern "C" {

/// @cond
/*
 * Class:     Java_org_axys1_lib_AxysLuaJavaBridge
 * Method:    callLuaFunctionWithString
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_axys1_lib_AxysLuaJavaBridge_callLuaFunctionWithString(JNIEnv*,
                                                                                             jclass,
                                                                                             jint,
                                                                                             jstring);

/*
 * Class:     Java_org_axys1_lib_AxysLuaJavaBridge
 * Method:    callLuaGlobalFunctionWithString
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_axys1_lib_AxysLuaJavaBridge_callLuaGlobalFunctionWithString(JNIEnv* env,
                                                                                                   jclass,
                                                                                                   jstring,
                                                                                                   jstring);

/*
 * Class:     Java_org_axys1_lib_AxysLuaJavaBridge
 * Method:    retainLuaFunction
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_axys1_lib_AxysLuaJavaBridge_retainLuaFunction(JNIEnv* env, jclass, jint);

/*
 * Class:     Java_org_axys1_lib_AxysLuaJavaBridge
 * Method:    releaseLuaFunction
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_axys1_lib_AxysLuaJavaBridge_releaseLuaFunction(JNIEnv* env, jclass, jint);

}  // extern "C"

#endif

/// @endcond
#endif  // SCRIPT_LUA_PLATFORM_ANDROID_JNI_JAVA_ORG_COCOS2DX_LIB_AXLUA_JAVA_BRIDEG_H
