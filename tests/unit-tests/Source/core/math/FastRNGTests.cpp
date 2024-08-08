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
#include <float.h>
#include "math/FastRNG.h"


TEST_SUITE("math/FastRNG") {
    TEST_CASE("next") {
        auto rng = ax::FastRNG();
        rng.seed(1);

        CHECK_EQ(1695105466, rng.next());
        CHECK_EQ(1423115009, rng.next());
        CHECK_EQ(634581793, rng.next());

        auto rng2 = ax::FastRNG();
        rng2.seed(2);

        CHECK_EQ(1086064458, rng2.next());
        CHECK_EQ(2256779072, rng2.next());
        CHECK_EQ(556893360, rng2.next());
    }

    TEST_CASE("nextInt")
    {
        auto rng = ax::FastRNG();
        rng.seed(12);

        CHECK_EQ(30, rng.nextInt<int8_t>(INT8_MIN, INT8_MAX));
        CHECK_EQ(124, rng.nextInt<uint8_t>(0, UINT8_MAX));

        CHECK_EQ(-14353, rng.nextInt<int16_t>(INT16_MIN, INT16_MAX));
        CHECK_EQ(61806, rng.nextInt<uint16_t>(0, UINT16_MAX));

        CHECK_EQ(-1775882703, rng.nextInt<int32_t>(INT32_MIN, INT32_MAX));
        CHECK_EQ(1623340863, rng.nextInt<uint32_t>(0, UINT32_MAX));
    }

    TEST_CASE("nextReal")
    {
        auto rng = ax::FastRNG();
        rng.seed(14);

        CHECK_EQ(doctest::Approx(0.927014), rng.nextReal<float>());
        CHECK_EQ(doctest::Approx(2.02437e+38), rng.nextReal<float>(0, FLT_MAX));
        CHECK_EQ(doctest::Approx(2.79091e+38), rng.nextReal<float>(FLT_MIN, FLT_MAX));

        CHECK_EQ(doctest::Approx(0.845991), rng.nextReal<double>());
        CHECK_EQ(doctest::Approx(1.50478e+308), rng.nextReal<double>(0, DBL_MAX));
        CHECK_EQ(doctest::Approx(9.94378e+307), rng.nextReal<double>(DBL_MIN, DBL_MAX));
    }

    TEST_CASE("range") {
        auto rng = ax::FastRNG();
        rng.seed(12345);

        CHECK_EQ(0, rng.range(0, 1));
        CHECK_EQ(-1, rng.range(-1, 0));
        CHECK_EQ(14, rng.range(10, 20));
        CHECK_EQ(19, rng.range(10, 20));
        CHECK_EQ(-18, rng.range(-20, -10));
        CHECK_EQ(-19, rng.range(-20, -10));

        CHECK_EQ(-704160663, rng.range(-INT_MAX, INT_MAX));
        CHECK_EQ(1299678946, rng.range(-INT_MAX, INT_MAX));
    }

    TEST_CASE("max")
    {
        auto rng = ax::FastRNG();
        rng.seed(1);

        CHECK_EQ(0, rng.max(1));
        CHECK_EQ(3, rng.max(10));
        CHECK_EQ(317290896, rng.max());
    }


    TEST_CASE("rangeu") {
        auto rng = ax::FastRNG();
        rng.seed(1);

        CHECK_EQ(0u, rng.rangeu(0u, 1u));
        CHECK_EQ(0u, rng.rangeu(0u, 1u));
        CHECK_EQ(11u, rng.rangeu(10u, 20u));

        CHECK_EQ(1068227752, rng.rangeu(0u, UINT_MAX));
    }

    TEST_CASE("maxu")
    {
        auto rng = ax::FastRNG();
        rng.seed(1);

        CHECK_EQ(0u, rng.maxu(1));
        CHECK_EQ(3u, rng.maxu(10));
        CHECK_EQ(634581792, rng.maxu());
    }

    TEST_CASE("rangef") {
        auto rng = ax::FastRNG();
        rng.seed(1);

        CHECK_EQ(doctest::Approx(-0.210655), rng.rangef(-1.0f, 1.0f));
        CHECK_EQ(doctest::Approx(-0.33731), rng.rangef(-1.0f, 1.0f));
        CHECK_EQ(doctest::Approx(11.4775), rng.rangef(10.0f, 20.0f));
        // These overflow
        // CHECK_EQ(doctest::Approx(1.0), rng.rangef(-FLT_MAX, FLT_MAX));
        // CHECK_EQ(doctest::Approx(1.0), rng.rangef(-FLT_MAX, FLT_MAX));
    }


    TEST_CASE("maxf") {
        auto rng = ax::FastRNG();
        rng.seed(1);

        CHECK_EQ(doctest::Approx(0.394672), rng.maxf(1.0f));
        CHECK_EQ(doctest::Approx(0.331345), rng.maxf(1.0f));
        CHECK_EQ(doctest::Approx(1.4775), rng.maxf(10.0f));
        CHECK_EQ(doctest::Approx(8.46337e+37), rng.maxf(FLT_MAX));
        CHECK_EQ(doctest::Approx(5.67875e+37), rng.maxf(FLT_MAX));
    }

    TEST_CASE("ranged")
    {
        auto rng = ax::FastRNG();
        rng.seed(1);

        CHECK_EQ(doctest::Approx(-0.210655), rng.ranged(-1.0, 1.0));
        CHECK_EQ(doctest::Approx(-0.7045), rng.ranged(-1.0, 1.0));
        CHECK_EQ(doctest::Approx(11.6688), rng.ranged(10.0, 20.0));
    }

    TEST_CASE("maxd")
    {
        auto rng = ax::FastRNG();
        rng.seed(1);

        CHECK_EQ(doctest::Approx(0.394672), rng.maxd(1.0f));
        CHECK_EQ(doctest::Approx(0.14775), rng.maxd(1.0f));
        CHECK_EQ(doctest::Approx(1.66884), rng.maxd(10.0f));
        CHECK_EQ(doctest::Approx(1.58118e+308), rng.maxd(DBL_MAX));
        CHECK_EQ(doctest::Approx(1.19643e+308), rng.maxd(DBL_MAX));
    }

    TEST_CASE("float01") {
        auto rng = ax::FastRNG();
        rng.seed(1);

        CHECK_EQ(doctest::Approx(0.394672), rng.float01());
        CHECK_EQ(doctest::Approx(0.331345), rng.float01());
        CHECK_EQ(doctest::Approx(0.14775), rng.float01());
        CHECK_EQ(doctest::Approx(0.248716), rng.float01());
    }

    TEST_CASE("double01")
    {
        auto rng = ax::FastRNG();
        rng.seed(1);

        CHECK_EQ(doctest::Approx(0.394672), rng.double01());
        CHECK_EQ(doctest::Approx(0.14775), rng.double01());
        CHECK_EQ(doctest::Approx(0.166884), rng.double01());
        CHECK_EQ(doctest::Approx(0.879563), rng.double01());
    }

    TEST_CASE("bool01") {
        auto rng = ax::FastRNG();
        rng.seed(1);

        auto t = 0;
        auto f = 0;
        for (auto i = 0; i < 100; i++) {
            if (rng.bool01()) {
                t++;
            } else {
                f++;
            }
        }

        CHECK_EQ(50, t);
        CHECK_EQ(50, f);
    }
}
