#include "LLGI.CommandListMetal.h"
#include "LLGI.BufferMetal.h"
#include "LLGI.GraphicsMetal.h"
#include "LLGI.Metal_Impl.h"
#include "LLGI.PipelineStateMetal.h"
#include "LLGI.RenderPassMetal.h"
#include "LLGI.TextureMetal.h"

#import <MetalKit/MetalKit.h>
#include <TargetConditionals.h>

namespace LLGI
{

CommandListMetal::CommandListMetal(Graphics* graphics)
{
	auto g = static_cast<GraphicsMetal*>(graphics);
	SafeAddRef(g);
	graphics_ = g;

	// Sampler
	for (int w = 0; w < 2; w++)
	{
		for (int f = 0; f < 2; f++)
		{
			for (int m = 0; m < 3; m++)
			{
				MTLSamplerAddressMode ws[2];
				ws[0] = MTLSamplerAddressModeClampToEdge;
				ws[1] = MTLSamplerAddressModeRepeat;

				MTLSamplerMinMagFilter fsmin[2];
				fsmin[0] = MTLSamplerMinMagFilterNearest;
				fsmin[1] = MTLSamplerMinMagFilterLinear;

				MTLSamplerMipFilter msmip[3];
				msmip[0] = MTLSamplerMipFilterNotMipmapped;
				msmip[1] = MTLSamplerMipFilterLinear;
				msmip[2] = MTLSamplerMipFilterNearest;

				MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
				samplerDescriptor.minFilter = fsmin[f];
				samplerDescriptor.magFilter = fsmin[f];
				samplerDescriptor.mipFilter = msmip[m];
				samplerDescriptor.sAddressMode = ws[w];
				samplerDescriptor.tAddressMode = ws[w];

				samplers_[w][f][m] = samplerDescriptor;
				samplerStates_[w][f][m] = [g->GetDevice() newSamplerStateWithDescriptor:samplerDescriptor];
			}
		}
	}

	fence_ = [g->GetDevice() newFence];
}

CommandListMetal::~CommandListMetal()
{
	if (isInRenderPass_)
	{
		EndRenderPass();
	}

	if (isInBegin_)
	{
		End();
	}

	WaitUntilCompleted();

	for (int w = 0; w < 2; w++)
	{
		for (int f = 0; f < 2; f++)
		{
			for (int m = 0; m < 3; m++)
			{
				[samplers_[w][f][m] release];
				[samplerStates_[w][f][m] release];
			}
		}
	}

	if (commandBuffer_ != nullptr)
	{
		[commandBuffer_ release];
		commandBuffer_ = nullptr;
	}

	if (renderEncoder_ != nullptr)
	{
		[renderEncoder_ release];
	}

	if (fence_ != nullptr)
	{
		[fence_ release];
	}

	SafeRelease(graphics_);
}

void CommandListMetal::Begin()
{
	@autoreleasepool
	{
		if (commandBuffer_ != nullptr)
		{
			[commandBuffer_ release];
			commandBuffer_ = nullptr;
		}

		commandBuffer_ = [graphics_->GetCommandQueue() commandBuffer];
		[commandBuffer_ retain];

		auto t = this;

		[commandBuffer_ addCompletedHandler:^(id buffer) {
		  t->isCompleted_ = true;
		}];

		CommandList::Begin();
	}
}

void CommandListMetal::End() { CommandList::End(); }

void CommandListMetal::SetScissor(int32_t x, int32_t y, int32_t width, int32_t height)
{
	MTLScissorRect rect;
	rect.x = x;
	rect.y = y;
	rect.width = width;
	rect.height = height;
	[renderEncoder_ setScissorRect:rect];
}

void CommandListMetal::Draw(int32_t primitiveCount, int32_t instanceCount)
{
	BindingVertexBuffer bvb;
	BindingIndexBuffer bib;
	PipelineState* bpip = nullptr;

	const int mipmapFilter = 1;

	bool isVBDirtied = false;
	bool isIBDirtied = false;
	bool isPipDirtied = false;

	GetCurrentVertexBuffer(bvb, isVBDirtied);
	GetCurrentIndexBuffer(bib, isIBDirtied);
	GetCurrentPipelineState(bpip, isPipDirtied);

	assert(bvb.vertexBuffer != nullptr);
	assert(bib.indexBuffer != nullptr);
	assert(bpip != nullptr);

	auto vb = static_cast<BufferMetal*>(bvb.vertexBuffer);
	auto ib = static_cast<BufferMetal*>(bib.indexBuffer);
	auto pip = static_cast<PipelineStateMetal*>(bpip);

	// set cull mode
	if (pip->Culling == LLGI::CullingMode::Clockwise)
	{
		[renderEncoder_ setCullMode:MTLCullModeFront];
	}
	else if (pip->Culling == LLGI::CullingMode::CounterClockwise)
	{
		[renderEncoder_ setCullMode:MTLCullModeBack];
	}
	else
	{
		[renderEncoder_ setCullMode:MTLCullModeNone];
	}

	[renderEncoder_ setFrontFacingWinding:MTLWindingCounterClockwise];

	if (isVBDirtied)
	{
		[renderEncoder_ setVertexBuffer:vb->GetBuffer() offset:bvb.offset atIndex:VertexBufferIndex];
	}

	// assign constant buffer
	Buffer* vcb = nullptr;
	GetCurrentConstantBuffer(ShaderStageType::Vertex, vcb);
	if (vcb != nullptr)
	{
		auto vcb_ = static_cast<BufferMetal*>(vcb);
		[renderEncoder_ setVertexBuffer:vcb_->GetBuffer() offset:vcb_->GetOffset() atIndex:0];
	}

	Buffer* pcb = nullptr;
	GetCurrentConstantBuffer(ShaderStageType::Pixel, pcb);
	if (pcb != nullptr)
	{
		auto pcb_ = static_cast<BufferMetal*>(pcb);
		[renderEncoder_ setFragmentBuffer:pcb_->GetBuffer() offset:pcb_->GetOffset() atIndex:0];
	}

	for (int stage_ind = 0; stage_ind < 2; stage_ind++)
	{
		// Assign textures
		for (int unit_ind = 0; unit_ind < currentTextures[stage_ind].size(); unit_ind++)
		{
			if (currentTextures[stage_ind][unit_ind].texture == nullptr)
				continue;

			auto texture = (TextureMetal*)currentTextures[stage_ind][unit_ind].texture;
			auto wm = (int32_t)currentTextures[stage_ind][unit_ind].wrapMode;
			auto mm = (int32_t)currentTextures[stage_ind][unit_ind].minMagFilter;
			auto pm = 0;
			if (texture->GetTexture().mipmapLevelCount >= 2)
			{
				pm = mipmapFilter;
			}

			if (stage_ind == (int32_t)ShaderStageType::Vertex)
			{
				[renderEncoder_ setVertexTexture:texture->GetTexture() atIndex:unit_ind];
				[renderEncoder_ setVertexSamplerState:samplerStates_[wm][mm][pm] atIndex:unit_ind];
			}

			if (stage_ind == (int32_t)ShaderStageType::Pixel)
			{
				[renderEncoder_ setFragmentTexture:texture->GetTexture() atIndex:unit_ind];
				[renderEncoder_ setFragmentSamplerState:samplerStates_[wm][mm][pm] atIndex:unit_ind];
			}
		}

		for (int unit_ind = 0; unit_ind < NumComputeBuffer; unit_ind++)
		{
			BindingComputeBuffer bcb;
			GetCurrentComputeBuffer(unit_ind, (ShaderStageType)stage_ind, bcb);
			if (bcb.computeBuffer == nullptr)
				continue;
			
			auto cb = static_cast<BufferMetal*>(bcb.computeBuffer);
			[computeEncoder_ setBuffer:cb->GetBuffer() offset:cb->GetOffset() atIndex:1 + unit_ind];
		}
	}

	if (isPipDirtied)
	{
		[renderEncoder_ setRenderPipelineState:pip->GetRenderPipelineState()];

		if (pip->GetDepthStencilState() != nullptr)
		{
			[renderEncoder_ setDepthStencilState:pip->GetDepthStencilState()];
		}

		[renderEncoder_ setStencilReferenceValue:pip->StencilRef];
	}

	// draw
	MTLPrimitiveType topology = MTLPrimitiveTypeTriangle;
	MTLIndexType indexType = MTLIndexTypeUInt32;
	int indexPerPrim = 0;

	if (bpip->Topology == TopologyType::Triangle)
	{
		indexPerPrim = 3;
		topology = MTLPrimitiveTypeTriangle;
	}
	else if (bpip->Topology == TopologyType::Line)
	{
		indexPerPrim = 2;
		topology = MTLPrimitiveTypeLine;
	}
	else if (bpip->Topology == TopologyType::Point)
	{
		indexPerPrim = 1;
		topology = MTLPrimitiveTypePoint;
	}
	else
	{
		assert(0);
	}

	assert(bib.stride == 2 || bib.stride == 4);
	if (bib.stride == 2)
	{
		indexType = MTLIndexTypeUInt16;
	}

	[renderEncoder_ drawIndexedPrimitives:topology
							   indexCount:primitiveCount * indexPerPrim
								indexType:indexType
							  indexBuffer:ib->GetBuffer()
						indexBufferOffset:bib.offset
							instanceCount:instanceCount];

	CommandList::Draw(primitiveCount, instanceCount);
}

void CommandListMetal::CopyTexture(Texture* src, Texture* dst)
{
	auto srcTex = static_cast<TextureMetal*>(src);
	CopyTexture(src, dst, {0, 0, 0}, {0, 0, 0}, srcTex->GetParameter().Size, 0, 0);
}

void CommandListMetal::CopyTexture(
	Texture* src, Texture* dst, const Vec3I& srcPos, const Vec3I& dstPos, const Vec3I& size, int srcLayer, int dstLayer)
{
	@autoreleasepool
	{
		if (isInRenderPass_)
		{
			Log(LogType::Error, "Please call CopyTexture outside of RenderPass");
			return;
		}

		auto srcTex = static_cast<TextureMetal*>(src);
		auto dstTex = static_cast<TextureMetal*>(dst);

		id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer_ blitCommandEncoder];

		MTLRegion region = {{(uint32_t)srcPos[0], (uint32_t)srcPos[1], (uint32_t)srcPos[2]},
							{(uint32_t)size[0], (uint32_t)size[1], (uint32_t)size[2]}};

		[blitEncoder copyFromTexture:srcTex->GetTexture()
						 sourceSlice:srcLayer
						 sourceLevel:0
						sourceOrigin:region.origin
						  sourceSize:region.size
						   toTexture:dstTex->GetTexture()
					destinationSlice:dstLayer
					destinationLevel:0
				   destinationOrigin:{(uint32_t)dstPos[0], (uint32_t)dstPos[1], (uint32_t)dstPos[2]}];
		[blitEncoder endEncoding];

		RegisterReferencedObject(src);
		RegisterReferencedObject(dst);
	}
}

