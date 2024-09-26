/**
Copyright 2013 BlackBerry Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "math/MathUtil.h"
#include "math/Mat4.h"
#include "base/Macros.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#    include <cpu-features.h>
#endif

#if defined(AX_SSE_INTRINSICS)
#    include "math/MathUtilSSE.inl"
#elif defined(AX_NEON_INTRINSICS)
#    include "math/MathUtilNeon.inl"
#endif

#include "math/MathUtil.inl"

NS_AX_MATH_BEGIN

void MathUtil::smooth(float* x, float target, float elapsedTime, float responseTime)
{
    AX_ASSERT(x);

    if (elapsedTime > 0)
    {
        *x += (target - *x) * elapsedTime / (elapsedTime + responseTime);
    }
}

void MathUtil::smooth(float* x, float target, float elapsedTime, float riseTime, float fallTime)
{
    AX_ASSERT(x);

    if (elapsedTime > 0)
    {
        float delta = target - *x;
        *x += delta * elapsedTime / (elapsedTime + (delta > 0 ? riseTime : fallTime));
    }
}

float MathUtil::lerp(float from, float to, float alpha)
{
    return from * (1.0f - alpha) + to * alpha;
}

bool MathUtil::isNeon32Enabled()
{
#if defined(AX_NEON_INTRINSICS) && !AX_64BITS
#    if AX_NEON_INTRINSICS == 1 && AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
    class AnrdoidNeonChecker
    {
    public:
        AnrdoidNeonChecker()
        {
            if (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM &&
                (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0)
                _isNeonEnabled = true;
            else
                _isNeonEnabled = false;
        }
        bool isNeonEnabled() const { return _isNeonEnabled; }

    private:
        bool _isNeonEnabled;
    };
    static AnrdoidNeonChecker checker;
    return checker.isNeonEnabled();
#    else
    return true;
#    endif
#else
    return false;
#endif
}

void MathUtil::addMatrix(const float* m, float scalar, float* dst)
{
#if defined(AX_SSE_INTRINSICS)
    MathUtilSSE::addMatrix(reinterpret_cast<const _xm128_t*>(m), scalar, reinterpret_cast<_xm128_t*>(dst));
#elif defined(AX_NEON_INTRINSICS)
#    if AX_64BITS || AX_NEON_INTRINSICS > 1
    MathUtilNeon::addMatrix(reinterpret_cast<const _xm128_t*>(m), scalar, reinterpret_cast<_xm128_t*>(dst));
#    else
    if (isNeon32Enabled())
        MathUtilNeon::addMatrix(reinterpret_cast<const _xm128_t*>(m), scalar, reinterpret_cast<_xm128_t*>(dst));
    else
        MathUtilC::addMatrix(m, scalar, dst);
#    endif
#else
    MathUtilC::addMatrix(m, scalar, dst);
#endif
}

void MathUtil::addMatrix(const float* m1, const float* m2, float* dst)
{
#if defined(AX_SSE_INTRINSICS)
    MathUtilSSE::addMatrix(reinterpret_cast<const _xm128_t*>(m1), reinterpret_cast<const _xm128_t*>(m2),
                           reinterpret_cast<_xm128_t*>(dst));
#elif defined(AX_NEON_INTRINSICS)
#    if AX_64BITS || AX_NEON_INTRINSICS > 1
    MathUtilNeon::addMatrix(reinterpret_cast<const _xm128_t*>(m1), reinterpret_cast<const _xm128_t*>(m2),
                            reinterpret_cast<_xm128_t*>(dst));
#    else
    if (isNeon32Enabled())
        MathUtilNeon::addMatrix(reinterpret_cast<const _xm128_t*>(m1), reinterpret_cast<const _xm128_t*>(m2),
                                reinterpret_cast<_xm128_t*>(dst));
    else
        MathUtilC::addMatrix(m1, m2, dst);
#    endif
#else
    MathUtilC::addMatrix(m1, m2, dst);
#endif
}

void MathUtil::subtractMatrix(const float* m1, const float* m2, float* dst)
{
#if defined(AX_SSE_INTRINSICS)
    MathUtilSSE::subtractMatrix(reinterpret_cast<const _xm128_t*>(m1), reinterpret_cast<const _xm128_t*>(m2),
                                reinterpret_cast<_xm128_t*>(dst));
#elif defined(AX_NEON_INTRINSICS)
#    if AX_64BITS || AX_NEON_INTRINSICS > 1
    MathUtilNeon::subtractMatrix(reinterpret_cast<const _xm128_t*>(m1), reinterpret_cast<const _xm128_t*>(m2),
                                 reinterpret_cast<_xm128_t*>(dst));
#    else
    if (isNeon32Enabled())
        MathUtilNeon::subtractMatrix(reinterpret_cast<const _xm128_t*>(m1), reinterpret_cast<const _xm128_t*>(m2),
                                     reinterpret_cast<_xm128_t*>(dst));
    else
        MathUtilC::subtractMatrix(m1, m2, dst);
#    endif
#else
    MathUtilC::subtractMatrix(m1, m2, dst);
#endif
}

void MathUtil::multiplyMatrix(const float* m, float scalar, float* dst)
{
#if defined(AX_SSE_INTRINSICS)
    MathUtilSSE::multiplyMatrix(reinterpret_cast<const _xm128_t*>(m), scalar, reinterpret_cast<_xm128_t*>(dst));
#elif defined(AX_NEON_INTRINSICS)
#    if AX_64BITS || AX_NEON_INTRINSICS > 1
    MathUtilNeon::multiplyMatrix(reinterpret_cast<const _xm128_t*>(m), scalar, reinterpret_cast<_xm128_t*>(dst));
#    else
    if (isNeon32Enabled())
        MathUtilNeon::multiplyMatrix(reinterpret_cast<const _xm128_t*>(m), scalar, reinterpret_cast<_xm128_t*>(dst));
    else
        MathUtilC::multiplyMatrix(m, scalar, dst);
#    endif
#else
    MathUtilC::multiplyMatrix(m, scalar, dst);
#endif
}

void MathUtil::multiplyMatrix(const float* m1, const float* m2, float* dst)
{
#if defined(AX_SSE_INTRINSICS)
    MathUtilSSE::multiplyMatrix(reinterpret_cast<const _xm128_t*>(m1), reinterpret_cast<const _xm128_t*>(m2),
                                reinterpret_cast<_xm128_t*>(dst));
#elif defined(AX_NEON_INTRINSICS)
#    if AX_64BITS || AX_NEON_INTRINSICS > 1
    MathUtilNeon::multiplyMatrix(reinterpret_cast<const _xm128_t*>(m1), reinterpret_cast<const _xm128_t*>(m2),
                                 reinterpret_cast<_xm128_t*>(dst));
#    else
    if (isNeon32Enabled())
        MathUtilNeon::multiplyMatrix(reinterpret_cast<const _xm128_t*>(m1), reinterpret_cast<const _xm128_t*>(m2),
                                     reinterpret_cast<_xm128_t*>(dst));
    else
        MathUtilC::multiplyMatrix(m1, m2, dst);
#    endif
#else
    MathUtilC::multiplyMatrix(m1, m2, dst);
#endif
}

void MathUtil::negateMatrix(const float* m, float* dst)
{
#if defined(AX_SSE_INTRINSICS)
    MathUtilSSE::negateMatrix(reinterpret_cast<const _xm128_t*>(m), reinterpret_cast<_xm128_t*>(dst));
#elif defined(AX_NEON_INTRINSICS)
#    if AX_64BITS || AX_NEON_INTRINSICS > 1
    MathUtilNeon::negateMatrix(reinterpret_cast<const _xm128_t*>(m), reinterpret_cast<_xm128_t*>(dst));
#    else
    if (isNeon32Enabled())
        MathUtilNeon::negateMatrix(reinterpret_cast<const _xm128_t*>(m), reinterpret_cast<_xm128_t*>(dst));
    else
        MathUtilC::negateMatrix(m, dst);
#    endif
#else
    MathUtilC::negateMatrix(m, dst);
#endif
}

void MathUtil::transposeMatrix(const float* m, float* dst)
{
#if defined(AX_SSE_INTRINSICS)
    MathUtilSSE::transposeMatrix(reinterpret_cast<const _xm128_t*>(m), reinterpret_cast<_xm128_t*>(dst));
#elif defined(AX_NEON_INTRINSICS)
#    if AX_64BITS || AX_NEON_INTRINSICS > 1
    MathUtilNeon::transposeMatrix(reinterpret_cast<const _xm128_t*>(m), reinterpret_cast<_xm128_t*>(dst));
#    else
    if (isNeon32Enabled())
        MathUtilNeon::transposeMatrix(reinterpret_cast<const _xm128_t*>(m), reinterpret_cast<_xm128_t*>(dst));
    else
        MathUtilC::transposeMatrix(m, dst);
#    endif
#else
    MathUtilC::transposeMatrix(m, dst);
#endif
}

void MathUtil::transformVec4(const float* m, float x, float y, float z, float w, float* dst /*vec3*/)
{
#if defined(AX_SSE_INTRINSICS)
    MathUtilSSE::transformVec4(reinterpret_cast<const _xm128_t*>(m), x, y, z, w, dst);
#elif defined(AX_NEON_INTRINSICS)
#    if AX_64BITS || AX_NEON_INTRINSICS > 1
    MathUtilNeon::transformVec4(reinterpret_cast<const _xm128_t*>(m), x, y, z, w, dst);
#    else
    if (isNeon32Enabled())
        MathUtilNeon::transformVec4(reinterpret_cast<const _xm128_t*>(m), x, y, z, w, dst);
    else
        MathUtilC::transformVec4(m, x, y, z, w, dst);
#    endif
#else
    MathUtilC::transformVec4(m, x, y, z, w, dst);
#endif
}

