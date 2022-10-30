#include "LLGI.DescriptorHeapDX12.h"

namespace LLGI
{

namespace DX12
{

std::shared_ptr<DescriptorHeapBlock>
DescriptorHeapBlock::Create(std::shared_ptr<GraphicsDX12> graphics, D3D12_DESCRIPTOR_HEAP_TYPE type, int size)
{

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	ID3D12DescriptorHeap* heap = nullptr;

	heapDesc.NumDescriptors = size;
	heapDesc.Type = type;
	heapDesc.Flags = (type <= D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
						 ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
						 : D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // Only descriptor heaps for CBV, SRV, UAV, sampler can be shader visible.

	heapDesc.NodeMask = 1;

	auto hr = graphics->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap));
	if (FAILED(hr))
	{
		SafeRelease(heap);
		return nullptr;
	}

	return std::make_shared<DescriptorHeapBlock>(graphics->GetDevice(), heap, type, size);
}

DescriptorHeapBlock::DescriptorHeapBlock(ID3D12Device* device,
										 ID3D12DescriptorHeap* descriptorHeap,
										 D3D12_DESCRIPTOR_HEAP_TYPE type,
										 int32_t size)
	: descriptorHeap_(descriptorHeap), type_(type), size_(size)
{
	handleSize_ = static_cast<int>(device->GetDescriptorHandleIncrementSize(type_));
}

DescriptorHeapBlock::~DescriptorHeapBlock() { SafeRelease(descriptorHeap_); }

bool DescriptorHeapBlock::Allocate(std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 32>& cpuDescriptorHandle,
								   std::array<D3D12_GPU_DESCRIPTOR_HANDLE, 32>& gpuDescriptorHandle,
								   int32_t requiredHandle)
{
	if (requiredHandle > 32)
	{
		Log(LogType::Error, "the number of register must be lower than 32.");
		return false;
	}

	if (size_ <= offset_ + requiredHandle)
		return false;

	auto cpuHandle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	auto gpuHandle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();

	for (int i = 0; i < requiredHandle; i++)
	{
		cpuDescriptorHandle[i] = cpuHandle;
		gpuDescriptorHandle[i] = gpuHandle;

		cpuDescriptorHandle[i].ptr += handleSize_ * (i + offset_);
		gpuDescriptorHandle[i].ptr += handleSize_ * (i + offset_);
	}

	offset_ += requiredHandle;

	return true;
}

ID3D12DescriptorHeap* DescriptorHeapBlock::GetHeap() const { return descriptorHeap_; }

void DescriptorHeapBlock::Reset() { offset_ = 0; }

DescriptorHeapAllocator::DescriptorHeapAllocator(std::shared_ptr<GraphicsDX12> graphics, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	graphics_ = graphics;
	type_ = type;

	auto block = DescriptorHeapBlock::Create(graphics_, type, DescriptorPerBlock);
	if (block == nullptr)
		throw "Failed to allocate descriptor";

	blocks_.emplace_back(block);
}

DescriptorHeapAllocator ::~DescriptorHeapAllocator() {}

bool DescriptorHeapAllocator::Allocate(ID3D12DescriptorHeap*& heap,
									   std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 32>& cpuDescriptorHandle,
									   std::array<D3D12_GPU_DESCRIPTOR_HANDLE, 32>& gpuDescriptorHandle,
									   int32_t requiredHandle)
{
	if (blocks_[offset_]->Allocate(cpuDescriptorHandle, gpuDescriptorHandle, requiredHandle))
	{
		heap = blocks_[offset_]->GetHeap();
		return true;
	}
	else
	{
		offset_++;
		if (offset_ < static_cast<int32_t>(blocks_.size()) &&
			blocks_[offset_]->Allocate(cpuDescriptorHandle, gpuDescriptorHandle, requiredHandle))
		{
			heap = blocks_[offset_]->GetHeap();
			return true;
		}

		auto block = DescriptorHeapBlock::Create(graphics_, type_, DescriptorPerBlock);
		if (block == nullptr)
		{
			Log(LogType::Error, "Faled to allocate descriptor block.");
			offset_--;
			return false;
		}

		blocks_.emplace_back(block);
		if (blocks_[offset_]->Allocate(cpuDescriptorHandle, gpuDescriptorHandle, requiredHandle))
		{
			heap = blocks_[offset_]->GetHeap();
			return true;
		}
	}

	throw "Failed to allocate descriptor even if it allocates block.";
}

void DescriptorHeapAllocator::Reset()
{
	for (auto& block : blocks_)
	{
		block->Reset();
	}
	offset_ = 0;
}

} // namespace DX12
} // namespace LLGI
