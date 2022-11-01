#include "LLGI.BufferVulkan.h"
#include "LLGI.SingleFrameMemoryPoolVulkan.h"

namespace LLGI
{

BufferVulkan::BufferVulkan() {}

BufferVulkan::~BufferVulkan() {}

bool BufferVulkan::Initialize(GraphicsVulkan* graphics, BufferUsageType usage, int32_t size)
{
	if (!VerifyUsage(usage))
	{
		return false;
	}

	SafeAddRef(graphics);
	graphics_ = CreateSharedPtr(graphics);

	buffer_ = std::unique_ptr<InternalBuffer>(new InternalBuffer(graphics));

	usage_ = usage;
	size_ = size;
	actualSize_ = size;

	vk::BufferUsageFlags vkUsage = {};

	vk::MemoryPropertyFlags memoryProperty = vk::MemoryPropertyFlagBits::eDeviceLocal;
	if (BitwiseContains(usage, BufferUsageType::MapWrite))
	{
		memoryProperty = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	}

	if (BitwiseContains(usage, BufferUsageType::MapRead))
	{
		memoryProperty = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	}

	if (BitwiseContains(usage, BufferUsageType::CopyDst))
	{
		vkUsage |= vk::BufferUsageFlagBits::eTransferDst;
	}

	if (BitwiseContains(usage, BufferUsageType::CopySrc))
	{
		vkUsage |= vk::BufferUsageFlagBits::eTransferSrc;
	}

	if (BitwiseContains(usage, BufferUsageType::Index))
	{
		vkUsage |= vk::BufferUsageFlagBits::eIndexBuffer;
	}

	if (BitwiseContains(usage, BufferUsageType::Vertex))
	{
		vkUsage |= vk::BufferUsageFlagBits::eVertexBuffer;
	}

	if (BitwiseContains(usage, BufferUsageType::Compute))
	{
		vkUsage |= vk::BufferUsageFlagBits::eStorageBuffer;
	}

	if (BitwiseContains(usage, BufferUsageType::Constant))
	{
		vkUsage |= vk::BufferUsageFlagBits::eUniformBuffer;
		actualSize_ = static_cast<int32_t>(GetAlignedSize(size, 256)); // buffer size should be multiple of 256
	}

	{
		vk::BufferCreateInfo ComputeBufferInfo;
		ComputeBufferInfo.size = actualSize_;
		ComputeBufferInfo.usage = vkUsage;
		vk::Buffer buffer = graphics_->GetDevice().createBuffer(ComputeBufferInfo);

		vk::MemoryRequirements memReqs = graphics_->GetDevice().getBufferMemoryRequirements(buffer);
		vk::MemoryAllocateInfo memAlloc;
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = graphics_->GetMemoryTypeIndex(memReqs.memoryTypeBits, memoryProperty);
		vk::DeviceMemory devMem = graphics_->GetDevice().allocateMemory(memAlloc);
		graphics_->GetDevice().bindBufferMemory(buffer, devMem, 0);

		buffer_->Attach(buffer, devMem);
	}

	return true;
}

bool BufferVulkan::InitializeAsShortTime(GraphicsVulkan* graphics, SingleFrameMemoryPoolVulkan* memoryPool, int32_t size)
{
	if (buffer_ == nullptr /* || readbackBuffer_ == nullptr || stagingBuffer_ == nullptr*/)
	{
		SafeAddRef(graphics);
		graphics_ = CreateSharedPtr(graphics);

		if (buffer_ == nullptr)
			buffer_ = std::unique_ptr<InternalBuffer>(new InternalBuffer(graphics_.get()));
	}

	auto alignedSize = static_cast<int32_t>(GetAlignedSize(size, 256));
	BufferVulkan* poolBuffer;
	if (memoryPool->GetConstantBuffer(alignedSize, poolBuffer, offset_))
	{
		buffer_->Attach(poolBuffer->buffer_->buffer(), poolBuffer->buffer_->devMem(), true);
		size_ = size;
		actualSize_ = alignedSize;

		return true;
	}
	else
	{
		return false;
	}
}

void* BufferVulkan::Lock()
{
	data = graphics_->GetDevice().mapMemory(buffer_->devMem(), offset_, actualSize_, vk::MemoryMapFlags());
	return data;
}

void* BufferVulkan::Lock(int32_t offset, int32_t size)
{
	data = graphics_->GetDevice().mapMemory(buffer_->devMem(), offset_ + offset, size, vk::MemoryMapFlags());
	return data;
}

void BufferVulkan::Unlock() { graphics_->GetDevice().unmapMemory(buffer_->devMem()); }

int32_t BufferVulkan::GetSize() { return size_; }

} // namespace LLGI
