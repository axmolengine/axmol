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
#include "base/Config.h"
#include "base/Types.h"
#include "math/MathBase.h"
#include "TestUtils.h"

#define INCLUDE_SSE
#define USE_SSE

#if defined(AX_SSE_INTRINSICS) || defined(AX_NEON_INTRINSICS)
#    define SKIP_SIMD_TEST doctest::skip(false)
#else
#    define SKIP_SIMD_TEST doctest::skip(true)
#endif

USING_NS_AX;

namespace UnitTest
{

#ifdef AX_NEON_INTRINSICS
#    include "math/MathUtilNeon.inl"
#elif defined(AX_SSE_INTRINSICS)
#    include "math/MathUtilSSE.inl"
#endif

#include "math/MathUtil.inl"

}  // namespace UnitTest

static void __checkMathUtilResult(std::string_view description, const float* a1, const float* a2, int size)
{
    // Check whether the result of the optimized instruction is the same as which is implemented in C
    for (int i = 0; i < size; ++i)
    {
        bool r = fabs(a1[i] - a2[i]) < 0.00001f;  // FLT_EPSILON;
        CHECK_MESSAGE(r, description, " a1[", i, "]=", a1[i], " a2[", i, "]=", a2[i]);
    }
}

TEST_SUITE("math/MathUtil")
{
    using namespace UnitTest::ax;

    static void checkVerticesAreEqual(const V3F_C4B_T2F* v1, const V3F_C4B_T2F* v2, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            CHECK_EQ(v1[i].vertices, v2[i].vertices);
            CHECK_EQ(v1[i].colors, v2[i].colors);
            CHECK_EQ(v1[i].texCoords, v2[i].texCoords);
        }
    }

    TEST_CASE("transformVertices")
    {
        auto count = 5;
        std::vector<V3F_C4B_T2F> src(count);
        std::vector<V3F_C4B_T2F> expected(count);
        std::vector<V3F_C4B_T2F> dst(count);

        for (int i = 0; i < count; ++i)
        {
            src[i].vertices.set(float(i), float(i + 1), float(i + 2));
            src[i].colors.set(uint8_t(i + 3), uint8_t(i + 4), uint8_t(i + 5), uint8_t(i + 6));
            src[i].texCoords.set(float(i + 7), float(i + 8));

            expected[i]            = src[i];
            expected[i].vertices.x = src[i].vertices.y * 4;
            expected[i].vertices.y = src[i].vertices.x * -5;
            expected[i].vertices.z = src[i].vertices.z * 6;
        }

        Mat4 transform(0, 4, 0, 0, -5, 0, 0, 0, 0, 0, 6, 0, 1, 2, 3, 1);

        SUBCASE("MathUtilC")
        {
            MathUtilC::transformVertices(dst.data(), src.data(), count, transform);
            checkVerticesAreEqual(expected.data(), dst.data(), count);
        }

#ifdef AX_NEON_INTRINSICS
        SUBCASE("MathUtilNeon")
        {
            MathUtilNeon::transformVertices(dst.data(), src.data(), count, transform);
            checkVerticesAreEqual(expected.data(), dst.data(), count);
        }
#elif defined(AX_SSE_INTRINSICS)
        SUBCASE("MathUtilSSE")
        {
            MathUtilSSE::transformVertices(dst.data(), src.data(), count, transform);
            checkVerticesAreEqual(expected.data(), dst.data(), count);
        }
#endif
    }

    TEST_CASE("transformIndices")
    {
        auto count = 43;
        std::vector<uint16_t> src(count);
        std::vector<uint16_t> expected(count);

        for (int i = 0; i < count; ++i)
        {
            src[i]      = i;
            expected[i] = i + 5;
        }

        uint16_t offset = 5;

        SUBCASE("MathUtilC")
        {
            std::vector<uint16_t> dst(count);
            MathUtilC::transformIndices(dst.data(), src.data(), count, offset);
            for (int i = 0; i < count; ++i)
                CHECK_EQ(expected[i], dst[i]);
        }

#if defined(AX_NEON_INTRINSICS) && AX_64BITS
        SUBCASE("MathUtilNeon")
        {
            std::vector<uint16_t> dst(count);
            MathUtilNeon::transformIndices(dst.data(), src.data(), count, offset);
            for (int i = 0; i < count; ++i)
                CHECK_EQ(expected[i], dst[i]);
        }
#elif defined(AX_SSE_INTRINSICS)
        SUBCASE("MathUtilSSE")
        {
            std::vector<uint16_t> dst(count);
            MathUtilSSE::transformIndices(dst.data(), src.data(), count, offset);
            for (int i = 0; i < count; ++i)
                CHECK_EQ(expected[i], dst[i]);
        }
#endif
    }
}

