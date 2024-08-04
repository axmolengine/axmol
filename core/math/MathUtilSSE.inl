/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2017 Chukong Technologies
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

NS_AX_MATH_BEGIN

#ifdef AX_SSE_INTRINSICS

struct MathUtilSSE
{

    static void addMatrix(const __m128 m[4], float scalar, __m128 dst[4])
    {
        __m128 s = _mm_set1_ps(scalar);
        dst[0]   = _mm_add_ps(m[0], s);
        dst[1]   = _mm_add_ps(m[1], s);
        dst[2]   = _mm_add_ps(m[2], s);
        dst[3]   = _mm_add_ps(m[3], s);
    }

    static void addMatrix(const __m128 m1[4], const __m128 m2[4], __m128 dst[4])
    {
        dst[0] = _mm_add_ps(m1[0], m2[0]);
        dst[1] = _mm_add_ps(m1[1], m2[1]);
        dst[2] = _mm_add_ps(m1[2], m2[2]);
        dst[3] = _mm_add_ps(m1[3], m2[3]);
    }

    static void subtractMatrix(const __m128 m1[4], const __m128 m2[4], __m128 dst[4])
    {
        dst[0] = _mm_sub_ps(m1[0], m2[0]);
        dst[1] = _mm_sub_ps(m1[1], m2[1]);
        dst[2] = _mm_sub_ps(m1[2], m2[2]);
        dst[3] = _mm_sub_ps(m1[3], m2[3]);
    }

    static void multiplyMatrix(const __m128 m[4], float scalar, __m128 dst[4])
    {
        __m128 s = _mm_set1_ps(scalar);
        dst[0]   = _mm_mul_ps(m[0], s);
        dst[1]   = _mm_mul_ps(m[1], s);
        dst[2]   = _mm_mul_ps(m[2], s);
        dst[3]   = _mm_mul_ps(m[3], s);
    }

    static void multiplyMatrix(const __m128 m1[4], const __m128 m2[4], __m128 dst[4])
    {
        __m128 dst0, dst1, dst2, dst3;
        {
            __m128 e0 = _mm_shuffle_ps(m2[0], m2[0], _MM_SHUFFLE(0, 0, 0, 0));
            __m128 e1 = _mm_shuffle_ps(m2[0], m2[0], _MM_SHUFFLE(1, 1, 1, 1));
            __m128 e2 = _mm_shuffle_ps(m2[0], m2[0], _MM_SHUFFLE(2, 2, 2, 2));
            __m128 e3 = _mm_shuffle_ps(m2[0], m2[0], _MM_SHUFFLE(3, 3, 3, 3));

            __m128 v0 = _mm_mul_ps(m1[0], e0);
            __m128 v1 = _mm_mul_ps(m1[1], e1);
            __m128 v2 = _mm_mul_ps(m1[2], e2);
            __m128 v3 = _mm_mul_ps(m1[3], e3);

            __m128 a0 = _mm_add_ps(v0, v1);
            __m128 a1 = _mm_add_ps(v2, v3);
            __m128 a2 = _mm_add_ps(a0, a1);

            dst0 = a2;
        }

        {
            __m128 e0 = _mm_shuffle_ps(m2[1], m2[1], _MM_SHUFFLE(0, 0, 0, 0));
            __m128 e1 = _mm_shuffle_ps(m2[1], m2[1], _MM_SHUFFLE(1, 1, 1, 1));
            __m128 e2 = _mm_shuffle_ps(m2[1], m2[1], _MM_SHUFFLE(2, 2, 2, 2));
            __m128 e3 = _mm_shuffle_ps(m2[1], m2[1], _MM_SHUFFLE(3, 3, 3, 3));

            __m128 v0 = _mm_mul_ps(m1[0], e0);
            __m128 v1 = _mm_mul_ps(m1[1], e1);
            __m128 v2 = _mm_mul_ps(m1[2], e2);
            __m128 v3 = _mm_mul_ps(m1[3], e3);

            __m128 a0 = _mm_add_ps(v0, v1);
            __m128 a1 = _mm_add_ps(v2, v3);
            __m128 a2 = _mm_add_ps(a0, a1);

            dst1 = a2;
        }

        {
            __m128 e0 = _mm_shuffle_ps(m2[2], m2[2], _MM_SHUFFLE(0, 0, 0, 0));
            __m128 e1 = _mm_shuffle_ps(m2[2], m2[2], _MM_SHUFFLE(1, 1, 1, 1));
            __m128 e2 = _mm_shuffle_ps(m2[2], m2[2], _MM_SHUFFLE(2, 2, 2, 2));
            __m128 e3 = _mm_shuffle_ps(m2[2], m2[2], _MM_SHUFFLE(3, 3, 3, 3));

            __m128 v0 = _mm_mul_ps(m1[0], e0);
            __m128 v1 = _mm_mul_ps(m1[1], e1);
            __m128 v2 = _mm_mul_ps(m1[2], e2);
            __m128 v3 = _mm_mul_ps(m1[3], e3);

            __m128 a0 = _mm_add_ps(v0, v1);
            __m128 a1 = _mm_add_ps(v2, v3);
            __m128 a2 = _mm_add_ps(a0, a1);

            dst2 = a2;
        }

        {
            __m128 e0 = _mm_shuffle_ps(m2[3], m2[3], _MM_SHUFFLE(0, 0, 0, 0));
            __m128 e1 = _mm_shuffle_ps(m2[3], m2[3], _MM_SHUFFLE(1, 1, 1, 1));
            __m128 e2 = _mm_shuffle_ps(m2[3], m2[3], _MM_SHUFFLE(2, 2, 2, 2));
            __m128 e3 = _mm_shuffle_ps(m2[3], m2[3], _MM_SHUFFLE(3, 3, 3, 3));

            __m128 v0 = _mm_mul_ps(m1[0], e0);
            __m128 v1 = _mm_mul_ps(m1[1], e1);
            __m128 v2 = _mm_mul_ps(m1[2], e2);
            __m128 v3 = _mm_mul_ps(m1[3], e3);

            __m128 a0 = _mm_add_ps(v0, v1);
            __m128 a1 = _mm_add_ps(v2, v3);
            __m128 a2 = _mm_add_ps(a0, a1);

            dst3 = a2;
        }
        dst[0] = dst0;
        dst[1] = dst1;
        dst[2] = dst2;
        dst[3] = dst3;
    }

