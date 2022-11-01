#include "LLGI.PlatformDX12.h"
#include "../Win/LLGI.WindowWin.h"
#include "LLGI.GraphicsDX12.h"

namespace LLGI
{

void PlatformDX12::Wait()
{
	if (fence == nullptr)
		return;

	HRESULT hr;

	auto fenceValue_ = fenceValue;
	hr = commandQueue->Signal(fence, fenceValue_);
	if (FAILED(hr))
	{
		return;
	}

	fenceValue++;

	if (fence->GetCompletedValue() < fenceValue_)
	{
		hr = fence->SetEventOnCompletion(fenceValue_, fenceEvent);
		if (FAILED(hr))
		{
			return;
		}
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

PlatformDX12::PlatformDX12()
{
	for (int32_t i = 0; i < SwapBufferCount; i++)
	{
		renderResources_[i] = nullptr;
	}

	renderTargets_.fill(nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE h;
	h.ptr = static_cast<SIZE_T>(0);
	handleRTV.fill(h);

	renderResources_.fill(nullptr);
	renderTargets_.fill(nullptr);
	renderPasses_.fill(nullptr);
	commandAllocators.fill(nullptr);
}

PlatformDX12::~PlatformDX12()
{
	Wait();

	for (size_t i = 0; i < renderTargets_.size(); i++)
	{
		SafeRelease(renderTargets_[i]);
	}

	SafeRelease(descriptorHeapRTV);

	for (int32_t i = 0; i < SwapBufferCount; i++)
	{
		SafeRelease(renderResources_[i]);
		SafeRelease(renderPasses_[i]);
		handleRTV[i] = {};
	}

	for (auto& commandAllocator : commandAllocators)
	{
		SafeRelease(commandAllocator);
	}

	SafeRelease(commandListStart);
	SafeRelease(commandListPresent);
	SafeRelease(commandQueue);
	SafeRelease(fence);
	SafeRelease(device);
	SafeRelease(dxgiFactory);
	SafeRelease(swapChain);

	if (fenceEvent != nullptr)
	{
		CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}

#if defined(_DEBUG)
	EndDX12_DRED_Debug();
#endif
}

void PlatformDX12::ResetSwapBuffer()
{

	Wait();

	for (size_t i = 0; i < renderTargets_.size(); i++)
	{
		SafeRelease(renderTargets_[i]);
	}

	SafeRelease(descriptorHeapRTV);

	for (int32_t i = 0; i < SwapBufferCount; i++)
	{
		SafeRelease(renderResources_[i]);
		SafeRelease(renderPasses_[i]);
		handleRTV[i] = {};
	}

	SafeRelease(swapChain);
}

bool PlatformDX12::GenerateSwapBuffer()
{
	// Swap chain
	DXGI_SWAP_CHAIN_DESC DXGISwapChainDesc;
	ZeroMemory(&DXGISwapChainDesc, sizeof(DXGISwapChainDesc));

	D3D12_DESCRIPTOR_HEAP_DESC renderPassHeapDesc = {};

	DXGISwapChainDesc.BufferDesc.Width = windowSize_.X;
	DXGISwapChainDesc.BufferDesc.Height = windowSize_.Y;
	DXGISwapChainDesc.OutputWindow = (HWND)window_->GetNativePtr(0);
	DXGISwapChainDesc.Windowed = TRUE;
	DXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	DXGISwapChainDesc.BufferCount = SwapBufferCount;
	DXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	DXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	DXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	DXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	DXGISwapChainDesc.SampleDesc.Count = 1;
	DXGISwapChainDesc.SampleDesc.Quality = 0;

	IDXGISwapChain* swapChain_ = nullptr;
	auto hr = dxgiFactory->CreateSwapChain(commandQueue, &DXGISwapChainDesc, &swapChain_);
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}

	hr = swapChain_->QueryInterface(&swapChain);
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		SafeRelease(swapChain_);
		goto FAILED_EXIT;
	}
	SafeRelease(swapChain_);

	// Render target

	// Render target DescriptorHeap
	renderPassHeapDesc.NumDescriptors = SwapBufferCount;
	renderPassHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	renderPassHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	renderPassHeapDesc.NodeMask = DirectX12::GetNodeMask();
	hr = device->CreateDescriptorHeap(&renderPassHeapDesc, IID_PPV_ARGS(&descriptorHeapRTV));
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}

	auto descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (int32_t i = 0; i < SwapBufferCount; ++i)
	{

		// get render target from swap chain
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&renderResources_[i]));
		if (FAILED(hr))
		{
			auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
						std::system_category().message(hr));
			::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
			goto FAILED_EXIT;
		}

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		handleRTV[i] = descriptorHeapRTV->GetCPUDescriptorHandleForHeapStart();
		handleRTV[i].ptr += descriptorHandleIncrementSize * i;
		device->CreateRenderTargetView(renderResources_[i], &rtvDesc, handleRTV[i]);

		renderTargets_[i] = new TextureDX12(renderResources_[i], device, commandQueue);
		renderPasses_[i] = new RenderPassDX12(device);
		renderPasses_[i]->Initialize(&renderTargets_[i], 1, nullptr, nullptr, nullptr);
	}

	return true;
