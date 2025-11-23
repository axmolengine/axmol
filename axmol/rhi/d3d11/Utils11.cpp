/****************************************************************************
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

// Direct3D / DXGI equivalent of the OpenGL `s_textureFormats` table.
// For compressed formats that D3D11/D3D12 do **not** natively support (PVRTC, ETC*, ATC, ASTC, …),
// we fall back to the closest broadly-available format so the rest of the engine can still
// create a valid texture object and sample from it.
//   • Lossy RGB‐only schemes → BC1 (a.k.a. DXT1) or R8G8B8A8_UNORM as a generic 32-bit placeholder
//   • Lossy RGBA schemes     → BC3 (DXT5) or R8G8B8A8_UNORM
//   • High-quality ASTC      → BC7 (best quality on PC GPUs)
//
// The **`type`** column has no real DXGI equivalent; it is kept for structural parity and
// set to `DXGI_FORMAT_UNKNOWN`.
//

#include "axmol/rhi/d3d11/Utils11.h"
#include "axmol/rhi/d3d11/Driver11.h"
#include "axmol/rhi/RHIUtils.h"

#include "axmol/base/Logging.h"

#include "ntcvt/ntcvt.hpp"

#include <dxgiformat.h>
#include <comdef.h>

namespace ax::rhi::d3d11
{

static TextureImpl* s_defaultColorAttachment;
static TextureImpl* s_defaultDepthStencilAttachment;

void UtilsD3D::updateDefaultRenderTargetAttachments(DriverImpl* driverImpl, IDXGISwapChain* swapChain)
{
    AX_SAFE_RELEASE_NULL(s_defaultColorAttachment);
    AX_SAFE_RELEASE_NULL(s_defaultDepthStencilAttachment);

    if (swapChain)
    {
        auto device  = driverImpl->getDevice();
        auto context = driverImpl->getContext();

        // swap chain only contains color attachment
        ComPtr<ID3D11RenderTargetView> rtv;
        ComPtr<ID3D11Texture2D> backBuffer;
        HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
        hr         = device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtv.GetAddressOf());
        s_defaultColorAttachment = new TextureImpl(device, backBuffer.Get());

        UINT fmtSupport = 0;
        device->CheckFormatSupport(DXGI_FORMAT_D24_UNORM_S8_UINT, &fmtSupport);
        assert(fmtSupport & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL);

        // create depth-stencil attachment
        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width                = s_defaultColorAttachment->getWidth();
        depthDesc.Height               = s_defaultColorAttachment->getHeight();
        depthDesc.MipLevels            = 1;
        depthDesc.ArraySize            = 1;
        depthDesc.Format               = DXGI_FORMAT_D24_UNORM_S8_UINT;  // 24-bit depth, 8-bit stencil
        depthDesc.Usage                = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags            = D3D11_BIND_DEPTH_STENCIL;
        depthDesc.SampleDesc.Count     = 1;
        depthDesc.SampleDesc.Quality   = 0;

        ComPtr<ID3D11Texture2D> depthStencilTexture;
        hr = device->CreateTexture2D(&depthDesc, nullptr, depthStencilTexture.GetAddressOf());

        ComPtr<ID3D11DepthStencilView> depthStencilView = nullptr;
        hr = device->CreateDepthStencilView(depthStencilTexture.Get(), nullptr, depthStencilView.GetAddressOf());

        context->OMSetRenderTargets(1, rtv.GetAddressOf(), depthStencilView.Get());

        s_defaultDepthStencilAttachment = new TextureImpl(device, depthStencilTexture.Get());
    }
}

TextureImpl* UtilsD3D::getDefaultColorAttachment()
{
    return s_defaultColorAttachment;
}

TextureImpl* UtilsD3D::getDefaultDepthStencilAttachment()
{
    return s_defaultDepthStencilAttachment;
}

void UtilsD3D::fatalError(std::string_view op, HRESULT hr)
{
    auto msg = fmt::format("{}: 0x{:08X}", op, static_cast<unsigned>(hr));
    showAlert(msg, "D3D11: Fatal Error", AlertStyle::IconError | AlertStyle::RequireSync);
    utils::killCurrentProcess();  // kill current process, don't cause crash when driver issue.
}

}  // namespace ax::rhi::d3d11
