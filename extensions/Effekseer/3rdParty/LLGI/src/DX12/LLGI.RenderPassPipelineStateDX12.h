#pragma once

#include "../LLGI.Graphics.h"
#include "LLGI.BaseDX12.h"
#include <functional>
#include <unordered_map>

namespace LLGI
{

class GraphicsDX12;
class RenderPassDX12;

class RenderPassPipelineStateDX12 : public RenderPassPipelineState
{
private:
public:
	RenderPassPipelineStateDX12() = default;
	~RenderPassPipelineStateDX12() override = default;
};

} // namespace LLGI
