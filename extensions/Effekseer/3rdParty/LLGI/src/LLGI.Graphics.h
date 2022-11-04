
#pragma once

#include "LLGI.Base.h"
#include "Utils/LLGI.FixedSizeVector.h"
#include <functional>
#include <unordered_map>

namespace LLGI
{
class CommandListPool;

struct TextureParameter
{
	TextureUsageType Usage = TextureUsageType::NoneFlag;
	TextureFormatType Format = TextureFormatType::R8G8B8A8_UNORM;
	int32_t Dimension = 2;
	Vec3I Size = Vec3I{1, 1, 1};
	int32_t MipLevelCount = 1;
	int SampleCount = 1;
};

struct TextureInitializationParameter
{
	Vec2I Size;
	TextureFormatType Format = TextureFormatType::R8G8B8A8_UNORM;
	int MipMapCount = 1;
};

struct RenderTextureInitializationParameter
{
	Vec2I Size;
	TextureFormatType Format = TextureFormatType::R8G8B8A8_UNORM;
	int32_t SamplingCount = 1;
};

enum class DepthTextureMode
{
	Depth,
	DepthStencil,
};

struct DepthTextureInitializationParameter
{
	Vec2I Size;
	int32_t SamplingCount = 1;
	DepthTextureMode Mode = DepthTextureMode::Depth;
};

/**
	@brief	provide a memory which is available in one frame
*/
class SingleFrameMemoryPool : public ReferenceObject
{
protected:
	int32_t currentSwapBuffer_ = -1;
	int32_t swapBufferCount_ = 0;
	std::vector<int32_t> offsets_;
	std::vector<std::vector<Buffer*>> buffers_;

	/**
		@brief	create buffer
	*/
	virtual Buffer* CreateBufferInternal(int32_t size) { return nullptr; }

	/**
		@brief	reinitialize buffer with a size
	*/
	virtual Buffer* ReinitializeBuffer(Buffer* cb, int32_t size) { return nullptr; }

public:
	SingleFrameMemoryPool(int32_t swapBufferCount = 3);
	~SingleFrameMemoryPool() override;

	/**
	@brief	Start new frame
	*/
	virtual void NewFrame();

	virtual Buffer* CreateConstantBuffer(int32_t size);
};

struct RenderPassPipelineStateKey
{
	bool IsPresent = false;
	TextureFormatType DepthFormat = TextureFormatType::Unknown;
	FixedSizeVector<TextureFormatType, RenderTargetMax> RenderTargetFormats;
	bool IsColorCleared = true;
	bool IsDepthCleared = true;
	bool HasResolvedRenderTarget = false;
	bool HasResolvedDepthTarget = false;
	int32_t SamplingCount = 1;

	bool operator==(const RenderPassPipelineStateKey& value) const
	{
		if (RenderTargetFormats.size() != value.RenderTargetFormats.size())
			return false;

		for (size_t i = 0; i < RenderTargetFormats.size(); i++)
		{
			if (RenderTargetFormats.at(i) != value.RenderTargetFormats.at(i))
				return false;
		}

		return (IsPresent == value.IsPresent && DepthFormat == value.DepthFormat && IsColorCleared == value.IsColorCleared &&
				IsDepthCleared == value.IsDepthCleared && SamplingCount == value.SamplingCount &&
				HasResolvedRenderTarget == value.HasResolvedRenderTarget && HasResolvedDepthTarget == value.HasResolvedDepthTarget);
	}

	struct Hash
	{
		typedef std::size_t result_type;

		std::size_t operator()(const RenderPassPipelineStateKey& key) const
		{
			auto ret = std::hash<bool>()(key.IsPresent);
			ret += std::hash<TextureFormatType>()(key.DepthFormat);
			ret += std::hash<bool>()(key.IsColorCleared);
			ret += std::hash<bool>()(key.IsDepthCleared);
			ret += std::hash<int32_t>()(key.SamplingCount);
			ret += std::hash<bool>()(key.HasResolvedRenderTarget);
			ret += std::hash<bool>()(key.HasResolvedDepthTarget);

			for (size_t i = 0; i < key.RenderTargetFormats.size(); i++)
			{
				ret += std::hash<uint64_t>()(static_cast<uint64_t>(key.RenderTargetFormats.at(i)));
			}

			return ret;
		}
	};
};

class RenderPass : public ReferenceObject
{
private:
	bool isColorCleared_ = false;

	bool isDepthCleared_ = false;

	Color8 color_;

	FixedSizeVector<Texture*, RenderTargetMax> renderTextures_;
	Texture* depthTexture_ = nullptr;
	Texture* resolvedRenderTexture_ = nullptr;
	Texture* resolvedDepthTexture_ = nullptr;

protected:
	Vec2I screenSize_;

	bool assignRenderTextures(Texture** textures, int32_t count);
	bool assignDepthTexture(Texture* depthTexture);
	bool assignResolvedRenderTexture(Texture* texture);
	bool assignResolvedDepthTexture(Texture* texture);

