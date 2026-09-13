/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include <doctest.h>
#include "axmol/base/Utils.h"

using namespace ax;

TEST_SUITE("base/Utils")
{
    TEST_CASE("parseIntegerList")
    {
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
