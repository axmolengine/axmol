#pragma once

#include "../LLGI.Buffer.h"
#include "LLGI.BaseDX12.h"
#include "LLGI.GraphicsDX12.h"

namespace LLGI
{
class BufferDX12 : public Buffer
{
private:
	ID3D12Resource* buffer_ = nullptr;
	uint8_t* mapped_ = nullptr;
	int32_t offset_ = 0;
	int32_t size_ = 0;
	int32_t actualSize_ = 0;
	D3D12_RESOURCE_STATES state_;

public:
	bool Initialize(GraphicsDX12* graphics, const BufferUsageType usage, const int32_t size);
	bool InitializeAsShortTime(SingleFrameMemoryPoolDX12* memoryPool, int32_t size);

	BufferDX12();
	~BufferDX12() override;

	void* Lock() override;
	void* Lock(int32_t offset, int32_t size) override;
	void Unlock() override;

	int32_t GetSize() override;

	int32_t GetActualSize() const;
	int32_t GetOffset() const;

	ID3D12Resource* Get() { return buffer_; }

	D3D12_RESOURCE_STATES GetResourceState() { return state_; }
};

} // namespace LLGI
