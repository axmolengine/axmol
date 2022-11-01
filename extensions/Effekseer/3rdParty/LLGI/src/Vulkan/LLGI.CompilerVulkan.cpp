
#include <memory>

#if defined(ENABLE_VULKAN_COMPILER)

#if defined(ENABLE_GLSLANG_WITHOUT_INSTALL)
#include <SPIRV/GlslangToSpv.h>
#else
#include <glslang/SPIRV/GlslangToSpv.h>
#endif

#include <glslang/Include/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#endif

#include "LLGI.CompilerVulkan.h"

namespace LLGI
{

#if defined(ENABLE_VULKAN_COMPILER)

const TBuiltInResource DefaultTBuiltInResource = {
	/* .MaxLights = */ 32,
	/* .MaxClipPlanes = */ 6,
	/* .MaxTextureUnits = */ 32,
	/* .MaxTextureCoords = */ 32,
	/* .MaxVertexAttribs = */ 64,
	/* .MaxVertexUniformComponents = */ 4096,
	/* .MaxVaryingFloats = */ 64,
	/* .MaxVertexTextureImageUnits = */ 32,
	/* .MaxCombinedTextureImageUnits = */ 80,
	/* .MaxTextureImageUnits = */ 32,
	/* .MaxFragmentUniformComponents = */ 4096,
	/* .MaxDrawBuffers = */ 32,
	/* .MaxVertexUniformVectors = */ 128,
	/* .MaxVaryingVectors = */ 8,
	/* .MaxFragmentUniformVectors = */ 16,
	/* .MaxVertexOutputVectors = */ 16,
	/* .MaxFragmentInputVectors = */ 15,
	/* .MinProgramTexelOffset = */ -8,
	/* .MaxProgramTexelOffset = */ 7,
	/* .MaxClipDistances = */ 8,
	/* .MaxComputeWorkGroupCountX = */ 65535,
	/* .MaxComputeWorkGroupCountY = */ 65535,
	/* .MaxComputeWorkGroupCountZ = */ 65535,
	/* .MaxComputeWorkGroupSizeX = */ 1024,
	/* .MaxComputeWorkGroupSizeY = */ 1024,
	/* .MaxComputeWorkGroupSizeZ = */ 64,
	/* .MaxComputeUniformComponents = */ 1024,
	/* .MaxComputeTextureImageUnits = */ 16,
	/* .MaxComputeImageUniforms = */ 8,
	/* .MaxComputeAtomicCounters = */ 8,
	/* .MaxComputeAtomicCounterBuffers = */ 1,
	/* .MaxVaryingComponents = */ 60,
	/* .MaxVertexOutputComponents = */ 64,
	/* .MaxGeometryInputComponents = */ 64,
	/* .MaxGeometryOutputComponents = */ 128,
	/* .MaxFragmentInputComponents = */ 128,
	/* .MaxImageUnits = */ 8,
	/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
	/* .MaxCombinedShaderOutputResources = */ 8,
	/* .MaxImageSamples = */ 0,
	/* .MaxVertexImageUniforms = */ 0,
	/* .MaxTessControlImageUniforms = */ 0,
	/* .MaxTessEvaluationImageUniforms = */ 0,
	/* .MaxGeometryImageUniforms = */ 0,
	/* .MaxFragmentImageUniforms = */ 8,
	/* .MaxCombinedImageUniforms = */ 8,
	/* .MaxGeometryTextureImageUnits = */ 16,
	/* .MaxGeometryOutputVertices = */ 256,
	/* .MaxGeometryTotalOutputComponents = */ 1024,
	/* .MaxGeometryUniformComponents = */ 1024,
	/* .MaxGeometryVaryingComponents = */ 64,
	/* .MaxTessControlInputComponents = */ 128,
	/* .MaxTessControlOutputComponents = */ 128,
	/* .MaxTessControlTextureImageUnits = */ 16,
	/* .MaxTessControlUniformComponents = */ 1024,
	/* .MaxTessControlTotalOutputComponents = */ 4096,
	/* .MaxTessEvaluationInputComponents = */ 128,
	/* .MaxTessEvaluationOutputComponents = */ 128,
	/* .MaxTessEvaluationTextureImageUnits = */ 16,
	/* .MaxTessEvaluationUniformComponents = */ 1024,
	/* .MaxTessPatchComponents = */ 120,
	/* .MaxPatchVertices = */ 32,
	/* .MaxTessGenLevel = */ 64,
	/* .MaxViewports = */ 16,
	/* .MaxVertexAtomicCounters = */ 0,
	/* .MaxTessControlAtomicCounters = */ 0,
	/* .MaxTessEvaluationAtomicCounters = */ 0,
	/* .MaxGeometryAtomicCounters = */ 0,
	/* .MaxFragmentAtomicCounters = */ 8,
	/* .MaxCombinedAtomicCounters = */ 8,
	/* .MaxAtomicCounterBindings = */ 1,
	/* .MaxVertexAtomicCounterBuffers = */ 0,
	/* .MaxTessControlAtomicCounterBuffers = */ 0,
	/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
	/* .MaxGeometryAtomicCounterBuffers = */ 0,
	/* .MaxFragmentAtomicCounterBuffers = */ 1,
	/* .MaxCombinedAtomicCounterBuffers = */ 1,
	/* .MaxAtomicCounterBufferSize = */ 16384,
	/* .MaxTransformFeedbackBuffers = */ 4,
	/* .MaxTransformFeedbackInterleavedComponents = */ 64,
	/* .MaxCullDistances = */ 8,
	/* .MaxCombinedClipAndCullDistances = */ 8,
	/* .MaxSamples = */ 4,
	/* .maxMeshOutputVerticesNV = */ 256,
	/* .maxMeshOutputPrimitivesNV = */ 512,
	/* .maxMeshWorkGroupSizeX_NV = */ 32,
	/* .maxMeshWorkGroupSizeY_NV = */ 1,
	/* .maxMeshWorkGroupSizeZ_NV = */ 1,
	/* .maxTaskWorkGroupSizeX_NV = */ 32,
	/* .maxTaskWorkGroupSizeY_NV = */ 1,
	/* .maxTaskWorkGroupSizeZ_NV = */ 1,
	/* .maxMeshViewCountNV = */ 4,
	/* .maxDualSourceDrawBuffersEXT = */ 1,

	/* .limits = */
	{
		/* .nonInductiveForLoops = */ 1,
		/* .whileLoops = */ 1,
		/* .doWhileLoops = */ 1,
		/* .generalUniformIndexing = */ 1,
		/* .generalAttributeMatrixVectorIndexing = */ 1,
		/* .generalVaryingIndexing = */ 1,
		/* .generalSamplerIndexing = */ 1,
		/* .generalVariableIndexing = */ 1,
		/* .generalConstantMatrixVectorIndexing = */ 1,
	}};

#endif

CompilerVulkan::CompilerVulkan()
{
#if defined(ENABLE_VULKAN_COMPILER)
	glslang::InitializeProcess();
#endif
}

CompilerVulkan::~CompilerVulkan()
{
#if defined(ENABLE_VULKAN_COMPILER)
	glslang::FinalizeProcess();
#endif
}

void CompilerVulkan::Initialize() {}

void CompilerVulkan::Compile(CompilerResult& result, const char* code, ShaderStageType shaderStage)
{
#if defined(ENABLE_VULKAN_COMPILER)
	EShLanguage stage;
	switch (shaderStage)
	{
	case ShaderStageType::Vertex:
		stage = EShLanguage::EShLangVertex;
		break;
	case ShaderStageType::Pixel:
		stage = EShLanguage::EShLangFragment;
		break;
	case ShaderStageType::Compute:
		stage = EShLanguage::EShLangCompute;
		break;
	default:
		result.Message = "Invalid shader stage.";
		return;
	}

	auto shader = std::make_shared<glslang::TShader>(stage);

	int ClientInputSemanticsVersion = 100; // #define VULKAN 100
	glslang::EShTargetClientVersion VulkanClientVersion = glslang::EShTargetVulkan_1_0;
	glslang::EShTargetLanguageVersion TargetVersion = glslang::EShTargetSpv_1_0;

	const char* shaderCode[1] = {code};
	const int shaderLenght[1] = {static_cast<int>(strlen(code))};
	const char* shaderName[1] = {"shadercode"};
	shader->setStringsWithLengthsAndNames(shaderCode, shaderLenght, shaderName, 1);
	shader->setEntryPoint("main");
	shader->setEnvInput(glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, ClientInputSemanticsVersion);
	shader->setEnvClient(glslang::EShClientVulkan, VulkanClientVersion);
	shader->setEnvTarget(glslang::EShTargetSpv, TargetVersion);

	TBuiltInResource Resources;
	Resources = DefaultTBuiltInResource;
	EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

	// compile
	if (!shader->parse(&Resources, 100, false, messages))
	{
		result.Message += "GLSL Parsing Failed:";
		result.Message += shader->getInfoLog();
		result.Message += shader->getInfoDebugLog();
		return;
	}

	// link
	auto program = std::make_shared<glslang::TProgram>();
	program->addShader(shader.get());
	if (!program->link(messages))
	{
		result.Message += shader->getInfoLog();
		result.Message += shader->getInfoDebugLog();
		return;
	}
	else if (shader->getInfoLog())
	{
		result.Message += shader->getInfoLog();
		result.Message += shader->getInfoDebugLog();
	}

	std::vector<unsigned int> spirvCode;
	spv::SpvBuildLogger logger;
	glslang::SpvOptions spvOptions;
	glslang::GlslangToSpv(*program->getIntermediate(stage), spirvCode, &logger, &spvOptions);

	result.Binary.resize(1);
	result.Binary[0].resize(spirvCode.size() * sizeof(unsigned int));
	memcpy(result.Binary[0].data(), spirvCode.data(), result.Binary[0].size());
#endif
}

} // namespace LLGI
