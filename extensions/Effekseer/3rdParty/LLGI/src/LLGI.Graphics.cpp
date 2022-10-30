#include "LLGI.Graphics.h"
#include "LLGI.Buffer.h"
#include "LLGI.Texture.h"

namespace LLGI
{

//! TODO should be moved
static std::function<void(LogType, const std::string&)> g_logger;

void SetLogger(const std::function<void(LogType, const std::string&)>& logger) { g_logger = logger; }

void Log(LogType logType, const std::string& message)
{
	if (g_logger != nullptr)
	{
		g_logger(logType, message);
	}
}

SingleFrameMemoryPool::SingleFrameMemoryPool(int32_t swapBufferCount) : swapBufferCount_(swapBufferCount)
{

	for (int i = 0; i < swapBufferCount_; i++)
	{
		offsets_.push_back(0);
		buffers_.push_back(std::vector<Buffer*>());
	}
}

SingleFrameMemoryPool::~SingleFrameMemoryPool()
{
	for (auto& buffer : buffers_)
	{
		for (auto c : buffer)
		{
			c->Release();
		}
	}
}

void SingleFrameMemoryPool::NewFrame()
{
	currentSwapBuffer_++;
	currentSwapBuffer_ %= swapBufferCount_;
	offsets_[currentSwapBuffer_] = 0;
}

Buffer* SingleFrameMemoryPool::CreateConstantBuffer(int32_t size)
{
	assert(currentSwapBuffer_ >= 0);

	if (static_cast<int32_t>(buffers_[currentSwapBuffer_].size()) <= offsets_[currentSwapBuffer_])
	{
		auto cb = CreateBufferInternal(size);
		if (cb == nullptr)
		{
			return nullptr;
		}

		buffers_[currentSwapBuffer_].push_back(cb);
		SafeAddRef(cb);
		offsets_[currentSwapBuffer_]++;
		return cb;
	}
	else
	{
		auto cb = buffers_[currentSwapBuffer_][offsets_[currentSwapBuffer_]];
		auto newCb = ReinitializeBuffer(cb, size);
		if (newCb == nullptr)
		{
			return nullptr;
		}

		SafeAddRef(newCb);
		offsets_[currentSwapBuffer_]++;
		return newCb;
	}
}

bool RenderPass::assignRenderTextures(Texture** textures, int32_t count)
{
	for (int32_t i = 0; i < count; i++)
	{
		if (!(textures[i]->GetType() == TextureType::Render || textures[i]->GetType() == TextureType::Screen))
		{
			Log(LogType::Error, "RenderPass : Invalid RenderTexture.");
			return false;
		}
	}

	for (int32_t i = 0; i < count; i++)
	{
		SafeAddRef(textures[i]);
	}

	for (int32_t i = 0; i < static_cast<int32_t>(renderTextures_.size()); i++)
	{
		renderTextures_.at(i)->Release();
	}

	renderTextures_.resize(count);

	for (int32_t i = 0; i < count; i++)
	{
		renderTextures_.at(i) = textures[i];
	}

	return true;
}

bool RenderPass::assignDepthTexture(Texture* depthTexture)
{
	if (depthTexture != nullptr && depthTexture->GetType() != TextureType::Depth)
	{
		Log(LogType::Error, "RenderPass : Invalid DepthTexture.");
		return false;
	}

	SafeAddRef(depthTexture);
	SafeRelease(depthTexture_);
	depthTexture_ = depthTexture;

	return true;
}

bool RenderPass::assignResolvedRenderTexture(Texture* texture)
{
	if (resolvedRenderTexture_ != nullptr && resolvedRenderTexture_->GetType() != TextureType::Render)
	{
		Log(LogType::Error, "RenderPass : Invalid ResolvedTexture.");
		return false;
	}

	SafeAddRef(texture);
	SafeRelease(resolvedRenderTexture_);
	resolvedRenderTexture_ = texture;

	return true;
}

bool RenderPass::assignResolvedDepthTexture(Texture* texture)
{
	if (resolvedDepthTexture_ != nullptr && resolvedDepthTexture_->GetType() != TextureType::Depth)
	{
		Log(LogType::Error, "RenderPass : Invalid ResolvedTexture.");
		return false;
	}

	SafeAddRef(texture);
	SafeRelease(resolvedDepthTexture_);
	resolvedDepthTexture_ = texture;

	return true;
}

bool RenderPass::getSize(Vec2I& size,
						 const Texture** textures,
						 int32_t textureCount,
						 Texture* depthTexture,
						 Texture* resolvedRenderTexture,
						 Texture* resolvedDepthTexture) const
{
	if (textureCount == 0)
	{
		Log(LogType::Error, "RenderPass : Invalid Count.");
		return false;
	}

	size = textures[0]->GetSizeAs2D();

	for (int i = 0; i < textureCount; i++)
	{
		auto temp = textures[i]->GetSizeAs2D();
		if (size.X != temp.X)
		{
			auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : "));
			::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
			goto FAIL;
		}
		if (size.Y != temp.Y)
		{
			auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : "));
			::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
			goto FAIL;
		}
	}

	if (depthTexture != nullptr)
	{
		if (size.X != depthTexture->GetSizeAs2D().X)
		{
			auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : "));
			::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
			goto FAIL;
		}
		if (size.Y != depthTexture->GetSizeAs2D().Y)
		{
			auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : "));
			::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
			goto FAIL;
		}
	}

	if (resolvedRenderTexture != nullptr)
	{
		if (size.X != resolvedRenderTexture->GetSizeAs2D().X)
		{
			auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : "));
			::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
			goto FAIL;
		}

		if (size.Y != resolvedRenderTexture->GetSizeAs2D().Y)
		{
			auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : "));
			::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
			goto FAIL;
		}
	}

	if (resolvedDepthTexture != nullptr)
	{
		if (size.X != resolvedDepthTexture->GetSizeAs2D().X)
		{
			auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : "));
			::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
			goto FAIL;
		}
		if (size.Y != resolvedDepthTexture->GetSizeAs2D().Y)
		{
			auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : "));
			::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
			goto FAIL;
		}
	}

	return true;

FAIL:;
	Log(LogType::Error, "RenderPass : Invalid Size.");
	return false;
}

bool RenderPass::sanitize()
{

	if (resolvedRenderTexture_ != nullptr)
	{
		if (renderTextures_.size() != 1)
		{
			Log(LogType::Error, "RenderPass : Invalid Size.");
			return false;
		}

		if (renderTextures_.at(0)->GetFormat() != resolvedRenderTexture_->GetFormat())
		{
			Log(LogType::Error, "RenderPass : Formats are not same between Render and Resolved.");
			return false;
		}
	}

	if (resolvedDepthTexture_ != nullptr)
	{
		if (depthTexture_ == nullptr)
		{
			Log(LogType::Error, "RenderPass : Require a depth texture.");
			return false;
		}

		if (depthTexture_->GetFormat() != resolvedDepthTexture_->GetFormat())
		{
			Log(LogType::Error, "RenderPass : Formats are not same between Render and Resolved.");
			return false;
		}
	}

	if (depthTexture_ != nullptr && renderTextures_.at(0)->GetSamplingCount() != depthTexture_->GetSamplingCount())
	{
		Log(LogType::Error, "RenderPass : SamplingCount are not same.");
		return false;
	}

	return true;
}

RenderPass::~RenderPass()
{
	SafeRelease(depthTexture_);
	SafeRelease(resolvedRenderTexture_);
	SafeRelease(resolvedDepthTexture_);

	for (size_t i = 0; i < renderTextures_.size(); i++)
	{
		renderTextures_.at(i)->Release();
	}
}

void RenderPass::SetIsColorCleared(bool isColorCleared) { isColorCleared_ = isColorCleared; }

void RenderPass::SetIsDepthCleared(bool isDepthCleared) { isDepthCleared_ = isDepthCleared; }

void RenderPass::SetClearColor(const Color8& color) { color_ = color; }

bool RenderPass::GetIsSwapchainScreen() const { return GetRenderTexture(0)->GetType() == TextureType::Screen; }

RenderPassPipelineStateKey RenderPass::GetKey() const
{
	RenderPassPipelineStateKey key;

	key.IsPresent = GetIsSwapchainScreen();
	key.IsColorCleared = GetIsColorCleared();
	key.IsDepthCleared = GetIsDepthCleared();
	key.RenderTargetFormats.resize(GetRenderTextureCount());
	key.SamplingCount = renderTextures_.at(0)->GetSamplingCount();
	key.HasResolvedRenderTarget = GetResolvedRenderTexture() != nullptr;
	key.HasResolvedDepthTarget = GetResolvedDepthTexture() != nullptr;

	for (size_t i = 0; i < key.RenderTargetFormats.size(); i++)
	{
		key.RenderTargetFormats.at(i) = GetRenderTexture(static_cast<int32_t>(i))->GetFormat();
	}

	if (GetHasDepthTexture())
	{
		key.DepthFormat = GetDepthTexture()->GetFormat();
	}
	else
	{
		key.DepthFormat = TextureFormatType::Unknown;
	}

	return key;
}

Graphics::~Graphics()
{
	if (disposed_ != nullptr)
	{
		disposed_();
	}
}

void Graphics::SetWindowSize(const Vec2I& windowSize) { windowSize_ = windowSize; }

void Graphics::Execute(CommandList* commandList) {}

// RenderPass* Graphics::GetCurrentScreen(const Color8& clearColor, bool isColorCleared, bool isDepthCleared) { return nullptr; }

Buffer* Graphics::CreateBuffer(BufferUsageType usage, int32_t size) { return nullptr; }

Shader* Graphics::CreateShader(DataStructure* data, int32_t count) { return nullptr; }

PipelineState* Graphics::CreatePiplineState() { return nullptr; }

SingleFrameMemoryPool* Graphics::CreateSingleFrameMemoryPool(int32_t constantBufferPoolSize, int32_t drawingCount) { return nullptr; }

CommandList* Graphics::CreateCommandList(SingleFrameMemoryPool* memoryPool) { return nullptr; }

Texture* Graphics::CreateTexture(uint64_t id) { return nullptr; }

RenderPassPipelineState* Graphics::CreateRenderPassPipelineState(RenderPass* renderPass) { return nullptr; }

std::vector<uint8_t> Graphics::CaptureRenderTarget(Texture* renderTarget)
{
	Log(LogType::Error, "GetColorBuffer is not implemented.");
	assert(0);
	return std::vector<uint8_t>();
}

void Graphics::SetDisposed(const std::function<void()>& disposed) { disposed_ = disposed; }

} // namespace LLGI
