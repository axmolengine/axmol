#pragma once

#include "LLGI.BaseDX12.h"
#include "LLGI.GraphicsDX12.h"

namespace LLGI
{

namespace DX12
{
class DescriptorHeapBlock
{
	ID3D12DescriptorHeap* descriptorHeap_ = nullptr;
	int size_ = 0;
	D3D12_DESCRIPTOR_HEAP_TYPE type_;
	int offset_ = 0;
	int handleSize_ = 0;

public:
	static std::shared_ptr<DescriptorHeapBlock> Create(std::shared_ptr<GraphicsDX12> graphics, D3D12_DESCRIPTOR_HEAP_TYPE type, int size);

	DescriptorHeapBlock(ID3D12Device* device, ID3D12DescriptorHeap* descriptorHeap, D3D12_DESCRIPTOR_HEAP_TYPE type, int32_t size);
	~DescriptorHeapBlock();

	bool Allocate(std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 32>& cpuDescriptorHandle,
				  std::array<D3D12_GPU_DESCRIPTOR_HANDLE, 32>& gpuDescriptorHandle,
				  int32_t requiredHandle);

	ID3D12DescriptorHeap* GetHeap() const;

	void Reset();
};

class DescriptorHeapAllocator
{
private:
	static const int DescriptorPerBlock = 128;
	std::shared_ptr<GraphicsDX12> graphics_;
	D3D12_DESCRIPTOR_HEAP_TYPE type_;
	std::vector<std::shared_ptr<DescriptorHeapBlock>> blocks_;
	int32_t offset_ = 0;

public:
	DescriptorHeapAllocator(std::shared_ptr<GraphicsDX12> graphics, D3D12_DESCRIPTOR_HEAP_TYPE type);
	virtual ~DescriptorHeapAllocator();

	bool Allocate(ID3D12DescriptorHeap*& heap,
				  std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 32>& cpuDescriptorHandle,
				  std::array<D3D12_GPU_DESCRIPTOR_HANDLE, 32>& gpuDescriptorHandle,
				  int32_t requiredHandle);

	void Reset();
};

} // namespace DX12

} // namespace LLGI
