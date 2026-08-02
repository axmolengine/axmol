#include "EffekseerRendererAxmol.h"

#include "axmol/rhi/DriverBase.h"
#include "axmol/rhi/GraphicsCore.h"
#include "axmol/rhi/RenderTarget.h"
#include "axmol/rhi/VertexLayout.h"
#include "axmol/platform/Image.h"
#include "axmol/platform/RenderViewCore.h"
#include "axmol/renderer/ProgramManager.h"
#include "axmol/renderer/Texture2D.h"
#include <EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h>
#include <EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h>
#include <EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h>
#include <unordered_map>
#include <memory>
#include <EffekseerRendererCommon/ModelLoader.h>
#include <EffekseerRendererCommon/VertexBuffer.h>

namespace EffekseerRendererAxmol
{
namespace
{
class ImagePMAAccessor : public ax::Image
{
public:
    static bool pngPremultipliedAlphaEnabled() { return PNG_PREMULTIPLIED_ALPHA_ENABLED; }
};

class ScopedPNGStraightAlpha
{
public:
    ScopedPNGStraightAlpha()
        : _previous(ImagePMAAccessor::pngPremultipliedAlphaEnabled())
    {
        ax::Image::setPNGPremultipliedAlphaEnabled(false);
    }

    ~ScopedPNGStraightAlpha() { ax::Image::setPNGPremultipliedAlphaEnabled(_previous); }

private:
    bool _previous = true;
};

float ToSamplerMode(Effekseer::TextureFilterType filter, Effekseer::TextureWrapType wrap)
{
    const bool nearest = filter == Effekseer::TextureFilterType::Nearest;
    const bool repeat = wrap == Effekseer::TextureWrapType::Repeat || wrap == Effekseer::TextureWrapType::Mirror;
    if (nearest)
        return repeat ? 3.0f : 2.0f;
    return repeat ? 1.0f : 0.0f;
}

class AxVertexBuffer : public Effekseer::Backend::VertexBuffer
{
public:
    AxVertexBuffer(ax::rhi::Buffer* buffer, int32_t size, const void* initialData) : _buffer(buffer), _shadow(size)
    {
        if (_buffer)
            _buffer->retain();
        if (initialData && size > 0)
            memcpy(_shadow.data(), initialData, size);
        if (_buffer && size > 0)
            _buffer->updateData(_shadow.data(), _shadow.size());
    }

    ~AxVertexBuffer() override { AX_SAFE_RELEASE(_buffer); }

    void UpdateData(const void* src, int32_t size, int32_t offset) override
    {
        if (!_buffer || !src || size <= 0 || offset < 0)
            return;

        const auto dstOffset = static_cast<size_t>(offset);
        const auto dstSize   = static_cast<size_t>(size);
        if (dstOffset + dstSize > _shadow.size())
            return;

        memcpy(_shadow.data() + dstOffset, src, dstSize);
        _buffer->updateData(_shadow.data(), _shadow.size());
    }

    ax::rhi::Buffer* get() const { return _buffer; }
    const uint8_t* shadowData() const { return _shadow.data(); }
    size_t shadowSize() const { return _shadow.size(); }

private:
    ax::rhi::Buffer* _buffer = nullptr;
    std::vector<uint8_t> _shadow;
};

class AxIndexBuffer : public Effekseer::Backend::IndexBuffer
{
public:
    AxIndexBuffer(ax::rhi::Buffer* buffer,
                  int32_t elementCount,
                  Effekseer::Backend::IndexBufferStrideType stride,
                  const void* initialData)
        : _buffer(buffer)
    {
        if (_buffer)
            _buffer->retain();
        strideType_ = stride;
        elementCount_ = elementCount;
        _strideSize = stride == Effekseer::Backend::IndexBufferStrideType::Stride2 ? 2 : 4;
        _shadow.resize(static_cast<size_t>(elementCount) * _strideSize);
        if (initialData && !_shadow.empty())
            memcpy(_shadow.data(), initialData, _shadow.size());
        if (_buffer && !_shadow.empty())
            _buffer->updateData(_shadow.data(), _shadow.size());
    }

    ~AxIndexBuffer() override { AX_SAFE_RELEASE(_buffer); }

    void UpdateData(const void* src, int32_t size, int32_t offset) override
    {
        if (!_buffer || !src || size <= 0 || offset < 0)
            return;

        const auto dstOffset = static_cast<size_t>(offset);
        const auto dstSize   = static_cast<size_t>(size);
        if (dstOffset + dstSize > _shadow.size())
            return;

        memcpy(_shadow.data() + dstOffset, src, dstSize);
        _buffer->updateData(_shadow.data(), _shadow.size());
    }

    ax::rhi::Buffer* get() const { return _buffer; }

private:
    ax::rhi::Buffer* _buffer = nullptr;
    size_t _strideSize = 2;
    std::vector<uint8_t> _shadow;
};

class AxTexture : public Effekseer::Backend::Texture
{
public:
    explicit AxTexture(ax::rhi::Texture* texture) : _texture(texture)
    {
        if (_texture)
            _texture->retain();
        param_.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
        param_.Dimension = 2;
        param_.Size = {_texture ? _texture->getWidth() : 1, _texture ? _texture->getHeight() : 1, 1};
    }

