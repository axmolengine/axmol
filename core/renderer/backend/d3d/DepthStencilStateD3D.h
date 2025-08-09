#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "../DepthStencilState.h"

namespace ax::backend::d3d
{

class DepthStencilStateImpl : public DepthStencilState
{
public:
    explicit DepthStencilStateImpl(ID3D11Device* device) : _device(device) {}

    void update(const DepthStencilDescriptor& desc) override;

    void apply(ID3D11DeviceContext* ctx, UINT stencilRef = 0) const;

    ID3D11DepthStencilState* internalHandle() const { return _state.Get(); }

private:
    ID3D11Device* _device = nullptr;  // weak ref
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _state;
};
}  // namespace ax::backend::d3d