FAILED_EXIT:;
	SafeRelease(swapChain_);

	ResetSwapBuffer();

	return false;
}

bool PlatformDX12::Initialize(Window* window, bool waitVSync)
{
	// Windows
	window_ = window;
	windowSize_ = window->GetWindowSize();
	waitVSync_ = waitVSync;

	// DirectX12
	HRESULT hr;

	UINT flagsDXGI = 0;

#if defined(_DEBUG)

	if (GetIsGPUDebugEnabled())
	{
		ID3D12Debug* spDebugController0;
		ID3D12Debug1* spDebugController1;
		D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0));
		spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1));
		spDebugController1->SetEnableGPUBasedValidation(true);
		spDebugController0->Release();
		spDebugController1->Release();
	}

	ID3D12Debug* debug_ = nullptr;
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug_));
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}

	// make debug enabled
	debug_->EnableDebugLayer();
	debug_->Release();
	debug_ = nullptr;

	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;

	StartDX12_DRED_Debug();
#endif

	// factory
	hr = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}

	// device

	IDXGIAdapter1* adapter = nullptr;

	for (UINT adapterIndex = 0; dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND; adapterIndex++)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&device)))
		{
			SafeRelease(adapter);
			break;
		}

		SafeRelease(adapter);
	}

	if (device == nullptr)
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}

#if defined(_DEBUG)
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(&infoQueue)))
	{
		D3D12_MESSAGE_SEVERITY severities[] = {D3D12_MESSAGE_SEVERITY_INFO};

		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		NewFilter.DenyList.NumSeverities = _countof(severities);
		NewFilter.DenyList.pSeverityList = severities;
		NewFilter.DenyList.NumIDs = _countof(denyIds);
		NewFilter.DenyList.pIDList = denyIds;

		infoQueue->PushStorageFilter(&NewFilter);
		SafeRelease(infoQueue);
	}
#endif

	/*
	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&device);

	if (FAILED(hr))
	{
		goto FAILED_EXIT;
	}
	*/

	// Create Command Queue
	D3D12_COMMAND_QUEUE_DESC descCommandQueue;
	ZeroMemory(&descCommandQueue, sizeof(descCommandQueue));
	descCommandQueue.Priority = 0;
	descCommandQueue.NodeMask = DirectX12::GetNodeMask();
	descCommandQueue.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	descCommandQueue.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hr = device->CreateCommandQueue(&descCommandQueue, IID_PPV_ARGS(&commandQueue));
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}

	// Fence
	hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}
	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Swap chain
	if (!GenerateSwapBuffer())
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}

	// Create Command Allocator
	for (auto& commandAllocator : commandAllocators)
	{
		hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
		if (FAILED(hr))
		{
			auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
						std::system_category().message(hr));
			::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
			goto FAILED_EXIT;
		}
	}

	// Create Command List
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0], NULL, IID_PPV_ARGS(&commandListStart));
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}
	commandListStart->Close();

	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0], NULL, IID_PPV_ARGS(&commandListPresent));
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}
	commandListPresent->Close();

	return true;