void CommandListMetal::GenerateMipMap(Texture* src)
{
	auto srcTex = static_cast<TextureMetal*>(src);

	id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer_ blitCommandEncoder];
	[blitEncoder generateMipmapsForTexture:srcTex->GetTexture()];
	[blitEncoder endEncoding];
}

void CommandListMetal::BeginRenderPass(RenderPass* renderPass)
{
	@autoreleasepool
	{
		auto rp = static_cast<RenderPassMetal*>(renderPass);
		auto rpd = rp->GetRenderPassDescriptor();

		for (size_t i = 0; i < rp->pixelFormats.size(); i++)
		{
			if (rp->isColorCleared)
			{
				auto r_ = rp->clearColor.R / 255.0;
				auto g_ = rp->clearColor.G / 255.0;
				auto b_ = rp->clearColor.B / 255.0;
				auto a_ = rp->clearColor.A / 255.0;

				rpd.colorAttachments[i].loadAction = MTLLoadActionClear;
				rpd.colorAttachments[i].clearColor = MTLClearColorMake(r_, g_, b_, a_);
			}
			else
			{
				rpd.colorAttachments[i].loadAction = MTLLoadActionDontCare;
			}
		}

		if (rp->isDepthCleared)
		{
			rpd.depthAttachment.loadAction = MTLLoadActionClear;
			rpd.depthAttachment.clearDepth = 1.0;

			if (rp->depthStencilFormat != MTLPixelFormatDepth32Float_Stencil8
#if !(TARGET_OS_IPHONE) && !(TARGET_OS_SIMULATOR)
				&& rp->depthStencilFormat != MTLPixelFormatDepth24Unorm_Stencil8
#endif
			)
			{
				rpd.stencilAttachment.loadAction = MTLLoadActionClear;
				rpd.stencilAttachment.clearStencil = 0;
			}
		}
		else
		{
			rpd.depthAttachment.loadAction = MTLLoadActionDontCare;
			rpd.stencilAttachment.loadAction = MTLLoadActionDontCare;
		}

		renderEncoder_ = [commandBuffer_ renderCommandEncoderWithDescriptor:rpd];
		[renderEncoder_ retain];
		[renderEncoder_ waitForFence:fence_ beforeStages:MTLRenderStageVertex];

		CommandList::BeginRenderPass(renderPass);
	}
}