    ~AxTexture() override { AX_SAFE_RELEASE(_texture); }
    ax::rhi::Texture* get() const { return _texture; }

private:
    ax::rhi::Texture* _texture = nullptr;
};

class AxVertexLayout : public Effekseer::Backend::VertexLayout
{
};

class AxUniformBuffer : public Effekseer::Backend::UniformBuffer
{
public:
    explicit AxUniformBuffer(int32_t size, const void* initialData) : data(size)
    {
        if (initialData && size > 0)
            memcpy(data.data(), initialData, size);
    }
    std::vector<uint8_t> data;
};

class AxPipelineState : public Effekseer::Backend::PipelineState
{
public:
    Effekseer::Backend::PipelineStateParameter param;
};

class AxShader : public Effekseer::Backend::Shader
{
};

class GraphicsDevice : public Effekseer::Backend::GraphicsDevice
{
public:
    Effekseer::Backend::VertexBufferRef CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override
    {
        auto buffer = ax::rhi::GraphicsCore::currentDriver()->createBuffer(
            static_cast<size_t>(size), ax::rhi::BufferType::VERTEX,
            isDynamic ? ax::rhi::BufferUsage::DYNAMIC : ax::rhi::BufferUsage::STATIC, initialData);
        auto ret = Effekseer::MakeRefPtr<AxVertexBuffer>(buffer, size, initialData);
        AX_SAFE_RELEASE(buffer);
        return ret;
    }

    Effekseer::Backend::IndexBufferRef CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override
    {
        const auto strideSize = stride == Effekseer::Backend::IndexBufferStrideType::Stride2 ? 2 : 4;
        auto buffer = ax::rhi::GraphicsCore::currentDriver()->createBuffer(
            static_cast<size_t>(elementCount * strideSize), ax::rhi::BufferType::INDEX, ax::rhi::BufferUsage::STATIC, initialData);
        auto ret = Effekseer::MakeRefPtr<AxIndexBuffer>(buffer, elementCount, stride, initialData);
        AX_SAFE_RELEASE(buffer);
        return ret;
    }

    bool UpdateVertexBuffer(Effekseer::Backend::VertexBufferRef& buffer, int32_t size, int32_t offset, const void* data) override
    {
        if (buffer == nullptr)
            return false;
        buffer->UpdateData(data, size, offset);
        return true;
    }

    bool UpdateIndexBuffer(Effekseer::Backend::IndexBufferRef& buffer, int32_t size, int32_t offset, const void* data) override
    {
        if (buffer == nullptr)
            return false;
        buffer->UpdateData(data, size, offset);
        return true;
    }

    Effekseer::Backend::VertexLayoutRef CreateVertexLayout(const Effekseer::Backend::VertexLayoutElement*, int32_t) override
    {
        return Effekseer::MakeRefPtr<AxVertexLayout>();
    }

    Effekseer::Backend::UniformBufferRef CreateUniformBuffer(int32_t size, const void* initialData) override
    {
        return Effekseer::MakeRefPtr<AxUniformBuffer>(size, initialData);
    }

    bool UpdateUniformBuffer(Effekseer::Backend::UniformBufferRef& buffer, int32_t size, int32_t offset, const void* data) override
    {
        auto ub = buffer.DownCast<AxUniformBuffer>();
        if (ub == nullptr || data == nullptr || offset < 0 || size < 0)
            return false;
        if (static_cast<size_t>(offset + size) > ub->data.size())
            ub->data.resize(offset + size);
        memcpy(ub->data.data() + offset, data, size);
        return true;
    }

    Effekseer::Backend::PipelineStateRef CreatePipelineState(const Effekseer::Backend::PipelineStateParameter& param) override
    {
        auto state = Effekseer::MakeRefPtr<AxPipelineState>();
        state->param = param;
        return state;
    }

    Effekseer::Backend::TextureRef CreateTexture(const Effekseer::Backend::TextureParameter& param,
                                                 const Effekseer::CustomVector<uint8_t>& initialData) override
    {
        ax::rhi::TextureDesc desc;
        desc.width = static_cast<uint16_t>(param.Size[0]);
        desc.height = static_cast<uint16_t>(param.Size[1]);
        desc.pixelFormat = ax::rhi::PixelFormat::RGBA8;
        auto texture = ax::rhi::GraphicsCore::currentDriver()->createTexture(desc);
        if (texture && !initialData.empty())
            texture->updateData(initialData.data(), desc.width, desc.height, 0, 0);
        auto ret = Effekseer::MakeRefPtr<AxTexture>(texture);
        AX_SAFE_RELEASE(texture);
        return ret;
    }

    Effekseer::Backend::ShaderRef CreateShaderFromBinary(const void*, int32_t, const void*, int32_t) override
    {
        return Effekseer::MakeRefPtr<AxShader>();
    }

    void Draw(const Effekseer::Backend::DrawParameter&) override {}
    std::string GetDeviceName() const override { return "Axmol RHI"; }
};

class DistortingCallbackAxmol : public EffekseerRenderer::DistortingCallback
{
public:
    DistortingCallbackAxmol() = default;
    ~DistortingCallbackAxmol() override { AX_SAFE_RELEASE(_backgroundTexture); }

    bool OnDistorting(EffekseerRenderer::Renderer* renderer) override
    {
        auto director    = ax::Director::getInstance();
        auto axRenderer  = director->getRenderer();
        if (!axRenderer)
            return false;

        auto rt = axRenderer->getRenderTarget();
        if (!rt)
            return false;

        if (rt->isDefaultRenderTarget())
            axRenderer->setFrameBufferOnly(false);

        // Lazily create the background texture matching current viewport and attachment format.
        ensureTexture(director->getRenderView(), rt);

        if (!_backgroundTexture || !_backgroundTexture->getRHITexture())
            return false;

        auto rhiTexture = _backgroundTexture->getRHITexture();
        auto context = axRenderer->getContext();
        if (context->copyTexture(rt, rhiTexture))
        {
            _efkTexture = Effekseer::MakeRefPtr<AxTexture>(rhiTexture);
            renderer->SetBackground(_efkTexture);
            return true;
        }

        return false;
    }

private:
    void ensureTexture(ax::RenderViewCore* renderView, ax::rhi::RenderTarget* rt)
    {
        auto renderSize = renderView->getRenderSize();
        auto w          = static_cast<int>(renderSize.x);
        auto h          = static_cast<int>(renderSize.y);
        if (w <= 0 || h <= 0)
            return;

        auto pixelFormat = rt ? rt->getColorAttachmentPixelFormat(0) : ax::rhi::PixelFormat::NONE;
        if (pixelFormat == ax::rhi::PixelFormat::NONE || pixelFormat == ax::rhi::PixelFormat::D24S8)
            return;

        if (_backgroundTexture && _backgroundTexture->getWidth() == w && _backgroundTexture->getHeight() == h &&
            _backgroundFormat == pixelFormat)
            return;

        AX_SAFE_RELEASE(_backgroundTexture);
        _backgroundFormat = ax::rhi::PixelFormat::NONE;

        ax::rhi::TextureDesc desc;
        desc.width = w;
        desc.height = h;
        desc.pixelFormat = pixelFormat;
        desc.textureUsage = ax::rhi::TextureUsage::READ;

        auto texture = new ax::Texture2D();
        if (texture->initWithSpec(desc, ax::Texture2D::DEFAULT_SLICE_DATA))
        {
            ax::rhi::SamplerDesc samplerDesc;
            samplerDesc.sAddressMode = ax::rhi::SamplerAddressMode::CLAMP;
            samplerDesc.tAddressMode = ax::rhi::SamplerAddressMode::CLAMP;
            texture->setTexParameters(samplerDesc);
            _backgroundTexture = texture;
            _backgroundFormat = pixelFormat;
            _efkTexture = nullptr;
        }
        else
        {
            AX_SAFE_RELEASE(texture);
        }
    }

