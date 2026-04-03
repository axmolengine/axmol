#pragma once
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

}  // namespace ax::rhi::dxutils
/** @} */

#define _AXASSERT_HR(hr)                                          \
    do                                                            \
    {                                                             \
        HRESULT _hr = (hr);                                       \
        if (FAILED(_hr))                                          \
            ::ax::rhi::dxutils::fatalError(#hr " failed"sv, _hr); \
    } while (0)
