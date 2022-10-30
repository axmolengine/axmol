#pragma once

#include "../LLGI.Graphics.h"
#import <MetalKit/MetalKit.h>
#include <functional>
#include <unordered_map>

namespace LLGI
{

struct Graphics_Impl;
struct RenderPass_Impl;
struct RenderPassPipelineState_Impl;

class GraphicsMetal;
class RenderPassMetal;
class RenderPassPipelineStateMetal;
class TextureMetal;

class RenderPassMetal : public RenderPass
{
	MTLRenderPassDescriptor* renderPassDescriptor_;

	bool Initialize();

	void UpdateTarget(TextureMetal** textures,
					  int32_t textureCount,
					  TextureMetal* depthTexture,
					  TextureMetal* resolvedTexture,
					  TextureMetal* resolvedDepthTexture);

public:
	RenderPassMetal();

	~RenderPassMetal() override;

	bool UpdateRenderTarget(
		Texture** textures, int32_t textureCount, Texture* depthTexture, Texture* resolvedTexture, Texture* resolvedDepthTexture);

	void SetIsColorCleared(bool isColorCleared) override;

	void SetIsDepthCleared(bool isDepthCleared) override;

	void SetClearColor(const Color8& color) override;

	MTLRenderPassDescriptor* GetRenderPassDescriptor() { return renderPassDescriptor_; }

	Color8 clearColor;
	bool isColorCleared;
	bool isDepthCleared;
	FixedSizeVector<MTLPixelFormat, RenderTargetMax> pixelFormats;
	MTLPixelFormat depthStencilFormat = MTLPixelFormatInvalid;
};

class RenderPassPipelineStateMetal : public RenderPassPipelineState
{
private:
	FixedSizeVector<MTLPixelFormat, RenderTargetMax> pixelFormats_;
	MTLPixelFormat depthStencilFormat_ = MTLPixelFormatInvalid;

public:
	RenderPassPipelineStateMetal();
	~RenderPassPipelineStateMetal() override = default;

	void SetKey(const RenderPassPipelineStateKey& key);

	const FixedSizeVector<MTLPixelFormat, RenderTargetMax>& GetPixelFormats() const { return pixelFormats_; }
	const MTLPixelFormat& GetDepthStencilFormat() const { return depthStencilFormat_; }
};

} // namespace LLGI