    ax::Texture2D* _backgroundTexture = nullptr;
    ax::rhi::PixelFormat _backgroundFormat = ax::rhi::PixelFormat::NONE;
    Effekseer::Backend::TextureRef _efkTexture;
};

} // namespace

EffekseerRenderer::DistortingCallback* createDistortingCallback()
{
    return new DistortingCallbackAxmol();
}

class Shader : public EffekseerRenderer::ShaderBase
{
public:
    Shader(const std::string& vsName, const std::string& psName, bool isDistortion = false)
    {
        _isDistortion = isDistortion;
        auto program = ax::ProgramManager::getInstance()->loadProgram(vsName, psName);
        _programState = new ax::rhi::ProgramState(program);
        _program = program;

        // VS uniforms
        _cameraProjLocation = _programState->getUniformLocation("mCameraProj");
        _uvInversedLocation = _programState->getUniformLocation("mUVInversed");

        // PS uniforms — common (shared between distortion and non-distortion)
        _flipbookParamLocation = _programState->getUniformLocation("fFlipbookParameter");
        _uvDistortionParamLocation = _programState->getUniformLocation("fUVDistortionParameter");
        _blendTextureParamLocation = _programState->getUniformLocation("fBlendTextureParameter");
        _softParticleParamLocation = _programState->getUniformLocation("softParticleParam");
        _reconstructionParam1Location = _programState->getUniformLocation("reconstructionParam1");
        _reconstructionParam2Location = _programState->getUniformLocation("reconstructionParam2");
        _uvInversedBackLocation = _programState->getUniformLocation("mUVInversedBack");
        _samplerModesLocation = _programState->getUniformLocation("fSamplerModes");

        if (isDistortion)
        {
            _distortionIntensityLocation = _programState->getUniformLocation("g_scale");
        }
        else
        {
            _lightDirectionLocation = _programState->getUniformLocation("fLightDirection");
            _lightColorLocation = _programState->getUniformLocation("fLightColor");
            _lightAmbientColorLocation = _programState->getUniformLocation("fLightAmbient");
            _cameraFrontDirectionLocation = _programState->getUniformLocation("fCameraFrontDirection");
            _falloffParamLocation = _programState->getUniformLocation("fFalloffParameter");
            _beginColorLocation = _programState->getUniformLocation("fFalloffBeginColor");
            _endColorLocation = _programState->getUniformLocation("fFalloffEndColor");
            _emmisiveParamLocation = _programState->getUniformLocation("fEmissiveScaling");
            _edgeColorLocation = _programState->getUniformLocation("fEdgeColor");
            _edgeParameterLocation = _programState->getUniformLocation("fEdgeParameter");
            _miscFlagsLocation = _programState->getUniformLocation("miscFlags");
        }

        _textureLocations[0] = _programState->getUniformLocation("u_tex0");
        _textureLocations[1] = _programState->getUniformLocation("u_tex1");
        _textureLocations[2] = _programState->getUniformLocation("u_tex2");
        _textureLocations[3] = _programState->getUniformLocation("u_tex3");
        _textureLocations[4] = _programState->getUniformLocation("u_tex4");
        _textureLocations[5] = _programState->getUniformLocation("u_tex5");
        _textureLocations[6] = _programState->getUniformLocation("u_tex6");
        _textureLocations[7] = _programState->getUniformLocation("u_tex7");
    }

    ~Shader() override
    {
        for (auto& kv : _vertexLayouts)
            AX_SAFE_RELEASE(kv.second);
        AX_SAFE_RELEASE(_programState);
    }

    void SetVertexConstantBufferSize(int32_t size) override { _vs.resize(size); }
    void SetPixelConstantBufferSize(int32_t size) override { _ps.resize(size); }
    void* GetVertexConstantBuffer() override { return _vs.data(); }
    void* GetPixelConstantBuffer() override { return _ps.data(); }

