/**
 Copyright 2013 BlackBerry Inc.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 Original file from GamePlay3D: http://gameplay3d.org

 This file was modified to fit the axmol project
 */

#include <arm_neon.h>

NS_AX_MATH_BEGIN

struct MathUtilNeon
{
#if defined(__EMSCRIPTEN__)
#    define vmlaq_lane_f32(a, b, c, lane) vaddq_f32(a, vmulq_lane_f32(b, c, lane))
#endif

    inline static void addMatrix(const _xm128_t* m, float scalar, _xm128_t* dst)
    {
        float32x4_t s = vdupq_n_f32(scalar);
        dst[0]        = vaddq_f32(m[0], s);
        dst[1]        = vaddq_f32(m[1], s);
        dst[2]        = vaddq_f32(m[2], s);
        dst[3]        = vaddq_f32(m[3], s);
    }

    inline static void addMatrix(const _xm128_t* m1, const _xm128_t* m2, _xm128_t* dst)
    {
        dst[0] = vaddq_f32(m1[0], m2[0]);
        dst[1] = vaddq_f32(m1[1], m2[1]);
        dst[2] = vaddq_f32(m1[2], m2[2]);
        dst[3] = vaddq_f32(m1[3], m2[3]);
    }

    inline static void subtractMatrix(const _xm128_t* m1, const _xm128_t* m2, _xm128_t* dst)
    {
        dst[0] = vsubq_f32(m1[0], m2[0]);
        dst[1] = vsubq_f32(m1[1], m2[1]);
        dst[2] = vsubq_f32(m1[2], m2[2]);
        dst[3] = vsubq_f32(m1[3], m2[3]);
    }

    inline static void multiplyMatrix(const _xm128_t* m, float scalar, _xm128_t* dst)
    {
        _xm128_t s = vdupq_n_f32(scalar);
        AX_UNROLL
        for (int i = 0; i < 4; ++i)
        {
            dst[i] = vmulq_f32(m[i], s);
        }
    }

    inline static void multiplyMatrix(const _xm128_t* m1, const _xm128_t* m2, _xm128_t* dst)
    {
        float32x4_t product[4];
        float32x4_t val;
        AX_UNROLL
        for (int i = 0; i < 4; ++i)
        {
            val        = vmulq_n_f32(m1[0], vgetq_lane_f32(m2[i], 0));
            val        = vmlaq_n_f32(val, m1[1], vgetq_lane_f32(m2[i], 1));
            val        = vmlaq_n_f32(val, m1[2], vgetq_lane_f32(m2[i], 2));
            val        = vmlaq_n_f32(val, m1[3], vgetq_lane_f32(m2[i], 3));
            product[i] = val;
        }
        memcpy(dst, product, sizeof(product));
    }

    inline static void negateMatrix(const _xm128_t* m, _xm128_t* dst)
    {
        AX_UNROLL
        for (int i = 0; i < 4; ++i)
        {
            dst[i] = vnegq_f32(m[i]);
        }
    }

    inline static void transposeMatrix(const _xm128_t* m, _xm128_t* dst)
    {
        auto tmp0 = vzipq_f32(m[0], m[2]);
        auto tmp1 = vzipq_f32(m[1], m[3]);
        auto tmp2 = vzipq_f32(tmp0.val[0], tmp1.val[0]);
        auto tmp3 = vzipq_f32(tmp0.val[1], tmp1.val[1]);

        dst[0] = tmp2.val[0];
        dst[1] = tmp2.val[1];
        dst[2] = tmp3.val[0];
        dst[3] = tmp3.val[1];
    }

    inline static void transformVec4(const _xm128_t* m, float x, float y, float z, float w, float* dst/*vec3*/)
    {
        auto v0 = vmulq_n_f32(m[0], x);
        auto v1 = vmulq_n_f32(m[1], y);
        auto v2 = vmulq_n_f32(m[2], z);
        auto v3 = vmulq_n_f32(m[3], w);
        auto prod = vaddq_f32(v0, vaddq_f32(v1, vaddq_f32(v2, v3)));
        vst1_f32(dst, vget_low_f32(prod));
        vst1_lane_f32(dst + 2, vget_high_f32(prod), 0);
    }

