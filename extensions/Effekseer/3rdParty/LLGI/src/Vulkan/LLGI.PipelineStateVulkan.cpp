#include "LLGI.PipelineStateVulkan.h"
#include "LLGI.ShaderVulkan.h"

// for x11
#undef Always

namespace LLGI
{

PipelineStateVulkan::PipelineStateVulkan()
{
	shaders.fill(0);
	for (size_t i = 0; i < descriptorSetLayouts.size(); i++)
	{
		descriptorSetLayouts[i] = nullptr;
	}
}

PipelineStateVulkan ::~PipelineStateVulkan()
{
	for (auto& shader : shaders)
	{
		SafeRelease(shader);
	}

	for (size_t i = 0; i < descriptorSetLayouts.size(); i++)
	{
		graphics_->GetDevice().destroyDescriptorSetLayout(descriptorSetLayouts[i]);
		descriptorSetLayouts[i] = nullptr;
	}

	if (pipelineLayout_)
	{
		graphics_->GetDevice().destroyPipelineLayout(pipelineLayout_);
		pipelineLayout_ = nullptr;
	}

	if (pipeline_)
	{
		graphics_->GetDevice().destroyPipeline(pipeline_);
		pipeline_ = nullptr;
	}

	for (size_t i = 0; i < computeDescriptorSetLayouts.size(); i++)
	{
		graphics_->GetDevice().destroyDescriptorSetLayout(computeDescriptorSetLayouts[i]);
		computeDescriptorSetLayouts[i] = nullptr;
	}

	if (computePipelineLayout_)
	{
		graphics_->GetDevice().destroyPipelineLayout(computePipelineLayout_);
		computePipelineLayout_ = nullptr;
	}

	if (computePipeline_)
	{
		graphics_->GetDevice().destroyPipeline(computePipeline_);
		computePipeline_ = nullptr;
	}

	SafeRelease(graphics_);
}

bool PipelineStateVulkan::Initialize(GraphicsVulkan* graphics)
{
	SafeRelease(graphics_);
	SafeAddRef(graphics);
	graphics_ = graphics;
	return true;
}

void PipelineStateVulkan::SetShader(ShaderStageType stage, Shader* shader)
{

	SafeAddRef(shader);
	SafeRelease(shaders[static_cast<int>(stage)]);
	shaders[static_cast<int>(stage)] = shader;
}

bool PipelineStateVulkan::Compile()
{
	if (shaders[static_cast<int>(ShaderStageType::Compute)] != nullptr)
	{
		return CreateComputePipeline();
	}

	return CreateGraphicsPipeline();
}

bool PipelineStateVulkan::CreateGraphicsPipeline()
{
	if (renderPassPipelineState_ == nullptr)
	{
		return false;
	}

	vk::GraphicsPipelineCreateInfo graphicsPipelineInfo;

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStageInfos;

	// setup shaders
	std::string mainName = "main";

	for (size_t i = 0; i < static_cast<int>(ShaderStageType::Compute); i++)
	{
		auto shader = static_cast<ShaderVulkan*>(shaders[i]);

		if (shader == nullptr)
			return false;

		vk::PipelineShaderStageCreateInfo info;

		if (i == static_cast<int>(ShaderStageType::Vertex))
			info.stage = vk::ShaderStageFlagBits::eVertex;

		if (i == static_cast<int>(ShaderStageType::Pixel))
			info.stage = vk::ShaderStageFlagBits::eFragment;

		info.module = shader->GetShaderModule();
		info.pName = mainName.c_str();
		shaderStageInfos.push_back(info);
	}

	graphicsPipelineInfo.pStages = shaderStageInfos.data();
	graphicsPipelineInfo.stageCount = static_cast<int32_t>(shaderStageInfos.size());

	// setup layouts
	std::vector<vk::VertexInputBindingDescription> bindDescs;
	std::vector<vk::VertexInputAttributeDescription> attribDescs;

	int vertexOffset = 0;
	for (int i = 0; i < VertexLayoutCount; i++)
	{
		vk::VertexInputAttributeDescription attribDesc;

		attribDesc.binding = 0;
		attribDesc.location = i;
		attribDesc.offset = vertexOffset;

		if (VertexLayouts[i] == VertexLayoutFormat::R32G32B32_FLOAT)
		{
			attribDesc.format = vk::Format::eR32G32B32Sfloat;
			vertexOffset += sizeof(float) * 3;
		}
		else if (VertexLayouts[i] == VertexLayoutFormat::R32G32B32A32_FLOAT)
		{
			attribDesc.format = vk::Format::eR32G32B32A32Sfloat;
			vertexOffset += sizeof(float) * 4;
		}
		else if (VertexLayouts[i] == VertexLayoutFormat::R32_FLOAT)
		{
			attribDesc.format = vk::Format::eR32Sfloat;
			vertexOffset += sizeof(float) * 1;
		}
		else if (VertexLayouts[i] == VertexLayoutFormat::R32G32_FLOAT)
		{
			attribDesc.format = vk::Format::eR32G32Sfloat;
			vertexOffset += sizeof(float) * 2;
		}
		else if (VertexLayouts[i] == VertexLayoutFormat::R8G8B8A8_UINT)
		{
			attribDesc.format = vk::Format::eR8G8B8A8Uint;
			vertexOffset += sizeof(float);
		}
		else if (VertexLayouts[i] == VertexLayoutFormat::R8G8B8A8_UNORM)
		{
			attribDesc.format = vk::Format::eR8G8B8A8Unorm;
			vertexOffset += sizeof(float);
		}
		else
		{
			Log(LogType::Error, "Unimplemented VertexLoayoutFormat");
			return false;
		}

		attribDescs.push_back(attribDesc);
	}

	vk::VertexInputBindingDescription bindDesc;
	bindDesc.binding = 0;
	bindDesc.stride = vertexOffset;
	bindDesc.inputRate = vk::VertexInputRate::eVertex;
	bindDescs.push_back(bindDesc);

	vk::PipelineVertexInputStateCreateInfo inputStateInfo;
	inputStateInfo.pVertexBindingDescriptions = bindDescs.data();
	inputStateInfo.pVertexAttributeDescriptions = attribDescs.data();
	inputStateInfo.vertexBindingDescriptionCount = static_cast<int32_t>(bindDescs.size());
	inputStateInfo.vertexAttributeDescriptionCount = static_cast<int32_t>(attribDescs.size());
	graphicsPipelineInfo.pVertexInputState = &inputStateInfo;

	// setup a topology
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo;
	if (Topology == TopologyType::Triangle)
	{
		inputAssemblyStateInfo.topology = vk::PrimitiveTopology::eTriangleList;
	}
	else if (Topology == TopologyType::Line)
	{
		inputAssemblyStateInfo.topology = vk::PrimitiveTopology::eLineList;
	}
	else if (Topology == TopologyType::Point)
	{
		inputAssemblyStateInfo.topology = vk::PrimitiveTopology::ePointList;
	}
	else
	{
		Log(LogType::Error, "Unimplemented TopologyType");
		return false;
	}

	inputAssemblyStateInfo.primitiveRestartEnable = false;

	graphicsPipelineInfo.pInputAssemblyState = &inputAssemblyStateInfo;

	// setup a viewport
	vk::PipelineViewportStateCreateInfo viewportStateInfo;

	vk::Viewport viewport;
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)1280; // TODO : temp
	viewport.height = (float)720; // TODO : temp
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	viewportStateInfo.pViewports = &viewport;
	viewportStateInfo.viewportCount = 1;