void MathUtil::transformVec4(const float* m, const float* v, float* dst /*vec4*/)
{
#if defined(AX_SSE_INTRINSICS)
    MathUtilSSE::transformVec4(reinterpret_cast<const _xm128_t*>(m), v, dst);
#elif defined(AX_NEON_INTRINSICS)
#    if AX_64BITS || AX_NEON_INTRINSICS > 1
    MathUtilNeon::transformVec4(reinterpret_cast<const _xm128_t*>(m), v, dst);
#    else
    if (isNeon32Enabled())
        MathUtilNeon::transformVec4(reinterpret_cast<const _xm128_t*>(m), v, dst);
    else
        MathUtilC::transformVec4(m, v, dst);
#    endif
#else
    MathUtilC::transformVec4(m, v, dst);
#endif
}

void MathUtil::crossVec3(const float* v1, const float* v2, float* dst)
{
#if defined(AX_SSE_INTRINSICS)
    MathUtilSSE::crossVec3(v1, v2, dst);
#elif defined(AX_NEON_INTRINSICS)
#    if AX_64BITS || AX_NEON_INTRINSICS > 1
    MathUtilNeon::crossVec3(v1, v2, dst);
#    else
    if (isNeon32Enabled())
        MathUtilNeon::crossVec3(v1, v2, dst);
    else
        MathUtilC::crossVec3(v1, v2, dst);
#    endif
#else
    MathUtilC::crossVec3(v1, v2, dst);
#endif
}

