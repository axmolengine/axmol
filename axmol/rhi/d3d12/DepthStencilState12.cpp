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
#include "axmol/rhi/d3d12/DepthStencilState12.h"
#include "axmol/tlx/hash.hpp"
#include "axmol/base/Logging.h"
#include <cstring>

namespace ax::rhi::d3d12
{
namespace
{
// CompareFunc -> D3D12_COMPARISON_FUNC
static D3D12_COMPARISON_FUNC to_d3d(CompareFunc func)
{
    switch (func)
    {
    case CompareFunc::NEVER:
        return D3D12_COMPARISON_FUNC_NEVER;
    case CompareFunc::LESS:
        return D3D12_COMPARISON_FUNC_LESS;
    case CompareFunc::LESS_EQUAL:
        return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case CompareFunc::GREATER:
        return D3D12_COMPARISON_FUNC_GREATER;
    case CompareFunc::GREATER_EQUAL:
        return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    case CompareFunc::EQUAL:
        return D3D12_COMPARISON_FUNC_EQUAL;
    case CompareFunc::NOT_EQUAL:
        return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case CompareFunc::ALWAYS:
        return D3D12_COMPARISON_FUNC_ALWAYS;
    default:
        return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    }
}

// StencilOp -> D3D12_STENCIL_OP
static D3D12_STENCIL_OP to_d3d(StencilOp op)
{
    // Lookup table indexed by StencilOp enum value
    static constexpr D3D12_STENCIL_OP lut[] = {
        D3D12_STENCIL_OP_KEEP,      // StencilOp::KEEP
        D3D12_STENCIL_OP_ZERO,      // StencilOp::ZERO
        D3D12_STENCIL_OP_REPLACE,   // StencilOp::REPLACE
        D3D12_STENCIL_OP_INVERT,    // StencilOp::INVERT
        D3D12_STENCIL_OP_INCR,      // StencilOp::INCREMENT_WRAP (map to INCR)
        D3D12_STENCIL_OP_DECR,      // StencilOp::DECREMENT_WRAP (map to DECR)
        D3D12_STENCIL_OP_INCR_SAT,  // StencilOp::INCREMENT_SATURATE
        D3D12_STENCIL_OP_DECR_SAT   // StencilOp::DECREMENT_SATURATE
    };

    const int idx = static_cast<int>(op);
    if (idx >= 0 && idx < static_cast<int>(std::size(lut)))
        return lut[idx];

    // Default fallback
    return D3D12_STENCIL_OP_KEEP;
}

static D3D12_DEPTH_STENCILOP_DESC make_op_desc(const StencilDesc& s)
{
    D3D12_DEPTH_STENCILOP_DESC d{};
    d.StencilFailOp      = to_d3d(s.stencilFailureOp);
    d.StencilPassOp      = to_d3d(s.depthStencilPassOp);
    d.StencilDepthFailOp = to_d3d(s.depthFailureOp);
    d.StencilFunc        = to_d3d(s.stencilCompareFunc);
    return d;
}
}  // namespace

DepthStencilStateImpl::DepthStencilStateImpl()
{
    _hash = tlx::hash_bytes(&_dsDesc, sizeof(_dsDesc));

    // Disabled state (depth test/write off, stencil off)
    _disableDesc.DepthEnable      = FALSE;
    _disableDesc.DepthWriteMask   = D3D12_DEPTH_WRITE_MASK_ZERO;
    _disableDesc.DepthFunc        = D3D12_COMPARISON_FUNC_ALWAYS;
    _disableDesc.StencilEnable    = FALSE;
    _disableDesc.StencilReadMask  = 0xFF;  // unused when disabled
    _disableDesc.StencilWriteMask = 0xFF;  // unused when disabled
    _disableDesc.FrontFace        = {D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP,
                                     D3D12_COMPARISON_FUNC_ALWAYS};
    _disableDesc.BackFace         = _disableDesc.FrontFace;

    _activeDesc = _disableDesc;
}

void DepthStencilStateImpl::update(const DepthStencilDesc& desc)
{
    DepthStencilState::update(desc);

    _hash = tlx::hash_bytes(&_dsDesc, sizeof(_dsDesc));

    if (!isEnabled())
    {
        _activeDesc = _disableDesc;
        return;
    }

    D3D12_DEPTH_STENCIL_DESC d{};
    const bool depthTest  = bitmask::any(desc.flags, DepthStencilFlags::DEPTH_TEST);
    const bool depthWrite = bitmask::any(desc.flags, DepthStencilFlags::DEPTH_WRITE);
    const bool stencil    = bitmask::any(desc.flags, DepthStencilFlags::STENCIL_TEST);

    d.DepthEnable    = depthTest ? TRUE : FALSE;
    d.DepthWriteMask = depthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    d.DepthFunc      = to_d3d(desc.depthCompareFunc);

    d.StencilEnable    = stencil ? TRUE : FALSE;
    d.StencilReadMask  = desc.frontFaceStencil.readMask;  // D3D12 has single read/write mask; we choose front's masks
    d.StencilWriteMask = desc.frontFaceStencil.writeMask;

    d.FrontFace = make_op_desc(desc.frontFaceStencil);
    d.BackFace  = make_op_desc(desc.backFaceStencil);

    _activeDesc = d;
}

}  // namespace ax::rhi::d3d12
