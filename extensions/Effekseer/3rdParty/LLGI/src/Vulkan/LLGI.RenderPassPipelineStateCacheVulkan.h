#pragma once

#include "../LLGI.Graphics.h"
#include "../Utils/LLGI.FixedSizeVector.h"
#include "LLGI.BaseVulkan.h"
#include "LLGI.RenderPassVulkan.h"
#include <functional>
#include <unordered_map>

namespace LLGI
{

class RenderPassPipelineStateCacheVulkan : public ReferenceObject
{
private:
	std::unordered_map<RenderPassPipelineStateKey, std::shared_ptr<RenderPassPipelineStateVulkan>, RenderPassPipelineStateKey::Hash>
		renderPassPipelineStates_;

	vk::Device device_;
	ReferenceObject* owner_ = nullptr;

public:
	RenderPassPipelineStateCacheVulkan(vk::Device device, ReferenceObject* owner);
	~RenderPassPipelineStateCacheVulkan() override;

	RenderPassPipelineStateVulkan* Create(const RenderPassPipelineStateKey key);
};

} // namespace LLGI
