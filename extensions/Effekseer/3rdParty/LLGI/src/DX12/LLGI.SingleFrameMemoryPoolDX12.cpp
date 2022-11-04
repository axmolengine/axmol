#include "LLGI.SingleFrameMemoryPoolDX12.h"
#include "LLGI.BufferDX12.h"
#include "LLGI.CommandListDX12.h"

namespace LLGI
{

InternalSingleFrameMemoryPoolDX12::InternalSingleFrameMemoryPoolDX12(GraphicsDX12* graphics,
																	 int32_t constantBufferPoolSize,
																	 int32_t drawingCount)
{
	constantBufferSize_ = (constantBufferPoolSize + 255) & ~255; // buffer size should be multiple of 256
	buffer_ = static_cast<BufferDX12*>(graphics->CreateBuffer(BufferUsageType::Constant | BufferUsageType::MapWrite, constantBufferSize_));
}

InternalSingleFrameMemoryPoolDX12 ::~InternalSingleFrameMemoryPoolDX12() { SafeRelease(buffer_); }

bool InternalSingleFrameMemoryPoolDX12::GetConstantBuffer(int32_t size, BufferDX12*& buffer, int32_t& offset)
{
	if (constantBufferOffset_ + size > constantBufferSize_)
		return false;

	buffer = buffer_;
	offset = constantBufferOffset_;
	constantBufferOffset_ += size;
	return true;
}

void InternalSingleFrameMemoryPoolDX12::Reset() { constantBufferOffset_ = 0; }

SingleFrameMemoryPoolDX12::SingleFrameMemoryPoolDX12(
	GraphicsDX12* graphics, bool isStrongRef, int32_t swapBufferCount, int32_t constantBufferPoolSize, int32_t drawingCount)
	: SingleFrameMemoryPool(swapBufferCount), graphics_(graphics), isStrongRef_(isStrongRef), drawingCount_(drawingCount)
{
	if (isStrongRef)
	{
		SafeAddRef(graphics_);
	}

	for (int32_t i = 0; i < swapBufferCount; i++)
	{
		auto memoryPool = std::make_shared<InternalSingleFrameMemoryPoolDX12>(graphics, constantBufferPoolSize, drawingCount);
		memoryPools.push_back(memoryPool);
	}

	currentSwap_ = -1;
}

SingleFrameMemoryPoolDX12 ::~SingleFrameMemoryPoolDX12()
{
	memoryPools.clear();

	if (isStrongRef_)
	{
		SafeRelease(graphics_);
	}
}

Buffer* SingleFrameMemoryPoolDX12::CreateBufferInternal(int32_t size)
{
	auto obj = new BufferDX12();
	if (!obj->InitializeAsShortTime(this, size))
	{
		SafeRelease(obj);
		return nullptr;
	}

	return obj;
}

Buffer* SingleFrameMemoryPoolDX12::ReinitializeBuffer(Buffer* cb, int32_t size)
{
	auto obj = static_cast<BufferDX12*>(cb);
	if (!obj->InitializeAsShortTime(this, size))
	{
		return nullptr;
	}

	return obj;
}

bool SingleFrameMemoryPoolDX12::GetConstantBuffer(int32_t size, BufferDX12*& buffer, int32_t& offset)
{
	assert(currentSwap_ >= 0);
	return memoryPools[currentSwap_]->GetConstantBuffer(size, buffer, offset);
}

InternalSingleFrameMemoryPoolDX12* SingleFrameMemoryPoolDX12::GetInternal() { return memoryPools[currentSwap_].get(); }

int32_t SingleFrameMemoryPoolDX12::GetDrawingCount() const { return drawingCount_; }

void SingleFrameMemoryPoolDX12::NewFrame()
{
	currentSwap_++;
	currentSwap_ %= memoryPools.size();
	memoryPools[currentSwap_]->Reset();
	SingleFrameMemoryPool::NewFrame();

	assert(currentSwap_ == currentSwapBuffer_);
}

} // namespace LLGI