    void SetConstantBuffer() override
    {
        if (_vs.size() >= sizeof(EffekseerRenderer::StandardRendererVertexBuffer))
        {
            auto vcb = reinterpret_cast<const EffekseerRenderer::StandardRendererVertexBuffer*>(_vs.data());
            ax::Mat4 m;
            memcpy(m.m, vcb->constantVSBuffer[1].Values, sizeof(m.m));
            _programState->setUniform(_cameraProjLocation, m.m, sizeof(m.m));
            _programState->setUniform(_uvInversedLocation, vcb->uvInversed, sizeof(vcb->uvInversed));
        }

        if (_isDistortion)
        {
            if (_ps.size() >= sizeof(EffekseerRenderer::PixelConstantBufferDistortion))
            {
                auto* pcb = reinterpret_cast<const EffekseerRenderer::PixelConstantBufferDistortion*>(_ps.data());
                _programState->setUniform(_distortionIntensityLocation, pcb->DistortionIntencity, sizeof(pcb->DistortionIntencity));
                _programState->setUniform(_uvInversedBackLocation, pcb->UVInversedBack, sizeof(pcb->UVInversedBack));
                _programState->setUniform(_flipbookParamLocation, pcb->FlipbookParam.Buffer, sizeof(pcb->FlipbookParam.Buffer));
                _programState->setUniform(_uvDistortionParamLocation, pcb->UVDistortionParam.Buffer, sizeof(pcb->UVDistortionParam.Buffer));
                _programState->setUniform(_blendTextureParamLocation, pcb->BlendTextureParam.Buffer, sizeof(pcb->BlendTextureParam.Buffer));
                _programState->setUniform(_softParticleParamLocation, pcb->SoftParticleParam.softParticleParams.data(), sizeof(float) * 4);
                _programState->setUniform(_reconstructionParam1Location, pcb->SoftParticleParam.reconstructionParam1.data(), sizeof(float) * 4);
                _programState->setUniform(_reconstructionParam2Location, pcb->SoftParticleParam.reconstructionParam2.data(), sizeof(float) * 4);
            }
        }
        else
        {
            if (_ps.size() >= sizeof(EffekseerRenderer::PixelConstantBuffer))
            {
                auto* pcb = reinterpret_cast<const EffekseerRenderer::PixelConstantBuffer*>(_ps.data());
                _programState->setUniform(_lightDirectionLocation, pcb->LightDirection.data(), sizeof(float) * 4);
                _programState->setUniform(_lightColorLocation, pcb->LightColor.data(), sizeof(float) * 4);
                _programState->setUniform(_lightAmbientColorLocation, pcb->LightAmbientColor.data(), sizeof(float) * 4);
                _programState->setUniform(_flipbookParamLocation, pcb->FlipbookParam.Buffer, sizeof(pcb->FlipbookParam.Buffer));
                _programState->setUniform(_uvDistortionParamLocation, pcb->UVDistortionParam.Buffer, sizeof(pcb->UVDistortionParam.Buffer));
                _programState->setUniform(_blendTextureParamLocation, pcb->BlendTextureParam.Buffer, sizeof(pcb->BlendTextureParam.Buffer));
                _programState->setUniform(_cameraFrontDirectionLocation, pcb->CameraFrontDirection, sizeof(float) * 4);
                _programState->setUniform(_falloffParamLocation, pcb->FalloffParam.Buffer, sizeof(pcb->FalloffParam.Buffer));
                _programState->setUniform(_beginColorLocation, pcb->FalloffParam.BeginColor.data(), sizeof(float) * 4);
                _programState->setUniform(_endColorLocation, pcb->FalloffParam.EndColor.data(), sizeof(float) * 4);
                _programState->setUniform(_emmisiveParamLocation, &pcb->EmmisiveParam.EmissiveScaling, sizeof(float));
                _programState->setUniform(_edgeColorLocation, pcb->EdgeParam.EdgeColor.data(), sizeof(float) * 4);
                float edgeParam[4] = { pcb->EdgeParam.Threshold, pcb->EdgeParam.ColorScaling, 0.0f, 0.0f };
                _programState->setUniform(_edgeParameterLocation, edgeParam, sizeof(float) * 4);
                _programState->setUniform(_softParticleParamLocation, pcb->SoftParticleParam.softParticleParams.data(), sizeof(float) * 4);
                _programState->setUniform(_reconstructionParam1Location, pcb->SoftParticleParam.reconstructionParam1.data(), sizeof(float) * 4);
                _programState->setUniform(_reconstructionParam2Location, pcb->SoftParticleParam.reconstructionParam2.data(), sizeof(float) * 4);
                _programState->setUniform(_uvInversedBackLocation, pcb->UVInversedBack, sizeof(float) * 4);
                _programState->setUniform(_miscFlagsLocation, pcb->MiscFlags.data(), sizeof(float) * 4);
            }
        }
    }

    ax::rhi::ProgramState* programState() const { return _programState; }

    void SetSamplerModes(const float* modes)
    {
        _programState->setUniform(_samplerModesLocation, modes, sizeof(float) * 4);
    }

    ax::rhi::VertexLayout* vertexLayout(int32_t stride)
    {
        auto it = _vertexLayouts.find(stride);
        if (it != _vertexLayouts.end())
            return it->second;

        const auto uvOffset = getUVOffset(stride);
        const bool isLighting = (stride == sizeof(EffekseerRenderer::LightingVertex) ||
                                 stride == sizeof(EffekseerRenderer::AdvancedLightingVertex));
        ax::rhi::VertexLayoutDesc desc;
        if (isLighting)
        {
            // LightingVertex: Pos(12) + Col(4) + Normal(4) + Tangent(4) + UV(8) + UV2(8) = 40
            desc.startLayout(6);
            desc.addAttrib(_program->getVertexInputDesc(ax::rhi::VertexSemantic::POSITION), ax::rhi::VertexElementType::FLOAT3, 0, false);
            desc.addAttrib(_program->getVertexInputDesc(ax::rhi::VertexSemantic::NORMAL), ax::rhi::VertexElementType::UBYTE4, 12, true);
            desc.addAttrib(_program->getVertexInputDesc(ax::rhi::VertexSemantic{"NORMAL", 1}), ax::rhi::VertexElementType::UBYTE4, 16, true);
            desc.addAttrib(_program->getVertexInputDesc(ax::rhi::VertexSemantic{"NORMAL", 2}), ax::rhi::VertexElementType::UBYTE4, 20, true);
            desc.addAttrib(_program->getVertexInputDesc(ax::rhi::VertexSemantic::TEXCOORD0), ax::rhi::VertexElementType::FLOAT2, uvOffset, false);
            desc.addAttrib(_program->getVertexInputDesc(ax::rhi::VertexSemantic::TEXCOORD1), ax::rhi::VertexElementType::FLOAT2, uvOffset + 8, false);
        }
        else
        {
            // SimpleVertex: Pos(12) + Col(4) + UV(8) = 24
            desc.startLayout(3);
            desc.addAttrib(_program->getVertexInputDesc(ax::rhi::VertexSemantic::POSITION), ax::rhi::VertexElementType::FLOAT3, 0, false);
            desc.addAttrib(_program->getVertexInputDesc(ax::rhi::VertexSemantic::NORMAL), ax::rhi::VertexElementType::UBYTE4, 12, true);
            desc.addAttrib(_program->getVertexInputDesc(ax::rhi::VertexSemantic::TEXCOORD0), ax::rhi::VertexElementType::FLOAT2, uvOffset, false);
        }
        desc.endLayout(stride);
        auto layout = ax::rhi::GraphicsCore::currentDriver()->createVertexLayout(std::move(desc));
        _vertexLayouts.emplace(stride, layout);
        return layout;
    }