	vk::Rect2D scissor = {};
	scissor.offset = vk::Offset2D(0, 0);
	scissor.extent.width = 1280; // TODO : temp
	scissor.extent.height = 720; // TODO : temp

	viewportStateInfo.pScissors = &scissor;
	viewportStateInfo.scissorCount = 1;

	graphicsPipelineInfo.pViewportState = &viewportStateInfo;

	// setup a rasterlizer
	vk::PipelineRasterizationStateCreateInfo rasterizationState;
	rasterizationState.depthClampEnable = false;
	rasterizationState.rasterizerDiscardEnable = false;
	rasterizationState.polygonMode = vk::PolygonMode::eFill;

	if (Culling == CullingMode::Clockwise)
	{
		rasterizationState.cullMode = vk::CullModeFlagBits::eBack;
	}
	else if (Culling == CullingMode::CounterClockwise)
	{
		rasterizationState.cullMode = vk::CullModeFlagBits::eFront;
	}
	else if (Culling == CullingMode::DoubleSide)
	{
		rasterizationState.cullMode = vk::CullModeFlagBits::eNone;
	}

	rasterizationState.frontFace = vk::FrontFace::eClockwise;

	rasterizationState.depthBiasEnable = false;
	rasterizationState.depthBiasConstantFactor = 0.0f;
	rasterizationState.depthBiasClamp = 0.0f;
	rasterizationState.depthBiasSlopeFactor = 0.0f;
	rasterizationState.lineWidth = 1.0f; // disable lineWidth. (Must not be zero)

