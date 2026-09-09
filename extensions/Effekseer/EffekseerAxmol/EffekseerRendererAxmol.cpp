#include "EffekseerRendererAxmol.h"

#include "axmol/rhi/GraphicsDevice.h"
#include "axmol/rhi/GraphicsCore.h"
#include "axmol/rhi/ComputePipeline.h"
#include "axmol/rhi/RenderTarget.h"
#include "axmol/rhi/SamplerRegistry.h"
#include "axmol/rhi/VertexLayout.h"
#include "axmol/rhi/ShaderCache.h"
#include "axmol/platform/Image.h"
#include "axmol/platform/RenderViewCore.h"
#include "axmol/renderer/ProgramManager.h"
#include "axmol/renderer/Texture2D.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/EventType.h"
#include <EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h>
#include <EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h>
#include <EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.GpuParticles.h>
#include <algorithm>
#include <cstddef>
#include <limits>
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

ax::rhi::SamplerDesc ToSamplerDesc(Effekseer::TextureFilterType filter, Effekseer::TextureWrapType wrap)
{
    ax::rhi::SamplerDesc samplerDesc;
    if (filter == Effekseer::TextureFilterType::Nearest)
    {
        samplerDesc.minFilter = ax::rhi::SamplerFilter::MIN_NEAREST;
        samplerDesc.magFilter = ax::rhi::SamplerFilter::MAG_NEAREST;
    }

    switch (wrap)
    {
    case Effekseer::TextureWrapType::Repeat:
        samplerDesc.sAddressMode = ax::rhi::SamplerAddressMode::REPEAT;
        samplerDesc.tAddressMode = ax::rhi::SamplerAddressMode::REPEAT;
        samplerDesc.wAddressMode = ax::rhi::SamplerAddressMode::REPEAT;
        break;
    case Effekseer::TextureWrapType::Mirror:
        samplerDesc.sAddressMode = ax::rhi::SamplerAddressMode::MIRROR;
        samplerDesc.tAddressMode = ax::rhi::SamplerAddressMode::MIRROR;
        samplerDesc.wAddressMode = ax::rhi::SamplerAddressMode::MIRROR;
        break;
    case Effekseer::TextureWrapType::Clamp:
    default:
        samplerDesc.sAddressMode = ax::rhi::SamplerAddressMode::CLAMP;
        samplerDesc.tAddressMode = ax::rhi::SamplerAddressMode::CLAMP;
        samplerDesc.wAddressMode = ax::rhi::SamplerAddressMode::CLAMP;
        break;
    }

    return samplerDesc;
}

bool RegisterGpuParticleSamplers()
{
    auto* registry = ax::rhi::SamplerRegistry::getInstance();
    const ax::rhi::SamplerDesc defaultSampler{};
    for (const auto* name : {"NoiseSamp", "FieldSamp", "GradientSamp", "ColorSamp", "NormalSamp"})
    {
        if (!registry->registerSampler(name, defaultSampler))
            return false;
    }
    return true;
}

bool SupportsGpuParticleFeatures(ax::rhi::GraphicsDevice* device)
{
    if (!device)
        return false;

    const auto& caps = device->getCaps();
    return device->checkForFeatureSupported(ax::rhi::FeatureType::COMPUTE_SHADER) &&
           device->checkForFeatureSupported(ax::rhi::FeatureType::STORAGE_BUFFER) &&
           device->checkForFeatureSupported(ax::rhi::FeatureType::TEXTURE_3D) &&
           caps.maxComputeWorkGroupSize[0] >= 256 && caps.maxComputeWorkGroupInvocations >= 256 &&
           caps.maxComputeWorkGroupCount[0] > 0 && caps.maxStorageBufferBindings >= 2 &&
           caps.maxTexture3DSize >= 8;
}

bool SupportsGpuParticleSettings(ax::rhi::GraphicsDevice* device,
                                 const Effekseer::GpuParticleSystem::Settings& settings)
{
    if (!SupportsGpuParticleFeatures(device))
        return false;

    const auto& caps = device->getCaps();
    const auto particleBytes =
        static_cast<size_t>(settings.ParticleMaxCount) * sizeof(EffekseerRenderer::GpuParticles::ParticleData);
    const auto trailBytes =
        static_cast<size_t>(settings.TrailMaxCount) * sizeof(EffekseerRenderer::GpuParticles::TrailData);
    const auto maxDispatchX = (static_cast<size_t>(settings.ParticleMaxCount) + 255u) / 256u;
    return caps.maxStorageBufferSize >= (std::max)(particleBytes, trailBytes) &&
           static_cast<size_t>(caps.maxComputeWorkGroupCount[0]) >= maxDispatchX;
}

// These structures are shared verbatim with the migrated HLSL. Fail at build
// time if an Effekseer update changes the C++ ABI without updating the shaders.
static_assert(sizeof(EffekseerRenderer::GpuParticles::ParameterData) == 368);
static_assert(sizeof(EffekseerRenderer::GpuParticles::EmitterData) == 112);
static_assert(sizeof(EffekseerRenderer::GpuParticles::ParticleData) == 80);
static_assert(sizeof(EffekseerRenderer::GpuParticles::TrailData) == 16);
static_assert(sizeof(EffekseerRenderer::GpuParticles::EmitPoint) == 32);
static_assert(sizeof(EffekseerRenderer::GpuParticles::ComputeConstants) == 16);
static_assert(sizeof(EffekseerRenderer::GpuParticles::RenderConstants) == 304);
static_assert(offsetof(EffekseerRenderer::GpuParticles::EmitterData, Transform) == 64);
static_assert(offsetof(EffekseerRenderer::GpuParticles::ParticleData, Transform) == 32);

ax::rhi::BlendOp ToBlendOp(Effekseer::Backend::BlendEquationType value)
{
    switch (value)
    {
    case Effekseer::Backend::BlendEquationType::Sub:
        return ax::rhi::BlendOp::SUBTRACT;
    case Effekseer::Backend::BlendEquationType::ReverseSub:
        return ax::rhi::BlendOp::REVERSE_SUBTRACT;
    case Effekseer::Backend::BlendEquationType::Add:
    case Effekseer::Backend::BlendEquationType::Min:
    case Effekseer::Backend::BlendEquationType::Max:
    default:
        return ax::rhi::BlendOp::ADD;
    }
}

