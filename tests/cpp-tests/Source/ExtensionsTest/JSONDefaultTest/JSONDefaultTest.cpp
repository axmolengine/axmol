/****************************************************************************
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

#include "JSONDefaultTest.h"
#include "JSONDefault/JSONDefault.h"

using namespace std;

using namespace ax;
USING_NS_AX_EXT;

JSONDefaultTests::JSONDefaultTests()
{
    ADD_TEST_CASE(JSONDefaultTest);
}

JSONDefaultTest::JSONDefaultTest()
{
    auto s     = Director::getInstance()->getWinSize();
    auto label = Label::createWithTTF("CCJSONDefault test Log data see console", "fonts/arial.ttf", 22);
    addChild(label, 0);
    label->setPosition(Vec2(s.width / 2, s.height - 50));

    this->_label = Label::createWithTTF("result", "fonts/arial.ttf", 12);
    addChild(this->_label, 0);

    label->setPosition(Vec2(s.width / 2, s.height - 50));
    this->_label->setPosition(Vec2(s.width / 2, s.height / 2));

    doTest();
}

void JSONDefaultTest::doTest()
{
    std::string strVal{this->_label->getString()};
    this->_label->setString(
        strVal.append("\n"
                      "********************** init value ***********************"));

    // set default value

    JSONDefault::getInstance()->setStringForKey("string", "value1");
    JSONDefault::getInstance()->setIntegerForKey("integer", 10);
    JSONDefault::getInstance()->setFloatForKey("float", 2.3f);
    JSONDefault::getInstance()->setDoubleForKey("double", 2.4);
    JSONDefault::getInstance()->setBoolForKey("bool", true);

    printValue();

    strVal = this->_label->getString();
    this->_label->setString(
        strVal.append("\n"
                      "********************** after change value ***********************"));

    // change the value

    JSONDefault::getInstance()->setStringForKey("string", "value2");
    JSONDefault::getInstance()->setIntegerForKey("integer", 11);
    JSONDefault::getInstance()->setFloatForKey("float", 2.5f);
    JSONDefault::getInstance()->setDoubleForKey("double", 2.6);
    JSONDefault::getInstance()->setBoolForKey("bool", false);

    JSONDefault::getInstance()->flush();

    // print value
    printValue();

    strVal = this->_label->getString();
    this->_label->setString(
        strVal.append("\n"
                      "********************** after delete value ***********************"));

    JSONDefault::getInstance()->deleteValueForKey("string");
    JSONDefault::getInstance()->deleteValueForKey("integer");
    JSONDefault::getInstance()->deleteValueForKey("float");
    JSONDefault::getInstance()->deleteValueForKey("double");
    JSONDefault::getInstance()->deleteValueForKey("bool");

    // print value
    printValue();
}

void JSONDefaultTest::printValue()
{
    char strTemp[256] = "";
    // print value
    std::string_view ret = JSONDefault::getInstance()->getStringForKey("string");
    sprintf(strTemp, "string is %s", ret.data());
    this->_label->setString((std::string)this->_label->getString() + "\n" + strTemp);

    double d = JSONDefault::getInstance()->getDoubleForKey("double");
    sprintf(strTemp, "double is %f", d);
    this->_label->setString((std::string)this->_label->getString() + "\n" + strTemp);

    int i = JSONDefault::getInstance()->getIntegerForKey("integer");
    sprintf(strTemp, "integer is %d", i);
    this->_label->setString((std::string)this->_label->getString() + "\n" + strTemp);

    float f = JSONDefault::getInstance()->getFloatForKey("float");
    sprintf(strTemp, "float is %f", f);
    this->_label->setString((std::string)this->_label->getString() + "\n" + strTemp);

    bool b = JSONDefault::getInstance()->getBoolForKey("bool");
    if (b)
    {
        sprintf(strTemp, "bool is true");
        this->_label->setString((std::string)this->_label->getString() + "\n" + strTemp);
    }
    else
    {
        sprintf(strTemp, "bool is false");
        this->_label->setString((std::string)this->_label->getString() + "\n" + strTemp);
    }
}

JSONDefaultTest::~JSONDefaultTest() {}
