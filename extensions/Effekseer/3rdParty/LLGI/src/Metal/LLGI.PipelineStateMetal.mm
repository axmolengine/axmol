#include "LLGI.PipelineStateMetal.h"
#include "LLGI.GraphicsMetal.h"
#include "LLGI.Metal_Impl.h"
#include "LLGI.RenderPassMetal.h"
#include "LLGI.ShaderMetal.h"

#import <MetalKit/MetalKit.h>

namespace LLGI
{

bool PipelineStateMetal::Compile(PipelineState* self, Graphics* graphics)
{
	auto pipstate = static_cast<PipelineStateMetal*>(self);
	if(pipstate->GetShaders()[static_cast<int>(ShaderStageType::Compute)])
	{
		return CreateComputePipelineState(self, graphics);
	}
	else
	{
		return CreateRenderPipelineState(self, graphics);
	}
}

bool PipelineStateMetal::CreateRenderPipelineState(PipelineState* self, Graphics* graphics)
{
	@autoreleasepool
	{
		auto g = static_cast<GraphicsMetal*>(graphics);

		auto pipstate = static_cast<PipelineStateMetal*>(self);
		auto renderPassPipelineState = static_cast<RenderPassPipelineStateMetal*>(pipstate->GetRenderPassPipelineState());

		auto pipelineStateDescriptor_ = [[[MTLRenderPipelineDescriptor alloc] init] autorelease];

		// vertex layout
		MTLVertexDescriptor* vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];

		int vertexOffset = 0;
		for (int i = 0; i < pipstate->VertexLayoutCount; i++)
		{
			vertexDescriptor.attributes[i].offset = vertexOffset;

			if (pipstate->VertexLayouts[i] == VertexLayoutFormat::R32G32B32_FLOAT)
			{
				vertexDescriptor.attributes[i].format = MTLVertexFormatFloat3;
				vertexDescriptor.attributes[i].bufferIndex = VertexBufferIndex;
				vertexOffset += sizeof(float) * 3;
			}
			else if (pipstate->VertexLayouts[i] == VertexLayoutFormat::R32G32B32A32_FLOAT)
			{
				vertexDescriptor.attributes[i].format = MTLVertexFormatFloat4;
				vertexDescriptor.attributes[i].bufferIndex = VertexBufferIndex;
				vertexOffset += sizeof(float) * 4;
			}
			else if (pipstate->VertexLayouts[i] == VertexLayoutFormat::R32G32_FLOAT)
			{
				vertexDescriptor.attributes[i].format = MTLVertexFormatFloat2;
				vertexDescriptor.attributes[i].bufferIndex = VertexBufferIndex;
				vertexOffset += sizeof(float) * 2;
			}
			else if (pipstate->VertexLayouts[i] == VertexLayoutFormat::R32_FLOAT)
			{
				vertexDescriptor.attributes[i].format = MTLVertexFormatFloat;
				vertexDescriptor.attributes[i].bufferIndex = VertexBufferIndex;
				vertexOffset += sizeof(float) * 1;
			}
			else if (pipstate->VertexLayouts[i] == VertexLayoutFormat::R8G8B8A8_UINT)
			{
				vertexDescriptor.attributes[i].format = MTLVertexFormatUChar4;
				vertexDescriptor.attributes[i].bufferIndex = VertexBufferIndex;
				vertexOffset += sizeof(float);
			}
			else if (pipstate->VertexLayouts[i] == VertexLayoutFormat::R8G8B8A8_UNORM)
			{
				vertexDescriptor.attributes[i].format = MTLVertexFormatUChar4Normalized;
				vertexDescriptor.attributes[i].bufferIndex = VertexBufferIndex;
				vertexOffset += sizeof(float);
			}
			else
			{
				Log(LogType::Error, "Unimplemented VertexLoayoutFormat");
				return false;
			}
		}

		vertexDescriptor.layouts[VertexBufferIndex].stepRate = 1;
		vertexDescriptor.layouts[VertexBufferIndex].stepFunction = MTLVertexStepFunctionPerVertex;
		vertexDescriptor.layouts[VertexBufferIndex].stride = vertexOffset;

		pipelineStateDescriptor_.vertexDescriptor = vertexDescriptor;

		// setup shaders
		auto vs = static_cast<ShaderMetal*>(pipstate->GetShaders()[static_cast<int>(ShaderStageType::Vertex)]);
		auto ps = static_cast<ShaderMetal*>(pipstate->GetShaders()[static_cast<int>(ShaderStageType::Pixel)]);

		if (vs == nullptr || ps == nullptr)
			return false;

		id<MTLFunction> vf = [[vs->GetLibrary() newFunctionWithName:@"main0"] autorelease];
		id<MTLFunction> pf = [[ps->GetLibrary() newFunctionWithName:@"main0"] autorelease];
		pipelineStateDescriptor_.vertexFunction = vf;
		pipelineStateDescriptor_.fragmentFunction = pf;

		// setup a depth
		if (renderPassPipelineState->GetDepthStencilFormat() != MTLPixelFormatInvalid)
		{

			MTLDepthStencilDescriptor* depthStencilDescriptor = [[MTLDepthStencilDescriptor alloc] init];
			depthStencilDescriptor.depthWriteEnabled = pipstate->IsDepthWriteEnabled;

			if (pipstate->IsDepthTestEnabled)
			{
				std::array<MTLCompareFunction, 10> depthCompareOps;
				depthCompareOps[static_cast<int>(DepthFuncType::Never)] = MTLCompareFunctionNever;
				depthCompareOps[static_cast<int>(DepthFuncType::Less)] = MTLCompareFunctionLess;
				depthCompareOps[static_cast<int>(DepthFuncType::Equal)] = MTLCompareFunctionEqual;
				depthCompareOps[static_cast<int>(DepthFuncType::LessEqual)] = MTLCompareFunctionLessEqual;
				depthCompareOps[static_cast<int>(DepthFuncType::Greater)] = MTLCompareFunctionGreater;
				depthCompareOps[static_cast<int>(DepthFuncType::NotEqual)] = MTLCompareFunctionNotEqual;
				depthCompareOps[static_cast<int>(DepthFuncType::GreaterEqual)] = MTLCompareFunctionGreaterEqual;
				depthCompareOps[static_cast<int>(DepthFuncType::Always)] = MTLCompareFunctionAlways;
				depthStencilDescriptor.depthCompareFunction = depthCompareOps[static_cast<int>(pipstate->DepthFunc)];
			}
			else
			{
				depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionAlways;
			}

			if (renderPassPipelineState->Key.DepthFormat == TextureFormatType::D24S8 ||
				renderPassPipelineState->Key.DepthFormat == TextureFormatType::D32S8)
			{
				MTLStencilDescriptor* stencilDescriptor = [[MTLStencilDescriptor alloc] init];

				if (pipstate->IsStencilTestEnabled)
				{
					std::array<MTLStencilOperation, 8> stencilOps;
					stencilOps[static_cast<int>(StencilOperatorType::Keep)] = MTLStencilOperationKeep;
					stencilOps[static_cast<int>(StencilOperatorType::Zero)] = MTLStencilOperationZero;
					stencilOps[static_cast<int>(StencilOperatorType::Replace)] = MTLStencilOperationReplace;
					stencilOps[static_cast<int>(StencilOperatorType::IncClamp)] = MTLStencilOperationIncrementClamp;
					stencilOps[static_cast<int>(StencilOperatorType::DecClamp)] = MTLStencilOperationDecrementClamp;
					stencilOps[static_cast<int>(StencilOperatorType::Invert)] = MTLStencilOperationDecrementClamp;
					stencilOps[static_cast<int>(StencilOperatorType::IncRepeat)] = MTLStencilOperationIncrementWrap;
					stencilOps[static_cast<int>(StencilOperatorType::DecRepeat)] = MTLStencilOperationDecrementWrap;

					std::array<MTLCompareFunction, 8> stencilCompareFuncs;
					stencilCompareFuncs[static_cast<int>(CompareFuncType::Never)] = MTLCompareFunctionNever;
					stencilCompareFuncs[static_cast<int>(CompareFuncType::Less)] = MTLCompareFunctionLess;
					stencilCompareFuncs[static_cast<int>(CompareFuncType::Equal)] = MTLCompareFunctionEqual;
					stencilCompareFuncs[static_cast<int>(CompareFuncType::LessEqual)] = MTLCompareFunctionLessEqual;
					stencilCompareFuncs[static_cast<int>(CompareFuncType::Greater)] = MTLCompareFunctionGreater;
					stencilCompareFuncs[static_cast<int>(CompareFuncType::NotEqual)] = MTLCompareFunctionNotEqual;
					stencilCompareFuncs[static_cast<int>(CompareFuncType::GreaterEqual)] = MTLCompareFunctionGreaterEqual;
					stencilCompareFuncs[static_cast<int>(CompareFuncType::Always)] = MTLCompareFunctionAlways;

					stencilDescriptor.depthFailureOperation = stencilOps[static_cast<int>(pipstate->StencilDepthFailOp)];
					stencilDescriptor.stencilFailureOperation = stencilOps[static_cast<int>(pipstate->StencilFailOp)];
					stencilDescriptor.depthStencilPassOperation = stencilOps[static_cast<int>(pipstate->StencilPassOp)];
					stencilDescriptor.stencilCompareFunction = stencilCompareFuncs[static_cast<int>(pipstate->StencilCompareFunc)];
					stencilDescriptor.readMask = pipstate->StencilReadMask;
					stencilDescriptor.writeMask = pipstate->StencilWriteMask;
				}
				else
				{
					// always write to stencil reference value
					stencilDescriptor.depthFailureOperation = MTLStencilOperationKeep;
					stencilDescriptor.stencilFailureOperation = MTLStencilOperationKeep;
					stencilDescriptor.depthStencilPassOperation = MTLStencilOperationReplace;
					stencilDescriptor.stencilCompareFunction = MTLCompareFunctionAlways;
					stencilDescriptor.readMask = 0xFF;
					stencilDescriptor.writeMask = 0xFF;
				}

				depthStencilDescriptor.frontFaceStencil = stencilDescriptor;
				depthStencilDescriptor.backFaceStencil = stencilDescriptor;
				[stencilDescriptor release];
			}

			depthStencilState_ = [g->GetDevice() newDepthStencilStateWithDescriptor:depthStencilDescriptor];
			[depthStencilDescriptor release];
		}

		// topology
		if (pipstate->Topology == TopologyType::Triangle)
		{
			if (@available(iOS 12.0, *))
			{
				pipelineStateDescriptor_.inputPrimitiveTopology = MTLPrimitiveTopologyClassTriangle;
			}
			else
			{
				// Fallback on earlier versions
			}
		}
		else if (pipstate->Topology == TopologyType::Line)
		{
			if (@available(iOS 12.0, *))
			{
				pipelineStateDescriptor_.inputPrimitiveTopology = MTLPrimitiveTopologyClassLine;
			}
			else
			{
				Log(LogType::Error, "OS is too old.");
				return false;
			}
		}
		else if (pipstate->Topology == TopologyType::Point)
		{
			if (@available(iOS 12.0, *))
			{
				pipelineStateDescriptor_.inputPrimitiveTopology = MTLPrimitiveTopologyClassPoint;
			}
			else
			{
				Log(LogType::Error, "OS is too old.");
				return false;
			}
		}
		else
		{
			Log(LogType::Error, "Unimplemented TopologyType");
			return false;
		}

		// clulling (on commandlist)

		// setup a blend

		MTLRenderPipelineColorAttachmentDescriptor* colorAttachment = pipelineStateDescriptor_.colorAttachments[0];

		if (pipstate->IsBlendEnabled)
		{
			colorAttachment.blendingEnabled = true;

			std::array<MTLBlendOperation, 10> blendOps;
			blendOps[static_cast<int>(BlendEquationType::Add)] = MTLBlendOperationAdd;
			blendOps[static_cast<int>(BlendEquationType::Sub)] = MTLBlendOperationSubtract;
			blendOps[static_cast<int>(BlendEquationType::ReverseSub)] = MTLBlendOperationReverseSubtract;
			blendOps[static_cast<int>(BlendEquationType::Min)] = MTLBlendOperationMin;
			blendOps[static_cast<int>(BlendEquationType::Max)] = MTLBlendOperationMax;

			std::array<MTLBlendFactor, 20> blendFuncs;
			blendFuncs[static_cast<int>(BlendFuncType::Zero)] = MTLBlendFactorZero;
			blendFuncs[static_cast<int>(BlendFuncType::One)] = MTLBlendFactorOne;
			blendFuncs[static_cast<int>(BlendFuncType::SrcColor)] = MTLBlendFactorSourceColor;
			blendFuncs[static_cast<int>(BlendFuncType::OneMinusSrcColor)] = MTLBlendFactorOneMinusSourceColor;
			blendFuncs[static_cast<int>(BlendFuncType::SrcAlpha)] = MTLBlendFactorSourceAlpha;
			blendFuncs[static_cast<int>(BlendFuncType::OneMinusSrcAlpha)] = MTLBlendFactorOneMinusSourceAlpha;
			blendFuncs[static_cast<int>(BlendFuncType::DstColor)] = MTLBlendFactorDestinationColor;
			blendFuncs[static_cast<int>(BlendFuncType::OneMinusDstColor)] = MTLBlendFactorOneMinusDestinationColor;
			blendFuncs[static_cast<int>(BlendFuncType::DstAlpha)] = MTLBlendFactorDestinationAlpha;
			blendFuncs[static_cast<int>(BlendFuncType::OneMinusDstAlpha)] = MTLBlendFactorOneMinusDestinationAlpha;

			colorAttachment.sourceRGBBlendFactor = blendFuncs[static_cast<int>(pipstate->BlendSrcFunc)];
			colorAttachment.destinationRGBBlendFactor = blendFuncs[static_cast<int>(pipstate->BlendDstFunc)];
			colorAttachment.sourceAlphaBlendFactor = blendFuncs[static_cast<int>(pipstate->BlendSrcFuncAlpha)];
			colorAttachment.destinationAlphaBlendFactor = blendFuncs[static_cast<int>(pipstate->BlendDstFuncAlpha)];
			colorAttachment.rgbBlendOperation = blendOps[static_cast<int>(pipstate->BlendEquationRGB)];
			colorAttachment.alphaBlendOperation = blendOps[static_cast<int>(pipstate->BlendEquationAlpha)];
		}
		else
		{
			colorAttachment.blendingEnabled = false;
		}

		for (size_t i = 0; i < renderPassPipelineState->GetPixelFormats().size(); i++)
		{
			[pipelineStateDescriptor_.colorAttachments objectAtIndexedSubscript:i].pixelFormat =
				renderPassPipelineState->GetPixelFormats().at(i);
		}

		if (renderPassPipelineState->GetDepthStencilFormat() != MTLPixelFormatInvalid)
		{
			pipelineStateDescriptor_.depthAttachmentPixelFormat = renderPassPipelineState->GetDepthStencilFormat();

			if (renderPassPipelineState->Key.DepthFormat == TextureFormatType::D24S8 ||
				renderPassPipelineState->Key.DepthFormat == TextureFormatType::D32S8)
			{
				pipelineStateDescriptor_.stencilAttachmentPixelFormat = renderPassPipelineState->GetDepthStencilFormat();
			}
		}

		pipelineStateDescriptor_.sampleCount = renderPassPipelineState->Key.SamplingCount;

		NSError* pipelineError = nil;
		pipelineState_ = [g->GetDevice() newRenderPipelineStateWithDescriptor:pipelineStateDescriptor_ error:&pipelineError];

		return true;
	}
}