ax::rhi::BlendFactor ToBlendFactor(Effekseer::Backend::BlendFuncType value)
{
    switch (value)
    {
    case Effekseer::Backend::BlendFuncType::Zero:
        return ax::rhi::BlendFactor::ZERO;
    case Effekseer::Backend::BlendFuncType::One:
        return ax::rhi::BlendFactor::ONE;
    case Effekseer::Backend::BlendFuncType::SrcColor:
        return ax::rhi::BlendFactor::SRC_COLOR;
    case Effekseer::Backend::BlendFuncType::OneMinusSrcColor:
        return ax::rhi::BlendFactor::ONE_MINUS_SRC_COLOR;
    case Effekseer::Backend::BlendFuncType::SrcAlpha:
        return ax::rhi::BlendFactor::SRC_ALPHA;
    case Effekseer::Backend::BlendFuncType::OneMinusSrcAlpha:
        return ax::rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
    case Effekseer::Backend::BlendFuncType::DstAlpha:
        return ax::rhi::BlendFactor::DST_ALPHA;
    case Effekseer::Backend::BlendFuncType::OneMinusDstAlpha:
        return ax::rhi::BlendFactor::ONE_MINUS_DST_ALPHA;
    case Effekseer::Backend::BlendFuncType::DstColor:
        return ax::rhi::BlendFactor::DST_COLOR;
    case Effekseer::Backend::BlendFuncType::OneMinusDstColor:
        return ax::rhi::BlendFactor::ONE_MINUS_DST_COLOR;
    default:
        return ax::rhi::BlendFactor::ONE;
    }
}

ax::rhi::CompareFunc ToCompareFunc(Effekseer::Backend::DepthFuncType value)
{
    return static_cast<ax::rhi::CompareFunc>(value);
}

ax::rhi::CullMode ToCullMode(Effekseer::Backend::CullingType value)
{
    switch (value)
    {
    case Effekseer::Backend::CullingType::Clockwise:
        return ax::rhi::CullMode::FRONT;
    case Effekseer::Backend::CullingType::CounterClockwise:
        return ax::rhi::CullMode::BACK;
    case Effekseer::Backend::CullingType::DoubleSide:
    default:
        return ax::rhi::CullMode::NONE;
    }
}

class VertexBufferAX : public Effekseer::Backend::VertexBuffer
{
public:
    VertexBufferAX(ax::rhi::Buffer* buffer, int32_t size, const void* initialData) : _buffer(buffer), _shadow(size)
    {
        if (_buffer)
            _buffer->retain();
        if (initialData && size > 0)
            memcpy(_shadow.data(), initialData, size);
        if (_buffer && size > 0)
            _buffer->updateData(_shadow.data(), _shadow.size());
    }

    ~VertexBufferAX() override { AX_SAFE_RELEASE(_buffer); }

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

class IndexBufferAX : public Effekseer::Backend::IndexBuffer
{
public:
    IndexBufferAX(ax::rhi::Buffer* buffer,
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

    ~IndexBufferAX() override { AX_SAFE_RELEASE(_buffer); }

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
    Effekseer::Backend::IndexBufferStrideType getStrideType() const { return strideType_; }
    int32_t getElementCount() const { return elementCount_; }

private:
    ax::rhi::Buffer* _buffer = nullptr;
    Effekseer::Backend::IndexBufferStrideType strideType_ = Effekseer::Backend::IndexBufferStrideType::Stride2;
    int32_t elementCount_ = 0;
    size_t _strideSize = 2;
    std::vector<uint8_t> _shadow;
};

class TextureAX : public Effekseer::Backend::Texture
{
public:
    explicit TextureAX(ax::rhi::Texture* texture) : _texture(texture)
    {
        if (_texture)
            _texture->retain();
        param_.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
        param_.Dimension = _texture && _texture->getTextureType() == ax::rhi::TextureType::TEXTURE_3D ? 3 : 2;
        param_.Size = {_texture ? _texture->getWidth() : 1, _texture ? _texture->getHeight() : 1,
                       _texture ? _texture->getDepth() : 1};
        param_.MipLevelCount = _texture ? _texture->getMipLevels() : 1;
    }

    ~TextureAX() override { AX_SAFE_RELEASE(_texture); }
    ax::rhi::Texture* get() const { return _texture; }

private:
    ax::rhi::Texture* _texture = nullptr;
};

class VertexLayoutAX : public Effekseer::Backend::VertexLayout
{
public:
    void setElements(const Effekseer::Backend::VertexLayoutElement* elements, int32_t count)
    {
        _elements.assign(elements, elements + count);
    }

    /**
     * Builds the backend vertex layout from the program's reflected vertex input
     * descriptions. The location/semantic index must come from the program
     * reflection (backends remap them differently), never from manual values.
     */
    void buildLayout(ax::rhi::Program* program)
    {
        AX_SAFE_RELEASE(_layout);
        if (!program || _elements.empty())
            return;

        ax::rhi::VertexLayoutDesc desc;
        desc.startLayout(_elements.size());
        size_t offset = 0;
        for (const auto& el : _elements)
        {
            ax::rhi::VertexElementType elementType = ax::rhi::VertexElementType::FLOAT3;
            bool needNormalize                      = false;
            uint32_t elementSize                    = 12;
            switch (el.Format)
            {
            case Effekseer::Backend::VertexLayoutFormat::R32_FLOAT:
                elementType = ax::rhi::VertexElementType::FLOAT;
                elementSize = 4;
                break;
            case Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT:
                elementType = ax::rhi::VertexElementType::FLOAT2;
                elementSize = 8;
                break;
            case Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT:
                elementType = ax::rhi::VertexElementType::FLOAT3;
                elementSize = 12;
                break;
            case Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT:
                elementType = ax::rhi::VertexElementType::FLOAT4;
                elementSize = 16;
                break;
            case Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM:
                elementType   = ax::rhi::VertexElementType::UBYTE4;
                needNormalize = true;
                elementSize   = 4;
                break;
            case Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UINT:
                elementType = ax::rhi::VertexElementType::UBYTE4;
                elementSize = 4;
                break;
            }

            auto inputDesc = program->getVertexInputDesc(
                ax::rhi::VertexSemantic{el.SemanticName.c_str(), static_cast<uint16_t>(el.SemanticIndex)});
            if (!inputDesc)
                return;
            desc.addAttrib(inputDesc, elementType, offset, needNormalize);
            offset += elementSize;
        }
        desc.endLayout(static_cast<int>(offset));

        _layout = ax::rhi::GraphicsCore::device()->createVertexLayout(std::move(desc));
    }

    ax::rhi::VertexLayout* get() const { return _layout; }

