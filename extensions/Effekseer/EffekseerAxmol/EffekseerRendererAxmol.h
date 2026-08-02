#pragma once

#include "axmol/axmol.h"
#include "axmol/rhi/Buffer.h"
#include "axmol/renderer/CustomCommand.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/rhi/Texture.h"
#include <Effekseer.h>
#include <EffekseerRendererCommon/EffekseerRenderer.Renderer.h>
#include <EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h>

namespace EffekseerRendererAxmol
{
class Renderer;
class Shader;
using RendererRef = Effekseer::RefPtr<Renderer>;

class Renderer : public EffekseerRenderer::Renderer, public Effekseer::ReferenceObject
{
public:
    static RendererRef Create(int32_t squareMaxCount);

    explicit Renderer(int32_t squareMaxCount);
    ~Renderer() override;

    void OnLostDevice() override {}
    void OnResetDevice() override {}
    void SetRestorationOfStatesFlag(bool flag) override { AX_UNUSED_PARAM(flag); }
    bool BeginRendering() override;
    bool EndRendering() override;
    int32_t GetSquareMaxCount() const override { return _squareMaxCount; }

    Effekseer::SpriteRendererRef CreateSpriteRenderer() override;
    Effekseer::RibbonRendererRef CreateRibbonRenderer() override;
    Effekseer::RingRendererRef CreateRingRenderer() override;
    Effekseer::ModelRendererRef CreateModelRenderer() override;
    Effekseer::TrackRendererRef CreateTrackRenderer() override;
    Effekseer::GpuTimerRef CreateGpuTimer() override { return nullptr; }
    Effekseer::GpuParticleSystemRef CreateGpuParticleSystem(const Effekseer::GpuParticleSystem::Settings& settings = {}) override;
    Effekseer::GpuParticleFactoryRef CreateGpuParticleFactory() override { return nullptr; }

    Effekseer::TextureLoaderRef CreateTextureLoader(Effekseer::FileInterfaceRef fileInterface = nullptr) override;
    Effekseer::ModelLoaderRef CreateModelLoader(Effekseer::FileInterfaceRef fileInterface = nullptr) override;
    Effekseer::MaterialLoaderRef CreateMaterialLoader(Effekseer::FileInterfaceRef fileInterface = nullptr) override;

    void ResetRenderState() override;
    EffekseerRenderer::DistortingCallback* GetDistortingCallback() override { return _distortingCallback; }
    void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

    Effekseer::Backend::GraphicsDeviceRef GetGraphicsDevice() const override { return _graphicsDevice; }
    EffekseerRenderer::RenderStateBase* GetRenderState() { return _renderState.get(); }
    EffekseerRenderer::StandardRenderer<Renderer, Shader>* GetStandardRenderer() { return _standardRenderer.get(); }
    Effekseer::Backend::IndexBufferRef GetIndexBuffer() { return _indexBuffer; }

    void BeginShader(EffekseerRenderer::ShaderBase* shader);
    void EndShader(EffekseerRenderer::ShaderBase* shader);
    void SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t stride);
    void SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer);
    void SetLayout(EffekseerRenderer::ShaderBase* shader);
    void SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset);
    void SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset);
    void SetTextures(EffekseerRenderer::ShaderBase* shader, Effekseer::Backend::TextureRef* textures, int32_t count);
    void DrawSprites(int32_t spriteCount, int32_t vertexOffset);
    void DrawPolygon(int32_t vertexCount, int32_t indexCount);
    void DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount);
    void ResetStateForDefferedContext() {}

    void BeginFrame(ax::Renderer* renderer);
    void ReleaseCachedCommands();
    void SetGlobalZOrder(float globalZOrder) { _globalZOrder = globalZOrder; }

    int GetRef() override { return Effekseer::ReferenceObject::GetRef(); }
    int AddRef() override { return Effekseer::ReferenceObject::AddRef(); }
    int Release() override { return Effekseer::ReferenceObject::Release(); }

private:
    bool initialize();

    int32_t _squareMaxCount = 0;
    Effekseer::Backend::GraphicsDeviceRef _graphicsDevice;
    Effekseer::Backend::IndexBufferRef _indexBuffer;
    Effekseer::Backend::VertexBufferRef _currentVertexBuffer;
    Effekseer::Backend::IndexBufferRef _currentIndexBuffer;
    int32_t _currentVertexStride = 0;
    EffekseerRenderer::ShaderBase* _currentShader = nullptr;
    std::unique_ptr<EffekseerRenderer::RenderStateBase> _renderState;
    std::unique_ptr<EffekseerRenderer::StandardRenderer<Renderer, Shader>> _standardRenderer;
    EffekseerRenderer::DistortingCallback* _distortingCallback = nullptr;
    ax::Renderer* _axRenderer = nullptr;
    float _globalZOrder = 0.0f;
    size_t _commandIndex = 0;
    std::vector<std::unique_ptr<ax::CustomCommand>> _commands;
    std::vector<size_t> _commandStrides;
};
// Factory for the distortion background callback.
EffekseerRenderer::DistortingCallback* createDistortingCallback();

} // namespace EffekseerRendererAxmol
