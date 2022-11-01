#include "LLGI.BaseVulkan.h"
#include "LLGI.GraphicsVulkan.h"

namespace LLGI
{
const char* VulkanHelper::getResultName(VkResult result)
{
#define VK_RESULT_VALUE(v)                                                                                                                 \
	case v:                                                                                                                                \
		return #v
	switch (result)
	{
		VK_RESULT_VALUE(VK_SUCCESS);
		VK_RESULT_VALUE(VK_NOT_READY);
		VK_RESULT_VALUE(VK_TIMEOUT);
		VK_RESULT_VALUE(VK_EVENT_SET);
		VK_RESULT_VALUE(VK_EVENT_RESET);
		VK_RESULT_VALUE(VK_INCOMPLETE); // and VK_RESULT_END_RANGE
		VK_RESULT_VALUE(VK_ERROR_OUT_OF_HOST_MEMORY);
		VK_RESULT_VALUE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
		VK_RESULT_VALUE(VK_ERROR_INITIALIZATION_FAILED);
		VK_RESULT_VALUE(VK_ERROR_DEVICE_LOST);
		VK_RESULT_VALUE(VK_ERROR_MEMORY_MAP_FAILED);
		VK_RESULT_VALUE(VK_ERROR_LAYER_NOT_PRESENT);
		VK_RESULT_VALUE(VK_ERROR_EXTENSION_NOT_PRESENT);
		VK_RESULT_VALUE(VK_ERROR_FEATURE_NOT_PRESENT);
		VK_RESULT_VALUE(VK_ERROR_INCOMPATIBLE_DRIVER);
		VK_RESULT_VALUE(VK_ERROR_TOO_MANY_OBJECTS);
		VK_RESULT_VALUE(VK_ERROR_FORMAT_NOT_SUPPORTED);
		VK_RESULT_VALUE(VK_ERROR_FRAGMENTED_POOL);		   // and VK_RESULT_BEGIN_RANGE
		VK_RESULT_VALUE(VK_ERROR_OUT_OF_POOL_MEMORY);	   // and VK_ERROR_OUT_OF_POOL_MEMORY_KHR
		VK_RESULT_VALUE(VK_ERROR_INVALID_EXTERNAL_HANDLE); // and VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR
		VK_RESULT_VALUE(VK_ERROR_SURFACE_LOST_KHR);
		VK_RESULT_VALUE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
		VK_RESULT_VALUE(VK_SUBOPTIMAL_KHR);
		VK_RESULT_VALUE(VK_ERROR_OUT_OF_DATE_KHR);
		VK_RESULT_VALUE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
		VK_RESULT_VALUE(VK_ERROR_VALIDATION_FAILED_EXT);
		VK_RESULT_VALUE(VK_ERROR_INVALID_SHADER_NV);
#if VK_HEADER_VERSION >= 74
		VK_RESULT_VALUE(VK_ERROR_FRAGMENTATION_EXT);
#endif
		VK_RESULT_VALUE(VK_ERROR_NOT_PERMITTED_EXT);
#if VK_HEADER_VERSION < 141
		VK_RESULT_VALUE(VK_RESULT_RANGE_SIZE);
#endif
		VK_RESULT_VALUE(VK_RESULT_MAX_ENUM);
	default:
		return "<Unkonwn VkResult>";
	}
#undef VK_RESULT_VALUE
}

struct FormatConversionItem
{
	TextureFormatType format;
	VkFormat vulkanFormat;
};

static FormatConversionItem s_formatConversionTable[] = {
	{TextureFormatType::R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM},
	{TextureFormatType::B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM},
	{TextureFormatType::R8_UNORM, VK_FORMAT_R8_UNORM},
	{TextureFormatType::R16G16_FLOAT, VK_FORMAT_R16G16_SFLOAT},
	{TextureFormatType::R16G16B16A16_FLOAT, VK_FORMAT_R16G16B16A16_SFLOAT},
	{TextureFormatType::R32G32B32A32_FLOAT, VK_FORMAT_R32G32B32A32_SFLOAT},
	{TextureFormatType::BC1, VK_FORMAT_BC1_RGBA_UNORM_BLOCK},
	{TextureFormatType::BC2, VK_FORMAT_BC2_UNORM_BLOCK},
	{TextureFormatType::BC3, VK_FORMAT_BC3_UNORM_BLOCK},
	{TextureFormatType::R8G8B8A8_UNORM_SRGB, VK_FORMAT_R8G8B8A8_SRGB},
	{TextureFormatType::B8G8R8A8_UNORM_SRGB, VK_FORMAT_B8G8R8A8_SRGB},
	{TextureFormatType::BC1_SRGB, VK_FORMAT_BC1_RGBA_SRGB_BLOCK},
	{TextureFormatType::BC2_SRGB, VK_FORMAT_BC2_SRGB_BLOCK},
	{TextureFormatType::BC3_SRGB, VK_FORMAT_BC3_SRGB_BLOCK},
	{TextureFormatType::D32, VK_FORMAT_D32_SFLOAT},
	{TextureFormatType::D24S8, VK_FORMAT_D24_UNORM_S8_UINT},
	{TextureFormatType::D32S8, VK_FORMAT_D32_SFLOAT_S8_UINT},
	{TextureFormatType::Unknown, VK_FORMAT_UNDEFINED},
};

VkFormat VulkanHelper::TextureFormatToVkFormat(TextureFormatType format)
{
	for (size_t i = 0; i < sizeof(s_formatConversionTable); i++)
	{
		if (s_formatConversionTable[i].format == format)
			return s_formatConversionTable[i].vulkanFormat;
	}

	auto format_str = to_string(format);

	Log(LogType::Error, format_str + " : LLGI cannot use this format.");
	return VK_FORMAT_UNDEFINED;
}

TextureFormatType VulkanHelper::VkFormatToTextureFormat(VkFormat format)
{
	for (size_t i = 0; i < sizeof(s_formatConversionTable); i++)
	{
		if (s_formatConversionTable[i].vulkanFormat == format)
			return s_formatConversionTable[i].format;
	}

	auto format_str = vk::to_string(static_cast<vk::Format>(format));

	Log(LogType::Error, format_str + " : LLGI cannot use this format.");
	return TextureFormatType::Unknown;
}

InternalBuffer::InternalBuffer(GraphicsVulkan* graphics)
{
	SafeAddRef(graphics);
	graphics_ = CreateSharedPtr(graphics);
}

InternalBuffer::~InternalBuffer()
{
	if (buffer_)
	{
		if (!isExternalResource_)
		{
			graphics_->GetDevice().destroyBuffer(buffer_);
			graphics_->GetDevice().freeMemory(devMem_);
		}
		buffer_ = nullptr;
	}
}

void InternalBuffer::Attach(vk::Buffer buffer, vk::DeviceMemory devMem, bool isExternalResource)
{
	buffer_ = buffer;
	devMem_ = devMem;
	isExternalResource_ = isExternalResource;
}

VulkanBuffer::VulkanBuffer() : graphics_(nullptr), nativeBuffer_(VK_NULL_HANDLE), nativeBufferMemory_(VK_NULL_HANDLE), size_(0) {}

bool VulkanBuffer::Initialize(GraphicsVulkan* graphics, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
	assert(!graphics_);
	assert(graphics);

	graphics_ = graphics;
	size_ = size;

	auto device = static_cast<VkDevice>(graphics_->GetDevice());

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	LLGI_VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &nativeBuffer_));

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, nativeBuffer_, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = graphics_->GetMemoryTypeIndex(memRequirements.memoryTypeBits, (vk::MemoryPropertyFlags)properties);

	LLGI_VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &nativeBufferMemory_));
	LLGI_VK_CHECK(vkBindBufferMemory(device, nativeBuffer_, nativeBufferMemory_, 0));

	return true;
}

