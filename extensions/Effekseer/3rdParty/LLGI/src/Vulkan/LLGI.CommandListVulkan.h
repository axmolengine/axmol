#pragma once

#include "../LLGI.CommandList.h"
#include "LLGI.BaseVulkan.h"

namespace LLGI
{

class DescriptorPoolVulkan
{
private:
	std::shared_ptr<GraphicsVulkan> graphics_;
	vk::DescriptorPool descriptorPool_ = nullptr;
	int32_t offset = 0;
	std::vector<std::vector<vk::DescriptorSet>> cache;
	int32_t slotSizeMax_;
	std::vector<vk::DescriptorSet> dummySet_;

	vk::DescriptorPool computeDescriptorPool_ = nullptr;
	int32_t computeOffset = 0;
	std::vector<std::vector<vk::DescriptorSet>> computeCache;

public:
	DescriptorPoolVulkan(std::shared_ptr<GraphicsVulkan> graphics, int32_t size, int stage);
	virtual ~DescriptorPoolVulkan();
	const std::vector<vk::DescriptorSet>& Get(PipelineStateVulkan* pip);
	const std::vector<vk::DescriptorSet>& GetCompute(PipelineStateVulkan* pip);
	void Reset();
};

struct PlatformContextVulkan
{
	VkCommandBuffer commandBuffer;
};

class CommandListVulkan : public CommandList
{
private:
	std::shared_ptr<GraphicsVulkan> graphics_;
	vk::CommandBuffer currentCommandBuffer_;
	std::vector<vk::CommandBuffer> commandBuffers_;
	std::vector<std::shared_ptr<DescriptorPoolVulkan>> descriptorPools;
	int32_t currentSwapBufferIndex_;
	std::vector<vk::Fence> fences_;
	vk::Sampler samplers_[2][2];
	bool isInValidRenderPass_ = false;

public:
	CommandListVulkan();
	~CommandListVulkan() override;

	bool Initialize(GraphicsVulkan* graphics, int32_t drawingCount);

	void Begin() override;
	void End() override;

	bool BeginWithPlatform(void* platformContextPtr) override;
	void EndWithPlatform() override;

	bool BeginRenderPassWithPlatformPtr(void* platformPtr) override;

	bool EndRenderPassWithPlatformPtr() override;

	void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height) override;
	void Draw(int32_t primitiveCount, int32_t instanceCount) override;
	void CopyTexture(Texture* src, Texture* dst) override;
	void CopyTexture(
		Texture* src, Texture* dst, const Vec3I& srcPos, const Vec3I& dstPos, const Vec3I& size, int srcLayer, int dstLayer) override;

	void GenerateMipMap(Texture* src) override;

	void CopyBuffer(Buffer* src, Buffer* dst) override;

	void BeginRenderPass(RenderPass* renderPass) override;
	void EndRenderPass() override;
	vk::CommandBuffer GetCommandBuffer() const;
	vk::Fence GetFence() const;

	void BeginComputePass() override;
	void EndComputePass() override;
	void Dispatch(int32_t groupX, int32_t groupY, int32_t groupZ, int32_t threadX, int32_t threadY, int32_t threadZ) override;

	void WaitUntilCompleted() override;
};

} // namespace LLGI