FAILED_EXIT:;

	SafeRelease(descriptorHeapRTV);

	for (int32_t i = 0; i < SwapBufferCount; i++)
	{
		SafeRelease(renderResources_[i]);
		handleRTV[i] = {};
	}

	for (auto& commandAllocator : commandAllocators)
	{
		SafeRelease(commandAllocator);
	}

	SafeRelease(commandListStart);
	SafeRelease(commandListPresent);
	SafeRelease(commandQueue);
	SafeRelease(fence);
	SafeRelease(device);
	SafeRelease(dxgiFactory);
	SafeRelease(swapChain);

	if (fenceEvent != nullptr)
	{
		CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}

	return false;
}

int PlatformDX12::GetCurrentFrameIndex() const { return frameIndex; }

int PlatformDX12::GetMaxFrameCount() const { return static_cast<int>(renderTargets_.size()); }

bool PlatformDX12::NewFrame()
{
	if (!window_->OnNewFrame())
	{
		return false;
	}

	frameIndex = swapChain->GetCurrentBackBufferIndex();

	commandAllocators.at(0)->Reset();
	commandListStart->Reset(commandAllocators.at(0), nullptr);

	renderTargets_[frameIndex]->ResourceBarrior(commandListStart, D3D12_RESOURCE_STATE_RENDER_TARGET);

	commandListStart->OMSetRenderTargets(1, &(handleRTV[frameIndex]), FALSE, nullptr);
	commandListStart->Close();

	ID3D12CommandList* commandList[] = {commandListStart};
	commandQueue->ExecuteCommandLists(1, commandList);

	inFrame_ = true;
	return true;
}

void PlatformDX12::Present()
{
	commandListPresent->Reset(commandAllocators.at(0), nullptr);

	renderTargets_[frameIndex]->ResourceBarrior(commandListPresent, D3D12_RESOURCE_STATE_PRESENT);

	commandListPresent->Close();

	ID3D12CommandList* commandList[] = {commandListPresent};
	commandQueue->ExecuteCommandLists(1, commandList);

	swapChain->Present(waitVSync_ ? 1 : 0, 0);
	Wait();

	inFrame_ = false;
}

Graphics* PlatformDX12::CreateGraphics()
{
	std::function<std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, Texture*>()> getScreenFunc =
		[this]() -> std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, Texture*> {
		std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, Texture*> ret(handleRTV[frameIndex], renderTargets_[frameIndex]);

		return ret;
	};

	std::function<void()> waitFunc = [this]() -> void { this->Wait(); };

	auto graphics = new GraphicsDX12(device, getScreenFunc, waitFunc, commandQueue, SwapBufferCount, this);

	graphics->SetWindowSize(windowSize_);

	return graphics;
}

ID3D12Device* PlatformDX12::GetDevice() { return device; }

void PlatformDX12::SetWindowSize(const Vec2I& windowSize)
{

	if (windowSize_ == windowSize)
	{
		return;
	}

	if (inFrame_)
	{
		Log(LogType::Error, "SetWindowSize can be only called before NewFrame after Present.");
		return;
	}

	windowSize_ = windowSize;

	ResetSwapBuffer();
	GenerateSwapBuffer();
}

RenderPass* PlatformDX12::GetCurrentScreen(const Color8& clearColor, bool isColorCleared, bool isDepthCleared)
{
	auto renderPass = renderPasses_[frameIndex];
	renderPass->SetClearColor(clearColor);
	renderPass->SetIsColorCleared(isColorCleared);
	renderPass->SetIsDepthCleared(isDepthCleared);
	return renderPass;
}

} // namespace LLGI