    void setTexture(int slot, ax::rhi::Texture* texture)
    {
        if (texture && slot >= 0 && slot < 8)
            _programState->setTexture(_textureLocations[slot], slot, texture);
    }

    void setDefaultTextures(ax::rhi::Texture* texture)
    {
        if (!texture)
            return;

        for (int slot = 0; slot < 8; slot++)
        {
            if (_textureLocations[slot])
                setTexture(slot, texture);
        }
    }

private:
    static size_t getUVOffset(int32_t stride)
    {
        if (stride == sizeof(EffekseerRenderer::SimpleVertex) || stride == sizeof(EffekseerRenderer::AdvancedSimpleVertex))
            return offsetof(EffekseerRenderer::SimpleVertex, UV);
        return offsetof(EffekseerRenderer::LightingVertex, UV);
    }

    ax::rhi::Program* _program = nullptr;
    std::vector<uint8_t> _vs;
    std::vector<uint8_t> _ps;
    ax::rhi::ProgramState* _programState = nullptr;
    std::unordered_map<int32_t, ax::rhi::VertexLayout*> _vertexLayouts;
    bool _isDistortion = false;

    ax::rhi::UniformLocation _cameraProjLocation;
    ax::rhi::UniformLocation _uvInversedLocation;

    ax::rhi::UniformLocation _flipbookParamLocation;
    ax::rhi::UniformLocation _uvDistortionParamLocation;
    ax::rhi::UniformLocation _blendTextureParamLocation;
    ax::rhi::UniformLocation _softParticleParamLocation;
    ax::rhi::UniformLocation _reconstructionParam1Location;
    ax::rhi::UniformLocation _reconstructionParam2Location;
    ax::rhi::UniformLocation _uvInversedBackLocation;
    ax::rhi::UniformLocation _samplerModesLocation;

    ax::rhi::UniformLocation _distortionIntensityLocation;

    ax::rhi::UniformLocation _lightDirectionLocation;
    ax::rhi::UniformLocation _lightColorLocation;
    ax::rhi::UniformLocation _lightAmbientColorLocation;
    ax::rhi::UniformLocation _cameraFrontDirectionLocation;
    ax::rhi::UniformLocation _falloffParamLocation;
    ax::rhi::UniformLocation _beginColorLocation;
    ax::rhi::UniformLocation _endColorLocation;
    ax::rhi::UniformLocation _emmisiveParamLocation;
    ax::rhi::UniformLocation _edgeColorLocation;
    ax::rhi::UniformLocation _edgeParameterLocation;
    ax::rhi::UniformLocation _miscFlagsLocation;

    ax::rhi::UniformLocation _textureLocations[8];
};

namespace
{
class RenderState : public EffekseerRenderer::RenderStateBase
{
public:
    void Update(bool forced) override { AX_UNUSED_PARAM(forced); }
};

class TextureLoader : public Effekseer::TextureLoader
{
public:
    explicit TextureLoader(Effekseer::FileInterfaceRef fileInterface) : _fileInterface(fileInterface) {}

    Effekseer::TextureRef Load(const char16_t* path, Effekseer::TextureType textureType) override
    {
        AX_UNUSED_PARAM(textureType);
        char utf8[512];
        Effekseer::ConvertUtf16ToUtf8(utf8, sizeof(utf8), path);

        ax::Data data;
        if (_fileInterface)
        {
            auto reader = _fileInterface->OpenRead(path);
            if (reader)
            {
                data.resize(reader->GetLength());
                reader->Read(data.getBytes(), data.getSize());
            }
        }
        else
        {
            data = ax::FileUtils::getInstance()->getDataFromFile(utf8);
        }

        if (data.isNull())
            return nullptr;

        ax::Image image;
        {
            ScopedPNGStraightAlpha straightAlpha;
            if (!image.initWithImageData(data.getBytes(), data.getSize()))
                return nullptr;
        }

        auto texture = new ax::Texture2D();
        if (!texture->initWithImage(&image))
        {
            texture->release();
            return nullptr;
        }

        auto backendTexture = Effekseer::MakeRefPtr<AxTexture>(texture->getRHITexture());
        texture->release();
        auto efkTexture = Effekseer::MakeRefPtr<Effekseer::Texture>();
        efkTexture->SetBackend(backendTexture);
        return efkTexture;
    }

    Effekseer::TextureRef Load(const void* data, int32_t size, Effekseer::TextureType textureType, bool isMipMapEnabled) override
    {
        AX_UNUSED_PARAM(textureType);
        AX_UNUSED_PARAM(isMipMapEnabled);
        ax::Image image;
        {
            ScopedPNGStraightAlpha straightAlpha;
            if (!image.initWithImageData(static_cast<const uint8_t*>(data), size))
                return nullptr;
        }
        auto texture = new ax::Texture2D();
        if (!texture->initWithImage(&image))
        {
            texture->release();
            return nullptr;
        }
        auto backendTexture = Effekseer::MakeRefPtr<AxTexture>(texture->getRHITexture());
        texture->release();
        auto efkTexture = Effekseer::MakeRefPtr<Effekseer::Texture>();
        efkTexture->SetBackend(backendTexture);
        return efkTexture;
    }