    ~VertexLayoutAX() override { AX_SAFE_RELEASE(_layout); }

private:
    std::vector<Effekseer::Backend::VertexLayoutElement> _elements;
    ax::rhi::VertexLayout* _layout = nullptr;
};

class UniformBufferAX : public Effekseer::Backend::UniformBuffer
{
public:
    explicit UniformBufferAX(int32_t size, const void* initialData) : data(size)
    {
        if (initialData && size > 0)
            memcpy(data.data(), initialData, size);
    }
    std::vector<uint8_t> data;
};

class StorageBufferAX : public Effekseer::Backend::StorageBuffer
{
public:
    StorageBufferAX(ax::rhi::Buffer* buffer, int32_t elementCount, int32_t elementSize, const void* initialData)
        : _buffer(buffer), _size(static_cast<size_t>(elementCount) * static_cast<size_t>(elementSize)),
          _shadow(initialData ? _size : 0)
    {
        if (_buffer)
            _buffer->retain();
        if (initialData && !_shadow.empty())
            memcpy(_shadow.data(), initialData, _shadow.size());
    }

    ~StorageBufferAX() override { AX_SAFE_RELEASE(_buffer); }

    bool updateData(const void* src, int32_t size, int32_t offset)
    {
        if (!_buffer || !src || size <= 0 || offset < 0)
            return false;

        const auto dstOffset = static_cast<size_t>(offset);
        const auto dstSize = static_cast<size_t>(size);
        if (dstOffset + dstSize > _size)
            return false;

        if (!_shadow.empty())
            memcpy(_shadow.data() + dstOffset, src, dstSize);
        _buffer->updateSubData(src, dstOffset, dstSize);
        return true;
    }

    ax::rhi::Buffer* get() const { return _buffer; }

private:
    ax::rhi::Buffer* _buffer = nullptr;
    size_t _size = 0;
    std::vector<uint8_t> _shadow;
};

class PipelineStateAX : public Effekseer::Backend::PipelineState
{
public:
    Effekseer::Backend::PipelineStateParameter param;
    ax::rhi::ProgramState* programState = nullptr;       // owned
    ax::rhi::ComputePipeline* computePipeline = nullptr;  // owned, lazily created for compute dispatch
    bool computePipelineFailed = false;

    ~PipelineStateAX() override
    {
        AX_SAFE_RELEASE(programState);
        AX_SAFE_RELEASE(computePipeline);
    }
};

class ShaderAX : public Effekseer::Backend::Shader
{
public:
    ShaderAX() = default;

    explicit ShaderAX(ax::rhi::Program* program) : _program(program) { AX_SAFE_RETAIN(_program); }

    ~ShaderAX() override { AX_SAFE_RELEASE(_program); }

    ax::rhi::Program* getProgram() const { return _program; }

    void setProgram(ax::rhi::Program* program)
    {
        AX_SAFE_RETAIN(program);
        AX_SAFE_RELEASE(_program);
        _program = program;
    }

private:
    ax::rhi::Program* _program = nullptr;
};

class GraphicsDevice : public Effekseer::Backend::GraphicsDevice
{
public:
    Effekseer::Backend::VertexBufferRef CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override
    {
        if (size <= 0)
            return nullptr;
        auto buffer = ax::rhi::GraphicsCore::device()->createBuffer(
            static_cast<size_t>(size), ax::rhi::BufferType::VERTEX,
            isDynamic ? ax::rhi::BufferUsage::DYNAMIC : ax::rhi::BufferUsage::STATIC, initialData);
        if (!buffer)
            return nullptr;
        auto ret = Effekseer::MakeRefPtr<VertexBufferAX>(buffer, size, initialData);
        AX_SAFE_RELEASE(buffer);
        return ret;
    }

