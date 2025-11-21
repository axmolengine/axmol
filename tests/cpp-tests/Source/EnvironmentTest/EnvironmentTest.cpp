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

#include "EnvironmentTest.h"
#include "../testResource.h"
#include "axmol/axmol.h"

using namespace ax;

EnvironmentTests::EnvironmentTests()
{
    ADD_TEST_CASE(EnvironmentLoadConfig);
    ADD_TEST_CASE(EnvironmentQuery);
    ADD_TEST_CASE(EnvironmentInvalid);
    ADD_TEST_CASE(EnvironmentDefault);
    ADD_TEST_CASE(EnvironmentSet);
}

std::string EnvironmentBase::title() const
{
    return "Environment Test";
}

//------------------------------------------------------------------
//
// EnvironmentLoadConfig
//
//------------------------------------------------------------------
void EnvironmentLoadConfig::onEnter()
{
    EnvironmentBase::onEnter();

    Environment::getInstance()->loadConfigFile("configs/config-test-ok.plist");
    std::string config = Environment::getInstance()->getInfo();
    AXLOGD("{}\n", config);
}

std::string EnvironmentLoadConfig::subtitle() const
{
    return "Loading config file manually. See console";
}

//------------------------------------------------------------------
//
// EnvironmentQuery
//
//------------------------------------------------------------------
void EnvironmentQuery::onEnter()
{
    EnvironmentBase::onEnter();

    AXLOGD("axmol version: %s", Environment::getInstance()->getValue("axmol.version").asString());
    AXLOGD("OpenGL version: %s", Environment::getInstance()->getValue("gl.version").asString());
}

std::string EnvironmentQuery::subtitle() const
{
    return "Check the console";
}

//------------------------------------------------------------------
//
// EnvironmentInvalid
//
//------------------------------------------------------------------
void EnvironmentInvalid::onEnter()
{
    EnvironmentBase::onEnter();

    Environment::getInstance()->loadConfigFile("configs/config-test-invalid.plist");
}

std::string EnvironmentInvalid::subtitle() const
{
    return "Loading an invalid config file";
}

//------------------------------------------------------------------
//
// EnvironmentDefault
//
//------------------------------------------------------------------
void EnvironmentDefault::onEnter()
{
    EnvironmentBase::onEnter();

    std::string c_value = Environment::getInstance()->getValue("invalid.key", Value("no key")).asString();
    if (c_value != "no key")
        AXLOGD("1. Test failed!");
    else
        AXLOGD("1. Test OK!");

    bool b_value = Environment::getInstance()->getValue("invalid.key", Value(true)).asBool();
    if (!b_value)
        AXLOGD("2. Test failed!");
    else
        AXLOGD("2. Test OK!");

    double d_value = Environment::getInstance()->getValue("invalid.key", Value(42.42)).asDouble();
    if (d_value != 42.42)
        AXLOGD("3. Test failed!");
    else
        AXLOGD("3. Test OK!");
}

std::string EnvironmentDefault::subtitle() const
{
    return "Tests defaults values";
}

//------------------------------------------------------------------
//
// EnvironmentSet
//
//------------------------------------------------------------------
void EnvironmentSet::onEnter()
{
    EnvironmentBase::onEnter();

    auto env = Environment::getInstance();

    env->setValue("this.is.an.int.value", Value(10));
    env->setValue("this.is.a.bool.value", Value(true));
    env->setValue("this.is.a.string.value", Value("hello world"));

    auto str = env->getInfo();
    AXLOGD("{}\n", str);
}

std::string EnvironmentSet::subtitle() const
{
    return "Tests setting values manually";
}