	graphicsPipelineInfo.pRasterizationState = &rasterizationState;

	assert(renderPassPipelineState_ != nullptr);
	auto renderPassPipelineState = static_cast<RenderPassPipelineStateVulkan*>(renderPassPipelineState_.get());
	auto renderPass = renderPassPipelineState->GetRenderPass();

	// setup a multisampling
	vk::PipelineMultisampleStateCreateInfo multisampleStateInfo;
	multisampleStateInfo.sampleShadingEnable = renderPassPipelineState->Key.SamplingCount > 1;
	multisampleStateInfo.rasterizationSamples = (vk::SampleCountFlagBits)renderPassPipelineState->Key.SamplingCount;
	multisampleStateInfo.minSampleShading = 1.0f;
	multisampleStateInfo.pSampleMask = nullptr;
	multisampleStateInfo.alphaToCoverageEnable = false;
	multisampleStateInfo.alphaToOneEnable = false;

	graphicsPipelineInfo.pMultisampleState = &multisampleStateInfo;

	// setup a depthstencil
	vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;

	// DepthTest flag must be enabled because DepthWrite and Stencil are depended on DepthTestFlag
	depthStencilInfo.depthTestEnable = true;

	depthStencilInfo.depthWriteEnable = IsDepthWriteEnabled;

	std::array<vk::CompareOp, 10> compareOps;
	compareOps[static_cast<int>(DepthFuncType::Never)] = vk::CompareOp::eNever;
	compareOps[static_cast<int>(DepthFuncType::Less)] = vk::CompareOp::eLess;
	compareOps[static_cast<int>(DepthFuncType::Equal)] = vk::CompareOp::eEqual;
	compareOps[static_cast<int>(DepthFuncType::LessEqual)] = vk::CompareOp::eLessOrEqual;
	compareOps[static_cast<int>(DepthFuncType::Greater)] = vk::CompareOp::eGreater;
	compareOps[static_cast<int>(DepthFuncType::NotEqual)] = vk::CompareOp::eNotEqual;
	compareOps[static_cast<int>(DepthFuncType::GreaterEqual)] = vk::CompareOp::eGreaterOrEqual;
	compareOps[static_cast<int>(DepthFuncType::Always)] = vk::CompareOp::eAlways;

	depthStencilInfo.depthCompareOp = compareOps[static_cast<int>(DepthFunc)];

	if (!IsDepthTestEnabled)
	{
		depthStencilInfo.depthCompareOp = vk::CompareOp::eAlways;
	}

	vk::StencilOpState stencil;
	depthStencilInfo.stencilTestEnable = true;