    void Unload(Effekseer::TextureRef data) override { AX_UNUSED_PARAM(data); }

private:
    Effekseer::FileInterfaceRef _fileInterface;
};

class MaterialLoader : public Effekseer::MaterialLoader
{
public:
    Effekseer::MaterialRef Load(const char16_t* path) override
    {
        char utf8[512];
        Effekseer::ConvertUtf16ToUtf8(utf8, sizeof(utf8), path);
        AXLOGW("Effekseer custom material is not supported yet: {}", utf8);
        return nullptr;
    }
};

template <typename Base>
class RendererAdapter : public Base
{
public:
    explicit RendererAdapter(Renderer* renderer) : Base(renderer) {}
};

} // namespace

RendererRef Renderer::Create(int32_t squareMaxCount)
{
    auto renderer = Effekseer::MakeRefPtr<Renderer>(squareMaxCount);
    if (!renderer->initialize())
        return nullptr;
    return renderer;
}

Renderer::Renderer(int32_t squareMaxCount) : _squareMaxCount(squareMaxCount) {}

Renderer::~Renderer()
{
    ReleaseCachedCommands();
    _currentVertexBuffer = nullptr;
    _currentIndexBuffer = nullptr;
    GetImpl()->DeleteProxyTextures(this);
    _distortingCallback = nullptr; // weak ref to manager
}

bool Renderer::initialize()
{
    _graphicsDevice = Effekseer::MakeRefPtr<GraphicsDevice>();
    _renderState = std::make_unique<RenderState>();
    _standardRenderer = std::make_unique<EffekseerRenderer::StandardRenderer<Renderer, Shader>>(this);

    GetImpl()->InternalVertexBuffer =
        std::make_shared<EffekseerRenderer::VertexBufferRing>(_graphicsDevice, EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * _squareMaxCount * 4, 1);

    std::vector<uint16_t> indices(_squareMaxCount * 6);
    for (int32_t i = 0; i < _squareMaxCount; i++)
    {
        const uint16_t v = static_cast<uint16_t>(i * 4);
        indices[i * 6 + 0] = v + 3;
        indices[i * 6 + 1] = v + 1;
        indices[i * 6 + 2] = v + 0;
        indices[i * 6 + 3] = v + 3;
        indices[i * 6 + 4] = v + 0;
        indices[i * 6 + 5] = v + 2;
    }
    _indexBuffer = _graphicsDevice->CreateIndexBuffer(static_cast<int32_t>(indices.size()), indices.data(), Effekseer::Backend::IndexBufferStrideType::Stride2);

    GetImpl()->ShaderUnlit = std::make_unique<Shader>("custom/sprite_unlit_vs", "custom/model_ps");
    GetImpl()->ShaderLit = std::make_unique<Shader>("custom/sprite_unlit_vs_1", "custom/model_ps_1");
    GetImpl()->ShaderDistortion = std::make_unique<Shader>("custom/sprite_unlit_vs_2", "custom/model_distortion_ps_1", true);
    GetImpl()->ShaderAdUnlit = std::make_unique<Shader>("custom/sprite_unlit_vs", "custom/model_ps");
    GetImpl()->ShaderAdLit = std::make_unique<Shader>("custom/sprite_unlit_vs_1", "custom/model_ps_1");
    GetImpl()->ShaderAdDistortion = std::make_unique<Shader>("custom/sprite_unlit_vs_2", "custom/model_distortion_ps_1", true);
    for (auto* shader : {GetImpl()->ShaderUnlit.get(), GetImpl()->ShaderLit.get(), GetImpl()->ShaderDistortion.get(),
                         GetImpl()->ShaderAdUnlit.get(), GetImpl()->ShaderAdLit.get(), GetImpl()->ShaderAdDistortion.get()})
    {
        shader->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
        shader->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));
    }

    GetImpl()->CreateProxyTextures(this);
    // process at shader
    // GetImpl()->SetBackgroundTextureUVStyle(EffekseerRenderer::UVStyle::VerticalFlipped);
    return true;
}

bool Renderer::BeginRendering()
{
    GetImpl()->CalculateCameraProjectionMatrix();
    _renderState->GetActiveState().Reset();
    _renderState->Update(true);
    _standardRenderer->ResetAndRenderingIfRequired();
    return true;
}

bool Renderer::EndRendering()
{
    _standardRenderer->ResetAndRenderingIfRequired();
    return true;
}

void Renderer::BeginFrame(ax::Renderer* renderer)
{
    _axRenderer = renderer;
    _commandIndex = 0;
}

void Renderer::ReleaseCachedCommands()
{
    for (auto& command : _commands)
    {
        if (command)
            command->releasePSVL();
    }
    _commands.clear();
    _commandStrides.clear();
    _commandIndex = 0;
}

Effekseer::SpriteRendererRef Renderer::CreateSpriteRenderer()
{
    return Effekseer::MakeRefPtr<RendererAdapter<EffekseerRenderer::SpriteRendererBase<Renderer, false>>>(this);
}

Effekseer::RibbonRendererRef Renderer::CreateRibbonRenderer()
{
    return Effekseer::MakeRefPtr<RendererAdapter<EffekseerRenderer::RibbonRendererBase<Renderer, false>>>(this);
}

Effekseer::RingRendererRef Renderer::CreateRingRenderer()
{
    return Effekseer::MakeRefPtr<RendererAdapter<EffekseerRenderer::RingRendererBase<Renderer, false>>>(this);
}