    inline static void transformVec4(const _xm128_t* m, const float* v /*vec4*/, float* dst /*vec4*/)
    {
        auto v0 = vmulq_n_f32(m[0], v[0]);
        auto v1 = vmulq_n_f32(m[1], v[1]);
        auto v2 = vmulq_n_f32(m[2], v[2]);
        auto v3 = vmulq_n_f32(m[3], v[3]);
        auto prod = vaddq_f32(v0, vaddq_f32(v1, vaddq_f32(v2, v3)));
        vst1q_f32(dst, prod);
    }

    inline static void crossVec3(const float* v1, const float* v2, float* dst)
    {
        // refer to:
        // https://developer.arm.com/documentation/den0018/a/NEON-Code-Examples-with-Mixed-Operations/Cross-product/Single-cross-product
        // Vector a is stored in memory such that ai is at the lower address and
        // ak is at the higher address. Vector b is also stored in the same way.

        float32x4_t vec_a     = vcombine_f32(vld1_f32(v1 + 1), vld1_f32(v1));  // Q register = [aj, ai, ak, aj]
        float32x4_t vec_b     = vcombine_f32(vld1_f32(v2 + 1), vld1_f32(v2));  // Q register = [bj, bi, bk, bj]
        float32x4_t vec_a_rot = vextq_f32(vec_a, vec_a, 1);
        float32x4_t vec_b_rot = vextq_f32(vec_b, vec_b, 1);

        float32x4_t prod = vmulq_f32(vec_a, vec_b_rot);

        // prod = [ ajbj, aibj, akbi, ajbk ]

        prod = vmlsq_f32(prod, vec_a_rot, vec_b);
        // prod = [ ajbj-ajbj, aibj-ajbi, akbi-aibk, ajbk-akbj ]

        vst1_f32(dst, vget_low_f32(prod));               // Store the lower two elements to address r
        vst1_lane_f32(dst + 2, vget_high_f32(prod), 0);  // Store the 3rd element
    }

#if AX_64BITS
    inline static void transformVertices(V3F_C4B_T2F* dst, const V3F_C4B_T2F* src, size_t count, const Mat4& transform)
    {
        auto end = dst + count;

        // Load matrix
        float32x4x4_t m = vld1q_f32_x4(transform.m);

        // Process 4 vertices at a time if there's enough data
        auto end4 = dst + count / 4 * 4;
        while (dst < end4)
        {
            // Do this for each vertex
            // dst->vertices.x = pos.x * m[0] + pos.y * m[4] + pos.z * m[8]  + m[12];
            // dst->vertices.y = pos.x * m[1] + pos.y * m[5] + pos.z * m[9]  + m[13];
            // dst->vertices.z = pos.x * m[2] + pos.y * m[6] + pos.z * m[10] + m[14];

            // First, load each vertex, multiply x by column 0 and add to column 3
            // Note: since we're reading 4 floats it will load color bytes into v.w
            float32x4_t v0 = vld1q_f32(&src[0].vertices.x);
            float32x4_t r0 = vmlaq_laneq_f32(m.val[3], m.val[0], v0, 0);
            float32x4_t v1 = vld1q_f32(&src[1].vertices.x);
            float32x4_t r1 = vmlaq_laneq_f32(m.val[3], m.val[0], v1, 0);
            float32x4_t v2 = vld1q_f32(&src[2].vertices.x);
            float32x4_t r2 = vmlaq_laneq_f32(m.val[3], m.val[0], v2, 0);
            float32x4_t v3 = vld1q_f32(&src[3].vertices.x);
            float32x4_t r3 = vmlaq_laneq_f32(m.val[3], m.val[0], v3, 0);

            // Load texCoords
            float32x2_t uv0 = vld1_f32(&src[0].texCoords.u);
            float32x2_t uv1 = vld1_f32(&src[1].texCoords.u);
            float32x2_t uv2 = vld1_f32(&src[2].texCoords.u);
            float32x2_t uv3 = vld1_f32(&src[3].texCoords.u);

            // Multiply y by column 1 and add to result
            r0 = vmlaq_laneq_f32(r0, m.val[1], v0, 1);
            r1 = vmlaq_laneq_f32(r1, m.val[1], v1, 1);
            r2 = vmlaq_laneq_f32(r2, m.val[1], v2, 1);
            r3 = vmlaq_laneq_f32(r3, m.val[1], v3, 1);

            // Multiply z by column 2 and add to result
            r0 = vmlaq_laneq_f32(r0, m.val[2], v0, 2);
            r1 = vmlaq_laneq_f32(r1, m.val[2], v1, 2);
            r2 = vmlaq_laneq_f32(r2, m.val[2], v2, 2);
            r3 = vmlaq_laneq_f32(r3, m.val[2], v3, 2);

            // Set w to loaded color
            r0 = vsetq_lane_f32(vgetq_lane_f32(v0, 3), r0, 3);
            r1 = vsetq_lane_f32(vgetq_lane_f32(v1, 3), r1, 3);
            r2 = vsetq_lane_f32(vgetq_lane_f32(v2, 3), r2, 3);
            r3 = vsetq_lane_f32(vgetq_lane_f32(v3, 3), r3, 3);

            // Store result
            vst1q_f32(&dst[0].vertices.x, r0);
            vst1_f32(&dst[0].texCoords.u, uv0);
            vst1q_f32(&dst[1].vertices.x, r1);
            vst1_f32(&dst[1].texCoords.u, uv1);
            vst1q_f32(&dst[2].vertices.x, r2);
            vst1_f32(&dst[2].texCoords.u, uv2);
            vst1q_f32(&dst[3].vertices.x, r3);
            vst1_f32(&dst[3].texCoords.u, uv3);

            dst += 4;
            src += 4;
        }

        // Process remaining vertices one by one
        while (dst < end)
        {
            float32x4_t v  = vld1q_f32(&src->vertices.x);
            float32x4_t r  = vmlaq_laneq_f32(m.val[3], m.val[0], v, 0);
            r              = vmlaq_laneq_f32(r, m.val[1], v, 1);
            r              = vmlaq_laneq_f32(r, m.val[2], v, 2);
            r              = vsetq_lane_f32(vgetq_lane_f32(v, 3), r, 3);
            float32x2_t uv = vld1_f32(&src->texCoords.u);
            vst1q_f32(&dst->vertices.x, r);
            vst1_f32(&dst->texCoords.u, uv);

            ++dst;
            ++src;
        }
    }

