#pragma once

#include "LLGI.PipelineStateDX12.h"
#include "../LLGI.CommandList.h"
#include "../LLGI.PipelineState.h"
#include "LLGI.RenderPassDX12.h"
#include "LLGI.RenderPassPipelineStateDX12.h"
#include "LLGI.ShaderDX12.h"
#include "LLGI.TextureDX12.h"

namespace LLGI
{

PipelineStateDX12::PipelineStateDX12(GraphicsDX12* graphics)
{
	SafeAddRef(graphics);
	graphics_ = CreateSharedPtr(graphics);
	shaders_.fill(nullptr);
}

PipelineStateDX12::~PipelineStateDX12()
{
	for (auto& shader : shaders_)
	{
		SafeRelease(shader);
	}
	SafeRelease(pipelineState_);
	SafeRelease(signature_);
	SafeRelease(rootSignature_);
	SafeRelease(computePipelineState_);
	SafeRelease(computeSignature_);
	SafeRelease(computeRootSignature_);
}

void PipelineStateDX12::SetShader(ShaderStageType stage, Shader* shader)
{
	SafeAddRef(shader);
	SafeRelease(shaders_[static_cast<int>(stage)]);
	shaders_[static_cast<int>(stage)] = shader;
}

bool PipelineStateDX12::Compile()
{
	if (shaders_[static_cast<int>(ShaderStageType::Compute)] != nullptr)
	{
		auto res = CreateComputeRootSignature();
		res &= CreateComputePipelineState();
		return res;
	}
	else
	{
		auto res = CreateRootSignature();
		res &= CreatePipelineState();
		return res;
	}
}

bool PipelineStateDX12::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE ranges[4] = {{}, {}, {}, {}};
	D3D12_ROOT_PARAMETER rootParameters[2] = {{}, {}};

	// descriptor range for constant buffer view
	ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	ranges[0].NumDescriptors = 2;
	ranges[0].BaseShaderRegister = 0;
	ranges[0].RegisterSpace = 0;
	ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// descriptor range for shader resorce view
	ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ranges[1].NumDescriptors = NumTexture * 2;
	ranges[1].BaseShaderRegister = 0;
	ranges[1].RegisterSpace = 0;
	ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// descriptor range for uav
	ranges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	ranges[2].NumDescriptors = NumComputeBuffer * 2;
	ranges[2].BaseShaderRegister = 0;
	ranges[2].RegisterSpace = 0;
	ranges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// descriptor range for sampler
	ranges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
	ranges[3].NumDescriptors = NumTexture * 2;
	ranges[3].BaseShaderRegister = 0;
	ranges[3].RegisterSpace = 0;
	ranges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// descriptor table for CBV/SRV/UAV
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 3;
	rootParameters[0].DescriptorTable.pDescriptorRanges = &ranges[0];
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// descriptor table for sampler
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	rootParameters[1].DescriptorTable.pDescriptorRanges = &ranges[3];
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = 2;
	desc.pParameters = rootParameters;
	desc.NumStaticSamplers = 0;
	desc.pStaticSamplers = nullptr;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	auto hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_, nullptr);

	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}

	hr = graphics_->GetDevice()->CreateRootSignature(
		0, signature_->GetBufferPointer(), signature_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}
	return true;

FAILED_EXIT:
	SafeRelease(signature_);
	return false;
}

bool PipelineStateDX12::CreateComputeRootSignature()
{
	D3D12_DESCRIPTOR_RANGE ranges[2] = {{}, {}};
	D3D12_ROOT_PARAMETER rootParameters[1] = {{}};

	// descriptor range for constant buffer view
	ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	ranges[0].NumDescriptors = 1;
	ranges[0].BaseShaderRegister = 0;
	ranges[0].RegisterSpace = 0;
	ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// descriptor range for uav
	ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	ranges[1].NumDescriptors = NumComputeBuffer;
	ranges[1].BaseShaderRegister = 0;
	ranges[1].RegisterSpace = 0;
	ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// descriptor table for CBV/UAV
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 2;
	rootParameters[0].DescriptorTable.pDescriptorRanges = &ranges[0];
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC desc = {};
	desc.NumParameters = 1;
	desc.pParameters = rootParameters;
	desc.NumStaticSamplers = 0;
	desc.pStaticSamplers = nullptr;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;

	auto hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &computeSignature_, nullptr);

	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}

	hr = graphics_->GetDevice()->CreateRootSignature(
		0, computeSignature_->GetBufferPointer(), computeSignature_->GetBufferSize(), IID_PPV_ARGS(&computeRootSignature_));
	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}
	return true;

