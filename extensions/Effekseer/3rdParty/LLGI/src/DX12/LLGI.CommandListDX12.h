
#pragma once

#include "../LLGI.CommandList.h"
#include "LLGI.BaseDX12.h"
#include "LLGI.DescriptorHeapDX12.h"
#include "LLGI.GraphicsDX12.h"
#include "LLGI.PipelineStateDX12.h"
#include "LLGI.RenderPassDX12.h"

namespace LLGI
{

struct PlatformContextDX12
{
	ID3D12GraphicsCommandList* commandList = nullptr;
};

class CommandListDX12 : public CommandList
{
private:
	static const int MaximumRenderTargetChange = 32;

	std::shared_ptr<DX12::DescriptorHeapAllocator> samplerDescriptorHeap_;
	std::shared_ptr<DX12::DescriptorHeapAllocator> cbDescriptorHeap_;
	std::shared_ptr<DX12::DescriptorHeapAllocator> computeDescriptorHeap_;
	std::shared_ptr<DX12::DescriptorHeapAllocator> rtDescriptorHeap_;
	std::shared_ptr<DX12::DescriptorHeapAllocator> dtDescriptorHeap_;

	std::shared_ptr<ID3D12GraphicsCommandList> commandList_;
	std::shared_ptr<ID3D12CommandAllocator> commandAllocator_;
	ID3D12Fence* fence_ = nullptr;
	HANDLE fenceEvent_ = nullptr;
	UINT64 fenceValue_ = 1;

	std::shared_ptr<GraphicsDX12> graphics_;
	std::shared_ptr<RenderPassDX12> renderPass_;

	ID3D12GraphicsCommandList* currentCommandList_ = nullptr;

	void BeginInternal();

public:
	CommandListDX12();
	~CommandListDX12() override;
	bool Initialize(GraphicsDX12* graphics, int32_t drawingCount);

	void Begin() override;
	void End() override;
	bool BeginWithPlatform(void* platformContextPtr) override;
	void EndWithPlatform() override;

	void BeginRenderPass(RenderPass* renderPass) override;
	void EndRenderPass() override;
	void Draw(int32_t primitiveCount, int32_t instanceCount) override;
	void CopyTexture(Texture* src, Texture* dst) override;
	void CopyTexture(
		Texture* src, Texture* dst, const Vec3I& srcPos, const Vec3I& dstPos, const Vec3I& size, int srcLayer, int dstLayer) override;

	void CopyBuffer(Buffer* src, Buffer* dst) override;

	void BeginComputePass() override;
	void EndComputePass() override;
	void Dispatch(int32_t groupX, int32_t groupY, int32_t groupZ, int32_t threadX, int32_t threadY, int32_t threadZ) override;

	void Clear(const Color8& color);

	void ClearDepth();

	ID3D12GraphicsCommandList* GetCommandList() const;

	ID3D12Fence* GetFence() const;
	UINT64 GetAndIncFenceValue();

	void WaitUntilCompleted() override;
};

} // namespace LLGI