TEST_SUITE("math/MathUtil" * SKIP_SIMD_TEST)
{
    TEST_CASE("old_tests")
    {
        // I know the next line looks ugly, but it's a way to test MathUtil. :)
        using namespace UnitTest::ax;

        const int MAT4_SIZE = 16;
        const int VEC4_SIZE = 4;

        const float inMat41[MAT4_SIZE] = {
            0.234023f, 2.472349f, 1.984244f, 2.23348f, 0.634124f, 0.234975f, 6.384572f, 0.82368f,
            0.738028f, 1.845237f, 1.934721f, 1.62343f, 0.339023f, 3.472452f, 1.324714f, 4.23852f,
        };

        const float inMat42[MAT4_SIZE] = {
            1.640232f, 4.472349f, 0.983244f, 1.23343f, 2.834124f, 8.234975f, 0.082572f, 3.82464f,
            3.238028f, 2.845237f, 0.331721f, 4.62544f, 4.539023f, 9.472452f, 3.520714f, 2.23252f,
        };

        const float scalar = 1.323298f;
        const float x      = 0.432234f;
        const float y      = 1.333229f;
        const float z      = 2.535292f;
        const float w      = 4.632234f;

        const float inVec4[VEC4_SIZE]  = {2.323478f, 0.238482f, 4.223783f, 7.238238f};
        const float inVec42[VEC4_SIZE] = {0.322374f, 8.258883f, 3.293683f, 2.838337f};

        float outMat4Opt[MAT4_SIZE] = {0};
        float outMat4C[MAT4_SIZE]   = {0};
        float outVec4Opt[VEC4_SIZE] = {0};
        float outVec4C[VEC4_SIZE]   = {0};

        // inline static void addMatrix(const float* m, float scalar, float* dst);
        MathUtilC::addMatrix(inMat41, scalar, outMat4C);

#ifdef AX_NEON_INTRINSICS
        MathUtilNeon::addMatrix(reinterpret_cast<const _xm128_t*>(inMat41), scalar,
                                reinterpret_cast<_xm128_t*>(outMat4Opt));
#endif

#ifdef AX_SSE_INTRINSICS
        MathUtilSSE::addMatrix(reinterpret_cast<const _xm128_t*>(inMat41), scalar,
                               reinterpret_cast<_xm128_t*>(outMat4Opt));
#endif

        __checkMathUtilResult("inline static void addMatrix(const float* m, float scalar, float* dst);", outMat4C,
                              outMat4Opt, MAT4_SIZE);
        // Clean
        memset(outMat4C, 0, sizeof(outMat4C));
        memset(outMat4Opt, 0, sizeof(outMat4Opt));

        // inline static void addMatrix(const float* m1, const float* m2, float* dst);
        MathUtilC::addMatrix(inMat41, inMat42, outMat4C);

#ifdef AX_NEON_INTRINSICS
        MathUtilNeon::addMatrix(reinterpret_cast<const _xm128_t*>(inMat41), reinterpret_cast<const _xm128_t*>(inMat42),
                                reinterpret_cast<_xm128_t*>(outMat4Opt));
#elif defined(AX_SSE_INTRINSICS)
        MathUtilSSE::addMatrix(reinterpret_cast<const _xm128_t*>(inMat41), reinterpret_cast<const _xm128_t*>(inMat42),
                               reinterpret_cast<_xm128_t*>(outMat4Opt));
#endif

        __checkMathUtilResult("inline static void addMatrix(const float* m1, const float* m2, float* dst);", outMat4C,
                              outMat4Opt, MAT4_SIZE);
        // Clean
        memset(outMat4C, 0, sizeof(outMat4C));
        memset(outMat4Opt, 0, sizeof(outMat4Opt));

        // inline static void subtractMatrix(const float* m1, const float* m2, float* dst);
        MathUtilC::subtractMatrix(inMat41, inMat42, outMat4C);

#ifdef AX_NEON_INTRINSICS
        MathUtilNeon::subtractMatrix(reinterpret_cast<const _xm128_t*>(inMat41),
                                     reinterpret_cast<const _xm128_t*>(inMat42),
                                     reinterpret_cast<_xm128_t*>(outMat4Opt));
#elif defined(AX_SSE_INTRINSICS)
        MathUtilSSE::subtractMatrix(reinterpret_cast<const _xm128_t*>(inMat41),
                                    reinterpret_cast<const _xm128_t*>(inMat42),
                                    reinterpret_cast<_xm128_t*>(outMat4Opt));
#endif

        __checkMathUtilResult("inline static void subtractMatrix(const float* m1, const float* m2, float* dst);",
                              outMat4C, outMat4Opt, MAT4_SIZE);
        // Clean
        memset(outMat4C, 0, sizeof(outMat4C));
        memset(outMat4Opt, 0, sizeof(outMat4Opt));

        // inline static void multiplyMatrix(const float* m, float scalar, float* dst);
        MathUtilC::multiplyMatrix(inMat41, scalar, outMat4C);

#ifdef AX_NEON_INTRINSICS
        MathUtilNeon::multiplyMatrix(reinterpret_cast<const _xm128_t*>(inMat41), scalar,
                                     reinterpret_cast<_xm128_t*>(outMat4Opt));
#elif defined(AX_SSE_INTRINSICS)
        MathUtilSSE::multiplyMatrix(reinterpret_cast<const _xm128_t*>(inMat41), scalar,
                                    reinterpret_cast<_xm128_t*>(outMat4Opt));
#endif

        __checkMathUtilResult("inline static void multiplyMatrix(const float* m, float scalar, float* dst);", outMat4C,
                              outMat4Opt, MAT4_SIZE);
        // Clean
        memset(outMat4C, 0, sizeof(outMat4C));
        memset(outMat4Opt, 0, sizeof(outMat4Opt));

        // inline static void multiplyMatrix(const float* m1, const float* m2, float* dst);
        MathUtilC::multiplyMatrix(inMat41, inMat42, outMat4C);

#ifdef AX_NEON_INTRINSICS
        MathUtilNeon::multiplyMatrix(reinterpret_cast<const _xm128_t*>(inMat41),
                                     reinterpret_cast<const _xm128_t*>(inMat42),
                                     reinterpret_cast<_xm128_t*>(outMat4Opt));
#elif defined(AX_SSE_INTRINSICS)
        MathUtilSSE::multiplyMatrix(reinterpret_cast<const _xm128_t*>(inMat41),
                                    reinterpret_cast<const _xm128_t*>(inMat42),
                                    reinterpret_cast<_xm128_t*>(outMat4Opt));
#endif

        __checkMathUtilResult("inline static void multiplyMatrix(const float* m1, const float* m2, float* dst);",
                              outMat4C, outMat4Opt, MAT4_SIZE);
        // Clean
        memset(outMat4C, 0, sizeof(outMat4C));
        memset(outMat4Opt, 0, sizeof(outMat4Opt));

        // inline static void negateMatrix(const float* m, float* dst);
        MathUtilC::negateMatrix(inMat41, outMat4C);

#ifdef AX_NEON_INTRINSICS
        MathUtilNeon::negateMatrix(reinterpret_cast<const _xm128_t*>(inMat41), reinterpret_cast<_xm128_t*>(outMat4Opt));
#elif defined(AX_SSE_INTRINSICS)
        MathUtilSSE::negateMatrix(reinterpret_cast<const _xm128_t*>(inMat41), reinterpret_cast<_xm128_t*>(outMat4Opt));
#endif

        __checkMathUtilResult("inline static void negateMatrix(const float* m, float* dst);", outMat4C, outMat4Opt,
                              MAT4_SIZE);
        // Clean
        memset(outMat4C, 0, sizeof(outMat4C));
        memset(outMat4Opt, 0, sizeof(outMat4Opt));

        // inline static void transposeMatrix(const float* m, float* dst);
        MathUtilC::transposeMatrix(inMat41, outMat4C);

#ifdef AX_NEON_INTRINSICS
        MathUtilNeon::transposeMatrix(reinterpret_cast<const _xm128_t*>(inMat41),
                                      reinterpret_cast<_xm128_t*>(outMat4Opt));
#elif defined(AX_SSE_INTRINSICS)
        MathUtilSSE::transposeMatrix(reinterpret_cast<const _xm128_t*>(inMat41),
                                     reinterpret_cast<_xm128_t*>(outMat4Opt));
#endif

        __checkMathUtilResult("inline static void transposeMatrix(const float* m, float* dst);", outMat4C, outMat4Opt,
                              MAT4_SIZE);
        // Clean
        memset(outMat4C, 0, sizeof(outMat4C));
        memset(outMat4Opt, 0, sizeof(outMat4Opt));

        // inline static void transformVec4(const float* m, float x, float y, float z, float w, float* dst);
        MathUtilC::transformVec4(inMat41, x, y, z, w, outVec4C);

#ifdef AX_NEON_INTRINSICS
        MathUtilNeon::transformVec4(reinterpret_cast<const _xm128_t*>(inMat41), x, y, z, w, outVec4Opt);
#elif defined(AX_SSE_INTRINSICS)
        // FIXME:
        MathUtilSSE::transformVec4(reinterpret_cast<const _xm128_t*>(inMat41), x, y, z, w, outVec4Opt);
#endif

        __checkMathUtilResult(
            "inline static void transformVec4(const float* m, float x, float y, float z, float w, float* dst);",
            outVec4C, outVec4Opt, VEC4_SIZE);
        // Clean
        memset(outVec4C, 0, sizeof(outVec4C));
        memset(outVec4Opt, 0, sizeof(outVec4Opt));

        // inline static void transformVec4(const float* m, const float* v, float* dst);
        MathUtilC::transformVec4(inMat41, inVec4, outVec4C);

#ifdef AX_NEON_INTRINSICS
        MathUtilNeon::transformVec4(reinterpret_cast<const _xm128_t*>(inMat41), reinterpret_cast<const float*>(inVec4),
                                    reinterpret_cast<float*>(outVec4Opt));
#elif defined(AX_SSE_INTRINSICS)
        MathUtilSSE::transformVec4(reinterpret_cast<const _xm128_t*>(inMat41), reinterpret_cast<const float*>(inVec4), reinterpret_cast<float*>(outVec4Opt));
#endif

        __checkMathUtilResult("inline static void transformVec4(const float* m, const float* v, float* dst);", outVec4C,
                              outVec4Opt, VEC4_SIZE);
        // Clean
        memset(outVec4C, 0, sizeof(outVec4C));
        memset(outVec4Opt, 0, sizeof(outVec4Opt));

        // inline static void crossVec3(const float* v1, const float* v2, float* dst);
        MathUtilC::crossVec3(inVec4, inVec42, outVec4C);

#ifdef AX_NEON_INTRINSICS
        MathUtilNeon::crossVec3(inVec4, inVec42, outVec4Opt);
#elif defined(AX_SSE_INTRINSICS)
        MathUtilSSE::crossVec3(inVec4, inVec42, outVec4Opt);
#endif

        __checkMathUtilResult("inline static void crossVec3(const float* v1, const float* v2, float* dst);", outVec4C,
                              outVec4Opt, VEC4_SIZE);
        // Clean
        memset(outVec4C, 0, sizeof(outVec4C));
        memset(outVec4Opt, 0, sizeof(outVec4Opt));
    }
}
