
#pragma once

#include "../LLGI.Platform.h"
#include "LLGI.BaseDX12.h"
#include "LLGI.TextureDX12.h"

#ifdef _WIN32
#include "../Win/LLGI.WindowWin.h"
#endif

namespace LLGI
{

class PlatformDX12 : public Platform
{
private:
	static const int SwapBufferCount = 2;
	Window* window_ = nullptr;

	Vec2I windowSize_;

	ID3D12Device* device = nullptr;
	IDXGIFactory4* dxgiFactory = nullptr;
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12Fence* fence = nullptr;
	HANDLE fenceEvent = nullptr;
	IDXGISwapChain3* swapChain = nullptr;

	ID3D12DescriptorHeap* descriptorHeapRTV = nullptr;
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, SwapBufferCount> handleRTV;
	std::array<ID3D12Resource*, SwapBufferCount> renderResources_;
	std::array<TextureDX12*, SwapBufferCount> renderTargets_;
	std::array<RenderPassDX12*, SwapBufferCount> renderPasses_;

	std::array<ID3D12CommandAllocator*, SwapBufferCount> commandAllocators;
	ID3D12GraphicsCommandList* commandListStart = nullptr;
	ID3D12GraphicsCommandList* commandListPresent = nullptr;
	UINT64 fenceValue = 1;

	int32_t frameIndex = 0;

	bool inFrame_ = false;

	void Wait();

	void ResetSwapBuffer();
	bool GenerateSwapBuffer();

public:
	PlatformDX12();
	~PlatformDX12() override;

	bool Initialize(Window* window, bool waitVSync);

	int GetCurrentFrameIndex() const override;
	int GetMaxFrameCount() const override;

	bool NewFrame() override;
	void Present() override;
	Graphics* CreateGraphics() override;

	ID3D12Device* GetDevice();

	void SetWindowSize(const Vec2I& windowSize) override;

	RenderPass* GetCurrentScreen(const Color8& clearColor, bool isColorCleared, bool isDepthCleared) override;

	DeviceType GetDeviceType() const override { return DeviceType::DirectX12; }
};

} // namespace LLGI
