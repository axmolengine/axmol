#pragma once

#include "../LLGI.Graphics.h"
#include "LLGI.BufferMetal.h"
#import <MetalKit/MetalKit.h>
#include <functional>
#include <unordered_map>

namespace LLGI
{

struct SingleFrameMemoryPoolMetal_Impl;
struct Buffer_Impl;

class GraphicsMetal;
class RenderPassMetal;
class RenderPassPipelineStateMetal;
class TextureMetal;

class InternalSingleFrameMemoryPoolMetal
{
private:
	BufferMetal* buffer_ = nullptr;
	int32_t constantBufferSize_ = 0;
	int32_t constantBufferOffset_ = 0;

public:
	InternalSingleFrameMemoryPoolMetal(GraphicsMetal* graphics, int32_t constantBufferPoolSize, int32_t drawingCount);
	virtual ~InternalSingleFrameMemoryPoolMetal();
	bool GetConstantBuffer(int32_t size, BufferMetal*& buffer, int32_t& offset);
	void Reset();
};

class SingleFrameMemoryPoolMetal : public SingleFrameMemoryPool
{
private:
	GraphicsMetal* graphics_ = nullptr;
	bool isStrongRef_ = false;
	std::vector<std::shared_ptr<InternalSingleFrameMemoryPoolMetal>> memoryPools;
	int32_t currentSwap_ = 0;

	Buffer* CreateBufferInternal(int32_t size) override;

	Buffer* ReinitializeBuffer(Buffer* cb, int32_t size) override;

public:
	SingleFrameMemoryPoolMetal(GraphicsMetal* graphics, bool isStrongRef, int32_t constantBufferPoolSize, int32_t drawingCount);
	~SingleFrameMemoryPoolMetal() override;
	virtual void NewFrame() override;
};

} // namespace LLGI
