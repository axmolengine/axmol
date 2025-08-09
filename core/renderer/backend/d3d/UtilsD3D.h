#pragma once

#include "renderer/backend/BaseDefs.h"
#include "renderer/backend/PixelBufferDescriptor.h"
#include "renderer/backend/d3d/TextureD3D.h"
#include <dxgi.h>

namespace ax::backend::d3d
{
    class DriverImpl;
    struct UtilsD3D
    {
        static void toD3DTypes(PixelFormat pf, DXGI_FORMAT& dxgiFormat, bool& isCompressed);

        static void updateDefaultRenderTargetAttachments(DriverImpl*, IDXGISwapChain*);

        static TextureImpl* getDefaultColorAttachment();
        static TextureImpl* getDefaultDepthStencilAttachment();
    };
}  // namespace ax::backend
