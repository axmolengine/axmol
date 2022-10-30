#include "LLGI.BufferDX12.h"
#include "LLGI.SingleFrameMemoryPoolDX12.h"

namespace LLGI
{

BufferDX12::BufferDX12()
{
	size_ = 0;
	state_ = D3D12_RESOURCE_STATE_COMMON;
}

BufferDX12::~BufferDX12()
{
	SafeRelease(buffer_);
}

bool BufferDX12::Initialize(GraphicsDX12* graphics, const BufferUsageType usage, const int32_t size)
{
	if (!VerifyUsage(usage))
	{
		return false;
	}

	usage_ = usage;

	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
	D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;

	size_ = size;
	actualSize_ = size;

	if (BitwiseContains(usage, BufferUsageType::MapWrite))
	{
		heapType = D3D12_HEAP_TYPE_UPLOAD;
		state_ |= D3D12_RESOURCE_STATE_GENERIC_READ;
	}
	else if (BitwiseContains(usage, BufferUsageType::MapRead))
	{
		heapType = D3D12_HEAP_TYPE_READBACK;
	}

	if (BitwiseContains(usage, BufferUsageType::CopySrc) && !BitwiseContains(usage, BufferUsageType::Compute))
	{
		state_ |= D3D12_RESOURCE_STATE_COPY_SOURCE;
	}

	if (BitwiseContains(usage, BufferUsageType::CopyDst) && !BitwiseContains(usage, BufferUsageType::Compute))
	{
		state_ |= D3D12_RESOURCE_STATE_COPY_DEST;
	}

	if (BitwiseContains(usage, BufferUsageType::Index))
	{
		state_ |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
	}

	if (BitwiseContains(usage, BufferUsageType::Vertex) || BitwiseContains(usage, BufferUsageType::Constant))
	{
		state_ |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	}

	if (BitwiseContains(usage, BufferUsageType::Compute))
	{
		state_ |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	if (BitwiseContains(usage, BufferUsageType::Constant))
	{
		actualSize_ = (size + 255) & ~255; // buffer size should be multiple of 256
	}

	buffer_ =
		graphics->CreateResource(heapType, DXGI_FORMAT_UNKNOWN, D3D12_RESOURCE_DIMENSION_BUFFER, state_, flags, Vec2I(actualSize_, 1));

	if (buffer_ == nullptr)
		return false;

	return true;
}

bool BufferDX12::InitializeAsShortTime(SingleFrameMemoryPoolDX12* memoryPool, int32_t size)
{
	auto old = buffer_;
	auto actualSize = (size + 255) & ~255; // buffer size should be multiple of 256

	BufferDX12* poolBuffer;
	if (memoryPool->GetConstantBuffer(actualSize, poolBuffer, offset_))
	{
		buffer_ = poolBuffer->Get();

		usage_ = poolBuffer->usage_;
		state_ = poolBuffer->state_;

		size_ = size;
		actualSize_ = actualSize;

		SafeAddRef(buffer_);
		SafeRelease(old);

		return true;
	}
	else
	{
		return false;
	}
}

void* BufferDX12::Lock()
{
	auto hr = buffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapped_));
	return SUCCEEDED(hr) ? (mapped_ + offset_) : nullptr;
}

void* BufferDX12::Lock(int32_t offset, int32_t size)
{
	auto hr = buffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapped_));
	return SUCCEEDED(hr) ? mapped_ + offset_ + offset : nullptr;
}

void BufferDX12::Unlock()
{
	buffer_->Unmap(0, nullptr);
	mapped_ = nullptr;
}

int32_t BufferDX12::GetSize() { return size_; }

int32_t BufferDX12::GetActualSize() const { return actualSize_; }

int32_t BufferDX12::GetOffset() const { return offset_; }

} // namespace LLGI
