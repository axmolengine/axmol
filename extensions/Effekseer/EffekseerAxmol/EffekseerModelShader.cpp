/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.
 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "EffekseerModelShader.h"
#include "axmol/base/CustomEventListener.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/EventType.h"
#include "axmol/renderer/ProgramManager.h"
#include "axmol/rhi/GraphicsCore.h"
#include "axmol/rhi/SamplerRegistry.h"
#include "axmol/rhi/VertexLayout.h"
#include <EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.h>
#include <algorithm>

namespace EffekseerRendererAxmol
{
namespace
{
using namespace ax::rhi;
using Type = EffekseerRenderer::RendererShaderType;
using PlainVS = EffekseerRenderer::ModelRendererVertexConstantBuffer<1>;
using AdvancedVS = EffekseerRenderer::ModelRendererAdvancedVertexConstantBuffer<1>;
using ColorPS = EffekseerRenderer::PixelConstantBuffer;
using DistortionPS = EffekseerRenderer::PixelConstantBufferDistortion;
// The HLSL constant blocks must match these CPU layouts for whole-block uploads.
static_assert(sizeof(PlainVS) == 224 && offsetof(PlainVS, ModelMatrix) == 64 && offsetof(PlainVS, UVInversed) == 208);
static_assert(sizeof(AdvancedVS) == 368 && offsetof(AdvancedVS, ModelFlipbookParameter) == 224 &&
              offsetof(AdvancedVS, ModelColor) == 288 && offsetof(AdvancedVS, UVInversed) == 352);
static_assert(sizeof(ColorPS) == 288 && offsetof(ColorPS, LightColor) == 16 && offsetof(ColorPS, MiscFlags) == 272);
static_assert(sizeof(DistortionPS) == 128 && offsetof(DistortionPS, SoftParticleParam) == 80);

struct Binding
{
    const char* texture;
    const char* sampler;
};
constexpr Binding color{"u_tex0", "sampler_colorTex"}, normal{"u_tex1", "sampler_normalTex"}, back{"u_tex1", "sampler_backTex"};
constexpr Binding adColor{"_colorTex", color.sampler}, adNormal{"_normalTex", normal.sampler}, adBack{"_backTex", back.sampler};
constexpr Binding alpha{"_alphaTex", "sampler_alphaTex"}, uvDist{"_uvDistortionTex", "sampler_uvDistortionTex"},
    blend{"_blendTex", "sampler_blendTex"}, blendAlpha{"_blendAlphaTex", "sampler_blendAlphaTex"},
    blendDist{"_blendUVDistortionTex", "sampler_blendUVDistortionTex"};

struct Variant
{
    Type type;
    const char* vs;
    const char* fs;
    std::unique_ptr<DrawShader> (*create)(const Variant&, Effekseer::Backend::GraphicsDeviceRef);
    std::array<Binding, 7> slots; // Effekseer's collector order, independent of RHI register allocation
};

// The base writes C++ objects, not serialized bytes. Use real aligned objects of
// the selected type rather than casting storage sized for another shader ABI.
template<class VS, class PS>
class ModelShader final : public DrawShader
{
public:
    static std::unique_ptr<DrawShader> create(const Variant& variant, Effekseer::Backend::GraphicsDeviceRef device)
    {
        auto ret = std::make_unique<ModelShader>();
        for (const auto& slot : variant.slots)
            if (slot.sampler && !SamplerRegistry::getInstance()->registerSampler(slot.sampler, SamplerDesc{}))
                return nullptr;

        auto* manager = ax::ProgramManager::getInstance();
        auto load = [&] { return manager->loadProgram(variant.vs, variant.fs, ax::VertexLayoutKind::Invalid,
                                                      ax::ProgramManager::LoadPolicy::Optional); };
        auto* program = load(); // borrowed; ProgramState takes its own reference
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
        // A cached program may outlive every EffectManager. A registry rebuild
        // can reassign custom sampler IDs even when the GL relink succeeded.
        if (program && (!program->isValid() || std::any_of(program->getActiveSamplerInfos().begin(),
                program->getActiveSamplerInfos().end(), [](const auto& info) {
                    return info.samplerId.value != SamplerRegistry::getInstance()->find(info.name).value;
                })))
        {
            manager->unloadProgram(program);
            program = load();
        }
#endif
        if (!program || !program->isValid())
            return nullptr;
        ret->_state = new ProgramState(program);
        ret->_layout = createModelLayout(device, program);
        if (!ret->_layout)
            return nullptr;

        bool hasVS = false, hasPS = false;
        for (const auto& block : program->getActiveUniformBlockInfos())
        {
            if (block.stage == ShaderStage::VERTEX && block.sizeBytes == sizeof(VS))
                hasVS = true;
            if (block.stage == ShaderStage::FRAGMENT && block.sizeBytes == sizeof(PS))
                hasPS = true;
        }
        if (!hasVS || !hasPS || !ret->uploadBlocks())
            return nullptr;

        for (const auto& slot : variant.slots)
        {
            if (!slot.texture)
                break;
            auto idx = ret->_slotCount++;
            ret->_textures[idx] = program->getUniformLocation(slot.texture);
            ret->_samplers[idx] = program->getSamplerLocation(slot.sampler);
            if (!ret->_textures[idx] || !ret->_samplers[idx])
                return nullptr;
        }
        if (program->getActiveTextureInfos().size() != static_cast<size_t>(ret->_slotCount))
            return nullptr;
        return ret;
    }
    ~ModelShader() override
    {
        AX_SAFE_RELEASE(_state);
        AX_SAFE_RELEASE(_layout);
    }
    void* GetVertexConstantBuffer() override { return &_vs; }
    void* GetPixelConstantBuffer() override { return &_ps; }
    void SetVertexConstantBufferSize(int32_t size) override { AXASSERT(size == sizeof(VS), "Model VS ABI mismatch"); }
    void SetPixelConstantBufferSize(int32_t size) override { AXASSERT(size == sizeof(PS), "Model PS ABI mismatch"); }
    void SetConstantBuffer() override { uploadBlocks(); }
    ProgramState* programState() const override { return _state; }
    VertexLayout* vertexLayout(int32_t) override { return _layout; }
    void setTexture(int slot, Texture* texture) override
    {
        if (slot >= 0 && slot < _slotCount && texture)
            _state->setTexture(_textures[slot], _textures[slot].location, texture);
    }
    void setSampler(int slot, const SamplerDesc& desc) override
    {
        if (slot >= 0 && slot < _slotCount)
            _state->setSampler(_samplers[slot], desc);
    }
    void setDefaultTextures(Texture* white) override
    {
        for (int i = 0; i < _slotCount; ++i)
            setTexture(i, white);
    }

private:
    bool uploadBlocks()
    {
        return _state->setUniformBlock(ShaderStage::VERTEX, "vs_ub", &_vs, sizeof(_vs)) &&
               _state->setUniformBlock(ShaderStage::FRAGMENT, "fs_ub", &_ps, sizeof(_ps));
    }
    VS _vs{};
    PS _ps{};
    ProgramState* _state = nullptr;
    VertexLayout* _layout = nullptr;
    int _slotCount = 0;
    std::array<UniformLocation, 7> _textures{};
    std::array<SamplerLocation, 7> _samplers{};
};

constexpr Variant variants[] = {
    {Type::Unlit, "custom/model_vs", "custom/model_ps_2", ModelShader<PlainVS, ColorPS>::create, {color}},
    {Type::Lit, "custom/model_vs_1", "custom/model_ps_3", ModelShader<PlainVS, ColorPS>::create, {color, normal}},
    {Type::BackDistortion, "custom/model_vs_2", "custom/model_distortion_ps_1", ModelShader<PlainVS, DistortionPS>::create,
     {color, back}},
    {Type::AdvancedUnlit, "custom/ad_model_vs", "custom/ad_model_ps", ModelShader<AdvancedVS, ColorPS>::create,
     {adColor, alpha, uvDist, blend, blendAlpha, blendDist}},
    {Type::AdvancedLit, "custom/ad_model_vs_1", "custom/ad_model_ps_1", ModelShader<AdvancedVS, ColorPS>::create,
     {adColor, adNormal, alpha, uvDist, blend, blendAlpha, blendDist}},
    {Type::AdvancedBackDistortion, "custom/ad_model_vs_2", "custom/ad_model_distortion_ps", ModelShader<AdvancedVS, DistortionPS>::create,
     {adColor, adBack, alpha, uvDist, blend, blendAlpha, blendDist}},
};
} // namespace

ModelShaderCache::ModelShaderCache()
{
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    _eventDispatcher = ax::Director::getInstance()->getEventDispatcher();
    AX_SAFE_RETAIN(_eventDispatcher);
    if (_eventDispatcher)
    {
        _recreated = ax::CustomEventListener::create(EVENT_RENDERER_RECREATED, [this](ax::CustomEvent*) { _entries = {}; });
        _eventDispatcher->addEventListenerWithFixedPriority(_recreated, 1);
    }
#endif
}
ModelShaderCache::~ModelShaderCache()
{
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    if (_recreated && _eventDispatcher)
        _eventDispatcher->removeEventListener(_recreated);
    AX_SAFE_RELEASE(_eventDispatcher);
#endif
}
DrawShader* ModelShaderCache::acquire(Type type, Effekseer::Backend::GraphicsDeviceRef device)
{
    static_assert(std::size(variants) == std::tuple_size_v<decltype(_entries)>);
    for (size_t i = 0; i < std::size(variants); ++i)
    {
        const auto& v = variants[i];
        if (v.type != type)
            continue;
        auto& entry = _entries[i];
        if (!entry.attempted)
        {
            entry.attempted = true;
            entry.shader = v.create(v, device);
            if (!entry.shader)
                AXLOGE("Effekseer model shader unavailable (backend={}, type={}, VS={}, PS={}); skipping this material",
                       static_cast<int>(ax::rhi::GraphicsCore::backend()), static_cast<int>(type), v.vs, v.fs);
        }
        return entry.shader.get();
    }
    return nullptr;
}
} // namespace EffekseerRendererAxmol
