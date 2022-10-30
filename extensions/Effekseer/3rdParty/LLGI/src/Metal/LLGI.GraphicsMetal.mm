#include "LLGI.GraphicsMetal.h"
#include "LLGI.CommandListMetal.h"
#include "LLGI.BufferMetal.h"
#include "LLGI.Metal_Impl.h"
#include "LLGI.PipelineStateMetal.h"
#include "LLGI.RenderPassMetal.h"
#include "LLGI.ShaderMetal.h"
#include "LLGI.SingleFrameMemoryPoolMetal.h"
#include "LLGI.TextureMetal.h"
#import <MetalKit/MetalKit.h>

#include <TargetConditionals.h>

namespace LLGI
{

GraphicsMetal::GraphicsMetal() {}

GraphicsMetal::~GraphicsMetal()
{
	for (auto cb : executingCommandList_)
	{
		cb->Release();
	}
	executingCommandList_.clear();

	renderPassPipelineStates_.clear();

	if (commandQueue_ != nullptr)
	{
		[commandQueue_ release];
	}

	if (device_ != nullptr)
	{
		[device_ release];
	}
}

bool GraphicsMetal::Initialize(std::function<GraphicsView()> getGraphicsView)
{
	getGraphicsView_ = getGraphicsView;

	device_ = MTLCreateSystemDefaultDevice();
	commandQueue_ = [device_ newCommandQueue];

	maxMultiSamplingCount_ = 0;
	int testSampleCounts[] = {8, 4, 2, 1};
	for (int count : testSampleCounts)
	{
		bool supported = [device_ supportsTextureSampleCount:count];
		if (supported)
		{
			maxMultiSamplingCount_ = count;
			break;
		}
	}
	if (maxMultiSamplingCount_ == 0)
		throw "Unsupported.";

	renderPass_ = CreateSharedPtr(new RenderPassMetal());

	return true;
}

void GraphicsMetal::SetWindowSize(const Vec2I& windowSize) { throw "Not inplemented"; }

void GraphicsMetal::Execute(CommandList* commandList)
{
	// remove finished commands
	auto it = std::remove_if(executingCommandList_.begin(),
							 executingCommandList_.end(),
							 [](CommandList* cb)
							 {
								 auto c = static_cast<CommandListMetal*>(cb);
								 if (c->GetIsCompleted())
								 {
									 cb->Release();
									 return true;
								 }
								 return false;
							 });

	executingCommandList_.erase(it, executingCommandList_.end());

	auto commandList_ = (CommandListMetal*)commandList;
	commandList_->ResetCompleted();
	[commandList_->GetCommandBuffer() commit];

	SafeAddRef(commandList);
	executingCommandList_.push_back(commandList);
}

void GraphicsMetal::WaitFinish()
{

	for (auto cb : executingCommandList_)
	{
		auto c = static_cast<CommandListMetal*>(cb);
		c->WaitUntilCompleted();
		cb->Release();
	}
	executingCommandList_.clear();
}

/*
RenderPass* GraphicsMetal::GetCurrentScreen(const Color8& clearColor, bool isColorCleared, bool isDepthCleared)
{
	assert(false);

	if (getGraphicsView_ != nullptr)
	{
		auto view = getGraphicsView_();
		impl->drawable = view.drawable;
	}

	renderPass_->SetClearColor(clearColor);
	renderPass_->SetIsColorCleared(isColorCleared);
	renderPass_->SetIsDepthCleared(isDepthCleared);
	//renderPass_->UpdateTarget(this);

	return renderPass_.get();
}
*/

Buffer* GraphicsMetal::CreateBuffer(BufferUsageType usage, int32_t size)
{
    auto obj = new BufferMetal();
    if (obj->Initialize(this, usage, size))
    {
        return obj;
    }

    SafeRelease(obj);
    return nullptr;
}

Shader* GraphicsMetal::CreateShader(DataStructure* data, int32_t count)
{
	auto shader = new ShaderMetal();
	if (shader->Initialize(this, data, count))
	{
		return shader;
	}

	SafeRelease(shader);
	return nullptr;
}

PipelineState* GraphicsMetal::CreatePiplineState()
{
	auto pipelineState = new PipelineStateMetal();
	if (pipelineState->Initialize(this))
	{
		return pipelineState;
	}

	SafeRelease(pipelineState);
	return nullptr;
}

SingleFrameMemoryPool* GraphicsMetal::CreateSingleFrameMemoryPool(int32_t constantBufferPoolSize, int32_t drawingCount)
{
	return new SingleFrameMemoryPoolMetal(this, false, constantBufferPoolSize, drawingCount);
}

CommandList* GraphicsMetal::CreateCommandList(SingleFrameMemoryPool* memoryPool) { return new CommandListMetal(this); }

RenderPass* GraphicsMetal::CreateRenderPass(Texture** textures, int32_t textureCount, Texture* depthTexture)
{
	auto renderPass = new RenderPassMetal();

	if (!renderPass->UpdateRenderTarget((Texture**)textures, textureCount, depthTexture, nullptr, nullptr))
	{
		SafeRelease(renderPass);
		return nullptr;
	}

	return renderPass;
}

RenderPass*
GraphicsMetal::CreateRenderPass(Texture* texture, Texture* resolvedTexture, Texture* depthTexture, Texture* resolvedDepthTexture)
{
	auto renderPass = new RenderPassMetal();

	std::array<Texture*, 1> textures;
	textures[0] = const_cast<Texture*>(texture);

	if (!renderPass->UpdateRenderTarget(
			textures.data(), 1, (Texture*)depthTexture, (Texture*)resolvedTexture, (Texture*)resolvedDepthTexture))
	{
		SafeRelease(renderPass);
		return nullptr;
	}

	return renderPass;
}

Texture* GraphicsMetal::CreateTexture(const TextureParameter& parameter)
{
	auto obj = new TextureMetal();
	if (!obj->Initialize(this, parameter))
	{
		SafeRelease(obj);
		return nullptr;
	}
	return obj;
}

Texture* GraphicsMetal::CreateTexture(const TextureInitializationParameter& parameter)
{
	TextureParameter param;
	param.Dimension = 2;
	param.Format = parameter.Format;
	param.MipLevelCount = parameter.MipMapCount;
	param.SampleCount = 1;
	param.Size = {parameter.Size.X, parameter.Size.Y, 1};
	return CreateTexture(param);
}

Texture* GraphicsMetal::CreateRenderTexture(const RenderTextureInitializationParameter& parameter)
{
	TextureParameter param;
	param.Dimension = 2;
	param.Format = parameter.Format;
	param.MipLevelCount = 1;
	param.SampleCount = parameter.SamplingCount;
	param.Size = {parameter.Size.X, parameter.Size.Y, 1};
	param.Usage = TextureUsageType::RenderTarget;
	return CreateTexture(param);
}

Texture* GraphicsMetal::CreateDepthTexture(const DepthTextureInitializationParameter& parameter)
{
	auto format = TextureFormatType::D32;
	if (parameter.Mode == DepthTextureMode::DepthStencil)
	{
		format = TextureFormatType::D24S8;

		if (!GetDevice().isDepth24Stencil8PixelFormatSupported)
		{
			return nullptr;
		}
	}

	TextureParameter param;
	param.Dimension = 2;
	param.Format = format;
	param.MipLevelCount = 1;
	param.SampleCount = parameter.SamplingCount;
	param.Size = {parameter.Size.X, parameter.Size.Y, 1};
	return CreateTexture(param);
}

Texture* GraphicsMetal::CreateTexture(uint64_t texid)
{

	auto o = new TextureMetal();
	if (o->Initialize(this, id<MTLTexture>(texid)))
	{
		return o;
	}

	SafeRelease(o);
	return nullptr;
}

RenderPassPipelineState* GraphicsMetal::CreateRenderPassPipelineState(RenderPass* renderPass)
{
	return CreateRenderPassPipelineState(renderPass->GetKey());
}

RenderPassPipelineState* GraphicsMetal::CreateRenderPassPipelineState(const RenderPassPipelineStateKey& key)
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

