#include "LLGI.GraphicsVulkan.h"
#include "LLGI.BaseVulkan.h"
#include "LLGI.BufferVulkan.h"
#include "LLGI.CommandListVulkan.h"
#include "LLGI.PipelineStateVulkan.h"
#include "LLGI.ShaderVulkan.h"
#include "LLGI.SingleFrameMemoryPoolVulkan.h"
#include "LLGI.TextureVulkan.h"

namespace LLGI
{

GraphicsVulkan::GraphicsVulkan(const vk::Device& device,
							   const vk::Queue& quque,
							   const vk::CommandPool& commandPool,
							   const vk::PhysicalDevice& pysicalDevice,
							   int32_t swapBufferCount,
							   std::function<void(vk::CommandBuffer, vk::Fence)> addCommand,
							   RenderPassPipelineStateCacheVulkan* renderPassPipelineStateCache,
							   ReferenceObject* owner)
	: vkDevice_(device)
	, vkQueue_(quque)
	, vkCmdPool_(commandPool)
	, vkPysicalDevice_(pysicalDevice)
	, addCommand_(addCommand)
	, renderPassPipelineStateCache_(renderPassPipelineStateCache)
	, owner_(owner)
{
	SafeAddRef(owner_);

	swapBufferCount_ = swapBufferCount;

	SafeAddRef(renderPassPipelineStateCache_);
	if (renderPassPipelineStateCache_ == nullptr)
	{
		renderPassPipelineStateCache_ = new RenderPassPipelineStateCacheVulkan(device, nullptr);
	}
}

GraphicsVulkan::~GraphicsVulkan()
{
	SafeRelease(renderPassPipelineStateCache_);

	SafeRelease(owner_);
}

void GraphicsVulkan::SetWindowSize(const Vec2I& windowSize) { throw "Not inplemented"; }

void GraphicsVulkan::Execute(CommandList* commandList)
{
	auto commandList_ = static_cast<CommandListVulkan*>(commandList);
	auto cmdBuf = commandList_->GetCommandBuffer();
	addCommand_(cmdBuf, commandList_->GetFence());
}

void GraphicsVulkan::WaitFinish() { vkQueue_.waitIdle(); }

Buffer* GraphicsVulkan::CreateBuffer(BufferUsageType usage, int32_t size)
{
	auto obj = new BufferVulkan();
	if (!obj->Initialize(this, usage, size))
	{
		SafeRelease(obj);
		return nullptr;
	}

	return obj;
}

Shader* GraphicsVulkan::CreateShader(DataStructure* data, int32_t count)
{
	auto obj = new ShaderVulkan();
	if (!obj->Initialize(this, data, count))
	{
		SafeRelease(obj);
		return nullptr;
	}
	return obj;
}

PipelineState* GraphicsVulkan::CreatePiplineState()
{

	auto pipelineState = new PipelineStateVulkan();

	if (pipelineState->Initialize(this))
	{
		return pipelineState;
	}

	SafeRelease(pipelineState);
	return nullptr;
}

SingleFrameMemoryPool* GraphicsVulkan::CreateSingleFrameMemoryPool(int32_t constantBufferPoolSize, int32_t drawingCount)
{
	return new SingleFrameMemoryPoolVulkan(this, true, swapBufferCount_, constantBufferPoolSize, drawingCount);
}

CommandList* GraphicsVulkan::CreateCommandList(SingleFrameMemoryPool* memoryPool)
{
	auto mp = static_cast<SingleFrameMemoryPoolVulkan*>(memoryPool);

	auto commandList = new CommandListVulkan();
	if (commandList->Initialize(this, mp->GetDrawingCount()))
	{
		return commandList;
	}
	SafeRelease(commandList);
	return nullptr;
}

RenderPass* GraphicsVulkan::CreateRenderPass(Texture** textures, int32_t textureCount, Texture* depthTexture)
{
	assert(textures != nullptr);
	if (textures == nullptr)
		return nullptr;

	for (int32_t i = 0; i < textureCount; i++)
	{
		assert(textures[i] != nullptr);
		if (textures[i] == nullptr)
			return nullptr;
	}

	auto dt = static_cast<TextureVulkan*>(depthTexture);

	auto renderPass = new RenderPassVulkan(renderPassPipelineStateCache_, GetDevice(), this);
	if (!renderPass->Initialize((const TextureVulkan**)textures, textureCount, dt, nullptr, nullptr))
	{
		SafeRelease(renderPass);
	}

	return renderPass;
}

RenderPass*
GraphicsVulkan::CreateRenderPass(Texture* texture, Texture* resolvedTexture, Texture* depthTexture, Texture* resolvedDepthTexture)
{
	if (texture == nullptr)
		return nullptr;

	auto dt = static_cast<const TextureVulkan*>(depthTexture);
	auto rt = static_cast<const TextureVulkan*>(resolvedTexture);
	auto rdt = static_cast<const TextureVulkan*>(resolvedDepthTexture);

	auto renderPass = new RenderPassVulkan(renderPassPipelineStateCache_, GetDevice(), this);
	if (!renderPass->Initialize((const TextureVulkan**)(&texture), 1, (TextureVulkan*)dt, (TextureVulkan*)rt, (TextureVulkan*)rdt))
	{
		SafeRelease(renderPass);
	}

	return renderPass;
}

Texture* GraphicsVulkan::CreateTexture(uint64_t id)
{
	auto info = reinterpret_cast<VulkanImageInfo*>(id);
	auto obj = new TextureVulkan();

	if (!obj->InitializeAsExternal(GetDevice(), *info, this))
	{
		SafeRelease(obj);
		return nullptr;
	}

	return obj;
}

Texture* GraphicsVulkan::CreateTexture(const TextureParameter& parameter)
{
	auto obj = new TextureVulkan();
	if (!obj->Initialize(this, this->GetDevice(), this->GetPysicalDevice(), this, parameter))
	{
		SafeRelease(obj);
		return nullptr;
	}
	return obj;
}

Texture* GraphicsVulkan::CreateTexture(const TextureInitializationParameter& parameter)
{
	TextureParameter param;
	param.Dimension = 2;
	param.Format = parameter.Format;
	param.MipLevelCount = parameter.MipMapCount;
	param.SampleCount = 1;
	param.Size = {parameter.Size.X, parameter.Size.Y, 1};
	return CreateTexture(param);
}

Texture* GraphicsVulkan::CreateRenderTexture(const RenderTextureInitializationParameter& parameter)
{
	TextureParameter param;
	param.Dimension = 2;
	param.Format = parameter.Format;
	param.MipLevelCount = 1;
	param.SampleCount = parameter.SamplingCount;
	param.Size = {parameter.Size.X, parameter.Size.Y, 1};
	param.Usage = TextureUsageType::RenderTarget;
	return CreateTexture(param);
}

Texture* GraphicsVulkan::CreateDepthTexture(const DepthTextureInitializationParameter& parameter)
{
	auto format = TextureFormatType::D32;
	if (parameter.Mode == DepthTextureMode::DepthStencil)
	{
		format = TextureFormatType::D24S8;
	}

	TextureParameter param;
	param.Dimension = 2;
	param.Format = format;
	param.MipLevelCount = 1;
	param.SampleCount = parameter.SamplingCount;
	param.Size = {parameter.Size.X, parameter.Size.Y, 1};
	return CreateTexture(param);
}

std::vector<uint8_t> GraphicsVulkan::CaptureRenderTarget(Texture* renderTarget)
{
	if (!renderTarget)
	{
		return std::vector<uint8_t>();
	}

	std::vector<uint8_t> result;
	VkDevice device = static_cast<VkDevice>(GetDevice());
	vkDeviceWaitIdle(device);

	auto texture = static_cast<TextureVulkan*>(renderTarget);
	auto width = texture->GetSizeAs2D().X;
	auto height = texture->GetSizeAs2D().Y;
	auto size = texture->GetMemorySize();
	auto image = static_cast<VkImage>(texture->GetImage());

	VulkanBuffer destBuffer;
	if (!destBuffer.Initialize(
			this, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
	{
		goto Exit;
	}

	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
		vk::CommandBuffer commandBufferCpp = static_cast<vk::CommandBuffer>(commandBuffer);
		auto oldLayout = texture->GetImageLayouts()[0];
		texture->ResourceBarrior(commandBufferCpp, vk::ImageLayout::eTransferSrcOptimal);

		/*
		// Swapchain image (VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) -> copy source (VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			VkImageMemoryBarrier imageMemoryBarrier = {};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.pNext = nullptr;
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			imageMemoryBarrier.image = image;
			imageMemoryBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
			vkCmdPipelineBarrier(commandBuffer,
								 VK_PIPELINE_STAGE_TRANSFER_BIT,
								 VK_PIPELINE_STAGE_TRANSFER_BIT,
								 0,
								 0,
								 nullptr,
								 0,
								 nullptr,
								 1,
								 &imageMemoryBarrier);
		}
		*/

		// Copy to destBuffer
		{
			VkBufferImageCopy region = {};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;
			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageOffset = {0, 0, 0};
			region.imageExtent = {(uint32_t)width, (uint32_t)height, 1};
			vkCmdCopyImageToBuffer(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, destBuffer.GetNativeBuffer(), 1, &region);
		}

		// Undo layout
		texture->ResourceBarrior(commandBufferCpp, oldLayout);
		/*
		{
			VkImageMemoryBarrier imageMemoryBarrier = {};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.pNext = nullptr;
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			imageMemoryBarrier.image = image;
			imageMemoryBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
			vkCmdPipelineBarrier(commandBuffer,
								 VK_PIPELINE_STAGE_TRANSFER_BIT,
								 VK_PIPELINE_STAGE_TRANSFER_BIT,
								 0,
								 0,
								 nullptr,
								 0,
								 nullptr,
								 1,
								 &imageMemoryBarrier);
		}
		*/

		// Submit and Wait
		if (!EndSingleTimeCommands(commandBuffer))
		{
			goto Exit;
		}
	}

	// Blit
	{
		void* rawData = nullptr;
		vkMapMemory(device, destBuffer.GetNativeBufferMemory(), 0, destBuffer.GetSize(), 0, &rawData);
		result.resize(static_cast<size_t>(destBuffer.GetSize()));
		memcpy(result.data(), rawData, result.size());
		vkUnmapMemory(device, destBuffer.GetNativeBufferMemory());
	}

Exit:
	destBuffer.Dispose();
	return result;
}

RenderPassPipelineState* GraphicsVulkan::CreateRenderPassPipelineState(RenderPass* renderPass)
{
	assert(renderPass != nullptr);
	auto rpvk = static_cast<RenderPassVulkan*>(renderPass);

	auto ret = rpvk->renderPassPipelineState;
	SafeAddRef(ret);
	return ret;
}

RenderPassPipelineState* GraphicsVulkan::CreateRenderPassPipelineState(const RenderPassPipelineStateKey& key)
{
	return renderPassPipelineStateCache_->Create(key);
}

int32_t GraphicsVulkan::GetSwapBufferCount() const { return swapBufferCount_; }

uint32_t GraphicsVulkan::GetMemoryTypeIndex(uint32_t bits, const vk::MemoryPropertyFlags& properties)
{
	return LLGI::GetMemoryTypeIndex(vkPysicalDevice_, bits, properties);
}

VkCommandBuffer GraphicsVulkan::BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = static_cast<VkCommandPool>(GetCommandPool());
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(static_cast<VkDevice>(GetDevice()), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

bool GraphicsVulkan::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	LLGI_VK_CHECK(vkQueueSubmit(static_cast<VkQueue>(vkQueue_), 1, &submitInfo, VK_NULL_HANDLE));
	LLGI_VK_CHECK(vkQueueWaitIdle(static_cast<VkQueue>(vkQueue_)));

	vkFreeCommandBuffers(static_cast<VkDevice>(GetDevice()), static_cast<VkCommandPool>(GetCommandPool()), 1, &commandBuffer);

	return true;
}

} // namespace LLGI