	/*
	enum class StencilOperatorType
{
	Keep,
	Zero,
	Replace,
	IncClamp,
	DecClamp,
	Invert,
	IncRepeat,
	DecRepeat,
};

	*/
	std::array<vk::StencilOp, 8> stencilOps;
	stencilOps[static_cast<int>(StencilOperatorType::Keep)] = vk::StencilOp::eKeep;
	stencilOps[static_cast<int>(StencilOperatorType::Zero)] = vk::StencilOp::eZero;
	stencilOps[static_cast<int>(StencilOperatorType::Replace)] = vk::StencilOp::eReplace;
	stencilOps[static_cast<int>(StencilOperatorType::IncClamp)] = vk::StencilOp::eIncrementAndClamp;
	stencilOps[static_cast<int>(StencilOperatorType::DecClamp)] = vk::StencilOp::eDecrementAndClamp;
	stencilOps[static_cast<int>(StencilOperatorType::Invert)] = vk::StencilOp::eInvert;
	stencilOps[static_cast<int>(StencilOperatorType::IncRepeat)] = vk::StencilOp::eIncrementAndWrap;
	stencilOps[static_cast<int>(StencilOperatorType::DecRepeat)] = vk::StencilOp::eDecrementAndWrap;

	if (IsStencilTestEnabled)
	{
		stencil.depthFailOp = stencilOps[static_cast<int>(StencilDepthFailOp)];
		stencil.failOp = stencilOps[static_cast<int>(StencilFailOp)];
		stencil.passOp = stencilOps[static_cast<int>(StencilPassOp)];
		stencil.compareOp = compareOps[static_cast<int>(StencilCompareFunc)];
		stencil.writeMask = StencilWriteMask;
		stencil.compareMask = StencilReadMask;
		stencil.reference = StencilRef;
	}
	else
	{
		stencil.depthFailOp = vk::StencilOp::eKeep;
		stencil.failOp = vk::StencilOp::eKeep;
		stencil.passOp = vk::StencilOp::eReplace;
		stencil.compareOp = vk::CompareOp::eAlways;
		stencil.writeMask = 0xff;
		stencil.compareMask = 0xff;
		stencil.reference = 0xff;
	}

	depthStencilInfo.front = stencil;
	depthStencilInfo.back = stencil;

	depthStencilInfo.minDepthBounds = 0.0f;
	depthStencilInfo.maxDepthBounds = 1.0f;
	depthStencilInfo.depthBoundsTestEnable = false;

	graphicsPipelineInfo.pDepthStencilState = &depthStencilInfo;

	// blending
	std::array<vk::PipelineColorBlendAttachmentState, RenderTargetMax> blendInfos;

