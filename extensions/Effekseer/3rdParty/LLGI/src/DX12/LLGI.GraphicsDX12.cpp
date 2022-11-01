#include "LLGI.GraphicsDX12.h"
#include "LLGI.BaseDX12.h"
#include "LLGI.BufferDX12.h"
#include "LLGI.CommandListDX12.h"
#include "LLGI.PipelineStateDX12.h"
#include "LLGI.PlatformDX12.h"
#include "LLGI.ShaderDX12.h"
#include "LLGI.SingleFrameMemoryPoolDX12.h"
#include "LLGI.TextureDX12.h"

namespace LLGI
{

GraphicsDX12::GraphicsDX12(ID3D12Device* device,
						   std::function<std::tuple<D3D12_CPU_DESCRIPTOR_HANDLE, Texture*>()> getScreenFunc,
						   std::function<void()> waitFunc,
						   ID3D12CommandQueue* commandQueue,
						   int32_t swapBufferCount,
						   ReferenceObject* owner)
	: device_(device)
	, getScreenFunc_(getScreenFunc)
	, waitFunc_(waitFunc)
	, commandQueue_(commandQueue)
	, swapBufferCount_(swapBufferCount)
	, owner_(owner)
{
	SafeAddRef(device_);
	SafeAddRef(commandQueue_);
	SafeAddRef(owner_);

	HRESULT hr;
	// Create Command Allocator
	hr = device->CreateCommandAllocator(commandListType_, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr));
}

GraphicsDX12::~GraphicsDX12()
{
	WaitFinish();

	SafeRelease(device_);
	SafeRelease(commandQueue_);
	SafeRelease(commandAllocator_);
	SafeRelease(owner_);
}

void GraphicsDX12::Execute(CommandList* commandList)
{
	if (commandList->GetIsInRenderPass())
	{
		Log(LogType::Error, "Please call Execute outside of RenderPass");
		return;
	}

	auto cl = (CommandListDX12*)commandList;
	auto cl_internal = cl->GetCommandList();
	commandQueue_->ExecuteCommandLists(1, (ID3D12CommandList**)(&cl_internal));
	commandQueue_->Signal(cl->GetFence(), cl->GetAndIncFenceValue());
}

void GraphicsDX12::WaitFinish()
{
	if (waitFunc_ != nullptr)
	{
		waitFunc_();
	}
}

Buffer* GraphicsDX12::CreateBuffer(BufferUsageType usage, int32_t size)
{
	auto obj = new BufferDX12();
	if (!obj->Initialize(this, usage, size))
	{
		SafeRelease(obj);
		return nullptr;
	}

	return obj;
}

Shader* GraphicsDX12::CreateShader(DataStructure* data, int32_t count)
{
	auto obj = new ShaderDX12();
	if (!obj->Initialize(data, count))
	{
		SafeRelease(obj);
		return nullptr;
	}
	return obj;
}

PipelineState* GraphicsDX12::CreatePiplineState() { return new PipelineStateDX12(this); }

SingleFrameMemoryPool* GraphicsDX12::CreateSingleFrameMemoryPool(int32_t constantBufferPoolSize, int32_t drawingCount)
{
	// Driver issue
	if (drawingCount > 512)
	{
		Log(LogType::Warning, "drawingCount is too large. It must be lower than 512");
		// drawingCount = 512;
	}

	return new SingleFrameMemoryPoolDX12(this, true, swapBufferCount_, constantBufferPoolSize, drawingCount);
}

CommandList* GraphicsDX12::CreateCommandList(SingleFrameMemoryPool* memoryPool)
{
	auto mp = static_cast<SingleFrameMemoryPoolDX12*>(memoryPool);

	auto obj = new CommandListDX12();
	if (!obj->Initialize(this, mp->GetDrawingCount()))
	{
		SafeRelease(obj);
		return nullptr;
	}
	return obj;
}

RenderPass* GraphicsDX12::CreateRenderPass(Texture** textures, int32_t textureCount, Texture* depthTexture)
{
	auto renderPass = new RenderPassDX12(this->device_);
	if (!renderPass->Initialize((TextureDX12**)textures, textureCount, (TextureDX12*)depthTexture, nullptr, nullptr))
	{
		SafeRelease(renderPass);
	}

	return renderPass;
}