void MathUtil::transformVertices(V3F_C4B_T2F* dst, const V3F_C4B_T2F* src, size_t count, const Mat4& transform)
{
    // Check some assumptions made by optimizations
    static_assert(sizeof(V3F_C4B_T2F) == 24);
    static_assert(offsetof(V3F_C4B_T2F, vertices) == 0);
    static_assert(offsetof(V3F_C4B_T2F, colors) == 12);
    static_assert(offsetof(V3F_C4B_T2F, texCoords) == 16);
#if defined(AX_SSE_INTRINSICS)
    MathUtilSSE::transformVertices(dst, src, count, transform);
#elif defined(AX_NEON_INTRINSICS)
#    if AX_64BITS || AX_NEON_INTRINSICS > 1
    MathUtilNeon::transformVertices(dst, src, count, transform);
#    else
    if (isNeon32Enabled())
        MathUtilNeon::transformVertices(dst, src, count, transform);
    else
        MathUtilC::transformVertices(dst, src, count, transform);
#    endif
#else
    MathUtilC::transformVertices(dst, src, count, transform);
#endif
}

void MathUtil::transformIndices(uint16_t* dst, const uint16_t* src, size_t count, uint16_t offset)
{
#if defined(AX_SSE_INTRINSICS)
    MathUtilSSE::transformIndices(dst, src, count, offset);
#elif defined(AX_NEON_INTRINSICS) && AX_64BITS
    MathUtilNeon::transformIndices(dst, src, count, offset);
#else
    MathUtilC::transformIndices(dst, src, count, offset);
#endif
}

NS_AX_MATH_END
