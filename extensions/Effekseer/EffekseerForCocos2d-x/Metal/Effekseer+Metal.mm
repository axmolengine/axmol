#include "../EffekseerForCocos2d-x.h"
#if AX_RENDER_API == AX_RENDER_API_MTL

#include "../../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"
#include "../../EffekseerRendererMetal/EffekseerRendererMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.GraphicsMetal.h"
#include "../../EffekseerRendererCommon/ModelLoader.h"
#include "axmol/rhi/metal/TextureMTL.h"
#include "axmol/rhi/metal/CommandBufferMTL.h"
#include "axmol/rhi/metal/UtilsMTL.h"
#include <Metal/LLGI.TextureMetal.h>

namespace efk {

void SetMTLObjectsFromCocos2d(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
    auto d = cocos2d::Director::getInstance();
    auto renderer = d->getRenderer();
    auto buffer = renderer->getCommandBuffer();
    auto bufferM = static_cast<cocos2d::rhi::CommandBufferMTL*>(buffer);

    // use render pass descriptor from Cocos and add depth test
    // auto descriptor = d->getRenderPassDescriptor();
//    ax::rhi::RenderPassDescriptor descriptor;
//    descriptor.flags.clear = true;
    // using axmol render pass
    auto target = renderer->getRenderTarget();

    renderer->beginRenderPass();
    auto v = renderer->getViewport();
    // important for ensuring znear and zfar are in sync with Cocos
    bufferM->setViewport(v.x, v.y, v.w, v.h);

    // set Command Buffer and Render Encoder from Cocos
    EffekseerRendererMetal::BeginCommandList(commandList, bufferM->getRenderCommandEncoder());
}


#pragma region DistortingCallbackMetal
class DistortingCallbackMetal
    : public EffekseerRenderer::DistortingCallback
{

    Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList_ = nullptr;
    id<MTLTexture>                                  texture = nullptr;
    Effekseer::Backend::TextureRef textureInternal_ = nullptr;

public:
    DistortingCallbackMetal(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList);

    virtual ~DistortingCallbackMetal();

    virtual bool OnDistorting(EffekseerRenderer::Renderer* renderer) override;
};

DistortingCallbackMetal::DistortingCallbackMetal(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
: commandList_(commandList)
{
}

DistortingCallbackMetal::~DistortingCallbackMetal()
{
    if(textureInternal_ != nullptr)
    {
        [texture release];
        textureInternal_.Reset();
    }
}

bool DistortingCallbackMetal::OnDistorting(EffekseerRenderer::Renderer* renderer)
{
    // to get viewport
    auto drawable = cocos2d::rhi::DriverMTL::getCurrentDrawable();

    if(textureInternal_ == nullptr)
    {
        auto driver = static_cast<cocos2d::rhi::DriverMTL*>(cocos2d::rhi::DriverBase::getInstance());

        MTLTextureDescriptor* textureDescriptor =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:cocos2d::rhi::UtilsMTL::getDefaultColorAttachmentPixelFormat()
                                                           width:drawable.texture.width
                                                          height:drawable.texture.height
                                                       mipmapped:NO];

        texture = [driver->getMTLDevice() newTextureWithDescriptor:textureDescriptor];
        textureInternal_ = EffekseerRendererMetal::CreateTexture(renderer->GetGraphicsDevice(), texture);
    }

    auto rendererAX = cocos2d::Director::getInstance()->getRenderer();
    auto commandBuffer = static_cast<cocos2d::rhi::CommandBufferMTL*>(rendererAX->getCommandBuffer());
    commandBuffer->endEncoding();

    EffekseerRendererMetal::EndCommandList(commandList_);

    MTLRegion region =
    {
        {0, 0, 0},          // MTLOrigin
        {texture.width, texture.height, 1}  // MTLSize
    };

    id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer->getMTLCommandBuffer() blitCommandEncoder];

    [blitEncoder copyFromTexture:drawable.texture sourceSlice:0 sourceLevel:0 sourceOrigin:region.origin sourceSize:region.size toTexture:texture destinationSlice:0 destinationLevel:0 destinationOrigin:{0, 0, 0}];
    [blitEncoder endEncoding];
    cocos2d::rhi::DriverBase::getInstance()->setFrameBufferOnly(true); // reset

    SetMTLObjectsFromCocos2d(commandList_);