    inline static void transformIndices(uint16_t* dst, const uint16_t* src, size_t count, uint16_t offset)
    {
        auto end = dst + count;
        auto off = vdupq_n_u16(offset);

        if (count < 8)
            goto LEFTOVER;

        // Process 32 indices at a time if there's enough data
        while (count >= 32)
        {
            // Load 32 indices
            uint16x8x4_t v = vld1q_u16_x4(src);

            // Add offset
            v.val[0] = vaddq_u16(v.val[0], off);
            v.val[1] = vaddq_u16(v.val[1], off);
            v.val[2] = vaddq_u16(v.val[2], off);
            v.val[3] = vaddq_u16(v.val[3], off);

            // Store result
            vst1q_u16_x4(dst, v);

            dst += 32;
            src += 32;
            count -= 32;
        }

        // Process 8 indices at a time if there's enough data
        while (count >= 8)
        {
            uint16x8_t v = vld1q_u16(src);
            v            = vaddq_u16(v, off);
            vst1q_u16(dst, v);

            dst += 8;
            src += 8;
            count -= 8;
        }

    LEFTOVER:
        // Process remaining indices one by one
        while (count > 0)
        {
            *dst = *src + offset;
            ++dst;
            ++src;
            --count;
        }
    }
#else
    inline static void transformVertices(ax::V3F_C4B_T2F* dst,
                                         const ax::V3F_C4B_T2F* src,
                                         size_t count,
                                         const ax::Mat4& transform)
    {
        auto end = dst + count;

        // Load matrix
        float32x4_t mc0 = vld1q_f32(transform.m);
        float32x4_t mc1 = vld1q_f32(transform.m + 4);
        float32x4_t mc2 = vld1q_f32(transform.m + 8);
        float32x4_t mc3 = vld1q_f32(transform.m + 12);

        // Process 4 vertices at a time
        auto end4 = dst + count / 4 * 4;
        while (dst < end4)
        {
            // Load 4 vertices. Note that color will also get loaded into w
            float32x2_t xy0 = vld1_f32(&src[0].vertices.x);
            float32x2_t zw0 = vld1_f32(&src[0].vertices.z);
            float32x2_t uv0 = vld1_f32(&src[0].texCoords.u);
            float32x2_t xy1 = vld1_f32(&src[1].vertices.x);
            float32x2_t zw1 = vld1_f32(&src[1].vertices.z);
            float32x2_t uv1 = vld1_f32(&src[1].texCoords.u);
            float32x2_t xy2 = vld1_f32(&src[2].vertices.x);
            float32x2_t zw2 = vld1_f32(&src[2].vertices.z);
            float32x2_t uv2 = vld1_f32(&src[2].texCoords.u);
            float32x2_t xy3 = vld1_f32(&src[3].vertices.x);
            float32x2_t zw3 = vld1_f32(&src[3].vertices.z);
            float32x2_t uv3 = vld1_f32(&src[3].texCoords.u);

            // Multiply x by column 0
            float32x4_t r0 = vmulq_lane_f32(mc0, xy0, 0);
            float32x4_t r1 = vmulq_lane_f32(mc0, xy1, 0);
            float32x4_t r2 = vmulq_lane_f32(mc0, xy2, 0);
            float32x4_t r3 = vmulq_lane_f32(mc0, xy3, 0);

            // Multiply y by column 1 and add to result
            r0 = vmlaq_lane_f32(r0, mc1, xy0, 1);
            r1 = vmlaq_lane_f32(r1, mc1, xy1, 1);
            r2 = vmlaq_lane_f32(r2, mc1, xy2, 1);
            r3 = vmlaq_lane_f32(r3, mc1, xy3, 1);

            // Multiply z by column 2 and add to result
            r0 = vmlaq_lane_f32(r0, mc2, zw0, 0);
            r1 = vmlaq_lane_f32(r1, mc2, zw1, 0);
            r2 = vmlaq_lane_f32(r2, mc2, zw2, 0);
            r3 = vmlaq_lane_f32(r3, mc2, zw3, 0);

            // Add column 3
            r0 = vaddq_f32(r0, mc3);
            r1 = vaddq_f32(r1, mc3);
            r2 = vaddq_f32(r2, mc3);
            r3 = vaddq_f32(r3, mc3);

            // Set color
            r0 = vsetq_lane_f32(vget_lane_f32(zw0, 1), r0, 3);
            r1 = vsetq_lane_f32(vget_lane_f32(zw1, 1), r1, 3);
            r2 = vsetq_lane_f32(vget_lane_f32(zw2, 1), r2, 3);
            r3 = vsetq_lane_f32(vget_lane_f32(zw3, 1), r3, 3);

            // Store result
            vst1q_f32(&dst[0].vertices.x, r0);
            vst1_f32(&dst[0].texCoords.u, uv0);
            vst1q_f32(&dst[1].vertices.x, r1);
            vst1_f32(&dst[1].texCoords.u, uv1);
            vst1q_f32(&dst[2].vertices.x, r2);
            vst1_f32(&dst[2].texCoords.u, uv2);
            vst1q_f32(&dst[3].vertices.x, r3);
            vst1_f32(&dst[3].texCoords.u, uv3);

            dst += 4;
            src += 4;
        }

        // Process remaining vertices
        while (dst < end)
        {
            // Load vertex
            float32x2_t xy = vld1_f32(&src->vertices.x);
            float32x2_t zw = vld1_f32(&src->vertices.z);
            float32x2_t uv = vld1_f32(&src->texCoords.u);

            // Multiply x by column 0
            float32x4_t r = vmulq_lane_f32(mc0, xy, 0);
            // Multiply y by column 1 and add to result
            r = vmlaq_lane_f32(r, mc1, xy, 1);
            // Multiply z by column 2 and add to result
            r = vmlaq_lane_f32(r, mc2, zw, 0);
            // Add column 3
            r = vaddq_f32(r, mc3);

            // Set color
            r = vsetq_lane_f32(vget_lane_f32(zw, 1), r, 3);

            // Store result
            vst1q_f32(&dst->vertices.x, r);
            vst1_f32(&dst->texCoords.u, uv);

            ++dst;
            ++src;
        }
    }
#endif
};

NS_AX_MATH_END