    static void negateMatrix(const __m128 m[4], __m128 dst[4])
    {
        __m128 z = _mm_setzero_ps();
        dst[0]   = _mm_sub_ps(z, m[0]);
        dst[1]   = _mm_sub_ps(z, m[1]);
        dst[2]   = _mm_sub_ps(z, m[2]);
        dst[3]   = _mm_sub_ps(z, m[3]);
    }

    static void transposeMatrix(const __m128 m[4], __m128 dst[4])
    {
        __m128 tmp0 = _mm_shuffle_ps(m[0], m[1], 0x44);
        __m128 tmp2 = _mm_shuffle_ps(m[0], m[1], 0xEE);
        __m128 tmp1 = _mm_shuffle_ps(m[2], m[3], 0x44);
        __m128 tmp3 = _mm_shuffle_ps(m[2], m[3], 0xEE);

        dst[0] = _mm_shuffle_ps(tmp0, tmp1, 0x88);
        dst[1] = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
        dst[2] = _mm_shuffle_ps(tmp2, tmp3, 0x88);
        dst[3] = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
    }

    static void transformVec4(const __m128 m[4], float x, float y, float z, float w, float* dst /*vec3*/)
    {
        //__m128 res = _mm_set_ps(w, z, y, x);
        //__m128 xx = _mm_shuffle_ps(res, res, _MM_SHUFFLE(0, 0, 0, 0));
        //__m128 yy = _mm_shuffle_ps(res, res, _MM_SHUFFLE(1, 1, 1, 1));
        //__m128 zz = _mm_shuffle_ps(res, res, _MM_SHUFFLE(2, 2, 2, 2));
        //__m128 ww = _mm_shuffle_ps(res, res, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 xx = _mm_set1_ps(x);
        __m128 yy = _mm_set1_ps(y);
        __m128 zz = _mm_set1_ps(z);
        __m128 ww = _mm_set1_ps(w);

        auto res = _mm_add_ps(_mm_add_ps(_mm_mul_ps(m[0], xx), _mm_mul_ps(m[1], yy)),
                              _mm_add_ps(_mm_mul_ps(m[2], zz), _mm_mul_ps(m[3], ww)));

        _mm_storel_pi((__m64*)dst, res);

#    if defined(__SSE4_1__)
        *reinterpret_cast<int*>(dst + 2) = _mm_extract_ps(res, 2);
#    else
        dst[2] = _mm_cvtss_f32(_mm_movehl_ps(res, res));
#    endif
    }

