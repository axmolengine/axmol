/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)

#    include "JNITest.h"

#    include "axmol/platform/android/jni/JniHelper.h"

#    include <string>

using namespace ax;

JNITests::JNITests()
{
    ADD_TEST_CASE(JNITest);
}

JNITest::JNITest()
{
    auto nameLabel = Label::createWithTTF("JNI Test", "fonts/arial.ttf", 28);
    nameLabel->setPosition(VisibleRect::center().x, VisibleRect::top().y - 50);
    addChild(nameLabel);

    auto checkLabel = Label::createWithTTF("Please check console output", "fonts/arial.ttf", 22);
    checkLabel->setPosition(VisibleRect::center());
    addChild(checkLabel);

    const char* classPath = "dev.axmol.cpp_tests.JNITest";

    JniHelper::callStaticVoidMethod(classPath, "voidMethod1");

    JniHelper::callStaticVoidMethod(classPath, "voidMethod2", "JNI is easy");

    JniHelper::callStaticVoidMethod(classPath, "voidMethod3", int(4), float(2.5), "JNI is really easy");

    bool b1 = JniHelper::callStaticBooleanMethod(classPath, "booleanMethod", int(5));
    AX_ASSERT(b1 == true);
    bool b2 = JniHelper::callStaticBooleanMethod(classPath, "booleanMethod", int(-3));
    AX_ASSERT(b2 == false);

    int i = JniHelper::callStaticIntMethod(classPath, "intMethod", int(10), int(10));
    AX_ASSERT(i == 20);

    float f = JniHelper::callStaticFloatMethod(classPath, "floatMethod", float(2.35), float(7.65));
    AX_ASSERT(f == 10.0);

    double d = JniHelper::callStaticDoubleMethod(classPath, "doubleMethod", double(2.5), int(4));
    AX_ASSERT(d == 10.0);

    std::string str = "ABCDEF";
    std::string s1  = JniHelper::callStaticStringMethod(classPath, "stringMethod", str, true);
    AX_ASSERT(s1 == "FEDCBA");
    std::string s2 = JniHelper::callStaticStringMethod(classPath, "stringMethod", str, false);
    AX_ASSERT(s2 == "ABCDEF");

    const char* cstr = "XYZ";
    std::string s3   = JniHelper::callStaticStringMethod(classPath, "stringMethod", cstr, true);
    AX_ASSERT(s3 == "ZYX");

    // should not crash
    for (int i = 0; i < 10000; i++)
    {
        JniHelper::callStaticVoidMethod(classPath, "voidMethod4", "ABCDEF");
    }

    // should not compile
    // JniHelper::callStaticVoidMethod(classPath, "voidMethod4", std::vector<int>());
}

#endif
