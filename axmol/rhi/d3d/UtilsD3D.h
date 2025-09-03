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
#pragma once

#include "axmol/rhi/RHITypes.h"
#include "axmol/rhi/PixelBufferDesc.h"
#include "axmol/rhi/d3d/TextureD3D.h"
#include <utility>
#include <dxgi.h>

namespace ax::rhi::d3d
{
class DriverImpl;

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

inline namespace UtilsD3D
{
const PixelFormatInfo* toDxgiFormatInfo(PixelFormat pf);

void updateDefaultRenderTargetAttachments(DriverImpl*, IDXGISwapChain*);

TextureImpl* getDefaultColorAttachment();
TextureImpl* getDefaultDepthStencilAttachment();

void fatalError(std::string_view op, HRESULT hr);

};  // namespace UtilsD3D

/** @} */

}  // namespace ax::rhi::d3d

#define AX_D3D_FAST_FAIL(expr)                                  \
    do                                                          \
    {                                                           \
        HRESULT _hr = (expr);                                   \
        if (FAILED(_hr))                                        \
            ::ax::rhi::d3d::fatalError(#expr " failed"sv, _hr); \
    } while (0)
