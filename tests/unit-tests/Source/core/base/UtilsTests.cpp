/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmolengine.github.io/

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
#include "base/Utils.h"

USING_NS_AX;


TEST_SUITE("base/Utils") {
    TEST_CASE("parseIntegerList") {
        using ax::utils::parseIntegerList;

        std::vector<int> res1{};
        CHECK_EQ(res1, parseIntegerList(""));

        std::vector<int> res2{1};
        CHECK_EQ(res2, parseIntegerList("1"));

        std::vector<int> res3{1, 2};
        CHECK_EQ(res3, parseIntegerList("1 2"));

        std::vector<int> res4{2, 4, 3, 1, 4, 2, 0, 4, 1, 0, 4, 5};
        CHECK_EQ(res4, parseIntegerList("2 4 3 1 4 2 0 4 1 0 4 5"));

        std::vector<int> res5{73, 48, 57, 117, 27, 117, 29, 77, 14, 62, 26, 7, 55, 2};
        CHECK_EQ(res5, parseIntegerList("73 48 57 117 27 117 29 77 14 62 26 7 55 2"));
    }
}
