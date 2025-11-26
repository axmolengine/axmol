#pragma once
#include "axmol/rhi/RHITypes.h"
#include <dxgi.h>
#include <d3dcommon.h>
#include <wrl/client.h>

namespace ax::rhi
{
using Microsoft::WRL::ComPtr;
}

namespace ax::rhi::dxutils
{

/**
 * @addtogroup _d3d11
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

}  // namespace ax::rhi::dxutils
/** @} */
