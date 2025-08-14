#pragma once

#include "rhi/RHITypes.h"
#include "rhi/PixelBufferDescriptor.h"
#include "rhi/d3d/TextureD3D.h"
#include <dxgi.h>

namespace ax::rhi::d3d
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

template <typename T, unsigned int N>
inline void SafeRelease(T (&resourceBlock)[N])
{
    for (unsigned int i = 0; i < N; i++)
    {
        SafeRelease(resourceBlock[i]);
    }
}

template <typename T>
inline void SafeRelease(T& resource)
{
    if (resource)
    {
        resource->Release();
        resource = nullptr;
    }
}

}  // namespace ax::rhi::d3d
