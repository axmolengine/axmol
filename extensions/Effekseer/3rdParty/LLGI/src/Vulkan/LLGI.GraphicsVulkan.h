#pragma once

#include "../LLGI.Graphics.h"
#include "LLGI.BaseVulkan.h"
#include "LLGI.RenderPassPipelineStateCacheVulkan.h"
#include "LLGI.RenderPassVulkan.h"
#include <functional>
#include <unordered_map>

namespace LLGI
{

class RenderPassVulkan;
class RenderPassPipelineStateVulkan;
class TextureVulkan;

class GraphicsVulkan : public Graphics
{
private:
	int32_t swapBufferCount_ = 0;

	vk::Device vkDevice_;
	vk::Queue vkQueue_;
	vk::CommandPool vkCmdPool_;
	vk::PhysicalDevice vkPysicalDevice_;

	std::function<void(vk::CommandBuffer, vk::Fence)> addCommand_;
	RenderPassPipelineStateCacheVulkan* renderPassPipelineStateCache_ = nullptr;
	ReferenceObject* owner_ = nullptr;

public:
	GraphicsVulkan(const vk::Device& device,
				   const vk::Queue& quque,
				   const vk::CommandPool& commandPool,
				   const vk::PhysicalDevice& pysicalDevice,
				   int32_t swapBufferCount,
				   std::function<void(vk::CommandBuffer, vk::Fence)> addCommand,
				   RenderPassPipelineStateCacheVulkan* renderPassPipelineStateCache = nullptr,
				   ReferenceObject* owner = nullptr);

	~GraphicsVulkan() override;

	void SetWindowSize(const Vec2I& windowSize) override;

	void Execute(CommandList* commandList) override;

	void WaitFinish() override;

	Buffer* CreateBuffer(BufferUsageType usage, int32_t size) override;
	Shader* CreateShader(DataStructure* data, int32_t count) override;
	PipelineState* CreatePiplineState() override;
	SingleFrameMemoryPool* CreateSingleFrameMemoryPool(int32_t constantBufferPoolSize, int32_t drawingCount) override;
	CommandList* CreateCommandList(SingleFrameMemoryPool* memoryPool) override;
	RenderPass* CreateRenderPass(Texture** textures, int32_t textureCount, Texture* depthTexture) override;

	RenderPass* CreateRenderPass(Texture* texture, Texture* resolvedTexture, Texture* depthTexture, Texture* resolvedDepthTexture) override;

	Texture* CreateTexture(const TextureParameter& parameter) override;
	Texture* CreateTexture(const TextureInitializationParameter& parameter) override;
	Texture* CreateRenderTexture(const RenderTextureInitializationParameter& parameter) override;
	Texture* CreateDepthTexture(const DepthTextureInitializationParameter& parameter) override;

	Texture* CreateTexture(uint64_t id) override;

	std::vector<uint8_t> CaptureRenderTarget(Texture* renderTarget) override;

	RenderPassPipelineState* CreateRenderPassPipelineState(RenderPass* renderPass) override;

	RenderPassPipelineState* CreateRenderPassPipelineState(const RenderPassPipelineStateKey& key) override;

	vk::PhysicalDevice GetPysicalDevice() const { return vkPysicalDevice_; }
	vk::Device GetDevice() const { return vkDevice_; }
	vk::CommandPool GetCommandPool() const { return vkCmdPool_; }
	vk::Queue GetQueue() const { return vkQueue_; }

	int32_t GetSwapBufferCount() const;
	uint32_t GetMemoryTypeIndex(uint32_t bits, const vk::MemoryPropertyFlags& properties);

	VkCommandBuffer BeginSingleTimeCommands();
	bool EndSingleTimeCommands(VkCommandBuffer commandBuffer);
};

} // namespace LLGI
