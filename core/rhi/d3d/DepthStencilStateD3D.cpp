#include "DepthStencilStateD3D.h"
#include <stdexcept>
#include <cstring>

namespace ax::rhi::d3d
{
namespace
{
static D3D11_COMPARISON_FUNC to_d3d(CompareFunction func)
{
    switch (func)
    {
    case CompareFunction::NEVER:
        return D3D11_COMPARISON_NEVER;
    case CompareFunction::LESS:
        return D3D11_COMPARISON_LESS;
    case CompareFunction::LESS_EQUAL:
        return D3D11_COMPARISON_LESS_EQUAL;
    case CompareFunction::GREATER:
        return D3D11_COMPARISON_GREATER;
    case CompareFunction::GREATER_EQUAL:
        return D3D11_COMPARISON_GREATER_EQUAL;
    case CompareFunction::EQUAL:
        return D3D11_COMPARISON_EQUAL;
    case CompareFunction::NOT_EQUAL:
        return D3D11_COMPARISON_NOT_EQUAL;
    case CompareFunction::ALWAYS:
        return D3D11_COMPARISON_ALWAYS;
    default:
        return D3D11_COMPARISON_LESS_EQUAL;
    }
}

static D3D11_STENCIL_OP to_d3d(StencilOperation op)
{
    switch (op)
    {
    case StencilOperation::KEEP:
        return D3D11_STENCIL_OP_KEEP;
    case StencilOperation::ZERO:
        return D3D11_STENCIL_OP_ZERO;
    case StencilOperation::REPLACE:
        return D3D11_STENCIL_OP_REPLACE;
    case StencilOperation::INVERT:
        return D3D11_STENCIL_OP_INVERT;
    case StencilOperation::INCREMENT_WRAP:
        return D3D11_STENCIL_OP_INCR;
    case StencilOperation::DECREMENT_WRAP:
        return D3D11_STENCIL_OP_DECR;
    default:
        return D3D11_STENCIL_OP_KEEP;
    }
}

static D3D11_DEPTH_STENCILOP_DESC make_op_desc(const StencilDescriptor& s)
{
    D3D11_DEPTH_STENCILOP_DESC d{};
    d.StencilFailOp      = to_d3d(s.stencilFailureOperation);
    d.StencilDepthFailOp = to_d3d(s.depthFailureOperation);
    d.StencilPassOp      = to_d3d(s.depthStencilPassOperation);
    d.StencilFunc        = to_d3d(s.stencilCompareFunction);
    return d;
}
}  // namespace

void DepthStencilStateImpl::update(const DepthStencilDescriptor& desc)
{
    if (_state && memcmp(&desc, &_dsDesc, sizeof(desc) == 0))
    {
        return;
    }

    DepthStencilState::update(desc);

    D3D11_DEPTH_STENCIL_DESC d{};

    d.DepthEnable    = bitmask::any(desc.flags, DepthStencilFlags::DEPTH_TEST) ? TRUE : FALSE;
    d.DepthWriteMask = bitmask::any(desc.flags, DepthStencilFlags::DEPTH_WRITE) ? D3D11_DEPTH_WRITE_MASK_ALL
                                                                                : D3D11_DEPTH_WRITE_MASK_ZERO;
    d.DepthFunc      = to_d3d(desc.depthCompareFunction);

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
        (std::memcmp(&desc.frontFaceStencil, &desc.backFaceStencil, sizeof(StencilDescriptor)) == 0);
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

        ID3D11DepthStencilState* pDSStateDisable = nullptr;
        _device->CreateDepthStencilState(&dsDesc, _disableState.GetAddressOf());
    }

    if (_disableState)
        ctx->OMSetDepthStencilState(_disableState.Get(), 0);
}
}  // namespace ax::rhi::d3d