    static void transformVec4(const __m128 m[4], const float* v /*vec4*/, float* dst /*vec4*/)
    {
        //__m128 res = _mm_loadu_ps(v);
        //__m128 xx = _mm_shuffle_ps(res, res, _MM_SHUFFLE(0, 0, 0, 0));
        //__m128 yy = _mm_shuffle_ps(res, res, _MM_SHUFFLE(1, 1, 1, 1));
        //__m128 zz = _mm_shuffle_ps(res, res, _MM_SHUFFLE(2, 2, 2, 2));
        //__m128 ww = _mm_shuffle_ps(res, res, _MM_SHUFFLE(3, 3, 3, 3));

        __m128 xx = _mm_set1_ps(v[0]);
        __m128 yy = _mm_set1_ps(v[1]);
        __m128 zz = _mm_set1_ps(v[2]);
        __m128 ww = _mm_set1_ps(v[3]);

        auto res = _mm_add_ps(_mm_add_ps(_mm_mul_ps(m[0], xx), _mm_mul_ps(m[1], yy)),
                              _mm_add_ps(_mm_mul_ps(m[2], zz), _mm_mul_ps(m[3], ww)));
        _mm_storeu_ps(dst, res);
    }

    static void crossVec3(const float* v1, const float* v2, float* dst)
    {
        __m128 a = _mm_set_ps(0.0f, v1[2], v1[1], v1[0]);
        __m128 b = _mm_set_ps(0.0f, v2[2], v2[1], v2[0]);

        __m128 a_yzx = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 b_yzx = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 res   = _mm_sub_ps(_mm_mul_ps(a, b_yzx), _mm_mul_ps(a_yzx, b));

        res = _mm_shuffle_ps(res, res, _MM_SHUFFLE(3, 0, 2, 1));

        _mm_storel_pi((__m64*)dst, res);
#    if defined(__SSE4_1__)
        *reinterpret_cast<int*>(dst + 2) = _mm_extract_ps(res, 2);
#    else
        dst[2] = _mm_cvtss_f32(_mm_movehl_ps(res, res));
#    endif
    }

    static void transformVertices(V3F_C4B_T2F* dst, const V3F_C4B_T2F* src, size_t count, const Mat4& transform)
    {
        auto& m = transform.col;

        for (size_t i = 0; i < count; ++i)
        {
            auto& vert = src[i].vertices;
            __m128 v   = _mm_set_ps(1.0f, vert.z, vert.y, vert.x);
            v          = _mm_add_ps(
                _mm_add_ps(_mm_mul_ps(m[0], _mm_shuffle_ps(v, v, 0)), _mm_mul_ps(m[1], _mm_shuffle_ps(v, v, 0x55))),
                _mm_add_ps(_mm_mul_ps(m[2], _mm_shuffle_ps(v, v, 0xaa)), _mm_mul_ps(m[3], _mm_shuffle_ps(v, v, 0xff))));
            _mm_storeu_ps((float*)&dst[i].vertices, v);

            // Copy tex coords and colors
            // dst[i].texCoords = src[i].texCoords;
            // dst[i].colors    = src[i].colors;
            memcpy(&dst[i].colors, &src[i].colors, sizeof(V3F_C4B_T2F::colors) + sizeof(V3F_C4B_T2F::texCoords));
        }
    }

    static void transformIndices(uint16_t* dst, const uint16_t* src, size_t count, uint16_t offset)
    {
        __m128i offset_vector = _mm_set1_epi16(offset);
        size_t remainder      = count % 8;
        size_t rounded_count  = count - remainder;

        for (size_t i = 0; i < rounded_count; i += 8)
        {
            __m128i current_values = _mm_loadu_si128((__m128i*)(src + i));          // Load 8 values.
            current_values         = _mm_add_epi16(current_values, offset_vector);  // Add offset to them.
            _mm_storeu_si128((__m128i*)(dst + i), current_values);                  // Store the result.
        }

        // If count is not divisible by 8, add offset for the remainder elements one by one.
        for (size_t i = 0; i < remainder; ++i)
        {
            dst[rounded_count + i] = src[rounded_count + i] + offset;
        }
    }
};

#endif

NS_AX_MATH_END