    Effekseer::Backend::IndexBufferRef CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override
    {
        if (elementCount <= 0)
            return nullptr;
        const auto strideSize = stride == Effekseer::Backend::IndexBufferStrideType::Stride2 ? 2 : 4;
        auto buffer = ax::rhi::GraphicsCore::device()->createBuffer(
            static_cast<size_t>(elementCount * strideSize), ax::rhi::BufferType::INDEX, ax::rhi::BufferUsage::STATIC, initialData);
        if (!buffer)
            return nullptr;
        auto ret = Effekseer::MakeRefPtr<IndexBufferAX>(buffer, elementCount, stride, initialData);
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

    Effekseer::Backend::VertexLayoutRef CreateVertexLayout(const Effekseer::Backend::VertexLayoutElement* elements, int32_t count) override
    {
        auto ret = Effekseer::MakeRefPtr<VertexLayoutAX>();
        if (elements && count > 0)
            ret->setElements(elements, count);
        return ret;
    }

    Effekseer::Backend::UniformBufferRef CreateUniformBuffer(int32_t size, const void* initialData) override
    {
        if (size <= 0)
            return nullptr;
        return Effekseer::MakeRefPtr<UniformBufferAX>(size, initialData);
    }

    bool UpdateUniformBuffer(Effekseer::Backend::UniformBufferRef& buffer, int32_t size, int32_t offset, const void* data) override
    {
        auto ub = buffer.DownCast<UniformBufferAX>();
        if (ub == nullptr || data == nullptr || offset < 0 || size < 0)
            return false;
        if (static_cast<size_t>(offset) + static_cast<size_t>(size) > ub->data.size())
            return false;
        memcpy(ub->data.data() + offset, data, size);
        return true;
    }

    Effekseer::Backend::StorageBufferRef CreateStorageBuffer(int32_t elementCount,
                                                             int32_t elementSize,
                                                             const void* initialData,
                                                             Effekseer::Backend::StorageBufferUsage usage) override
    {
        if (elementCount <= 0 || elementSize <= 0)
            return nullptr;

        ax::rhi::BufferDesc desc;
        desc.size = static_cast<size_t>(elementCount) * static_cast<size_t>(elementSize);
        desc.stride = static_cast<uint32_t>(elementSize);
        desc.type = ax::rhi::BufferType::STORAGE;
        // Effekseer particle/trail buffers persist across multiple dispatches
        // and frames. BufferAccess controls SRV/UAV permissions; CPU usage must
        // remain STATIC so Vulkan does not rotate per-frame backing buffers.
        desc.usage = ax::rhi::BufferUsage::STATIC;
        desc.access = usage == Effekseer::Backend::StorageBufferUsage::ReadOnly ? ax::rhi::BufferAccess::READ_ONLY
                                                                                : ax::rhi::BufferAccess::READ_WRITE;

        auto buffer = ax::rhi::GraphicsCore::device()->createBuffer(desc, initialData);
        if (!buffer)
            return nullptr;
        auto ret = Effekseer::MakeRefPtr<StorageBufferAX>(buffer, elementCount, elementSize, initialData);
        AX_SAFE_RELEASE(buffer);
        return ret;
    }

    bool UpdateStorageBuffer(Effekseer::Backend::StorageBufferRef& buffer, int32_t size, int32_t offset, const void* data) override
    {
        auto storageBuffer = buffer.DownCast<StorageBufferAX>();
        return storageBuffer ? storageBuffer->updateData(data, size, offset) : false;
    }

    Effekseer::Backend::PipelineStateRef CreatePipelineState(const Effekseer::Backend::PipelineStateParameter& param) override
    {
        auto shader = param.ShaderPtr.DownCast<ShaderAX>();
        if (!shader || !shader->getProgram() || !shader->getProgram()->isValid())
            return nullptr;

        auto state = Effekseer::MakeRefPtr<PipelineStateAX>();
        state->param = param;
        state->programState = new ax::rhi::ProgramState(shader->getProgram());

        if (shader->getProgram()->getCSModule())
        {
            state->computePipeline =
                ax::rhi::GraphicsCore::device()->createComputePipeline(shader->getProgram());
            if (!state->computePipeline || !state->computePipeline->isValid())
            {
                AX_SAFE_RELEASE(state->computePipeline);
                return nullptr;
            }
        }

        // Compute pipelines do not have a vertex layout. Graphics pipelines do,
        // and an unusable reflected layout must fail system initialization.
        auto layout = param.VertexLayoutPtr.DownCast<VertexLayoutAX>();
        if (layout)
        {
            layout->buildLayout(shader->getProgram());
            if (!layout->get())
                return nullptr;
        }
        return state;
    }

    Effekseer::Backend::TextureRef CreateTexture(const Effekseer::Backend::TextureParameter& param,
                                                 const Effekseer::CustomVector<uint8_t>& initialData) override
    {
        if (param.Format != Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM ||
            (param.Dimension != 2 && param.Dimension != 3) || param.Size[0] <= 0 || param.Size[1] <= 0 ||
            param.Size[0] > std::numeric_limits<uint16_t>::max() ||
            param.Size[1] > std::numeric_limits<uint16_t>::max() ||
            (param.Dimension == 3 &&
             (param.Size[2] <= 0 || param.Size[2] > std::numeric_limits<uint16_t>::max())))
        {
            return nullptr;
        }

        ax::rhi::TextureDesc desc;
        desc.width  = static_cast<uint16_t>(param.Size[0]);
        desc.height = static_cast<uint16_t>(param.Size[1]);
        desc.mipLevels = static_cast<uint16_t>((std::max)(param.MipLevelCount, 1));
        desc.pixelFormat = ax::rhi::PixelFormat::RGBA8;
        if (param.Dimension == 3)
        {
            desc.textureType = ax::rhi::TextureType::TEXTURE_3D;
            desc.depth       = static_cast<uint16_t>(param.Size[2]);
        }
        auto texture = ax::rhi::GraphicsCore::device()->createTexture(desc);
        if (!texture)
            return nullptr;
        if (texture && !initialData.empty())
        {
            if (param.Dimension == 3)
                texture->updateData3D(initialData.data(), desc.width, desc.height, desc.depth, 0);
            else
                texture->updateData(initialData.data(), desc.width, desc.height, 0, 0);
        }
        auto ret = Effekseer::MakeRefPtr<TextureAX>(texture);
        AX_SAFE_RELEASE(texture);
        return ret;
    }

    Effekseer::Backend::ShaderRef CreateShaderFromBinary(const void* vsCode, int32_t vsSize, const void* psCode, int32_t psSize) override
    {
        if (!vsCode || vsSize <= 0 || !psCode || psSize <= 0)
            return nullptr;

        ax::Data vsData;
        vsData.copy(static_cast<const uint8_t*>(vsCode), vsSize);
        ax::Data psData;
        psData.copy(static_cast<const uint8_t*>(psCode), psSize);

        auto program = ax::rhi::GraphicsCore::device()->createProgram(std::move(vsData), std::move(psData));
        if (!program || !program->isValid())
        {
            AX_SAFE_RELEASE(program);
            return nullptr;
        }
        auto ret = Effekseer::MakeRefPtr<ShaderAX>(program);
        AX_SAFE_RELEASE(program);
        return ret;
    }

    Effekseer::Backend::ShaderRef CreateComputeShader(const void* csCode, int32_t csSize) override
    {
        if (!csCode || csSize <= 0)
            return nullptr;

        ax::Data csData;
        csData.copy(static_cast<const uint8_t*>(csCode), csSize);
        auto program = ax::rhi::GraphicsCore::device()->createComputeProgram(std::move(csData));
        if (!program || !program->isValid())
        {
            AX_SAFE_RELEASE(program);
            return nullptr;
        }

        auto ret = Effekseer::MakeRefPtr<ShaderAX>(program);
        AX_SAFE_RELEASE(program);
        return ret;
    }

    void Dispatch(const Effekseer::Backend::DispatchParameter& command) override
    {
        auto pipeline = command.PipelineStatePtr.DownCast<PipelineStateAX>();
        if (!pipeline || !pipeline->programState)
            return;
        if (command.GroupCount[0] <= 0 || command.GroupCount[1] <= 0 || command.GroupCount[2] <= 0)
            return;

        auto ps = pipeline->programState;
        auto program = ps->getProgram();
        if (!program)
            return;

        const auto& localSize = program->getComputeLocalSize();
        for (size_t i = 0; i < localSize.size(); ++i)
        {
            if (command.ThreadCount[i] != localSize[i])
            {
                AXLOGE("Effekseer compute local size mismatch at dimension {}: command={}, shader={}", i,
                       command.ThreadCount[i], localSize[i]);
                return;
            }
        }

        const auto* device = ax::rhi::GraphicsCore::device();
        if (!device)
            return;
        const auto& caps = device->getCaps();
        for (size_t i = 0; i < localSize.size(); ++i)
        {
            if (command.GroupCount[i] > caps.maxComputeWorkGroupCount[i] ||
                command.ThreadCount[i] > caps.maxComputeWorkGroupSize[i])
            {
                AXLOGE("Effekseer compute dispatch exceeds device limits at dimension {}", i);
                return;
            }
        }

        static constexpr std::array<std::string_view, Effekseer::Backend::DispatchParameter::BufferSlotCount>
            uniformBlockNames = {"cb0", "cb1", "cb2", "cb3"};
        size_t boundUniformBlockCount = 0;
        for (int i = 0; i < Effekseer::Backend::DispatchParameter::BufferSlotCount; ++i)
        {
            auto ub = command.UniformBufferPtrs[i].DownCast<UniformBufferAX>();
            if (ub && !ub->data.empty())
                boundUniformBlockCount += ps->setUniformBlock(ax::rhi::ShaderStage::COMPUTE, uniformBlockNames[i],
                                                               ub->data.data(), ub->data.size());
        }
        const auto activeUniformBlockCount = static_cast<size_t>(std::count_if(
            ps->getActiveUniformBlockInfos().begin(), ps->getActiveUniformBlockInfos().end(),
            [](const ax::rhi::UniformBlockInfo& block) { return block.stage == ax::rhi::ShaderStage::COMPUTE; }));
        if (boundUniformBlockCount != activeUniformBlockCount)
        {
            AXLOGE("Failed to bind all Effekseer compute uniform blocks");
            return;
        }

        if (!bindResources(ps, command.ResourceBinders,
                           Effekseer::Backend::DispatchParameter::ResourceSlotCount))
        {
            AXLOGE("Failed to bind all Effekseer compute resources");
            return;
        }

        if (!pipeline->computePipeline && !pipeline->computePipelineFailed)
        {
            pipeline->computePipeline = ax::rhi::GraphicsCore::device()->createComputePipeline(ps->getProgram());
            if (!pipeline->computePipeline || !pipeline->computePipeline->isValid())
            {
                AX_SAFE_RELEASE(pipeline->computePipeline);
                pipeline->computePipelineFailed = true;
                AXLOGE("Failed to create compute pipeline for Effekseer dispatch");
                return;
            }
        }
        if (!pipeline->computePipeline)
            return;

        ax::rhi::ComputeDispatchDesc desc;
        desc.pipeline     = pipeline->computePipeline;
        desc.programState = ps;
        desc.groupCountX  = static_cast<uint32_t>(command.GroupCount[0]);
        desc.groupCountY  = static_cast<uint32_t>(command.GroupCount[1]);
        desc.groupCountZ  = static_cast<uint32_t>(command.GroupCount[2]);

        auto context = _renderer && _renderer->getAxRenderer() ? _renderer->getAxRenderer()->getContext() : nullptr;
        if (!context || !context->dispatch(desc))
            AXLOGE("Failed to submit an Effekseer compute dispatch");
    }

    void Draw(const Effekseer::Backend::DrawParameter& param) override
    {
        if (!_renderer)
            return;

        auto pipeline = param.PipelineStatePtr.DownCast<PipelineStateAX>();
        if (!pipeline || !pipeline->programState)
            return;
        auto vb = param.VertexBufferPtr.DownCast<VertexBufferAX>();
        auto ib = param.IndexBufferPtr.DownCast<IndexBufferAX>();
        if (!vb || !vb->get() || !ib || !ib->get())
            return;

        auto layoutRef = pipeline->param.VertexLayoutPtr.DownCast<VertexLayoutAX>();
        auto layout    = layoutRef ? layoutRef->get() : nullptr;
        if (!layout)
            return;

        const auto stride = static_cast<size_t>(param.VertexStride);
        if (stride == 0 || vb->shadowSize() < stride || vb->shadowSize() % stride != 0 ||
            param.PrimitiveCount <= 0 || param.InstanceCount <= 0 || param.IndexOffset < 0)
            return;

        // PrimitiveCount describes indexed triangles. It is not the number of
        // vertices in the source model (a quad has 2 triangles and 4 vertices).
        const auto indexCount  = static_cast<size_t>(param.PrimitiveCount) * 3u;
        const auto indexOffset = static_cast<size_t>(param.IndexOffset);
        if (indexOffset + indexCount > static_cast<size_t>(ib->getElementCount()))
            return;

        auto axRenderer = _renderer->getAxRenderer();
        if (!axRenderer)
            return;

        // Commands are queued and may execute after Effekseer has prepared the
        // next draw. Keep uniforms, textures, samplers and storage bindings
        // independent for every queued GPU draw.
        auto ps = pipeline->programState->clone();
        static constexpr std::array<std::string_view, Effekseer::Backend::DrawParameter::BufferSlotCount>
            vertexUniformBlockNames = {"cb0", "cb1", "cb2", "cb3"};
        static constexpr std::array<std::string_view, Effekseer::Backend::DrawParameter::BufferSlotCount>
            fragmentUniformBlockNames = {"RenderConstantsPS", "ParameterDataPS", "", ""};
        size_t boundVertexUniformBlockCount = 0;
        size_t boundFragmentUniformBlockCount = 0;
        for (int i = 0; i < Effekseer::Backend::DrawParameter::BufferSlotCount; ++i)
        {
            auto ubv = param.VertexUniformBufferPtrs[i].DownCast<UniformBufferAX>();
            if (ubv && !ubv->data.empty())
                boundVertexUniformBlockCount +=
                    ps->setUniformBlock(ax::rhi::ShaderStage::VERTEX, vertexUniformBlockNames[i], ubv->data.data(),
                                        ubv->data.size());
            auto ubp = param.PixelUniformBufferPtrs[i].DownCast<UniformBufferAX>();
            if (ubp && !ubp->data.empty())
                boundFragmentUniformBlockCount +=
                    ps->setUniformBlock(ax::rhi::ShaderStage::FRAGMENT, fragmentUniformBlockNames[i], ubp->data.data(),
                                        ubp->data.size());
        }
        const auto& uniformBlocks = ps->getActiveUniformBlockInfos();
        const auto activeVertexUniformBlockCount = static_cast<size_t>(std::count_if(
            uniformBlocks.begin(), uniformBlocks.end(),
            [](const ax::rhi::UniformBlockInfo& block) { return block.stage == ax::rhi::ShaderStage::VERTEX; }));
        const auto activeFragmentUniformBlockCount = static_cast<size_t>(std::count_if(
            uniformBlocks.begin(), uniformBlocks.end(),
            [](const ax::rhi::UniformBlockInfo& block) { return block.stage == ax::rhi::ShaderStage::FRAGMENT; }));
        if (boundVertexUniformBlockCount != activeVertexUniformBlockCount ||
            boundFragmentUniformBlockCount != activeFragmentUniformBlockCount)
        {
            AXLOGE("Failed to bind all Effekseer graphics uniform blocks");
            AX_SAFE_RELEASE(ps);
            return;
        }

        if (!bindResources(ps, param.ResourceBinders, Effekseer::Backend::DrawParameter::ResourceSlotCount))
        {
            AXLOGE("Failed to bind all Effekseer graphics resources");
            AX_SAFE_RELEASE(ps);
            return;
        }

        if (_commandIndex >= _commands.size())
            _commands.emplace_back(std::make_unique<ax::CustomCommand>());
        auto* command = _commands[_commandIndex].get();
        _commandIndex++;

        command->init(_globalZOrder);
        command->setOwnPSVL(ps, layout, ax::RenderCommand::ADOPT_FLAG_PS);
        command->setDrawType(ax::CustomCommand::DrawType::ELEMENT_INSTANCED);
        command->setPrimitiveType(ax::CustomCommand::PrimitiveType::TRIANGLE);
        command->setTransparent(pipeline->param.IsBlendEnabled);

        const auto depthTest  = pipeline->param.IsDepthTestEnabled;
        const auto depthWrite = pipeline->param.IsDepthWriteEnabled;
        const auto depthFunc  = ToCompareFunc(pipeline->param.DepthFunc);
        const auto cullMode   = ToCullMode(pipeline->param.Culling);
        struct RenderStateBackup
        {
            bool depthTest = false;
            bool depthWrite = false;
            ax::rhi::CompareFunc depthFunc = ax::rhi::CompareFunc::LESS;
            ax::rhi::CullMode cullMode = ax::rhi::CullMode::NONE;
            ax::rhi::Winding winding = ax::rhi::Winding::COUNTER_CLOCK_WISE;
        };
        auto stateBackup = std::make_shared<RenderStateBackup>();
        command->setBeforeCallback([renderer = axRenderer, stateBackup, depthTest, depthWrite, depthFunc, cullMode]() {
            stateBackup->depthTest  = renderer->getDepthTest();
            stateBackup->depthWrite = renderer->getDepthWrite();
            stateBackup->depthFunc  = renderer->getDepthCompareFunc();
            stateBackup->cullMode   = renderer->getCullMode();
            stateBackup->winding    = renderer->getWinding();
            renderer->setDepthTest(depthTest);
            renderer->setDepthWrite(depthWrite);
            renderer->setDepthCompareFunc(depthFunc);
            renderer->setCullMode(cullMode);
            renderer->setWinding(ax::rhi::Winding::COUNTER_CLOCK_WISE);
        });
        command->setAfterCallback([renderer = axRenderer, stateBackup]() {
            renderer->setDepthTest(stateBackup->depthTest);
            renderer->setDepthWrite(stateBackup->depthWrite);
            renderer->setDepthCompareFunc(stateBackup->depthFunc);
            renderer->setCullMode(stateBackup->cullMode);
            renderer->setWinding(stateBackup->winding);
        });

        command->setVertexBuffer(vb->get());

        const auto indexFormat = ib->getStrideType() == Effekseer::Backend::IndexBufferStrideType::Stride2
                                     ? ax::CustomCommand::IndexFormat::U_SHORT
                                     : ax::CustomCommand::IndexFormat::U_INT;
        command->setIndexBuffer(ib->get(), indexFormat);
        command->setIndexDrawInfo(indexOffset, indexCount);
        command->setInstanceDrawInfo(param.InstanceCount);

        auto& blendDesc = command->blendDesc();
        blendDesc.blendEnabled                 = pipeline->param.IsBlendEnabled;
        blendDesc.rgbBlendOp                    = ToBlendOp(pipeline->param.BlendEquationRGB);
        blendDesc.alphaBlendOp                  = ToBlendOp(pipeline->param.BlendEquationAlpha);
        blendDesc.sourceRGBBlendFactor          = ToBlendFactor(pipeline->param.BlendSrcFunc);
        blendDesc.destinationRGBBlendFactor     = ToBlendFactor(pipeline->param.BlendDstFunc);
        blendDesc.sourceAlphaBlendFactor        = ToBlendFactor(pipeline->param.BlendSrcFuncAlpha);
        blendDesc.destinationAlphaBlendFactor   = ToBlendFactor(pipeline->param.BlendDstFuncAlpha);

        axRenderer->addCommand(command);
    }

    void setAxmolRenderer(class Renderer* renderer) { _renderer = renderer; }

    void setGlobalZOrder(float globalZOrder) { _globalZOrder = globalZOrder; }

    void beginFrame() { _commandIndex = 0; }

    void releaseCachedCommands()
    {
        for (auto& command : _commands)
        {
            if (command)
                command->releasePSVL();
        }
        _commands.clear();
        _commandIndex = 0;
    }

private:
    template <size_t N>
    bool bindResources(ax::rhi::ProgramState* ps,
                       const std::array<Effekseer::Backend::ResourceBinder, N>& binders,
                       int count)
    {
        if (!ps || !ps->getProgram())
            return false;

        const auto slotCount = (std::min)(static_cast<size_t>((std::max)(count, 0)), N);
        std::vector<bool> touchedStorageSlots(slotCount, false);
        std::vector<bool> touchedTextureSlots(slotCount, false);
        for (size_t slot = 0; slot < slotCount; ++slot)
        {
            auto& binder = binders[slot];
            if (auto* sb = std::get_if<Effekseer::Backend::StorageBufferBinder>(&binder))
            {
                auto sbb = sb->StorageBuffer.template DownCast<StorageBufferAX>();
                if (sbb && sbb->get())
                {
                    ps->setStorageBuffer(static_cast<int>(slot), sbb->get(),
                                         sb->Access == Effekseer::Backend::StorageBufferAccess::ReadWrite
                                             ? ax::rhi::BufferAccess::READ_WRITE
                                             : ax::rhi::BufferAccess::READ_ONLY);
                    touchedStorageSlots[slot] = true;
                }
            }
            else if (auto* tb = std::get_if<Effekseer::Backend::TextureBinder>(&binder))
            {
                auto tex = tb->Texture.template DownCast<TextureAX>();
                if (!tex || !tex->get())
                    continue;

                ax::rhi::UniformLocation textureLocation;
                for (auto& [name, info] : ps->getProgram()->getActiveTextureInfos())
                {
                    if (info->location == static_cast<int>(slot))
                    {
                        textureLocation = ps->getUniformLocation(name);
                        break;
                    }
                }
                if (textureLocation)
                {
                    ps->setTexture(textureLocation, static_cast<int>(slot), tex->get());
                    touchedTextureSlots[slot] = true;
                }

                auto samplerLoc = ps->getProgram()->getTextureSamplerLocation(static_cast<int>(slot));
                if (samplerLoc)
                {
                    const auto filter = tb->SamplingType == Effekseer::Backend::TextureSamplingType::Linear
                                            ? Effekseer::TextureFilterType::Linear
                                            : Effekseer::TextureFilterType::Nearest;
                    const auto wrap   = tb->WrapType == Effekseer::Backend::TextureWrapType::Mirror
                                            ? Effekseer::TextureWrapType::Mirror
                                            : tb->WrapType == Effekseer::Backend::TextureWrapType::Repeat
                                                  ? Effekseer::TextureWrapType::Repeat
                                                  : Effekseer::TextureWrapType::Clamp;
                    ps->setSampler(samplerLoc, ToSamplerDesc(filter, wrap));

                    const auto& samplers = ps->getProgram()->getActiveSamplerInfos();
                    auto sampler = std::find_if(samplers.begin(), samplers.end(), [samplerLoc](const auto& info) {
                        return info.binding == samplerLoc.binding && info.space == samplerLoc.space;
                    });
                    if (sampler != samplers.end() && sampler->presetIndex < 0 &&
                        !ps->getSamplerOverride(samplerLoc.binding))
                        return false;
                }
            }
        }

        const auto& storageBindings = ps->getStorageBufferBindingSets();
        for (const auto& storage : ps->getProgram()->getActiveStorageBufferInfos())
        {
            if (storage.binding < 0 || static_cast<size_t>(storage.binding) >= touchedStorageSlots.size() ||
                !touchedStorageSlots[storage.binding])
            {
                AXLOGE("Effekseer compute storage resource '{}' is not bound (binding={}, access={})", storage.name,
                       storage.binding, storage.access == ax::rhi::BufferAccess::READ_WRITE ? "read_write" : "read_only");
                return false;
            }
            auto binding = storageBindings.find(storage.binding);
            if (binding == storageBindings.end() || !binding->second.buffer ||
                binding->second.access != storage.access)
            {
                AXLOGE("Effekseer compute storage resource '{}' has an invalid binding (binding={}, buffer={}, access={})",
                       storage.name, storage.binding, binding != storageBindings.end() && binding->second.buffer ? "set" : "null",
                       binding != storageBindings.end() && binding->second.access == ax::rhi::BufferAccess::READ_WRITE
                           ? "read_write"
                           : "read_only");
                return false;
            }
        }

        const auto& textureBindings = ps->getTextureBindingSets();
        for (const auto& [_, texture] : ps->getProgram()->getActiveTextureInfos())
        {
            if (texture->location < 0 || static_cast<size_t>(texture->location) >= touchedTextureSlots.size() ||
                !touchedTextureSlots[texture->location])
            {
                AXLOGE("Effekseer compute texture '{}' is not bound (binding={})", _, texture->location);
                return false;
            }
            auto binding = textureBindings.find(texture->location);
            if (binding == textureBindings.end() || binding->second.texs.size() != texture->count)
            {
                AXLOGE("Effekseer compute texture '{}' has an invalid binding (binding={}, expected={}, actual={})", _,
                       texture->location, texture->count,
                       binding == textureBindings.end() ? 0 : binding->second.texs.size());
                return false;
            }
            if (std::any_of(binding->second.texs.begin(), binding->second.texs.end(),
                            [](const ax::rhi::Texture* value) { return value == nullptr; }))
            {
                AXLOGE("Effekseer compute texture '{}' contains a null texture (binding={})", _, texture->location);
                return false;
            }
        }

        return true;
    }

    class Renderer* _renderer = nullptr;
    float _globalZOrder        = 0.0f;
    std::vector<std::unique_ptr<ax::CustomCommand>> _commands;
    size_t _commandIndex = 0;

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
            _efkTexture = Effekseer::MakeRefPtr<TextureAX>(rhiTexture);
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

        _samplerLocations[0] = _program->getSamplerLocation("sampler_colorTex");
        _samplerLocations[1] = _program->getSamplerLocation(isDistortion ? "sampler_backTex" : "sampler_normalTex");
        _samplerLocations[2] = _program->getSamplerLocation("sampler_alphaTex");
        _samplerLocations[3] = _program->getSamplerLocation("sampler_uvDistortionTex");
        _samplerLocations[4] = _program->getSamplerLocation("sampler_blendTex");
        _samplerLocations[5] = _program->getSamplerLocation("sampler_blendAlphaTex");
        _samplerLocations[6] = _program->getSamplerLocation("sampler_blendUVDistortionTex");
        _samplerLocations[7] = _program->getSamplerLocation("sampler_depthTex");
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
        auto layout = ax::rhi::GraphicsCore::device()->createVertexLayout(std::move(desc));
        _vertexLayouts.emplace(stride, layout);
        return layout;
    }