	bool getSize(Vec2I& size,
				 const Texture** textures,
				 int32_t textureCount,
				 Texture* depthTexture,
				 Texture* resolvedRenderTexture = nullptr,
				 Texture* resolvedDepthTexture = nullptr) const;
	bool sanitize();

public:
	RenderPass() = default;
	~RenderPass() override;

	virtual bool GetIsColorCleared() const { return isColorCleared_; }

	virtual bool GetIsDepthCleared() const { return isDepthCleared_; }

	virtual Color8 GetClearColor() const { return color_; }

	virtual void SetIsColorCleared(bool isColorCleared);

	virtual void SetIsDepthCleared(bool isDepthCleared);

	virtual void SetClearColor(const Color8& color);

	virtual Texture* GetRenderTexture(int index) const { return renderTextures_.at(index); }

	virtual int GetRenderTextureCount() const { return static_cast<int32_t>(renderTextures_.size()); }

	Texture* GetDepthTexture() const { return depthTexture_; }

	bool GetHasDepthTexture() const { return GetDepthTexture() != nullptr; }

	Texture* GetResolvedRenderTexture() const { return resolvedRenderTexture_; }

	Texture* GetResolvedDepthTexture() const { return resolvedDepthTexture_; }

	virtual bool GetIsSwapchainScreen() const;

	virtual Vec2I GetScreenSize() const { return screenSize_; }

	RenderPassPipelineStateKey GetKey() const;
};

/**
	@brief	A class to send a data for RenderPass
*/
class RenderPassPipelineState : public ReferenceObject
{
private:
public:
	RenderPassPipelineState() = default;
	~RenderPassPipelineState() override = default;
	RenderPassPipelineStateKey Key;
};

/**
	@note
	please call WaitFinish before releasing
*/
class Graphics : public ReferenceObject
{
protected:
	Vec2I windowSize_;
	std::function<void()> disposed_;

public:
	Graphics() = default;
	~Graphics() override;

	/*[[deprecated("use Platform::SetWindowSize.")]]*/ virtual void SetWindowSize(const Vec2I& windowSize);

	/**
		@brief	Execute commands
		@note
		Don't release before finish executing commands.
	*/
	virtual void Execute(CommandList* commandList);

	/**
	@brief	to prevent instances to be disposed before finish rendering, finish all renderings.
	*/
	virtual void WaitFinish() {}

	virtual Buffer* CreateBuffer(BufferUsageType usage ,int32_t size);

	virtual Shader* CreateShader(DataStructure* data, int32_t count);
	virtual PipelineState* CreatePiplineState();

	/**
		@brief create a memory pool
		@param  drawingCount(drawingCount is ignored in DirectX12)
	*/
	virtual SingleFrameMemoryPool* CreateSingleFrameMemoryPool(int32_t constantBufferPoolSize, int32_t drawingCount);

	/**
		@brief
		@param memoryPool if memory pool is null, allocate memory from graphics
	*/
	virtual CommandList* CreateCommandList(SingleFrameMemoryPool* memoryPool);

	virtual RenderPass* CreateRenderPass(Texture** textures, int32_t textureCount, Texture* depthTexture) { return nullptr; }

	virtual RenderPass* CreateRenderPass(Texture* texture, Texture* resolvedTexture, Texture* depthTexture, Texture* resolvedDepthTexture)
	{
		return nullptr;
	}

	virtual Texture* CreateTexture(const TextureParameter& parameter) { return nullptr; }

	virtual Texture* CreateTexture(const TextureInitializationParameter& parameter) { return nullptr; }

	virtual Texture* CreateRenderTexture(const RenderTextureInitializationParameter& parameter) { return nullptr; }

	virtual Texture* CreateDepthTexture(const DepthTextureInitializationParameter& parameter) { return nullptr; }

	/**
		@brief	create texture from pointer or id in current platform
	*/
	virtual Texture* CreateTexture(uint64_t id);

	/**
		@brief	create a RenderPassPipelineState
		@note
		This is a function to create an object.
		But it is very fast. So it can call it in everyframe.
	*/
	virtual RenderPassPipelineState* CreateRenderPassPipelineState(RenderPass* renderPass);

	/**
		@brief	create a RenderPassPipelineState
		@note
		This is a function to create an object.
		But it is very fast. So it can call it in everyframe.
	*/
	virtual RenderPassPipelineState* CreateRenderPassPipelineState(const RenderPassPipelineStateKey& key) { return nullptr; }

	/** For testing. Wait for all commands in queue to complete. Then read data from specified render target. */
	virtual std::vector<uint8_t> CaptureRenderTarget(Texture* renderTarget);

	/**
		@brief	specify a function which is called when this instance is disposed.
		@param	disposed	called function
	*/
	void SetDisposed(const std::function<void()>& disposed);

	virtual bool IsResolvedDepthSupported() const { return false; }
};

} // namespace LLGI
