
#pragma once

#include "../LLGI.PipelineState.h"
#include "LLGI.BaseVulkan.h"
#include "LLGI.GraphicsVulkan.h"

namespace LLGI
{

class PipelineStateVulkan : public PipelineState
{
private:
	GraphicsVulkan* graphics_ = nullptr;
	std::array<Shader*, static_cast<int>(ShaderStageType::Max)> shaders;

	vk::Pipeline pipeline_ = nullptr;
	vk::PipelineLayout pipelineLayout_ = nullptr;
	std::array<vk::DescriptorSetLayout, 2> descriptorSetLayouts;

	vk::Pipeline computePipeline_ = nullptr;
	vk::PipelineLayout computePipelineLayout_ = nullptr;
	std::array<vk::DescriptorSetLayout, 2> computeDescriptorSetLayouts;

	bool CreateGraphicsPipeline();
	bool CreateComputePipeline();

public:
	PipelineStateVulkan();
	~PipelineStateVulkan() override;

	bool Initialize(GraphicsVulkan* graphics);

	void SetShader(ShaderStageType stage, Shader* shader) override;

	bool Compile() override;

	vk::Pipeline GetPipeline() const { return pipeline_; }

	vk::PipelineLayout GetPipelineLayout() const { return pipelineLayout_; }

	const std::array<vk::DescriptorSetLayout, 2>& GetDescriptorSetLayout() const { return descriptorSetLayouts; }
	
	vk::Pipeline GetComputePipeline() const { return computePipeline_; }

	vk::PipelineLayout GetComputePipelineLayout() const { return computePipelineLayout_; }

	const std::array<vk::DescriptorSetLayout, 2>& GetComputeDescriptorSetLayout() const { return computeDescriptorSetLayouts; }
};

} // namespace LLGI
