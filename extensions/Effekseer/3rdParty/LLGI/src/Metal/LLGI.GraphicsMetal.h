#pragma once

#include "../LLGI.Graphics.h"
#include "../Utils/LLGI.FixedSizeVector.h"
#import <MetalKit/MetalKit.h>
#include <functional>
#include <unordered_map>

namespace LLGI
{

class GraphicsMetal;
class RenderPassMetal;
class RenderPassPipelineStateMetal;
class TextureMetal;

struct GraphicsView
{
	id<CAMetalDrawable> drawable;
};

class GraphicsMetal : public Graphics
{
	//! cached
	std::unordered_map<RenderPassPipelineStateKey, std::shared_ptr<RenderPassPipelineStateMetal>, RenderPassPipelineStateKey::Hash>
		renderPassPipelineStates_;

	std::shared_ptr<RenderPassMetal> renderPass_ = nullptr;
	std::function<GraphicsView()> getGraphicsView_;
	std::vector<CommandList*> executingCommandList_;

	id<MTLDevice> device_ = nullptr;
	id<MTLCommandQueue> commandQueue_ = nullptr;
	int maxMultiSamplingCount_ = 0;

public:
	GraphicsMetal();
	~GraphicsMetal() override;

	bool Initialize(std::function<GraphicsView()> getGraphicsView);

	void SetWindowSize(const Vec2I& windowSize) override;

	void Execute(CommandList* commandList) override;

	void WaitFinish() override;
    
    Buffer* CreateBuffer(BufferUsageType usage, int32_t size) override;

	Shader* CreateShader(DataStructure* data, int32_t count) override;

	PipelineState* CreatePiplineState() override;

	SingleFrameMemoryPool* CreateSingleFrameMemoryPool(int32_t constantBufferPoolSize, int32_t drawingCount) override;

	CommandList* CreateCommandList(SingleFrameMemoryPool* memoryPool) override;

	RenderPass* CreateRenderPass(Texture** textures, int32_t textureCount, Texture* depthTexture) override;

	RenderPass* CreateRenderPass(Texture* texture, Texture* resolvedTexture, Texture* depthTexture, Texture* resolvedDepthTexture) override;

	Texture* CreateTexture(const TextureParameter& parameter) override;

	Texture* CreateTexture(const TextureInitializationParameter& parameter) override;

	Texture* CreateRenderTexture(const RenderTextureInitializationParameter& parameter) override;

	Texture* CreateDepthTexture(const DepthTextureInitializationParameter& parameter) override;

	Texture* CreateTexture(uint64_t texid) override;

	RenderPassPipelineState* CreateRenderPassPipelineState(RenderPass* renderPass) override;

	RenderPassPipelineState* CreateRenderPassPipelineState(const RenderPassPipelineStateKey& key) override;

	std::vector<uint8_t> CaptureRenderTarget(Texture* renderTarget) override;

	RenderPassMetal* GetRenderPass() const { return renderPass_.get(); }

	id<MTLDevice>& GetDevice();

	id<MTLCommandQueue>& GetCommandQueue();
};

} // namespace LLGI