    void setTexture(int slot, ax::rhi::Texture* texture)
    {
        if (texture && slot >= 0 && slot < 8)
            _programState->setTexture(_textureLocations[slot], slot, texture);
    }

    bool setSampler(int slot, const ax::rhi::SamplerDesc& samplerDesc)
    {
        if (slot < 0 || slot >= 8 || !_samplerLocations[slot])
            return false;
        _programState->setSampler(_samplerLocations[slot], samplerDesc);
        return true;
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
    ax::rhi::SamplerLocation _samplerLocations[8];
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

        auto backendTexture = Effekseer::MakeRefPtr<TextureAX>(texture->getRHITexture());
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
        auto backendTexture = Effekseer::MakeRefPtr<TextureAX>(texture->getRHITexture());
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

class GpuParticleFactoryAX final : public EffekseerRenderer::GpuParticleFactory
{
public:
    using EffekseerRenderer::GpuParticleFactory::GpuParticleFactory;

    Effekseer::GpuParticles::ResourceRef CreateResource(const Effekseer::GpuParticles::ParamSet& paramSet,
                                                        const Effekseer::Effect* effect) override
    {
        auto resource = EffekseerRenderer::GpuParticleFactory::CreateResource(paramSet, effect);
        auto axResource = resource.DownCast<EffekseerRenderer::GpuParticles::Resource>();
        if (!axResource || !axResource->ParamBuffer)
        {
            AXLOGE("Failed to create Effekseer GPU particle parameter resources");
            return nullptr;
        }

        if (paramSet.Force.TurbulencePower != 0.0f && !axResource->NoiseTexture)
        {
            AXLOGE("Failed to create the Effekseer GPU particle turbulence Texture3D");
            return nullptr;
        }

        const auto colorType = paramSet.RenderColor.ColorAllType;
        if ((colorType == Effekseer::GpuParticles::ColorParamType::FCurve ||
             colorType == Effekseer::GpuParticles::ColorParamType::Gradient) &&
            !axResource->GradientTexture)
        {
            AXLOGE("Failed to create the Effekseer GPU particle gradient texture");
            return nullptr;
        }

        return resource;
    }
};

/**
 * @brief Axmol GPU particle system: loads the five compiled GPU particle shaders
 * and hands them to the EffekseerRenderer::GpuParticleSystem common implementation.
 */
class GpuParticleSystemAX : public EffekseerRenderer::GpuParticleSystem
{
public:
    GpuParticleSystemAX(Renderer* renderer) : GpuParticleSystem(renderer) {}

    bool InitSystem(const Settings& settings) override
    {
        if (!RegisterGpuParticleSamplers())
        {
            AXLOGE("Failed to register Effekseer GPU particle custom samplers");
            return false;
        }

        auto graphics = renderer_->GetGraphicsDevice();
        if (!graphics)
        {
            AXLOGE("Effekseer GPU particle system has no graphics device");
            return false;
        }

        auto loadArchive = [](const char* name) -> ax::Data {
            auto file = ax::FileUtils::getInstance()->fullPathForFilename(name);
            if (file.empty())
            {
                AXLOGE("Effekseer GPU particle shader archive was not found: {}", name);
                return {};
            }

            auto data = ax::FileUtils::getInstance()->getDataFromFile(file);
            if (data.isNull())
                AXLOGE("Failed to read Effekseer GPU particle shader archive: {}", file);
            return data;
        };
        auto loadComputeShader = [&loadArchive, &graphics](const char* name) -> Effekseer::Backend::ShaderRef {
            auto data = loadArchive(name);
            if (data.isNull())
                return nullptr;

            auto shader = graphics->CreateComputeShader(data.getBytes(), static_cast<int32_t>(data.getSize()));
            if (!shader)
                AXLOGE("Failed to create Effekseer GPU particle compute shader: {}", name);
            return shader;
        };

        Shaders shaders;
        shaders.CsParticleClear  = loadComputeShader("custom/gpu_particles_clear_cs");
        shaders.CsParticleSpawn  = loadComputeShader("custom/gpu_particles_spawn_cs");
        shaders.CsParticleUpdate = loadComputeShader("custom/gpu_particles_update_cs");

        auto renderVS = loadArchive("custom/gpu_particles_render_vs");
        auto renderPS = loadArchive("custom/gpu_particles_render_ps");
        if (!renderVS.isNull() && !renderPS.isNull())
        {
            shaders.RsParticleRender = graphics->CreateShaderFromBinary(
                renderVS.getBytes(), static_cast<int32_t>(renderVS.getSize()),
                renderPS.getBytes(), static_cast<int32_t>(renderPS.getSize()));
            if (!shaders.RsParticleRender)
                AXLOGE("Failed to create the Effekseer GPU particle render shader");
        }

        if (!shaders.CsParticleClear || !shaders.CsParticleSpawn || !shaders.CsParticleUpdate ||
            !shaders.RsParticleRender)
            return false;

        // Validate shader archives before allocating the large persistent
        // particle and trail buffers.
        if (!GpuParticleSystem::InitSystem(settings))
        {
            AXLOGE("Failed to allocate Effekseer GPU particle system resources");
            return false;
        }

        if (!SetShaders(shaders))
        {
            AXLOGE("Failed to create Effekseer GPU particle compute pipelines");
            return false;
        }

        EffekseerRenderer::GpuParticles::PipelineStateKey renderKey{};
        if (!GetOrCreatePipelineState(renderKey))
        {
            AXLOGE("Failed to create the Effekseer GPU particle render pipeline");
            return false;
        }
        return true;
    }
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
    if (_graphicsDevice)
        static_cast<GraphicsDevice*>(_graphicsDevice.Get())->setAxmolRenderer(nullptr);
}

bool Renderer::initialize()
{
    _graphicsDevice = Effekseer::MakeRefPtr<GraphicsDevice>();
    static_cast<GraphicsDevice*>(_graphicsDevice.Get())->setAxmolRenderer(this);
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
    if (_graphicsDevice)
        static_cast<GraphicsDevice*>(_graphicsDevice.Get())->beginFrame();
    SetGlobalZOrder(_globalZOrder);
}

void Renderer::SetGlobalZOrder(float globalZOrder)
{
    _globalZOrder = globalZOrder;
    if (_graphicsDevice)
        static_cast<GraphicsDevice*>(_graphicsDevice.Get())->setGlobalZOrder(globalZOrder);
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

    if (_graphicsDevice)
        static_cast<GraphicsDevice*>(_graphicsDevice.Get())->releaseCachedCommands();
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
    auto device = ax::rhi::GraphicsCore::device();
    if (!SupportsGpuParticleSettings(device, settings))
    {
        static bool s_warned = false;
        if (!s_warned)
        {
            s_warned = true;
            AXLOGW("Effekseer GPU particles are not supported by the current RHI backend; GPU particle nodes will be skipped.");
        }
        return nullptr;
    }

    auto system = Effekseer::MakeRefPtr<GpuParticleSystemAX>(this);
    if (!system->InitSystem(settings))
    {
        AXLOGE("Failed to initialize the Effekseer GPU particle system");
        return nullptr;
    }
    return system;
}

Effekseer::GpuParticleFactoryRef Renderer::CreateGpuParticleFactory()
{
    auto device = ax::rhi::GraphicsCore::device();
    if (!SupportsGpuParticleFeatures(device))
        return nullptr;

    return Effekseer::MakeRefPtr<GpuParticleFactoryAX>(_graphicsDevice);
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

    auto proxyTexture = GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White).DownCast<TextureAX>();
    if (proxyTexture && proxyTexture->get())
        shader->setDefaultTextures(proxyTexture->get());

    float samplerModes[4] = {};
    const auto& state = _renderState->GetActiveState();
    for (int32_t i = 0; i < count; i++)
    {
        if (i < 4)
            samplerModes[i] = ToSamplerMode(state.TextureFilterTypes[i], state.TextureWrapTypes[i]);

        auto texture = textures[i].DownCast<TextureAX>();
        if (texture && texture->get())
        {
            auto samplerDesc = ToSamplerDesc(state.TextureFilterTypes[i], state.TextureWrapTypes[i]);
            if (!shader->setSampler(i, samplerDesc))
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

    auto vb = static_cast<VertexBufferAX*>(_currentVertexBuffer.Get());
    auto ib = static_cast<IndexBufferAX*>(_currentIndexBuffer.Get());
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
