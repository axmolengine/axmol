#include "LLGI.RenderPassVulkan.h"
#include "LLGI.BaseVulkan.h"
#include "LLGI.CommandListVulkan.h"
#include "LLGI.GraphicsVulkan.h"
#include "LLGI.PipelineStateVulkan.h"
#include "LLGI.ShaderVulkan.h"
#include "LLGI.SingleFrameMemoryPoolVulkan.h"
#include "LLGI.TextureVulkan.h"

namespace LLGI
{

RenderPassVulkan::RenderPassVulkan(RenderPassPipelineStateCacheVulkan* renderPassPipelineStateCache,
								   vk::Device device,
								   ReferenceObject* owner)
	: renderPassPipelineStateCache_(renderPassPipelineStateCache), device_(device), owner_(owner), renderPassPipelineState(nullptr)
{
	SafeAddRef(renderPassPipelineStateCache_);
	SafeAddRef(owner_);
}

RenderPassVulkan::~RenderPassVulkan()
{
	if (frameBuffer_)
	{
		device_.destroyFramebuffer(frameBuffer_);
	}

	SafeRelease(renderPassPipelineState);
	SafeRelease(renderPassPipelineStateCache_);
	SafeRelease(owner_);
}

bool RenderPassVulkan::Initialize(const TextureVulkan** textures,
								  int32_t textureCount,
								  TextureVulkan* depthTexture,
								  TextureVulkan* resolvedTexture,
								  TextureVulkan* resolvedDepthTexture)
{
	if (textureCount == 0)
		return false;

	if (!assignRenderTextures((Texture**)(textures), textureCount))
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

	if (!getSize(screenSize_, reinterpret_cast<const Texture**>(textures), textureCount, depthTexture))
	{
		return false;
	}

	if (!sanitize())
	{
		return false;
	}

	renderTargetProperties.resize(textureCount);

	for (int32_t i = 0; i < textureCount; i++)
	{
		auto texture = const_cast<TextureVulkan*>(textures[i]);
		SafeAddRef(texture);
		renderTargetProperties.at(i).colorBufferPtr = CreateSharedPtr(texture);
	}

	for (int32_t i = 0; i < textureCount; i++)
	{
		renderTargetProperties.at(i).format = textures[i]->GetVulkanFormat();
	}

	FixedSizeVector<vk::ImageView, RenderTargetMax + 1> views;
	views.resize(textureCount);

	for (int32_t i = 0; i < textureCount; i++)
	{
		views.at(i) = textures[i]->GetView();
	}

	if (GetHasDepthTexture())
	{
		views.resize(views.size() + 1);
		views.at(views.size() - 1) = depthTexture->GetView();
	}

	if (auto resolvedTextureVulkan = static_cast<TextureVulkan*>(GetResolvedRenderTexture()))
	{
		views.resize(views.size() + 1);
		views.at(views.size() - 1) = resolvedTextureVulkan->GetView();
	}

	// if (auto resolvedTextureVulkan = static_cast<TextureVulkan*>(GetResolvedDepthTexture()))
	//{
	// Wait 1.2
	// views.resize(views.size() + 1);
	// views.at(views.size() - 1) = resolvedTexture->GetView();
	//}

	ResetRenderPassPipelineState();

	vk::FramebufferCreateInfo framebufferCreateInfo;
	framebufferCreateInfo.renderPass = renderPassPipelineState->GetRenderPass();
	framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(views.size());
	framebufferCreateInfo.pAttachments = views.data();
	framebufferCreateInfo.width = screenSize_.X;
	framebufferCreateInfo.height = screenSize_.Y;
	framebufferCreateInfo.layers = 1;

	frameBuffer_ = device_.createFramebuffer(framebufferCreateInfo);

	isValid_ = true;

	return true;
}

Vec2I RenderPassVulkan::GetImageSize() const { return screenSize_; }

void RenderPassVulkan::SetIsColorCleared(bool isColorCleared)
{
	RenderPass::SetIsColorCleared(isColorCleared);
	ResetRenderPassPipelineState();
}

void RenderPassVulkan::SetIsDepthCleared(bool isDepthCleared)
{
	RenderPass::SetIsDepthCleared(isDepthCleared);
	ResetRenderPassPipelineState();
}

void RenderPassVulkan::ResetRenderPassPipelineState()
{
	SafeRelease(renderPassPipelineState);

	FixedSizeVector<vk::Format, RenderTargetMax> formats;
	formats.resize(renderTargetProperties.size());
	for (size_t i = 0; i < renderTargetProperties.size(); i++)
	{
		formats.at(i) = renderTargetProperties.at(i).colorBufferPtr->GetVulkanFormat();
	}

	this->renderPassPipelineState = renderPassPipelineStateCache_->Create(GetKey());
}

RenderPassPipelineStateVulkan::RenderPassPipelineStateVulkan(vk::Device device, ReferenceObject* owner)
{
	device_ = device;
	owner_ = owner;
	SafeAddRef(owner);
}

RenderPassPipelineStateVulkan::~RenderPassPipelineStateVulkan()
{
	if (renderPass_)
	{
		device_.destroyRenderPass(renderPass_);
	}

	SafeRelease(owner_);
}

vk::RenderPass RenderPassPipelineStateVulkan::GetRenderPass() const { return renderPass_; }

} // namespace LLGI
