
#include "LLGI.PipelineState.h"
#include "LLGI.Graphics.h"

namespace LLGI
{

PipelineState::PipelineState() { VertexLayoutSemantics.fill(0); }

void PipelineState::SetShader(ShaderStageType stage, Shader* shader) {}

void PipelineState::SetRenderPassPipelineState(RenderPassPipelineState* renderPassPipelineState)
{
	SafeAddRef(renderPassPipelineState);
	renderPassPipelineState_ = CreateSharedPtr(renderPassPipelineState);
}

bool PipelineState::Compile() { return false; }

} // namespace LLGI
