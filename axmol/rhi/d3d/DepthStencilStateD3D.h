#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "axmol/rhi/DepthStencilState.h"

namespace ax::rhi::d3d
{
using namespace Microsoft::WRL;
class DepthStencilStateImpl : public DepthStencilState
{
public:
    explicit DepthStencilStateImpl(ID3D11Device* device) : _device(device) {}

    void update(const DepthStencilDescriptor& desc) override;

    void apply(ID3D11DeviceContext* ctx, UINT stencilRef = 0) const;

    void reset(ID3D11DeviceContext* ctx);

    ID3D11DepthStencilState* internalHandle() const { return _state.Get(); }

private:
    ID3D11Device* _device = nullptr;  // weak ref
    ComPtr<ID3D11DepthStencilState> _state;

    ComPtr<ID3D11DepthStencilState> _disableState;
};
}  // namespace ax::rhi::d3d