void CommandListMetal::EndRenderPass()
{
	if (renderEncoder_)
	{
		[renderEncoder_ updateFence:fence_ afterStages:MTLRenderStageFragment];
		[renderEncoder_ endEncoding];
		[renderEncoder_ release];
		renderEncoder_ = nullptr;
	}

	CommandList::EndRenderPass();
}

void CommandListMetal::WaitUntilCompleted()
{
	if (commandBuffer_ != nullptr)
	{
		auto status = [commandBuffer_ status];
		if (status == MTLCommandBufferStatusNotEnqueued)
		{
			return;
		}

		[commandBuffer_ waitUntilCompleted];
	}
}

bool CommandListMetal::BeginWithPlatform(void* platformContextPtr) { return CommandList::BeginWithPlatform(platformContextPtr); }

void CommandListMetal::EndWithPlatform() { CommandList::EndWithPlatform(); }

bool CommandListMetal::BeginRenderPassWithPlatformPtr(void* platformPtr)
{
	auto pp = reinterpret_cast<CommandListMetalPlatformRenderPassContext*>(platformPtr);

	this->renderEncoder_ = pp->RenderEncoder;

	if (this->renderEncoder_)
	{
		[this->renderEncoder_ retain];
		// TODO : make correct. wait can do only once per encorder
		// [this->renderEncoder waitForFence:fence beforeStages:MTLRenderStageVertex];
	}

	return CommandList::BeginRenderPassWithPlatformPtr(platformPtr);
}