void VulkanBuffer::Dispose()
{
	auto device = static_cast<VkDevice>(graphics_->GetDevice());

	if (nativeBufferMemory_)
	{
		vkFreeMemory(device, nativeBufferMemory_, nullptr);
		nativeBufferMemory_ = VK_NULL_HANDLE;
	}

	if (nativeBuffer_)
	{
		vkDestroyBuffer(device, nativeBuffer_, nullptr);
		nativeBuffer_ = VK_NULL_HANDLE;
	}

	graphics_ = nullptr;
}

static vk::PipelineStageFlags GetStageFlag(vk::ImageLayout layout)
{

	if (layout == vk::ImageLayout::ePreinitialized)
	{
		return vk::PipelineStageFlagBits::eHost;
	}
	else if (layout == vk::ImageLayout::eTransferDstOptimal || layout == vk::ImageLayout::eTransferSrcOptimal)
	{
		return vk::PipelineStageFlagBits::eTransfer;
	}
	else if (layout == vk::ImageLayout::eColorAttachmentOptimal)
	{
		return vk::PipelineStageFlagBits::eColorAttachmentOutput;
	}
	else if (layout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		return vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader;
	}

	return vk::PipelineStageFlagBits::eTopOfPipe;
}

void SetImageLayout(vk::CommandBuffer cmdbuffer,
					vk::Image image,
					vk::ImageLayout oldImageLayout,
					vk::ImageLayout newImageLayout,
					vk::ImageSubresourceRange subresourceRange)
{
	vk::ImageMemoryBarrier imageMemoryBarrier;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = subresourceRange;

	// current layout
	if (oldImageLayout == vk::ImageLayout::ePreinitialized)
	{
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
	}
	else if (oldImageLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	}
	else if (oldImageLayout == vk::ImageLayout::eColorAttachmentOptimal)
	{
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
	}
	else if (oldImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	}
	else if (oldImageLayout == vk::ImageLayout::eTransferSrcOptimal)
	{
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
	}
	else if (oldImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
	}
	else if (oldImageLayout == vk::ImageLayout::ePresentSrcKHR)
	{
		imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
	}

	// next layout

	if (newImageLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
	}
	else if (newImageLayout == vk::ImageLayout::eTransferSrcOptimal)
	{
		imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead; // | imageMemoryBarrier.srcAccessMask;
	}
	else if (newImageLayout == vk::ImageLayout::eColorAttachmentOptimal)
	{
		imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
	}
	else if (newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	}
	else if (newImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
	}
	else if (newImageLayout == vk::ImageLayout::ePresentSrcKHR)
	{
		imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
	}

	if (imageMemoryBarrier.dstAccessMask == vk::AccessFlagBits::eTransferWrite ||
		imageMemoryBarrier.dstAccessMask == vk::AccessFlagBits::eTransferRead ||
		imageMemoryBarrier.dstAccessMask == vk::AccessFlagBits::eColorAttachmentWrite ||
		imageMemoryBarrier.dstAccessMask == vk::AccessFlagBits::eShaderRead ||
		imageMemoryBarrier.srcAccessMask == vk::AccessFlagBits::eColorAttachmentWrite ||
		imageMemoryBarrier.srcAccessMask == vk::AccessFlagBits::eTransferRead)
	{
		cmdbuffer.pipelineBarrier(
			GetStageFlag(oldImageLayout), GetStageFlag(newImageLayout), vk::DependencyFlags(), nullptr, nullptr, imageMemoryBarrier);
	}
	else if (imageMemoryBarrier.dstAccessMask == vk::AccessFlagBits::eDepthStencilAttachmentWrite)
	{
		cmdbuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
								  vk::PipelineStageFlagBits::eEarlyFragmentTests,
								  vk::DependencyFlags(),
								  nullptr,
								  nullptr,
								  imageMemoryBarrier);
	}
	else
	{
		// Put barrier inside setup command buffer
		cmdbuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,
								  vk::PipelineStageFlagBits::eTopOfPipe,
								  vk::DependencyFlags(),
								  nullptr,
								  nullptr,
								  imageMemoryBarrier);
	}
}

