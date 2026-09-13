/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once
#include "axmol/math/MathUtil.h"
#include "axmol/rhi/RHITypes.h"
#include <dxgi.h>
#include <d3dcommon.h>
#include "axmol/platform/msw/ComPtr.h"

namespace ax::rhi::dxutils
{

/**
 * @addtogroup _d3d
 * @{
 */

struct PixelFormatInfo
{
    DXGI_FORMAT format;
    DXGI_FORMAT fmtSrv;   // View format for SRV/UAV
    DXGI_FORMAT fmtDsv;   // View format for DSV
    DXGI_FORMAT fmtSrgb;  // fmtSrgb
};

const PixelFormatInfo* toDxgiFormatInfo(PixelFormat pf);

int evalulateMaxTexSize(D3D_FEATURE_LEVEL fl);

DXGI_FORMAT getUAVCompatibleFormat(DXGI_FORMAT format);

void fatalError(std::string_view op, HRESULT hr);

template <typename _Viewport>
inline bool viewportsEqual(const _Viewport& a, const _Viewport& b)
{  // we don't compare minDepth and maxDepth because they are always 0.0f and 1.0f in axmol, and comparing them may
   // cause redundant state changes due to precision loss
    return MathUtil::fuzzyEquals(a.TopLeftX, b.TopLeftX) && MathUtil::fuzzyEquals(a.TopLeftY, b.TopLeftY) &&
           MathUtil::fuzzyEquals(a.Width, b.Width) && MathUtil::fuzzyEquals(a.Height, b.Height);
}

template <typename _Rect>
inline bool rectsEqual(const _Rect& a, const _Rect& b)
{
    return a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom;
}

}  // namespace ax::rhi::dxutils
/** @} */

#define _AXASSERT_HR(hr)                                          \
    do                                                            \
    {                                                             \
        HRESULT _hr = (hr);                                       \
        if (FAILED(_hr))                                          \
            ::ax::rhi::dxutils::fatalError(#hr " failed"sv, _hr); \
    } while (0)
