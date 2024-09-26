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

#include <doctest.h>
#include "base/Value.h"

using namespace ax;


TEST_SUITE("base/Value") {
    TEST_CASE("old_tests") {
        Value v1;
        CHECK(v1.getType() == Value::Type::NONE);
        CHECK(v1.isNull());

        Value v2(100);
        CHECK(v2.getType() == Value::Type::INTEGER);
        CHECK(!v2.isNull());

        Value v3(101.4f);
        CHECK(v3.getType() == Value::Type::FLOAT);
        CHECK(!v3.isNull());

        Value v4(106.1);
        CHECK(v4.getType() == Value::Type::DOUBLE);
        CHECK(!v4.isNull());

        unsigned char byte = 50;
        Value v5(byte);
        CHECK(v5.getType() == Value::Type::INT_UI32);
        CHECK(!v5.isNull());

        Value v6(true);
        CHECK(v6.getType() == Value::Type::BOOLEAN);
        CHECK(!v6.isNull());

        Value v7("string");
        CHECK(v7.getType() == Value::Type::STRING);
        CHECK(!v7.isNull());

        Value v8(std::string("string2"));
        CHECK(v8.getType() == Value::Type::STRING);
        CHECK(!v8.isNull());

        auto createValueVector = [&]() {
            ValueVector ret;
            ret.emplace_back(v1);
            ret.emplace_back(v2);
            ret.emplace_back(v3);
            return ret;
        };

        Value v9(createValueVector());
        CHECK(v9.getType() == Value::Type::VECTOR);
        CHECK(!v9.isNull());

        auto createValueMap = [&]() {
            ValueMap ret;
            ret["aaa"] = v1;
            ret["bbb"] = v2;
            ret["ccc"] = v3;
            return ret;
        };

        Value v10(createValueMap());
        CHECK(v10.getType() == Value::Type::MAP);
        CHECK(!v10.isNull());

        auto createValueMapIntKey = [&]() {
            ValueMapIntKey ret;
            ret[111] = v1;
            ret[222] = v2;
            ret[333] = v3;
            return ret;
        };

        Value v11(createValueMapIntKey());
        CHECK(v11.getType() == Value::Type::INT_KEY_MAP);
        CHECK(!v11.isNull());
    }
}