uint32_t GetMemoryTypeIndex(vk::PhysicalDevice& phDevice, uint32_t bits, const vk::MemoryPropertyFlags& properties)
{
	vk::PhysicalDeviceMemoryProperties deviceMemoryProperties = phDevice.getMemoryProperties();
	for (uint32_t i = 0; i < 32; i++)
	{
		if ((bits & 1) == 1)
		{
			if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		bits >>= 1;
	}

	assert(false); //"NOT found memory type.\n"
	return 0xffffffff;
}

bool CreateDepthBuffer(vk::Image& image,
					   vk::ImageView view,
					   vk::DeviceMemory devMem,
					   vk::Device& device,
					   vk::PhysicalDevice& phDevice,
					   const Vec2I& size,
					   vk::Format format,
					   vk::CommandBuffer* commandBuffer)
{
	// check a format whether specified format is supported
	vk::Format depthFormat = format;
	vk::FormatProperties formatProps = phDevice.getFormatProperties(depthFormat);
	if (!(formatProps.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment))
	{
		return false;
	}

	vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

	// create an image
	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.extent = vk::Extent3D(size.X, size.Y, 1);
	imageCreateInfo.format = depthFormat;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	image = device.createImage(imageCreateInfo);

	// allocate memory
	vk::MemoryRequirements memReqs = device.getImageMemoryRequirements(image);
	vk::MemoryAllocateInfo memAlloc;
	memAlloc.allocationSize = memReqs.size;
	memAlloc.memoryTypeIndex = GetMemoryTypeIndex(phDevice, memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	devMem = device.allocateMemory(memAlloc);
	device.bindImageMemory(image, devMem, 0);

	// create view
	vk::ImageViewCreateInfo viewCreateInfo;
	viewCreateInfo.viewType = vk::ImageViewType::e2D;
	viewCreateInfo.format = depthFormat;
	viewCreateInfo.components = {vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA};
	viewCreateInfo.subresourceRange.aspectMask = aspect;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.layerCount = 1;
	viewCreateInfo.image = image;
	view = device.createImageView(viewCreateInfo);

	// change layout(nt needed?)

	if (commandBuffer != nullptr)
	{
		vk::ImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = aspect;
		subresourceRange.levelCount = 1;
		subresourceRange.layerCount = 1;
		// SetImageBarrior(
		SetImageLayout(
			*commandBuffer, image, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal, subresourceRange);
	}

	return true;
}

} // namespace LLGI
