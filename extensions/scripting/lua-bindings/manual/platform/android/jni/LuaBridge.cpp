/****************************************************************************
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

#include "LuaBridge.h"

#if defined(__ANDROID__)
#    include <android/log.h>

#    include "lua-bindings/manual/platform/android/LuaJavaBridge.h"

#    include "base/UTF8.h"

#    define LOG_TAG "LuaBridge_java"
#    define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

extern "C" {

JNIEXPORT jint JNICALL Java_org_axmol_lib_LuaBridge_callLuaFunctionWithString(JNIEnv* env,
                                                                                             jclass,
                                                                                             jint functionId,
                                                                                             jstring value)
{
    std::string strValue = ax::StringUtils::getStringUTFCharsJNI(env, value);
    int ret              = LuaJavaBridge::callLuaFunctionById(functionId, strValue.c_str());
    return ret;
}

JNIEXPORT jint JNICALL
Java_org_axmol_lib_LuaBridge_callLuaGlobalFunctionWithString(JNIEnv* env,
                                                                            jclass,
                                                                            jstring luaFunctionName,
                                                                            jstring value)
{
    std::string functionNameStr = ax::StringUtils::getStringUTFCharsJNI(env, luaFunctionName);
    std::string valueStr        = ax::StringUtils::getStringUTFCharsJNI(env, value);

    int ret = LuaJavaBridge::callLuaGlobalFunction(functionNameStr.c_str(), valueStr.c_str());
    return ret;
}

JNIEXPORT jint JNICALL Java_org_axmol_lib_LuaBridge_retainLuaFunction(JNIEnv* env,
                                                                                     jclass,
                                                                                     jint luaFunctionId)
{
    return LuaJavaBridge::retainLuaFunctionById(luaFunctionId);
}

JNIEXPORT jint JNICALL Java_org_axmol_lib_LuaBridge_releaseLuaFunction(JNIEnv* env,
                                                                                      jclass,
                                                                                      jint luaFunctionId)
{
    return LuaJavaBridge::releaseLuaFunctionById(luaFunctionId);
}

}  // extern "C"

#endif