	std::shared_ptr<RenderPassPipelineStateMetal> ret = LLGI::CreateSharedPtr<>(new RenderPassPipelineStateMetal());
	ret->SetKey(key);

	renderPassPipelineStates_[key] = ret;

	{
		auto ptr = ret.get();
		SafeAddRef(ptr);
		return ptr;
	}
}

std::vector<uint8_t> GraphicsMetal::CaptureRenderTarget(Texture* renderTarget)
{
	auto metalTexture = static_cast<TextureMetal*>(renderTarget);
	auto width = metalTexture->GetSizeAs2D().X;
	auto height = metalTexture->GetSizeAs2D().Y;
	auto& texture = metalTexture->GetTexture();

	id<MTLCommandQueue> queue = [this->device_ newCommandQueue];
	id<MTLCommandBuffer> commandBuffer = [queue commandBuffer];
	id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];

#if !(TARGET_OS_IPHONE) && !(TARGET_OS_SIMULATOR)
	[blitEncoder synchronizeTexture:texture slice:0 level:0];
#endif
	[blitEncoder endEncoding];

	[commandBuffer commit];
	[commandBuffer waitUntilCompleted];

	NSUInteger bytesPerPixel =
		GetTextureMemorySize(renderTarget->GetFormat(), Vec3I{renderTarget->GetSizeAs2D().X, renderTarget->GetSizeAs2D().Y, 1}) / width /
		height;
	NSUInteger imageByteCount = width * height * bytesPerPixel;
	NSUInteger bytesPerRow = width * bytesPerPixel;
	MTLRegion region = MTLRegionMake2D(0, 0, width, height);

	std::vector<uint8_t> data;
	data.resize(imageByteCount);
	[texture getBytes:data.data() bytesPerRow:bytesPerRow fromRegion:region mipmapLevel:0];

	return data;
}

id<MTLDevice>& GraphicsMetal::GetDevice() { return device_; }

id<MTLCommandQueue>& GraphicsMetal::GetCommandQueue() { return commandQueue_; }

}
