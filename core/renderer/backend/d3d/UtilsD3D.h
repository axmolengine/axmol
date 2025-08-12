#pragma once

#include "renderer/backend/BaseDefs.h"
#include "renderer/backend/PixelBufferDescriptor.h"
#include "renderer/backend/d3d/TextureD3D.h"
#include <dxgi.h>

namespace ax::backend::d3d
{
    class DriverImpl;

    struct PixelFormatInfo
    {
        DXGI_FORMAT format;
        DXGI_FORMAT fmtSrv;   // View format for SRV/UAV
        DXGI_FORMAT fmtDsv;   // View format for DSV
        DXGI_FORMAT fmtSrgb;  // fmtSrgb
    };

    struct UtilsD3D
    {
        static const PixelFormatInfo* toDxgiFormatInfo(PixelFormat pf);

        static void updateDefaultRenderTargetAttachments(DriverImpl*, IDXGISwapChain*);

        static TextureImpl* getDefaultColorAttachment();
        static TextureImpl* getDefaultDepthStencilAttachment();
    };
}  // namespace ax::backend
