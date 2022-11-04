#include "LLGI.BufferMetal.h"
#include "LLGI.CommandListMetal.h"
#include "LLGI.GraphicsMetal.h"
#include "LLGI.Metal_Impl.h"
#include "LLGI.PipelineStateMetal.h"
#include "LLGI.RenderPassMetal.h"
#include "LLGI.ShaderMetal.h"
#include "LLGI.SingleFrameMemoryPoolMetal.h"
#include "LLGI.TextureMetal.h"

#import <MetalKit/MetalKit.h>

namespace LLGI
{

BufferMetal::BufferMetal()
{
	
}

BufferMetal::~BufferMetal()
{
    if (isExternalResource_)
        return;
    
	if (buffer_ != nullptr)
	{
		[buffer_ release];
		buffer_ = nullptr;
	}
}

bool BufferMetal::Initialize(Graphics* graphics, BufferUsageType usage, int32_t size)
{
	if (!VerifyUsage(usage))
	{
		return false;
	}

    auto g = static_cast<GraphicsMetal*>(graphics);
    
    if(BitwiseContains(usage, BufferUsageType::MapWrite) || BitwiseContains(usage, BufferUsageType::MapRead))
    {
        buffer_ = [g->GetDevice() newBufferWithLength:size options:MTLResourceStorageModeShared];
    }
    else
    {
        buffer_ = [g->GetDevice() newBufferWithLength:size options:MTLResourceStorageModePrivate];
    }
        
    size_ = size;
    
    return true;
}

bool BufferMetal::InitializeAsShortTime(BufferMetal* buffer, int32_t offset, int32_t size)
{
    buffer_ = buffer->GetBuffer();
    size_ = size;
    offset_ = offset;
    isExternalResource_ = true;
    return true;
}

void* BufferMetal::Lock()
{
    auto buffer = static_cast<uint8_t*>(buffer_.contents);
    buffer += offset_;
    return buffer;
}

void* BufferMetal::Lock(int32_t offset, int32_t size)
{
    NSCAssert(0 <= offset && offset + offset_ + size <= GetBufferSize(), @"Run off the buffer");

    auto buffer = static_cast<uint8_t*>(buffer_.contents);
    buffer += offset + offset_;
    return buffer;
}

void BufferMetal::Unlock() {}

int32_t BufferMetal::GetSize() { return size_; }

}
