#pragma once

#include "../LLGI.Graphics.h"
#include "LLGI.BaseDX12.h"

namespace LLGI
{

namespace DX12
{
class DescriptorHeapAllocator;
}

class GraphicsDX12;
class TextureDX12;
class RenderPassPipelineStateDX12;
class RenderTargetDX12;
class CommandListDX12;

class RenderPassDX12 : public RenderPass
{
private:
	ID3D12Device* device_ = nullptr;

	std::vector<RenderTargetDX12> renderTargets_;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handleRTV_;
	D3D12_CPU_DESCRIPTOR_HANDLE handleDSV_;

	int32_t numRenderTarget_ = 0;

	std::shared_ptr<RenderPassPipelineStateDX12> renderPassPipelineState_;

public:
	RenderPassDX12(ID3D12Device* device);
	~RenderPassDX12() override;

	bool Initialize(TextureDX12** textures,
					int numTextures,
					TextureDX12* depthTexture,
					TextureDX12* resolvedRenderTexture,
					TextureDX12* resolvedDepthTexture);

	const D3D12_CPU_DESCRIPTOR_HANDLE* GetHandleRTV() const { return handleRTV_.data(); }
	const RenderTargetDX12* GetRenderTarget(int idx) const { return &renderTargets_[idx]; }
	int32_t GetCount() const { return numRenderTarget_; }

	bool ReinitializeRenderTargetViews(CommandListDX12* commandList,
									   std::shared_ptr<DX12::DescriptorHeapAllocator> rtDescriptorHeap,
									   std::shared_ptr<DX12::DescriptorHeapAllocator> dtDescriptorHeap);

	const D3D12_CPU_DESCRIPTOR_HANDLE* GetHandleDSV() const
	{
		if (GetHasDepthTexture())
		{
			return &handleDSV_;
		}

		return nullptr;
	}
};

class RenderTargetDX12
{
public:
	TextureDX12* texture_ = nullptr;
	ID3D12Resource* renderPass_ = nullptr;

	RenderTargetDX12() {}
};
} // namespace LLGI
