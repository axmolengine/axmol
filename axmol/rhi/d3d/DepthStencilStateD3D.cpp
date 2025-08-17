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
#include "axmol/rhi/d3d/DepthStencilStateD3D.h"
#include <stdexcept>
#include <cstring>

namespace ax::rhi::d3d
{
namespace
{
static D3D11_COMPARISON_FUNC to_d3d(CompareFunc func)
{
    switch (func)
    {
    case CompareFunc::NEVER:
        return D3D11_COMPARISON_NEVER;
    case CompareFunc::LESS:
        return D3D11_COMPARISON_LESS;
    case CompareFunc::LESS_EQUAL:
        return D3D11_COMPARISON_LESS_EQUAL;
    case CompareFunc::GREATER:
        return D3D11_COMPARISON_GREATER;
    case CompareFunc::GREATER_EQUAL:
        return D3D11_COMPARISON_GREATER_EQUAL;
    case CompareFunc::EQUAL:
        return D3D11_COMPARISON_EQUAL;
    case CompareFunc::NOT_EQUAL:
        return D3D11_COMPARISON_NOT_EQUAL;
    case CompareFunc::ALWAYS:
        return D3D11_COMPARISON_ALWAYS;
    default:
        return D3D11_COMPARISON_LESS_EQUAL;
    }
}

static D3D11_STENCIL_OP to_d3d(StencilOp op)
{
    switch (op)
    {
    case StencilOp::KEEP:
        return D3D11_STENCIL_OP_KEEP;
    case StencilOp::ZERO:
        return D3D11_STENCIL_OP_ZERO;
    case StencilOp::REPLACE:
        return D3D11_STENCIL_OP_REPLACE;
    case StencilOp::INVERT:
        return D3D11_STENCIL_OP_INVERT;
    case StencilOp::INCREMENT_WRAP:
        return D3D11_STENCIL_OP_INCR;
    case StencilOp::DECREMENT_WRAP:
        return D3D11_STENCIL_OP_DECR;
    default:
        return D3D11_STENCIL_OP_KEEP;
    }
}

static D3D11_DEPTH_STENCILOP_DESC make_op_desc(const StencilDesc& s)
{
    D3D11_DEPTH_STENCILOP_DESC d{};
    d.StencilFailOp      = to_d3d(s.stencilFailureOp);
    d.StencilDepthFailOp = to_d3d(s.depthFailureOp);
    d.StencilPassOp      = to_d3d(s.depthStencilPassOp);
    d.StencilFunc        = to_d3d(s.stencilCompareFunc);
    return d;
}
}  // namespace

void DepthStencilStateImpl::update(const DepthStencilDesc& desc)
{
    if (_state && memcmp(&desc, &_dsDesc, sizeof(desc)) == 0)
    {
        return;
    }

    DepthStencilState::update(desc);

    D3D11_DEPTH_STENCIL_DESC d{};

    d.DepthEnable    = bitmask::any(desc.flags, DepthStencilFlags::DEPTH_TEST) ? TRUE : FALSE;
    d.DepthWriteMask = bitmask::any(desc.flags, DepthStencilFlags::DEPTH_WRITE) ? D3D11_DEPTH_WRITE_MASK_ALL
                                                                                : D3D11_DEPTH_WRITE_MASK_ZERO;
    d.DepthFunc      = to_d3d(desc.depthCompareFunc);

    d.StencilEnable    = bitmask::any(desc.flags, DepthStencilFlags::STENCIL_TEST) ? TRUE : FALSE;
    d.StencilReadMask  = desc.frontFaceStencil.readMask;
    d.StencilWriteMask = desc.frontFaceStencil.writeMask;
    d.FrontFace        = make_op_desc(desc.frontFaceStencil);
    d.BackFace         = make_op_desc(desc.backFaceStencil);

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> newState;
    HRESULT hr = _device->CreateDepthStencilState(&d, newState.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("CreateDepthStencilState failed");
    }

    _state  = std::move(newState);
    _dsDesc = desc;
    _isBackFrontStencilEqual =
        (std::memcmp(&desc.frontFaceStencil, &desc.backFaceStencil, sizeof(StencilDesc)) == 0);
}

void DepthStencilStateImpl::apply(ID3D11DeviceContext* ctx, UINT stencilRef) const
{
    if (ctx && _state)
        ctx->OMSetDepthStencilState(_state.Get(), stencilRef);
}

void DepthStencilStateImpl::reset(ID3D11DeviceContext* ctx)
{
    if (!_disableState)
    {
        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable              = FALSE;
        dsDesc.DepthWriteMask           = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc                = D3D11_COMPARISON_ALWAYS;

        dsDesc.StencilEnable = FALSE;

        _device->CreateDepthStencilState(&dsDesc, _disableState.GetAddressOf());
    }

    if (_disableState)
        ctx->OMSetDepthStencilState(_disableState.Get(), 0);
}
}  // namespace ax::rhi::d3d