	for (int32_t i = 0; i < renderPassPipelineState->RenderTargetCount; i++)
	{
		auto& blendInfo = blendInfos[i];

		blendInfo.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
								   vk::ColorComponentFlagBits::eA;

		if (IsBlendEnabled)
		{
			blendInfo.blendEnable = true;

			std::array<vk::BlendOp, 10> blendOps;
			blendOps[static_cast<int>(BlendEquationType::Add)] = vk::BlendOp::eAdd;
			blendOps[static_cast<int>(BlendEquationType::Sub)] = vk::BlendOp::eSubtract;
			blendOps[static_cast<int>(BlendEquationType::ReverseSub)] = vk::BlendOp::eReverseSubtract;
			blendOps[static_cast<int>(BlendEquationType::Min)] = vk::BlendOp::eMin;
			blendOps[static_cast<int>(BlendEquationType::Max)] = vk::BlendOp::eMax;

			std::array<vk::BlendFactor, 20> blendFuncs;
			blendFuncs[static_cast<int>(BlendFuncType::Zero)] = vk::BlendFactor::eZero;
			blendFuncs[static_cast<int>(BlendFuncType::One)] = vk::BlendFactor::eOne;
			blendFuncs[static_cast<int>(BlendFuncType::SrcColor)] = vk::BlendFactor::eSrcColor;
			blendFuncs[static_cast<int>(BlendFuncType::OneMinusSrcColor)] = vk::BlendFactor::eOneMinusSrcColor;
			blendFuncs[static_cast<int>(BlendFuncType::SrcAlpha)] = vk::BlendFactor::eSrcAlpha;
			blendFuncs[static_cast<int>(BlendFuncType::OneMinusSrcAlpha)] = vk::BlendFactor::eOneMinusSrcAlpha;
			blendFuncs[static_cast<int>(BlendFuncType::DstColor)] = vk::BlendFactor::eDstColor;
			blendFuncs[static_cast<int>(BlendFuncType::OneMinusDstColor)] = vk::BlendFactor::eOneMinusDstColor;
			blendFuncs[static_cast<int>(BlendFuncType::DstAlpha)] = vk::BlendFactor::eDstAlpha;
			blendFuncs[static_cast<int>(BlendFuncType::OneMinusDstAlpha)] = vk::BlendFactor::eDstAlpha;

			blendInfo.srcColorBlendFactor = blendFuncs[static_cast<int>(BlendSrcFunc)];
			blendInfo.dstColorBlendFactor = blendFuncs[static_cast<int>(BlendDstFunc)];
			blendInfo.srcAlphaBlendFactor = blendFuncs[static_cast<int>(BlendSrcFuncAlpha)];
			blendInfo.dstAlphaBlendFactor = blendFuncs[static_cast<int>(BlendDstFuncAlpha)];
			blendInfo.colorBlendOp = blendOps[static_cast<int>(BlendEquationRGB)];
			blendInfo.alphaBlendOp = blendOps[static_cast<int>(BlendEquationAlpha)];
		}
		else
		{
			blendInfo.blendEnable = false;
		}
	}

	vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
	colorBlendInfo.logicOpEnable = VK_FALSE;
	colorBlendInfo.logicOp = vk::LogicOp::eCopy;
	colorBlendInfo.attachmentCount = renderPassPipelineState->RenderTargetCount;
	colorBlendInfo.pAttachments = blendInfos.data();
	colorBlendInfo.blendConstants[0] = 0.0f;
	colorBlendInfo.blendConstants[1] = 0.0f;
	colorBlendInfo.blendConstants[2] = 0.0f;
	colorBlendInfo.blendConstants[3] = 0.0f;

	graphicsPipelineInfo.pColorBlendState = &colorBlendInfo;

	// dynamic state
	vk::DynamicState dynamicStates[] = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

	vk::PipelineDynamicStateCreateInfo dynamicStateInfo;
	dynamicStateInfo.pDynamicStates = dynamicStates;
	dynamicStateInfo.dynamicStateCount = 2;

	graphicsPipelineInfo.pDynamicState = &dynamicStateInfo;

	// setup a render pass

	graphicsPipelineInfo.renderPass = renderPass;

	// uniform layout info
	std::array<vk::DescriptorSetLayoutBinding, TextureSlotMax + 1> uboLayoutBindings;
	uboLayoutBindings[0].binding = 0;
	uboLayoutBindings[0].descriptorType = vk::DescriptorType::eUniformBufferDynamic;
	uboLayoutBindings[0].descriptorCount = 1;
	uboLayoutBindings[0].stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
	uboLayoutBindings[0].pImmutableSamplers = nullptr;

	for (size_t i = 1; i < uboLayoutBindings.size(); i++)
	{
		uboLayoutBindings[i].binding = static_cast<uint32_t>(i);
		uboLayoutBindings[i].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		uboLayoutBindings[i].descriptorCount = 1;
		uboLayoutBindings[i].stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
		uboLayoutBindings[i].pImmutableSamplers = nullptr;
	}

	vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo;
	descriptorSetLayoutInfo.bindingCount = static_cast<int32_t>(uboLayoutBindings.size());
	descriptorSetLayoutInfo.pBindings = uboLayoutBindings.data();

	descriptorSetLayouts[0] = graphics_->GetDevice().createDescriptorSetLayout(descriptorSetLayoutInfo);
	descriptorSetLayouts[1] = graphics_->GetDevice().createDescriptorSetLayout(descriptorSetLayoutInfo);