bool CommandListMetal::EndRenderPassWithPlatformPtr()
{
	if (renderEncoder_)
	{
		// TODO : make correct. wait can do only once per encorder
		// [renderEncoder updateFence:fence afterStages:MTLRenderStageFragment];
		[renderEncoder_ release];
		renderEncoder_ = nullptr;
	}

	return CommandList::EndRenderPassWithPlatformPtr();
}

void CommandListMetal::BeginComputePass()
{
	computeEncoder_ = [commandBuffer_ computeCommandEncoder];
	[computeEncoder_ retain];
}

void CommandListMetal::EndComputePass()
{
	if (computeEncoder_)
	{
		[computeEncoder_ endEncoding];
		[computeEncoder_ release];
		computeEncoder_ = nullptr;
	}
}

void CommandListMetal::Dispatch(int32_t groupX, int32_t groupY, int32_t groupZ, int32_t threadX, int32_t threadY, int32_t threadZ)
{
	PipelineState* bpip = nullptr;

	bool isPipDirtied = false;

	GetCurrentPipelineState(bpip, isPipDirtied);

	assert(bpip != nullptr);
	assert(computeEncoder_ != nullptr);

	auto pip = static_cast<PipelineStateMetal*>(bpip);
    
    for (int unit_ind = 0; unit_ind < NumComputeBuffer; unit_ind++)
    {
        BindingComputeBuffer bcb;
        GetCurrentComputeBuffer(unit_ind, ShaderStageType::Compute, bcb);
        if (bcb.computeBuffer == nullptr)
            continue;
        
        auto cb = static_cast<BufferMetal*>(bcb.computeBuffer);
        [computeEncoder_ setBuffer:cb->GetBuffer() offset:cb->GetOffset() atIndex:1 + unit_ind];
    }
    
	// assign constant buffer
	Buffer* ccb = nullptr;
	GetCurrentConstantBuffer(ShaderStageType::Compute, ccb);
	if (ccb != nullptr)
	{
		auto ccb_ = static_cast<BufferMetal*>(ccb);
		[computeEncoder_ setBuffer:ccb_->GetBuffer() offset:ccb_->GetOffset() atIndex:0];
	}

	if (isPipDirtied)
	{
		[computeEncoder_ setComputePipelineState:pip->GetComputePipelineState()];
	}

	[computeEncoder_ dispatchThreadgroups:{(uint32_t)groupX, (uint32_t)groupY, (uint32_t)groupZ} threadsPerThreadgroup:{(uint32_t)threadX, (uint32_t)threadY, (uint32_t)threadZ}];

	CommandList::Dispatch(groupX, groupY, groupZ, threadX, threadY, threadZ);
}

void CommandListMetal::CopyBuffer(Buffer* src, Buffer* dst)
{
    auto srcBuf = static_cast<BufferMetal*>(src);
    auto srcGpuBuf = srcBuf->GetBuffer();

    auto dstBuf = static_cast<BufferMetal*>(dst);
    auto dstGpuBuf = dstBuf->GetBuffer();

    auto encoder = [commandBuffer_ blitCommandEncoder];
    [encoder retain];
    [encoder copyFromBuffer:srcGpuBuf sourceOffset:srcBuf->GetOffset() toBuffer:dstGpuBuf destinationOffset:dstBuf->GetOffset() size:srcBuf->GetSize()];
    [encoder endEncoding];
    [encoder release];
}

}
