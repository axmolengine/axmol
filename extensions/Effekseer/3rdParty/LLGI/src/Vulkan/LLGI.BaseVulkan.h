
#pragma once

#include "../LLGI.Base.h"
#include <sstream>
#include <unordered_map>

#ifdef _WIN32
#define VK_PROTOTYPES
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_PROTOTYPES
#define VK_USE_PLATFORM_XCB_KHR
#endif

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

#define LLGI_VK_CHECK(f)                                                                                                                   \
	do                                                                                                                                     \
	{                                                                                                                                      \
		VkResult r = (f);                                                                                                                  \
		if (r != VK_SUCCESS)                                                                                                               \
		{                                                                                                                                  \
			std::stringstream ss;                                                                                                          \
			ss << #f << "; VkResult:" << r << "(" << VulkanHelper::getResultName(r) << ")";                                                \
			Log(LogType::Error, ss.str());                                                                                                 \
			return false;                                                                                                                  \
		}                                                                                                                                  \
	} while (false)

namespace std
{

template <> struct hash<vk::Format>
{
	size_t operator()(const vk::Format& _Keyval) const noexcept { return std::hash<uint32_t>()(static_cast<uint32_t>(_Keyval)); }
};

} // namespace std

namespace LLGI
{

class GraphicsVulkan;
class BufferVulkan;
class PipelineStateVulkan;
class TextureVulkan;
class RenderPassVulkan;
class RenderPassPipelineStateCacheVulkan;

struct VulkanImageInfo
{
	VkImage image;
	VkImageAspectFlags aspect;
	VkFormat format;
};

class VulkanHelper
{
public:
	static const char* getResultName(VkResult result);
	static VkFormat TextureFormatToVkFormat(TextureFormatType format);
	static TextureFormatType VkFormatToTextureFormat(VkFormat format);
};

class InternalBuffer
{
	std::shared_ptr<GraphicsVulkan> graphics_;
	vk::Buffer buffer_;
	vk::DeviceMemory devMem_;
	bool isExternalResource_ = false;

public:
	InternalBuffer(GraphicsVulkan* graphics);
	virtual ~InternalBuffer();
	void Attach(vk::Buffer buffer, vk::DeviceMemory devMem, bool isExternalResource = false);
	vk::Buffer buffer() const { return buffer_; }
	vk::DeviceMemory devMem() const { return devMem_; }
};

class VulkanBuffer
{
public:
	VulkanBuffer();
	bool Initialize(GraphicsVulkan* graphics, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	void Dispose();
	VkBuffer GetNativeBuffer() const { return nativeBuffer_; }
	VkDeviceMemory GetNativeBufferMemory() const { return nativeBufferMemory_; }
	VkDeviceSize GetSize() const { return size_; }

private:
	GraphicsVulkan* graphics_;
	VkBuffer nativeBuffer_;
	VkDeviceMemory nativeBufferMemory_;
	VkDeviceSize size_;
};

void SetImageLayout(vk::CommandBuffer cmdbuffer,
					vk::Image image,
					vk::ImageLayout oldImageLayout,
					vk::ImageLayout newImageLayout,
					vk::ImageSubresourceRange subresourceRange);

uint32_t GetMemoryTypeIndex(vk::PhysicalDevice& phDevice, uint32_t bits, const vk::MemoryPropertyFlags& properties);

bool CreateDepthBuffer(vk::Image& image,
					   vk::ImageView view,
					   vk::DeviceMemory devMem,
					   vk::Device& device,
					   vk::PhysicalDevice& phDevice,
					   const Vec2I& size,
					   vk::Format format,
					   vk::CommandBuffer* commandBuffer);

} // namespace LLGI