    auto r = static_cast<EffekseerRendererLLGI::Renderer*>(renderer);
    r->SetBackground(textureInternal_);

    return true;
}
#pragma endregion

static ::Effekseer::Backend::GraphicsDeviceRef g_graphicsDevice = nullptr;

class EffekseerGraphicsDevice : public ::EffekseerRendererLLGI::Backend::GraphicsDevice
{
private:

public:
    EffekseerGraphicsDevice(LLGI::Graphics* graphics)
        : ::EffekseerRendererLLGI::Backend::GraphicsDevice(graphics)
    {
    }

    virtual ~EffekseerGraphicsDevice()
    {
        g_graphicsDevice = nullptr;
    }

    static ::Effekseer::Backend::GraphicsDeviceRef create()
    {
        if (g_graphicsDevice == nullptr)
        {
            auto graphics = new LLGI::GraphicsMetal();
            graphics->Initialize(nullptr);

            g_graphicsDevice = std::move(Effekseer::MakeRefPtr<EffekseerGraphicsDevice>(graphics));
            ES_SAFE_RELEASE(graphics);
        }

        //creating_ = false;

        return g_graphicsDevice;
    }

    int Release() override
    {
        auto ret = ::EffekseerRendererLLGI::Backend::GraphicsDevice::Release();
        if (ret == 1)
        {
            g_graphicsDevice = nullptr;
        }

        return ret;
    }
};

Effekseer::ModelLoaderRef CreateModelLoader(Effekseer::FileInterfaceRef effectFile)
{
	return Effekseer::MakeRefPtr<::EffekseerRenderer::ModelLoader>(EffekseerGraphicsDevice::create(), effectFile);
}

::Effekseer::MaterialLoaderRef CreateMaterialLoader(Effekseer::FileInterfaceRef effectFile)
{
    auto device = EffekseerGraphicsDevice::create();
    auto ret = EffekseerRendererMetal::CreateMaterialLoader(device, effectFile);
    return ret;
}

void UpdateTextureData(::Effekseer::TextureRef textureData, cocos2d::Texture2D* texture)
{
    auto textureImpl = static_cast<ax::rhi::mtl::TextureImpl*>(texture->getBackendTexture());
	auto device = EffekseerGraphicsDevice::create().DownCast<::EffekseerRendererLLGI::Backend::GraphicsDevice>();

    auto backend = device->CreateTexture(textureImpl->internalHandle(), []() -> void {});
	textureData->SetBackend(backend);
}

void CleanupTextureData(::Effekseer::TextureRef textureData)
{
}

::EffekseerRenderer::DistortingCallback* CreateDistortingCallback(::EffekseerRenderer::RendererRef renderer, Effekseer::RefPtr<::EffekseerRenderer::CommandList> commandList)
{
    return new DistortingCallbackMetal(commandList);
}


void EffectEmitter::beforeRender(EffekseerRenderer::RendererRef renderer, Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
    SetMTLObjectsFromCocos2d(commandList);
}

void EffectEmitter::afterRender(EffekseerRenderer::RendererRef renderer, Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
    EffekseerRendererMetal::EndCommandList(commandList);
}

void EffectManager::onDestructor()
{
    EffekseerRendererMetal::EndCommandList(commandList_);
}

void EffectManager::CreateRenderer(int32_t spriteSize)
{
    auto device = EffekseerGraphicsDevice::create();
    renderer2d = EffekseerRendererMetal::Create(device,
                                                spriteSize,
                                                cocos2d::rhi::UtilsMTL::getDefaultColorAttachmentPixelFormat(),
                                                cocos2d::rhi::UtilsMTL::getDefaultDepthStencilAttachmentPixelFormat(),
                                                false);

    memoryPool_ = EffekseerRenderer::CreateSingleFrameMemoryPool(device);
    commandList_ = EffekseerRenderer::CreateCommandList(device, memoryPool_);
    renderer2d->SetCommandList(commandList_);
}

void EffectManager::newFrame()
{
    if(memoryPool_ != nullptr)
    {
        memoryPool_->NewFrame();
    }

    auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer2d.Get());
    auto vb = static_cast<::EffekseerRendererLLGI::VertexBuffer*>(r->GetVertexBuffer());
}

void ResetBackground(::EffekseerRenderer::RendererRef renderer)
{
    auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer.Get());
    r->SetBackground(nullptr);
}

}

#endif
