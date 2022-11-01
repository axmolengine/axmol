
#pragma once

#include "LLGI.BaseDX12.h"
#include "LLGI.DescriptorHeapDX12.h"
#include "LLGI.GraphicsDX12.h"

namespace LLGI
{

class InternalSingleFrameMemoryPoolDX12
{
private:
	BufferDX12* buffer_ = nullptr;
	int32_t constantBufferSize_ = 0;
	int32_t constantBufferOffset_ = 0;

public:
	InternalSingleFrameMemoryPoolDX12(GraphicsDX12* graphics, int32_t constantBufferPoolSize, int32_t drawingCount);
	virtual ~InternalSingleFrameMemoryPoolDX12();
	bool GetConstantBuffer(int32_t size, BufferDX12*& buffer, int32_t& offset);
	void Reset();
};

class SingleFrameMemoryPoolDX12 : public SingleFrameMemoryPool
{
private:
	GraphicsDX12* graphics_ = nullptr;
	bool isStrongRef_ = false;
	std::vector<std::shared_ptr<InternalSingleFrameMemoryPoolDX12>> memoryPools;
	int32_t currentSwap_ = 0;
	int32_t drawingCount_ = 0;

	Buffer* CreateBufferInternal(int32_t size) override;

	Buffer* ReinitializeBuffer(Buffer* cb, int32_t size) override;

public:
	SingleFrameMemoryPoolDX12(
		GraphicsDX12* graphics, bool isStrongRef, int32_t swapBufferCount, int32_t constantBufferPoolSize, int32_t drawingCount);
	~SingleFrameMemoryPoolDX12() override;

	bool GetConstantBuffer(int32_t size, BufferDX12*& buffer, int32_t& offset);

	InternalSingleFrameMemoryPoolDX12* GetInternal();

	int32_t GetDrawingCount() const;

	void NewFrame() override;
};

} // namespace LLGI