RenderPass* GraphicsDX12::CreateRenderPass(Texture* texture, Texture* resolvedTexture, Texture* depthTexture, Texture* resolvedDepthTexture)
{
	auto renderPass = new RenderPassDX12(this->device_);

	std::array<TextureDX12*, 1> t;
	t[0] = const_cast<TextureDX12*>(static_cast<const TextureDX12*>(texture));

	if (!renderPass->Initialize(t.data(),
								static_cast<int32_t>(t.size()),
								(TextureDX12*)depthTexture,
								(TextureDX12*)resolvedTexture,
								(TextureDX12*)resolvedDepthTexture))
	{
		SafeRelease(renderPass);
	}

	return renderPass;
}

Texture* GraphicsDX12::CreateTexture(const TextureParameter& parameter)
{
	auto obj = new TextureDX12(this, true);
	if (!obj->Initialize(parameter))
	{
		SafeRelease(obj);
		return nullptr;
	}
	return obj;
}

Texture* GraphicsDX12::CreateTexture(uint64_t id)
{
	auto obj = new TextureDX12(this, true);
	if (!obj->Initialize(reinterpret_cast<ID3D12Resource*>(id)))
	{
		SafeRelease(obj);
		return nullptr;
	}
	return obj;
}

Texture* GraphicsDX12::CreateTexture(const TextureInitializationParameter& parameter)
{
	TextureParameter param;
	param.Dimension = 2;
	param.Format = parameter.Format;
	param.MipLevelCount = parameter.MipMapCount;
	param.SampleCount = 1;
	param.Size = {parameter.Size.X, parameter.Size.Y, 1};

	auto obj = new TextureDX12(this, true);
	if (!obj->Initialize(param))
	{
		SafeRelease(obj);
		return nullptr;
	}
	return obj;
}

Texture* GraphicsDX12::CreateRenderTexture(const RenderTextureInitializationParameter& parameter)
{
	TextureParameter param;
	param.Dimension = 2;
	param.Format = parameter.Format;
	param.MipLevelCount = 1;
	param.SampleCount = parameter.SamplingCount;
	param.Size = {parameter.Size.X, parameter.Size.Y, 1};
	param.Usage = TextureUsageType::RenderTarget;

	auto obj = new TextureDX12(this, true);
	if (!obj->Initialize(param))
	{
		SafeRelease(obj);
		return nullptr;
	}
	return obj;
}

Texture* GraphicsDX12::CreateDepthTexture(const DepthTextureInitializationParameter& parameter)
{
	auto format = TextureFormatType::D32;
	if (parameter.Mode == DepthTextureMode::DepthStencil)
	{
		format = TextureFormatType::D24S8;
	}

	TextureParameter param;
	param.Dimension = 2;
	param.Format = format;
	param.MipLevelCount = 1;
	param.SampleCount = parameter.SamplingCount;
	param.Size = {parameter.Size.X, parameter.Size.Y, 1};

	auto obj = new TextureDX12(this, true);
	if (!obj->Initialize(param))
	{
		SafeRelease(obj);
		return nullptr;
	}
	return obj;
}

RenderPassPipelineState* GraphicsDX12::CreateRenderPassPipelineState(RenderPass* renderPass)
{
	auto renderPass_ = static_cast<RenderPassDX12*>(renderPass);
	auto key = renderPass_->GetKey();
	return CreateRenderPassPipelineState(key);
}

RenderPassPipelineState* GraphicsDX12::CreateRenderPassPipelineState(const RenderPassPipelineStateKey& key)
{
	// already?
	{
		auto it = renderPassPipelineStates_.find(key);

		if (it != renderPassPipelineStates_.end())
		{
			auto ret = it->second;

			if (ret != nullptr)
			{
				auto ptr = ret.get();
				SafeAddRef(ptr);
				return ptr;
			}
		}
	}

	auto ret = CreateSharedPtr<>(new RenderPassPipelineStateDX12());
	renderPassPipelineStates_[key] = ret;
	ret->Key = key;

	{
		auto ptr = ret.get();
		SafeAddRef(ptr);
		return ptr;
	}
}

