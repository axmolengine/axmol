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
    TEST_CASE("rng") {
        auto rng = FastRNG(1);

        CHECK_EQ(622173, rng.rng());
        CHECK_EQ(1462482235, rng.rng());
        CHECK_EQ(3913629391, rng.rng());

        auto rng2 = FastRNG(2);

        CHECK_EQ(760312, rng2.rng());
        CHECK_EQ(2413601167, rng2.rng());
        CHECK_EQ(927245950, rng2.rng());
    }


    TEST_CASE("range") {
        auto rng = FastRNG(12345);

        CHECK_EQ(0, rng.range(0, 1));
        CHECK_EQ(-1, rng.range(-1, 0));
        CHECK_EQ(13, rng.range(10, 20));
        CHECK_EQ(12, rng.range(10, 20));
        CHECK_EQ(-11, rng.range(-20, -10));
        CHECK_EQ(-18, rng.range(-20, -10));

        CHECK_EQ(260724215, rng.range(-INT_MAX, INT_MAX));
        CHECK_EQ(1458015400, rng.range(-INT_MAX, INT_MAX));
    }


    TEST_CASE("rangeu") {
        auto rng = FastRNG(1);

        CHECK_EQ(0u, rng.rangeu(0u, 1u));
        CHECK_EQ(0u, rng.rangeu(0u, 1u));
        CHECK_EQ(19u, rng.rangeu(10u, 20u));

        CHECK_EQ(128334119, rng.rangeu(0u, UINT_MAX));
    }


    TEST_CASE("max") {
        auto rng = FastRNG(1);

        CHECK_EQ(0, rng.max(1));
        CHECK_EQ(3, rng.max(10));
        CHECK_EQ(1956814720, rng.max());
    }


    TEST_CASE("maxu") {
        auto rng = FastRNG(1);

        CHECK_EQ(0u, rng.maxu(1));
        CHECK_EQ(3u, rng.maxu(10));
        CHECK_EQ(3913629391, rng.maxu());
    }


    TEST_CASE("rangef") {
        auto rng = FastRNG(1);

        CHECK_EQ(doctest::Approx(-0.99971), rng.rangef(-1.0f, 1.0f));
        CHECK_EQ(doctest::Approx(-0.318979), rng.rangef(-1.0f, 1.0f));
        CHECK_EQ(doctest::Approx(19.1121), rng.rangef(10.0f, 20.0f));
        // These overflow
        //CHECK_EQ(doctest::Approx(1.0), rng.rangef(-FLT_MAX, FLT_MAX));
        //CHECK_EQ(doctest::Approx(1.0), rng.rangef(-FLT_MAX, FLT_MAX));
    }


    TEST_CASE("maxf") {
        auto rng = FastRNG(1);

        CHECK_EQ(doctest::Approx(0.000144861), rng.maxf(1.0f));
        CHECK_EQ(doctest::Approx(0.340511), rng.maxf(1.0f));
        CHECK_EQ(doctest::Approx(9.11213), rng.maxf(10.0f));
        CHECK_EQ(doctest::Approx(1.01677e+37), rng.maxf(FLT_MAX));
        CHECK_EQ(doctest::Approx(1.50114e+38), rng.maxf(FLT_MAX));
    }


    TEST_CASE("float01") {
        auto rng = FastRNG(1);

        CHECK_EQ(doctest::Approx(0.000144861), rng.float01());
        CHECK_EQ(doctest::Approx(0.340511), rng.float01());
        CHECK_EQ(doctest::Approx(0.911213), rng.float01());
        CHECK_EQ(doctest::Approx(0.0298801), rng.float01());
    }


    TEST_CASE("bool01") {
        auto rng = FastRNG(1);

        auto t = 0;
        auto f = 0;
        for (auto i = 0; i < 100; i++) {
            if (rng.bool01()) {
                t++;
            } else {
                f++;
            }
        }

        CHECK_EQ(45, t);
        CHECK_EQ(55, f);
    }
}
