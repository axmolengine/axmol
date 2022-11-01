#pragma once

#include "../LLGI.PipelineState.h"
#import <MetalKit/MetalKit.h>
#include <memory>

namespace LLGI
{

class GraphicsMetal;

class PipelineStateMetal : public PipelineState
{
private:
	GraphicsMetal* graphics_ = nullptr;
	std::array<Shader*, static_cast<int>(ShaderStageType::Max)> shaders;

	id<MTLRenderPipelineState> pipelineState_ = nullptr;
	id<MTLDepthStencilState> depthStencilState_ = nullptr;
	id<MTLComputePipelineState> computePipelineState_ = nullptr;

	bool Compile(PipelineState* self, Graphics* graphics);
	bool CreateRenderPipelineState(PipelineState* self, Graphics* graphics);
	bool CreateComputePipelineState(PipelineState* self, Graphics* graphics);

public:
	PipelineStateMetal();
	~PipelineStateMetal() override;

	bool Initialize(GraphicsMetal* graphics);
	void SetShader(ShaderStageType stage, Shader* shader) override;
	bool Compile() override;

	std::array<Shader*, static_cast<int>(ShaderStageType::Max)> GetShaders() const { return shaders; }

	RenderPassPipelineState* GetRenderPassPipelineState() const { return renderPassPipelineState_.get(); }

	id<MTLRenderPipelineState>& GetRenderPipelineState() { return pipelineState_; }

	id<MTLDepthStencilState>& GetDepthStencilState() { return depthStencilState_; }

	id<MTLComputePipelineState>& GetComputePipelineState() { return computePipelineState_; }
};

} // namespace LLGI