FAILED_EXIT:
	SafeRelease(computeSignature_);
	return false;
}

bool PipelineStateDX12::CreatePipelineState()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {};

	for (size_t i = 0; i < 2; i++)
	{
		auto shader = static_cast<ShaderDX12*>(shaders_.at(i));
		if (shader == nullptr)
			return false;

		auto& shaderData = shader->GetData();

		if (i == static_cast<int>(ShaderStageType::Pixel))
		{
			pipelineStateDesc.PS.pShaderBytecode = shaderData.data();
			pipelineStateDesc.PS.BytecodeLength = shaderData.size();
		}
		else if (i == static_cast<int>(ShaderStageType::Vertex))
		{
			pipelineStateDesc.VS.pShaderBytecode = shaderData.data();
			pipelineStateDesc.VS.BytecodeLength = shaderData.size();
		}
	}

	// setup a vertex layout
	std::array<D3D12_INPUT_ELEMENT_DESC, 16> elementDescs;
	elementDescs.fill(D3D12_INPUT_ELEMENT_DESC{});
	int32_t elementOffset = 0;

	for (int i = 0; i < VertexLayoutCount; i++)
	{
		elementDescs[i].SemanticName = this->VertexLayoutNames[i].c_str();
		elementDescs[i].SemanticIndex = this->VertexLayoutSemantics[i];
		elementDescs[i].AlignedByteOffset = elementOffset;
		elementDescs[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

		if (VertexLayouts[i] == VertexLayoutFormat::R32_FLOAT)
		{
			elementDescs[i].Format = DXGI_FORMAT_R32_FLOAT;
			elementOffset += sizeof(float) * 1;
		}
		else if (VertexLayouts[i] == VertexLayoutFormat::R32G32_FLOAT)
		{
			elementDescs[i].Format = DXGI_FORMAT_R32G32_FLOAT;
			elementOffset += sizeof(float) * 2;
		}
		else if (VertexLayouts[i] == VertexLayoutFormat::R32G32B32_FLOAT)
		{
			elementDescs[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			elementOffset += sizeof(float) * 3;
		}
		else if (VertexLayouts[i] == VertexLayoutFormat::R32G32B32A32_FLOAT)
		{
			elementDescs[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			elementOffset += sizeof(float) * 4;
		}
		else if (VertexLayouts[i] == VertexLayoutFormat::R8G8B8A8_UNORM)
		{
			elementDescs[i].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			elementOffset += sizeof(float) * 1;
		}
		else if (VertexLayouts[i] == VertexLayoutFormat::R8G8B8A8_UINT)
		{
			elementDescs[i].Format = DXGI_FORMAT_R8G8B8A8_UINT;
			elementOffset += sizeof(float) * 1;
		}
		else
		{
			Log(LogType::Error, "Unimplemented VertexLoayoutFormat");
			return false;
		}
	}

	// setup a topology
	if (Topology == TopologyType::Triangle)
	{
		pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}
	else if (Topology == TopologyType::Line)
	{
		pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	}
	else if (Topology == TopologyType::Point)
	{
		pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	}
	else
	{
		Log(LogType::Error, "Unimplemented TopologyType");
		return false;
	}

	// TODO...(generate from parameters)
	D3D12_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	if (Culling == CullingMode::Clockwise)
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	if (Culling == CullingMode::CounterClockwise)
		rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
	if (Culling == CullingMode::DoubleSide)
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// setup render target blend
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	if (IsBlendEnabled)
	{
		renderTargetBlendDesc.BlendEnable = TRUE;

		std::array<D3D12_BLEND_OP, 10> blendOps;
		blendOps[static_cast<int>(BlendEquationType::Add)] = D3D12_BLEND_OP_ADD;
		blendOps[static_cast<int>(BlendEquationType::Sub)] = D3D12_BLEND_OP_SUBTRACT;
		blendOps[static_cast<int>(BlendEquationType::ReverseSub)] = D3D12_BLEND_OP_REV_SUBTRACT;
		blendOps[static_cast<int>(BlendEquationType::Min)] = D3D12_BLEND_OP_MIN;
		blendOps[static_cast<int>(BlendEquationType::Max)] = D3D12_BLEND_OP_MAX;

		std::array<D3D12_BLEND, 20> blendFuncs;
		blendFuncs[static_cast<int>(BlendFuncType::Zero)] = D3D12_BLEND_ZERO;
		blendFuncs[static_cast<int>(BlendFuncType::One)] = D3D12_BLEND_ONE;
		blendFuncs[static_cast<int>(BlendFuncType::SrcColor)] = D3D12_BLEND_SRC_COLOR;
		blendFuncs[static_cast<int>(BlendFuncType::OneMinusSrcColor)] = D3D12_BLEND_INV_SRC_COLOR;
		blendFuncs[static_cast<int>(BlendFuncType::SrcAlpha)] = D3D12_BLEND_SRC_ALPHA;
		blendFuncs[static_cast<int>(BlendFuncType::OneMinusSrcAlpha)] = D3D12_BLEND_INV_SRC_ALPHA;
		blendFuncs[static_cast<int>(BlendFuncType::DstColor)] = D3D12_BLEND_DEST_COLOR;
		blendFuncs[static_cast<int>(BlendFuncType::OneMinusDstColor)] = D3D12_BLEND_INV_DEST_COLOR;
		blendFuncs[static_cast<int>(BlendFuncType::DstAlpha)] = D3D12_BLEND_DEST_ALPHA;
		blendFuncs[static_cast<int>(BlendFuncType::OneMinusDstAlpha)] = D3D12_BLEND_INV_DEST_ALPHA;

		renderTargetBlendDesc.SrcBlend = blendFuncs[static_cast<int>(BlendSrcFunc)];
		renderTargetBlendDesc.DestBlend = blendFuncs[static_cast<int>(BlendDstFunc)];
		renderTargetBlendDesc.SrcBlendAlpha = blendFuncs[static_cast<int>(BlendSrcFuncAlpha)];
		renderTargetBlendDesc.DestBlendAlpha = blendFuncs[static_cast<int>(BlendDstFuncAlpha)];
		renderTargetBlendDesc.BlendOp = blendOps[static_cast<int>(BlendEquationRGB)];
		renderTargetBlendDesc.BlendOpAlpha = blendOps[static_cast<int>(BlendEquationAlpha)];

		// TODO:
		renderTargetBlendDesc.LogicOpEnable = FALSE;
		renderTargetBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	}
	else
		renderTargetBlendDesc.BlendEnable = FALSE;

	// ?
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// setup a blend state
	D3D12_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		blendDesc.RenderTarget[i] = renderTargetBlendDesc;
	}

	pipelineStateDesc.InputLayout.pInputElementDescs = elementDescs.data();
	pipelineStateDesc.InputLayout.NumElements = VertexLayoutCount;
	pipelineStateDesc.pRootSignature = rootSignature_;
	pipelineStateDesc.RasterizerState = rasterizerDesc;
	pipelineStateDesc.BlendState = blendDesc;

	auto renderPassPipelineState = static_cast<RenderPassPipelineStateDX12*>(renderPassPipelineState_.get());
	pipelineStateDesc.NumRenderTargets = static_cast<UINT>(renderPassPipelineState->Key.RenderTargetFormats.size());

	for (uint32_t i = 0; i < pipelineStateDesc.NumRenderTargets; i++)
	{
		pipelineStateDesc.RTVFormats[i] = ConvertFormat(renderPassPipelineState->Key.RenderTargetFormats.at(i));
	}

	// setup a depth stencil
	if (renderPassPipelineState->Key.DepthFormat != TextureFormatType::Unknown)
	{
		std::array<D3D12_COMPARISON_FUNC, 10> compareOps;
		compareOps[static_cast<int>(DepthFuncType::Never)] = D3D12_COMPARISON_FUNC_NEVER;
		compareOps[static_cast<int>(DepthFuncType::Less)] = D3D12_COMPARISON_FUNC_LESS;
		compareOps[static_cast<int>(DepthFuncType::Equal)] = D3D12_COMPARISON_FUNC_EQUAL;
		compareOps[static_cast<int>(DepthFuncType::LessEqual)] = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		compareOps[static_cast<int>(DepthFuncType::Greater)] = D3D12_COMPARISON_FUNC_GREATER;
		compareOps[static_cast<int>(DepthFuncType::NotEqual)] = D3D12_COMPARISON_FUNC_NOT_EQUAL;
		compareOps[static_cast<int>(DepthFuncType::GreaterEqual)] = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		compareOps[static_cast<int>(DepthFuncType::Always)] = D3D12_COMPARISON_FUNC_ALWAYS;

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthFunc = compareOps[static_cast<int>(DepthFunc)];
		depthStencilDesc.DepthWriteMask = IsDepthWriteEnabled ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthEnable = IsDepthTestEnabled | IsDepthWriteEnabled;
		depthStencilDesc.StencilEnable = IsStencilTestEnabled;

		if (!IsDepthTestEnabled)
		{
			depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		}

		std::array<D3D12_STENCIL_OP, 8> stencilOps;
		stencilOps[static_cast<int>(StencilOperatorType::DecClamp)] = D3D12_STENCIL_OP::D3D12_STENCIL_OP_DECR_SAT;
		stencilOps[static_cast<int>(StencilOperatorType::DecRepeat)] = D3D12_STENCIL_OP::D3D12_STENCIL_OP_DECR;
		stencilOps[static_cast<int>(StencilOperatorType::IncClamp)] = D3D12_STENCIL_OP::D3D12_STENCIL_OP_INCR_SAT;
		stencilOps[static_cast<int>(StencilOperatorType::IncRepeat)] = D3D12_STENCIL_OP::D3D12_STENCIL_OP_INCR;
		stencilOps[static_cast<int>(StencilOperatorType::Invert)] = D3D12_STENCIL_OP::D3D12_STENCIL_OP_INVERT;
		stencilOps[static_cast<int>(StencilOperatorType::Keep)] = D3D12_STENCIL_OP::D3D12_STENCIL_OP_KEEP;
		stencilOps[static_cast<int>(StencilOperatorType::Replace)] = D3D12_STENCIL_OP::D3D12_STENCIL_OP_REPLACE;
		stencilOps[static_cast<int>(StencilOperatorType::Zero)] = D3D12_STENCIL_OP::D3D12_STENCIL_OP_ZERO;

		D3D12_DEPTH_STENCILOP_DESC stencilDesc;
		if (IsStencilTestEnabled)
		{
			stencilDesc.StencilDepthFailOp = stencilOps[static_cast<int>(this->StencilDepthFailOp)];
			stencilDesc.StencilFailOp = stencilOps[static_cast<int>(this->StencilFailOp)];
			stencilDesc.StencilPassOp = stencilOps[static_cast<int>(this->StencilPassOp)];
			stencilDesc.StencilFunc = compareOps[static_cast<int>(this->StencilCompareFunc)];
			depthStencilDesc.StencilReadMask = this->StencilReadMask;
			depthStencilDesc.StencilWriteMask = this->StencilWriteMask;
		}
		else
		{
			stencilDesc.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
			stencilDesc.StencilFailOp = D3D12_STENCIL_OP_KEEP;
			stencilDesc.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
			stencilDesc.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
			depthStencilDesc.StencilReadMask = 0xff;
			depthStencilDesc.StencilWriteMask = 0xff;
		}

		depthStencilDesc.BackFace = stencilDesc;
		depthStencilDesc.FrontFace = stencilDesc;

		pipelineStateDesc.DepthStencilState = depthStencilDesc;
		pipelineStateDesc.DSVFormat = ConvertFormat(renderPassPipelineState->Key.DepthFormat);
	}

	pipelineStateDesc.SampleDesc.Count = renderPassPipelineState->Key.SamplingCount;
	pipelineStateDesc.SampleMask = UINT_MAX;

	auto hr = graphics_->GetDevice()->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState_));

	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}

	return true;

FAILED_EXIT:
	SafeRelease(pipelineState_);
	return false;
}

bool PipelineStateDX12::CreateComputePipelineState()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc = {};

	auto shader = static_cast<ShaderDX12*>(shaders_.at(static_cast<int>(ShaderStageType::Compute)));
	if (shader == nullptr)
		return false;

	auto& shaderData = shader->GetData();

	pipelineStateDesc.CS.pShaderBytecode = shaderData.data();
	pipelineStateDesc.CS.BytecodeLength = shaderData.size();
	pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pipelineStateDesc.NodeMask = 1;
	pipelineStateDesc.pRootSignature = computeRootSignature_;

	auto hr = graphics_->GetDevice()->CreateComputePipelineState(&pipelineStateDesc, IID_PPV_ARGS(&computePipelineState_));

	if (FAILED(hr))
	{
		auto msg = (std::string("Error : ") + std::string(__FILE__) + " : " + std::to_string(__LINE__) + std::string(" : ") +
					std::system_category().message(hr));
		::LLGI::Log(::LLGI::LogType::Error, msg.c_str());
		goto FAILED_EXIT;
	}

	return true;

FAILED_EXIT:
	SafeRelease(computePipelineState_);
	return false;
}

} // namespace LLGI
