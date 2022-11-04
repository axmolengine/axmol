
#pragma once

#include "LLGI.Base.h"

namespace LLGI
{
static constexpr int NumTexture = TextureSlotMax;
static constexpr int NumComputeBuffer = TextureSlotMax;

class VertexBuffer;
class IndexBuffer;

/**
	@brief	command list
	@note
	CommandList has a swap buffer. So you don't need to consider buffering.
	CommandList has references related to rendering.
	References are released after the number of swapCount has passed.

	Limitation :
	Begin and End are need to call only once in one frame.
	Command list must not be released after finishing rendering.
*/
class CommandList : public ReferenceObject
{
protected:
	struct BindingVertexBuffer
	{
		Buffer* vertexBuffer = nullptr;
		int32_t stride = 0;
		int32_t offset = 0;
	};

	struct BindingIndexBuffer
	{
		Buffer* indexBuffer = nullptr;
		int32_t stride = 0;
		int32_t offset = 0;
	};

	struct BindingTexture
	{
		Texture* texture = nullptr;
		TextureWrapMode wrapMode = TextureWrapMode::Clamp;
		TextureMinMagFilter minMagFilter = TextureMinMagFilter::Nearest;
	};

	struct BindingComputeBuffer
	{
		Buffer* computeBuffer = nullptr;
		int32_t stride = 0;
	};

private:
	struct SwapObject
	{
		std::vector<ReferenceObject*> referencedObjects;
	};

	int32_t swapIndex_ = -1;
	int32_t swapCount_ = 0;
	std::vector<SwapObject> swapObjects;

	BindingVertexBuffer bindingVertexBuffer;
	BindingIndexBuffer bindingIndexBuffer;

	PipelineState* currentPipelineState = nullptr;

	bool isVertexBufferDirtied = true;
	bool isCurrentIndexBufferDirtied = true;
	bool isPipelineDirtied = true;
	bool doesBeginWithPlatform_ = false;

	std::array<Buffer*, static_cast<int>(ShaderStageType::Max)> constantBuffers;
	std::array<std::array<BindingComputeBuffer, NumComputeBuffer>, static_cast<int>(ShaderStageType::Max)> computeBuffers_;

protected:
	bool isInRenderPass_ = false;
	bool isInBegin_ = false;

	std::array<std::array<BindingTexture, NumTexture>, 2> currentTextures;

protected:
	void GetCurrentVertexBuffer(BindingVertexBuffer& buffer, bool& isDirtied);
	void GetCurrentIndexBuffer(BindingIndexBuffer& buffer, bool& isDirtied);
	void GetCurrentPipelineState(PipelineState*& pipelineState, bool& isDirtied);
	void GetCurrentConstantBuffer(ShaderStageType type, Buffer*& buffer);
	void GetCurrentComputeBuffer(int32_t unit, ShaderStageType shaderStage, BindingComputeBuffer& buffer);
	void RegisterReferencedObject(ReferenceObject* referencedObject);

public:
	CommandList(int32_t swapCount = 3);
	~CommandList() override;

	/**
	@brief
	added a command into supecified context.
	This function can be called once by a frame.
	*/
	virtual void Begin();

	/**
		@brief
		added a command into supecified context. This function is supported in some platform.
		Internal context is not used if platformContextPtr is null in Metal.
		This function can be called once by a frame.
	*/
	virtual bool BeginWithPlatform(void* platformContextPtr);

	virtual void End();
	virtual void EndWithPlatform();

	virtual void SetScissor(int32_t x, int32_t y, int32_t width, int32_t height);
	virtual void Draw(int32_t primitiveCount, int32_t instanceCount = 1);
	virtual void SetVertexBuffer(Buffer* vertexBuffer, int32_t stride, int32_t offset);
	virtual void SetIndexBuffer(Buffer* indexBuffer, int32_t stride, int32_t offset = 0);
	virtual void SetPipelineState(PipelineState* pipelineState);
	virtual void SetConstantBuffer(Buffer* constantBuffer, ShaderStageType shaderStage);
	virtual void SetComputeBuffer(Buffer* computeBuffer, int32_t stride, int32_t unit, ShaderStageType shaderStage);

	/**
		@brief	copy a texture
	*/
	virtual void CopyTexture(Texture* src, Texture* dst) {}

	/**
		@brief	copy a texture
	*/
	virtual void
	CopyTexture(Texture* src, Texture* dst, const Vec3I& srcPos, const Vec3I& dstPos, const Vec3I& size, int srcLayer, int dstLayer)
	{
	}

	/**
		@brief specify textures
		@note
		shaderStage is ignored in DirectX12 (common textures are used on all stages)
	*/
	virtual void
	SetTexture(Texture* texture, TextureWrapMode wrapMode, TextureMinMagFilter minmagFilter, int32_t unit, ShaderStageType shaderStage);

	/**
		@brief generate mipmap
		@note
		use this method before use mipmap required texture.
	*/
	virtual void GenerateMipMap(Texture* src) {}

	/**
		@brief	reset textures and set null.
	*/
	virtual void ResetTextures();

	virtual void BeginRenderPass(RenderPass* renderPass);

	/**
		@brief
		added a command into supecified renderpass. This function is supported in some platform.
	*/
	virtual bool BeginRenderPassWithPlatformPtr(void* platformPtr);

	virtual void EndRenderPass() { isInRenderPass_ = false; }

	/**
		@brief
		The pair of BeginRenderPassWithPlatformPtr
	*/
	virtual bool EndRenderPassWithPlatformPtr() { return false; }

	virtual void ResetComputeBuffer();
	virtual void BeginComputePass() {}
	virtual void EndComputePass() {}
	virtual void Dispatch(int32_t groupX, int32_t groupY, int32_t groupZ, int32_t threadX, int32_t threadY, int32_t threadZ);

	virtual void CopyBuffer(Buffer* src, Buffer* dst) {}

	/**
		@brief	send a memory in specified texture from cpu to gpu
	*/
	virtual void SetImageData2D(Texture* texture, int32_t x, int32_t y, int32_t width, int32_t height, const void* data);

	/**
		@brief wait until this command is completed.
	*/
	virtual void WaitUntilCompleted();

	bool GetIsInRenderPass() const;
};

} // namespace LLGI
