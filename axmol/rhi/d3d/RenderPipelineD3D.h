#pragma once

#include "../RenderPipeline.h"
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace ax::rhi::d3d
{
class RenderPipelineImpl : public RenderPipeline
{
public:
    RenderPipelineImpl(ID3D11Device* device, ID3D11DeviceContext* context) : _device(device), _context(context) {}
    void update(const RenderTarget*, const PipelineDescriptor& desc) override;

    private:
    ID3D11Device* _device         = nullptr;
    ID3D11DeviceContext* _context = nullptr;

    tsl::robin_map<uint64_t, ComPtr<ID3D11BlendState>> _blendCache;
};
}  // namespace ax::rhi::d3d
