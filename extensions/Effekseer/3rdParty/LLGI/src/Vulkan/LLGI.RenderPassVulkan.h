#pragma once

#include "../LLGI.Graphics.h"
#include "../Utils/LLGI.FixedSizeVector.h"
#include "LLGI.BaseVulkan.h"
#include <functional>
#include <unordered_map>

namespace LLGI
{

class RenderPassVulkan;
class RenderPassPipelineStateVulkan;
class RenderPassPipelineStateCacheVulkan;
class TextureVulkan;

class RenderPassVulkan : public RenderPass
{
private:
	RenderPassPipelineStateCacheVulkan* renderPassPipelineStateCache_ = nullptr;
	vk::Device device_;
	ReferenceObject* owner_ = nullptr;

	std::shared_ptr<TextureVulkan> depthBufferPtr;
	bool isValid_ = false;

public:
	struct RenderTargetProperty
	{
		vk::Format format;
		vk::Image colorBuffer;
		std::shared_ptr<TextureVulkan> colorBufferPtr;
	};

	RenderPassPipelineStateVulkan* renderPassPipelineState = nullptr;

	vk::Framebuffer frameBuffer_;

	FixedSizeVector<RenderTargetProperty, RenderTargetMax> renderTargetProperties;

	vk::Image depthBuffer;

	RenderPassVulkan(RenderPassPipelineStateCacheVulkan* renderPassPipelineStateCache, vk::Device device, ReferenceObject* owner);
	~RenderPassVulkan() override;

	bool Initialize(const TextureVulkan** textures,
					int32_t textureCount,
					TextureVulkan* depthTexture,
					TextureVulkan* resolvedTexture,
					TextureVulkan* resolvedDepthTexture);

	Vec2I GetImageSize() const;

	virtual void SetIsColorCleared(bool isColorCleared) override;

	virtual void SetIsDepthCleared(bool isDepthCleared) override;

	bool GetIsValid() const { return isValid_; }

private:
	void ResetRenderPassPipelineState();
};

class RenderPassPipelineStateVulkan : public RenderPassPipelineState
{
private:
	vk::Device device_;
	ReferenceObject* owner_ = nullptr;

public:
	RenderPassPipelineStateVulkan(vk::Device device, ReferenceObject* owner);

	~RenderPassPipelineStateVulkan() override;

	vk::RenderPass renderPass_ = nullptr;
	int32_t RenderTargetCount = 0;
	FixedSizeVector<vk::ImageLayout, RenderTargetMax + 1> finalLayouts_;

	vk::RenderPass GetRenderPass() const;
};

} // namespace LLGI