	vk::PipelineLayoutCreateInfo layoutInfo = {};
	layoutInfo.setLayoutCount = 2;
	layoutInfo.pSetLayouts = descriptorSetLayouts.data();
	layoutInfo.pushConstantRangeCount = 0;
	layoutInfo.pPushConstantRanges = nullptr;

	pipelineLayout_ = graphics_->GetDevice().createPipelineLayout(layoutInfo);
	graphicsPipelineInfo.layout = pipelineLayout_;

#if VK_HEADER_VERSION >= 136
	// setup a pipeline
	const auto pipeline = graphics_->GetDevice().createGraphicsPipeline(nullptr, graphicsPipelineInfo);
	if (pipeline.result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Cannnot create graphicPipeline: " + std::to_string(static_cast<int>(pipeline.result)));
	}
	pipeline_ = pipeline.value;
#else
	pipeline_ = graphics_->GetDevice().createGraphicsPipeline(nullptr, graphicsPipelineInfo);
#endif

	return true;
}

bool PipelineStateVulkan::CreateComputePipeline()
{
	vk::ComputePipelineCreateInfo computePipelineInfo;

	// setup shaders
	std::string mainName = "main";

	auto shader = static_cast<ShaderVulkan*>(shaders[static_cast<int>(ShaderStageType::Compute)]);

	if (shader == nullptr)
		return false;

	vk::PipelineShaderStageCreateInfo info;

	info.stage = vk::ShaderStageFlagBits::eCompute;
	info.module = shader->GetShaderModule();
	info.pName = mainName.c_str();
	computePipelineInfo.stage = info;

	// uniform layout info
	std::array<vk::DescriptorSetLayoutBinding, 1 + TextureSlotMax> uboLayoutBindings;

	uboLayoutBindings[0].binding = 0;
	uboLayoutBindings[0].descriptorType = vk::DescriptorType::eUniformBufferDynamic;
	uboLayoutBindings[0].descriptorCount = 1;
	uboLayoutBindings[0].stageFlags = vk::ShaderStageFlagBits::eCompute;
	uboLayoutBindings[0].pImmutableSamplers = nullptr;

	for (size_t i = 1; i < uboLayoutBindings.size(); i++)
	{
		uboLayoutBindings[i].binding = static_cast<uint32_t>(i);
		uboLayoutBindings[i].descriptorType = vk::DescriptorType::eStorageBufferDynamic;
		uboLayoutBindings[i].descriptorCount = 1;
		uboLayoutBindings[i].stageFlags = vk::ShaderStageFlagBits::eCompute;
		uboLayoutBindings[i].pImmutableSamplers = nullptr;
	}

	vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo;
	descriptorSetLayoutInfo.bindingCount = static_cast<int32_t>(uboLayoutBindings.size());
	descriptorSetLayoutInfo.pBindings = uboLayoutBindings.data();

	computeDescriptorSetLayouts[0] = graphics_->GetDevice().createDescriptorSetLayout(descriptorSetLayoutInfo);

	vk::PipelineLayoutCreateInfo layoutInfo = {};
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = computeDescriptorSetLayouts.data();
	layoutInfo.pushConstantRangeCount = 0;
	layoutInfo.pPushConstantRanges = nullptr;

	computePipelineLayout_ = graphics_->GetDevice().createPipelineLayout(layoutInfo);
	computePipelineInfo.layout = computePipelineLayout_;

#if VK_HEADER_VERSION >= 136
	// setup a pipeline
	const auto pipeline = graphics_->GetDevice().createComputePipeline(nullptr, computePipelineInfo);
	if (pipeline.result != vk::Result::eSuccess)
	{
		throw std::runtime_error("Cannnot create graphicPipeline: " + std::to_string(static_cast<int>(pipeline.result)));
	}
	computePipeline_ = pipeline.value;
#else
	computePipeline_ = graphics_->GetDevice().createComputePipeline(nullptr, computePipelineInfo);
#endif

	return true;
}

} // namespace LLGI