bool PipelineStateMetal::CreateComputePipelineState(PipelineState* self, Graphics* graphics)
{
	@autoreleasepool
	{
		auto g = static_cast<GraphicsMetal*>(graphics);

		auto pipstate = static_cast<PipelineStateMetal*>(self);

		auto cs = static_cast<ShaderMetal*>(pipstate->GetShaders()[static_cast<int>(ShaderStageType::Compute)]);

		if (cs == nullptr)
			return false;

		id<MTLFunction> cf = [[cs->GetLibrary() newFunctionWithName:@"main0"] autorelease];

		NSError* pipelineError = nil;
		computePipelineState_ = [g->GetDevice() newComputePipelineStateWithFunction:cf error:&pipelineError];

		return true;
	}
}

PipelineStateMetal::PipelineStateMetal() { shaders.fill(nullptr); }

PipelineStateMetal::~PipelineStateMetal()
{
	for (auto& shader : shaders)
	{
		SafeRelease(shader);
	}

	if (depthStencilState_ != nullptr)
	{
		[depthStencilState_ release];
		depthStencilState_ = nullptr;
	}

	if (pipelineState_ != nullptr)
	{
		[pipelineState_ release];
		pipelineState_ = nullptr;
	}

	if (computePipelineState_ != nullptr)
	{
		[computePipelineState_ release];
		computePipelineState_ = nullptr;
	}

	SafeRelease(graphics_);
}

bool PipelineStateMetal::Initialize(GraphicsMetal* graphics)
{
	SafeAddRef(graphics);
	SafeRelease(graphics_);
	graphics_ = graphics;

	return true;
}

void PipelineStateMetal::SetShader(ShaderStageType stage, Shader* shader)
{
	SafeAddRef(shader);
	SafeRelease(shaders[static_cast<int>(stage)]);
	shaders[static_cast<int>(stage)] = shader;
}

bool PipelineStateMetal::Compile() { return Compile(this, graphics_); }

}
