#include "LLGI.RenderPassDX12.h"
#include "LLGI.CommandListDX12.h"
#include "LLGI.DescriptorHeapDX12.h"
#include "LLGI.GraphicsDX12.h"
#include "LLGI.RenderPassPipelineStateDX12.h"
#include "LLGI.TextureDX12.h"

namespace LLGI
{

RenderPassDX12::RenderPassDX12(ID3D12Device* device) : device_(device) { SafeAddRef(device_); }

RenderPassDX12 ::~RenderPassDX12()
{
	for (int32_t i = 0; i < numRenderTarget_; i++)
	{
		if (renderTargets_[i].texture_ != nullptr)
			SafeRelease(renderTargets_[i].texture_);
	}
	renderTargets_.clear();

	SafeRelease(device_);
}

bool RenderPassDX12::Initialize(
	TextureDX12** textures, int numTextures, TextureDX12* depthTexture, TextureDX12* resolvedTexture, TextureDX12* resolvedDepthTexture)
{
	if (textures[0]->Get() == nullptr)
		return false;

	if (!assignRenderTextures((Texture**)textures, numTextures))
	{
		return false;
	}

	if (!assignDepthTexture(depthTexture))
	{
		return false;
	}

	if (!assignResolvedRenderTexture(resolvedTexture))
	{
		return false;
	}

	if (!assignResolvedDepthTexture(resolvedDepthTexture))
	{
		return false;
	}

	if (!sanitize())
	{
		return false;
	}

	if (!getSize(screenSize_, (const Texture**)textures, numTextures, depthTexture))
	{
		return false;
	}

	renderTargets_.resize(numTextures);
	numRenderTarget_ = numTextures;

	for (int32_t i = 0; i < numTextures; i++)
	{
		renderTargets_[i].texture_ = textures[i];
		renderTargets_[i].renderPass_ = textures[i]->Get();
		SafeAddRef(renderTargets_[i].texture_);
	}

	return true;
}

bool RenderPassDX12::ReinitializeRenderTargetViews(CommandListDX12* commandList,
												   std::shared_ptr<DX12::DescriptorHeapAllocator> rtDescriptorHeap,
												   std::shared_ptr<DX12::DescriptorHeapAllocator> dtDescriptorHeap)
{
	if (numRenderTarget_ == 0)
		return false;

	ID3D12DescriptorHeap* heapRTV = nullptr;
	ID3D12DescriptorHeap* heapDSV = nullptr;

	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 32> cpuDescriptorHandleRTV;
	std::array<D3D12_GPU_DESCRIPTOR_HANDLE, 32> gpuDescriptorHandleRTV;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 32> cpuDescriptorHandleDSV;
	std::array<D3D12_GPU_DESCRIPTOR_HANDLE, 32> gpuDescriptorHandleDSV;

	if (!rtDescriptorHeap->Allocate(heapRTV, cpuDescriptorHandleRTV, gpuDescriptorHandleRTV, numRenderTarget_))
	{
		return nullptr;
	}

	if (GetHasDepthTexture())
	{
		if (!dtDescriptorHeap->Allocate(heapDSV, cpuDescriptorHandleDSV, gpuDescriptorHandleDSV, 1))
		{
			return nullptr;
		}
	}

	handleRTV_.resize(numRenderTarget_);

	for (int i = 0; i < numRenderTarget_; i++)
	{
		D3D12_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format = renderTargets_[i].texture_->GetDXGIFormat();

		if (GetRenderTexture(0)->GetSamplingCount() > 1)
		{
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		}
		auto cpuHandle = cpuDescriptorHandleRTV[i];
		device_->CreateRenderTargetView(renderTargets_[i].renderPass_, &desc, cpuHandle);
		handleRTV_[i] = cpuHandle;

		// memory barrior to make a rendertarget
		renderTargets_[i].texture_->ResourceBarrior(commandList->GetCommandList(), D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	if (GetHasDepthTexture())
	{
		auto depthTexture = static_cast<TextureDX12*>(GetDepthTexture());

		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = depthTexture->GetDXGIFormat();

		if (depthTexture->GetSamplingCount() > 1)
		{
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		}

		auto cpuHandle = cpuDescriptorHandleDSV[0];
		device_->CreateDepthStencilView(depthTexture->Get(), &desc, cpuHandle);
		handleDSV_ = cpuHandle;

		depthTexture->ResourceBarrior(commandList->GetCommandList(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}

	return true;
}

} // namespace LLGI