Effekseer::ModelRendererRef Renderer::CreateModelRenderer()
{
    return nullptr;
}

Effekseer::TrackRendererRef Renderer::CreateTrackRenderer()
{
    return Effekseer::MakeRefPtr<RendererAdapter<EffekseerRenderer::TrackRendererBase<Renderer, false>>>(this);
}

Effekseer::GpuParticleSystemRef Renderer::CreateGpuParticleSystem(const Effekseer::GpuParticleSystem::Settings& settings)
{
    AX_UNUSED_PARAM(settings);
    return nullptr;
}

Effekseer::TextureLoaderRef Renderer::CreateTextureLoader(Effekseer::FileInterfaceRef fileInterface)
{
    return Effekseer::MakeRefPtr<TextureLoader>(fileInterface);
}

Effekseer::ModelLoaderRef Renderer::CreateModelLoader(Effekseer::FileInterfaceRef fileInterface)
{
    return EffekseerRenderer::CreateModelLoader(_graphicsDevice, fileInterface);
}

Effekseer::MaterialLoaderRef Renderer::CreateMaterialLoader(Effekseer::FileInterfaceRef fileInterface)
{
    AX_UNUSED_PARAM(fileInterface);
    return Effekseer::MakeRefPtr<MaterialLoader>();
}

void Renderer::ResetRenderState()
{
    if (_renderState)
        _renderState->GetActiveState().Reset();
}

void Renderer::SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback)
{
    _distortingCallback = callback;
}

void Renderer::BeginShader(EffekseerRenderer::ShaderBase* shader) { _currentShader = shader; }
void Renderer::EndShader(EffekseerRenderer::ShaderBase* shader)
{
    AX_UNUSED_PARAM(shader);
    _currentShader = nullptr;
}

void Renderer::SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t stride)
{
    _currentVertexBuffer = vertexBuffer;
    _currentVertexStride = stride;
}

void Renderer::SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer) { _currentIndexBuffer = indexBuffer; }
void Renderer::SetLayout(EffekseerRenderer::ShaderBase* shader) { AX_UNUSED_PARAM(shader); }

void Renderer::SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
    if (!_currentShader)
        return;
    auto dst = static_cast<uint8_t*>(_currentShader->GetVertexConstantBuffer()) + dstOffset;
    memcpy(dst, data, size);
}

void Renderer::SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
    if (!_currentShader)
        return;
    auto dst = static_cast<uint8_t*>(_currentShader->GetPixelConstantBuffer()) + dstOffset;
    memcpy(dst, data, size);
}

void Renderer::SetTextures(EffekseerRenderer::ShaderBase* shaderBase, Effekseer::Backend::TextureRef* textures, int32_t count)
{
    auto shader = static_cast<Shader*>(shaderBase);
    if (!shader)
        return;

    auto proxyTexture = GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White).DownCast<AxTexture>();
    if (proxyTexture && proxyTexture->get())
        shader->setDefaultTextures(proxyTexture->get());

    float samplerModes[4] = {};
    const auto& state = _renderState->GetActiveState();
    for (int32_t i = 0; i < count; i++)
    {
        if (i < 4)
            samplerModes[i] = ToSamplerMode(state.TextureFilterTypes[i], state.TextureWrapTypes[i]);

        auto texture = textures[i].DownCast<AxTexture>();
        if (texture && texture->get())
        {
            ax::rhi::SamplerDesc samplerDesc;
            if (state.TextureFilterTypes[i] == Effekseer::TextureFilterType::Nearest)
            {
                samplerDesc.minFilter = ax::rhi::SamplerFilter::MIN_NEAREST;
                samplerDesc.magFilter = ax::rhi::SamplerFilter::MAG_NEAREST;
            }

            switch (state.TextureWrapTypes[i])
            {
            case Effekseer::TextureWrapType::Repeat:
                samplerDesc.sAddressMode = ax::rhi::SamplerAddressMode::REPEAT;
                samplerDesc.tAddressMode = ax::rhi::SamplerAddressMode::REPEAT;
                break;
            case Effekseer::TextureWrapType::Mirror:
                samplerDesc.sAddressMode = ax::rhi::SamplerAddressMode::MIRROR;
                samplerDesc.tAddressMode = ax::rhi::SamplerAddressMode::MIRROR;
                break;
            case Effekseer::TextureWrapType::Clamp:
            default:
                samplerDesc.sAddressMode = ax::rhi::SamplerAddressMode::CLAMP;
                samplerDesc.tAddressMode = ax::rhi::SamplerAddressMode::CLAMP;
                break;
            }

            texture->get()->updateSamplerDesc(samplerDesc);
            shader->setTexture(i, texture->get());
        }
    }
    shader->SetSamplerModes(samplerModes);
}