ID3D12Device* GraphicsDX12::GetDevice() { return device_; }

int32_t GraphicsDX12::GetSwapBufferCount() const { return swapBufferCount_; }

ID3D12Resource* GraphicsDX12::CreateResource(D3D12_HEAP_TYPE heapType,
											 DXGI_FORMAT format,
											 D3D12_RESOURCE_DIMENSION resourceDimention,
											 D3D12_RESOURCE_STATES resourceState,
											 D3D12_RESOURCE_FLAGS flags,
											 Vec2I size)
{
	return CreateResourceBuffer(device_, heapType, format, resourceDimention, resourceState, flags, {size.X, size.Y, 1}, 1);
}

std::vector<uint8_t> GraphicsDX12::CaptureRenderTarget(Texture* renderTarget)
{
	if (!renderTarget)
	{
		return std::vector<uint8_t>();
	}

	if (renderTarget->GetFormat() != TextureFormatType::R8G8B8A8_UNORM &&
		renderTarget->GetFormat() != TextureFormatType::R8G8B8A8_UNORM_SRGB &&
		renderTarget->GetFormat() != TextureFormatType::R32G32B32A32_FLOAT && renderTarget->GetFormat() != TextureFormatType::R8_UNORM)
	{
		Log(LogType::Error, "Unimplemented.");
		return std::vector<uint8_t>();
	}

	const auto rtSize = renderTarget->GetSizeAs2D();
	const auto rtSize3 = Vec3I{rtSize.X, rtSize.Y, 1};
	auto device = GetDevice();

	std::vector<uint8_t> result;
	auto texture = static_cast<TextureDX12*>(renderTarget);
	auto size = texture->GetSizeAs2D();
	D3D12_TEXTURE_COPY_LOCATION src = {}, dst = {};

	auto dstFootprint = texture->GetFootprint().Footprint;

	BufferDX12 dstBuffer;
	if (!dstBuffer.Initialize(this, BufferUsageType::CopyDst | BufferUsageType::MapRead, dstFootprint.RowPitch * dstFootprint.Height))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : "));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;

	auto hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}

	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, NULL, IID_PPV_ARGS(&commandList));
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		SafeRelease(commandAllocator);
		goto FAILED_EXIT;
	}

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	UINT64 totalSize;
	auto textureDesc = texture->Get()->GetDesc();
	device->GetCopyableFootprints(&textureDesc, 0, 1, 0, &footprint, nullptr, nullptr, &totalSize);

	src.pResource = texture->Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	src.SubresourceIndex = 0;

	dst.pResource = dstBuffer.Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	dst.PlacedFootprint = footprint;

	texture->ResourceBarrior(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
	commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
	texture->ResourceBarrior(commandList, D3D12_RESOURCE_STATE_GENERIC_READ);

	commandList->Close();
	ID3D12CommandList* list[] = {commandList};
	GetCommandQueue()->ExecuteCommandLists(1, list);

	// TODO optimize it
	WaitFinish();
	SafeRelease(commandList);
	SafeRelease(commandAllocator);

	auto locked = dstBuffer.Lock();

	if (locked)
	{
		auto raw = static_cast<uint8_t*>(locked);

		if (GetTextureMemorySize(renderTarget->GetFormat(), rtSize3) != dstBuffer.GetSize())
		{
			result.resize(GetTextureMemorySize(renderTarget->GetFormat(), rtSize3));

			for (int32_t y = 0; y < renderTarget->GetSizeAs2D().Y; y++)
			{
				auto pitch = GetTextureMemorySize(renderTarget->GetFormat(), rtSize3) / renderTarget->GetSizeAs2D().Y;
				memcpy(result.data() + pitch * y, raw + dstFootprint.RowPitch * y, pitch);
			}
		}
		else
		{
			result.resize(dstBuffer.GetSize());
			memcpy(result.data(), raw, result.size());
		}
		dstBuffer.Unlock();
	}

	return result;

FAILED_EXIT:
	SafeRelease(commandList);
	return std::vector<uint8_t>();
}

} // namespace LLGI
