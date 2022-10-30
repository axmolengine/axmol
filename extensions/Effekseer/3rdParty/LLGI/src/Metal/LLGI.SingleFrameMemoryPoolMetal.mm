#include "LLGI.SingleFrameMemoryPoolMetal.h"
#include "LLGI.BufferMetal.h"
#include "LLGI.GraphicsMetal.h"
#import <MetalKit/MetalKit.h>

namespace LLGI
{

InternalSingleFrameMemoryPoolMetal::InternalSingleFrameMemoryPoolMetal(GraphicsMetal* graphics,
																	   int32_t constantBufferPoolSize,
																	   int32_t drawingCount)
{
	constantBufferSize_ = static_cast<int32_t>(GetAlignedSize(static_cast<size_t>(constantBufferPoolSize), 256));
    buffer_ = new BufferMetal();
    buffer_->Initialize(graphics, BufferUsageType::Constant | BufferUsageType::MapWrite, constantBufferSize_);
}

InternalSingleFrameMemoryPoolMetal::~InternalSingleFrameMemoryPoolMetal() { SafeRelease(buffer_); }

bool InternalSingleFrameMemoryPoolMetal::GetConstantBuffer(int32_t size, BufferMetal*& buffer, int32_t& offset)
{
	if (constantBufferOffset_ + size > constantBufferSize_)
		return false;

	buffer = buffer_;
	offset = constantBufferOffset_;
	constantBufferOffset_ += size;

	// shift 256
	constantBufferOffset_ = static_cast<int32_t>(GetAlignedSize(static_cast<size_t>(constantBufferOffset_), 256));
	return true;
}

void InternalSingleFrameMemoryPoolMetal::Reset() { constantBufferOffset_ = 0; }

SingleFrameMemoryPoolMetal::SingleFrameMemoryPoolMetal(GraphicsMetal* graphics,
													   bool isStrongRef,
													   int32_t constantBufferPoolSize,
													   int32_t drawingCount)
	: SingleFrameMemoryPool(3), graphics_(graphics), isStrongRef_(isStrongRef)
{
	if (isStrongRef)
	{
		SafeAddRef(graphics_);
	}

	// TODO improve it
	int swapBufferCount = 3;

	for (size_t i = 0; i < swapBufferCount; i++)
	{
		auto memoryPool = std::make_shared<InternalSingleFrameMemoryPoolMetal>(graphics, constantBufferPoolSize, drawingCount);
		memoryPools.push_back(memoryPool);
	}

	currentSwap_ = -1;
}

SingleFrameMemoryPoolMetal::~SingleFrameMemoryPoolMetal()
{
	memoryPools.clear();

	if (isStrongRef_)
	{
		SafeRelease(graphics_);
	}
}

Buffer* SingleFrameMemoryPoolMetal::CreateBufferInternal(int32_t size)
{
	int32_t offset = 0;
	BufferMetal* buffer = nullptr;
	if (memoryPools[currentSwap_]->GetConstantBuffer(size, buffer, offset))
	{
		auto obj = new BufferMetal();
		if (!obj->InitializeAsShortTime(buffer, offset, size))
		{
			SafeRelease(obj);
			return nullptr;
		}

		return obj;
	}

	return nullptr;
}

Buffer* SingleFrameMemoryPoolMetal::ReinitializeBuffer(Buffer* cb, int32_t size)
{
	int32_t offset = 0;
	BufferMetal* buffer = nullptr;
	if (memoryPools[currentSwap_]->GetConstantBuffer(size, buffer, offset))
	{
		auto obj = static_cast<BufferMetal*>(cb);
		if (!obj->InitializeAsShortTime(buffer, offset, size))
		{
			return nullptr;
		}

		return obj;
	}

	return nullptr;
}

void SingleFrameMemoryPoolMetal::NewFrame()
{
	currentSwap_++;
	currentSwap_ %= memoryPools.size();
	memoryPools[currentSwap_]->Reset();
	SingleFrameMemoryPool::NewFrame();
}

}