void Renderer::DrawSprites(int32_t spriteCount, int32_t vertexOffset)
{
    if (!_axRenderer || !_currentShader || !_currentVertexBuffer || !_currentIndexBuffer || spriteCount <= 0)
        return;

    auto shader = static_cast<Shader*>(_currentShader);
    shader->SetConstantBuffer();

    auto vb = static_cast<AxVertexBuffer*>(_currentVertexBuffer.Get());
    auto ib = static_cast<AxIndexBuffer*>(_currentIndexBuffer.Get());
    const auto vertexCount = static_cast<size_t>(spriteCount * 4);
    const auto vertexBytes = vertexCount * static_cast<size_t>(_currentVertexStride);
    const auto vertexOffsetBytes = static_cast<size_t>(vertexOffset) * static_cast<size_t>(_currentVertexStride);
    if (vertexOffsetBytes + vertexBytes > vb->shadowSize())
        return;

    if (_commandIndex >= _commands.size())
    {
        _commands.emplace_back(std::make_unique<ax::CustomCommand>());
        _commandStrides.emplace_back(0);
    }

    auto* command = _commands[_commandIndex].get();
    auto strideChanged = _commandStrides[_commandIndex] != static_cast<size_t>(_currentVertexStride);
    _commandStrides[_commandIndex] = static_cast<size_t>(_currentVertexStride);
    _commandIndex++;
    command->init(_globalZOrder);
    command->setOwnPSVL(shader->programState()->clone(), shader->vertexLayout(_currentVertexStride), ax::RenderCommand::ADOPT_FLAG_PS);
    command->setDrawType(ax::CustomCommand::DrawType::ELEMENT);
    command->setPrimitiveType(ax::CustomCommand::PrimitiveType::TRIANGLE);
    command->setTransparent(true);

    const auto depthTest = _renderState->GetActiveState().DepthTest;
    const auto depthWrite = _renderState->GetActiveState().DepthWrite;
    struct DepthStateBackup
    {
        bool depthTest = false;
        bool depthWrite = false;
        ax::rhi::CompareFunc compareFunc = ax::rhi::CompareFunc::LESS;
    };
    auto depthStateBackup = std::make_shared<DepthStateBackup>();
    command->setBeforeCallback([renderer = _axRenderer, depthStateBackup, depthTest, depthWrite]() {
        if (!renderer)
            return;
        depthStateBackup->depthTest = renderer->getDepthTest();
        depthStateBackup->depthWrite = renderer->getDepthWrite();
        depthStateBackup->compareFunc = renderer->getDepthCompareFunc();
        renderer->setDepthTest(depthTest);
        renderer->setDepthWrite(depthWrite);
        renderer->setDepthCompareFunc(ax::rhi::CompareFunc::LESS_EQUAL);
    });
    command->setAfterCallback([renderer = _axRenderer, depthStateBackup]() {
        if (!renderer)
            return;
        renderer->setDepthTest(depthStateBackup->depthTest);
        renderer->setDepthWrite(depthStateBackup->depthWrite);
        renderer->setDepthCompareFunc(depthStateBackup->compareFunc);
    });

    if (command->getVertexCapacity() < vertexCount || strideChanged)
        command->createVertexBuffer(static_cast<size_t>(_currentVertexStride), vertexCount, ax::CustomCommand::BufferUsage::DYNAMIC);
    command->updateVertexBuffer(vb->shadowData() + vertexOffsetBytes, vertexBytes);

    command->setIndexBuffer(ib->get(), ax::CustomCommand::IndexFormat::U_SHORT);
    command->setIndexDrawInfo(0, static_cast<size_t>(spriteCount * 6));

    auto& blendDesc = command->blendDesc();
    blendDesc.blendEnabled = true;
    blendDesc.rgbBlendOp = ax::rhi::BlendOp::ADD;
    blendDesc.alphaBlendOp = ax::rhi::BlendOp::ADD;

    switch (_renderState->GetActiveState().AlphaBlend)
    {
    case Effekseer::AlphaBlendType::Opacity:
        blendDesc.blendEnabled = false;
        blendDesc.sourceRGBBlendFactor = ax::rhi::BlendFactor::ONE;
        blendDesc.destinationRGBBlendFactor = ax::rhi::BlendFactor::ZERO;
        blendDesc.sourceAlphaBlendFactor = ax::rhi::BlendFactor::ONE;
        blendDesc.destinationAlphaBlendFactor = ax::rhi::BlendFactor::ZERO;
        break;
    case Effekseer::AlphaBlendType::Add:
        blendDesc.sourceRGBBlendFactor = ax::rhi::BlendFactor::SRC_ALPHA;
        blendDesc.destinationRGBBlendFactor = ax::rhi::BlendFactor::ONE;
        blendDesc.sourceAlphaBlendFactor = ax::rhi::BlendFactor::ZERO;
        blendDesc.destinationAlphaBlendFactor = ax::rhi::BlendFactor::ONE;
        break;
    case Effekseer::AlphaBlendType::Sub:
        blendDesc.rgbBlendOp = ax::rhi::BlendOp::REVERSE_SUBTRACT;
        blendDesc.alphaBlendOp = ax::rhi::BlendOp::REVERSE_SUBTRACT;
        blendDesc.sourceRGBBlendFactor = ax::rhi::BlendFactor::SRC_ALPHA;
        blendDesc.destinationRGBBlendFactor = ax::rhi::BlendFactor::ONE;
        blendDesc.sourceAlphaBlendFactor = ax::rhi::BlendFactor::ZERO;
        blendDesc.destinationAlphaBlendFactor = ax::rhi::BlendFactor::ONE;
        break;
    case Effekseer::AlphaBlendType::Mul:
        blendDesc.sourceRGBBlendFactor = ax::rhi::BlendFactor::ZERO;
        blendDesc.destinationRGBBlendFactor = ax::rhi::BlendFactor::SRC_COLOR;
        blendDesc.sourceAlphaBlendFactor = ax::rhi::BlendFactor::ZERO;
        blendDesc.destinationAlphaBlendFactor = ax::rhi::BlendFactor::ONE;
        break;
    case Effekseer::AlphaBlendType::Blend:
    default:
        blendDesc.sourceRGBBlendFactor = ax::rhi::BlendFactor::SRC_ALPHA;
        blendDesc.destinationRGBBlendFactor = ax::rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
        blendDesc.sourceAlphaBlendFactor = ax::rhi::BlendFactor::ONE;
        blendDesc.destinationAlphaBlendFactor = ax::rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
        break;
    }

    _axRenderer->addCommand(command);

    GetImpl()->drawcallCount++;
    GetImpl()->drawvertexCount += spriteCount * 4;
}

void Renderer::DrawPolygon(int32_t vertexCount, int32_t indexCount)
{
    AX_UNUSED_PARAM(vertexCount);
    AX_UNUSED_PARAM(indexCount);
}

void Renderer::DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount)
{
    AX_UNUSED_PARAM(vertexCount);
    AX_UNUSED_PARAM(indexCount);
    AX_UNUSED_PARAM(instanceCount);
}
} // namespace EffekseerRendererAxmol
