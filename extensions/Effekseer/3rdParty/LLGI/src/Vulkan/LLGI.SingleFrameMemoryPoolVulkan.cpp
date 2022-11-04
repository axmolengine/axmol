#include "LLGI.SingleFrameMemoryPoolVulkan.h"
#include "LLGI.BufferVulkan.h"

namespace LLGI
{

InternalSingleFrameMemoryPoolVulkan::InternalSingleFrameMemoryPoolVulkan() {}

InternalSingleFrameMemoryPoolVulkan ::~InternalSingleFrameMemoryPoolVulkan() {}

bool InternalSingleFrameMemoryPoolVulkan::Initialize(GraphicsVulkan* graphics, int32_t constantBufferPoolSize, int32_t drawingCount)
{
	constantBufferSize_ = (constantBufferPoolSize + 255) & ~255; // buffer size should be multiple of 256

	buffer_ = std::unique_ptr<BufferVulkan>(
		static_cast<BufferVulkan*>(graphics->CreateBuffer(BufferUsageType::Constant | BufferUsageType::MapWrite, constantBufferSize_)));

	return true;
}

void InternalSingleFrameMemoryPoolVulkan::Dispose() { buffer_ = nullptr; }

bool InternalSingleFrameMemoryPoolVulkan::GetConstantBuffer(int32_t size, BufferVulkan*& buffer, int32_t& outOffset)
{
	if (constantBufferOffset_ + size > constantBufferSize_)
		return false;

	buffer = buffer_.get();
	outOffset = constantBufferOffset_;
	constantBufferOffset_ += size;

	return true;
}

void InternalSingleFrameMemoryPoolVulkan::Reset() { constantBufferOffset_ = 0; }

Buffer* SingleFrameMemoryPoolVulkan::CreateBufferInternal(int32_t size)
{
	auto obj = new BufferVulkan();
	if (!obj->InitializeAsShortTime(graphics_, this, size))
	{
		SafeRelease(obj);
		return nullptr;
	}

	return obj;
}

Buffer* SingleFrameMemoryPoolVulkan::ReinitializeBuffer(Buffer* cb, int32_t size)
{
	auto obj = static_cast<BufferVulkan*>(cb);
	if (!obj->InitializeAsShortTime(graphics_, this, size))
	{
		return nullptr;
	}

	return obj;
}

SingleFrameMemoryPoolVulkan::SingleFrameMemoryPoolVulkan(
	GraphicsVulkan* graphics, bool isStrongRef, int32_t swapBufferCount, int32_t constantBufferPoolSize, int32_t drawingCount)
	: SingleFrameMemoryPool(swapBufferCount), graphics_(graphics), isStrongRef_(isStrongRef), currentSwap_(-1), drawingCount_(drawingCount)
{
	if (isStrongRef)
	{
		SafeAddRef(graphics_);
	}

	for (int32_t i = 0; i < swapBufferCount; i++)
	{
		auto memoryPool = std::make_shared<InternalSingleFrameMemoryPoolVulkan>();
		if (!memoryPool->Initialize(graphics, constantBufferPoolSize, drawingCount))
		{
			return;
		}
		memoryPools.push_back(memoryPool);
	}
}

SingleFrameMemoryPoolVulkan ::~SingleFrameMemoryPoolVulkan()
{
	for (auto& pool : memoryPools)
	{
		pool->Dispose();
	}
	memoryPools.clear();

	if (isStrongRef_)
	{
		SafeRelease(graphics_);
	}
}

bool SingleFrameMemoryPoolVulkan::GetConstantBuffer(int32_t size, BufferVulkan*& buffer, int32_t& outOffset)
{
	assert(currentSwap_ >= 0);
	return memoryPools[currentSwap_]->GetConstantBuffer(size, buffer, outOffset);
}

InternalSingleFrameMemoryPoolVulkan* SingleFrameMemoryPoolVulkan::GetInternal() { return memoryPools[currentSwap_].get(); }

int32_t SingleFrameMemoryPoolVulkan::GetDrawingCount() const { return drawingCount_; }

void SingleFrameMemoryPoolVulkan::NewFrame()
{
	currentSwap_++;
	currentSwap_ %= memoryPools.size();
	memoryPools[currentSwap_]->Reset();
	SingleFrameMemoryPool::NewFrame();
}

} // namespace LLGI
