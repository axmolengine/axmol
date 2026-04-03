/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "axmol/rhi/d3d12/RenderPipeline12.h"
#include "axmol/rhi/d3d12/VertexLayout12.h"
#include "axmol/rhi/d3d12/DepthStencilState12.h"
#include "axmol/rhi/d3d12/Program12.h"
#include "axmol/rhi/d3d12/RenderContext12.h"
#include "axmol/rhi/d3d12/Driver12.h"
#include "axmol/base/Logging.h"
#include "axmol/tlx/hash.hpp"

namespace ax::rhi::d3d12
{

static constexpr D3D12_BLEND kBlendFactorMap[] = {
    D3D12_BLEND_ZERO,              // ZERO
    D3D12_BLEND_ONE,               // ONE
    D3D12_BLEND_SRC_COLOR,         // SRC_COLOR
    D3D12_BLEND_INV_SRC_COLOR,     // ONE_MINUS_SRC_COLOR
    D3D12_BLEND_SRC_ALPHA,         // SRC_ALPHA
    D3D12_BLEND_INV_SRC_ALPHA,     // ONE_MINUS_SRC_ALPHA
    D3D12_BLEND_DEST_COLOR,        // DST_COLOR
    D3D12_BLEND_INV_DEST_COLOR,    // ONE_MINUS_DST_COLOR
    D3D12_BLEND_DEST_ALPHA,        // DST_ALPHA
    D3D12_BLEND_INV_DEST_ALPHA,    // ONE_MINUS_DST_ALPHA
    D3D12_BLEND_BLEND_FACTOR,      // CONSTANT_ALPHA
    D3D12_BLEND_SRC_ALPHA_SAT,     // SRC_ALPHA_SATURATE
    D3D12_BLEND_INV_BLEND_FACTOR,  // ONE_MINUS_CONSTANT_ALPHA
    D3D12_BLEND_BLEND_FACTOR       // BLEND_COLOR
};

static const D3D12_BLEND_OP kBlendOpMap[] = {
    D3D12_BLEND_OP_ADD,          // ADD
    D3D12_BLEND_OP_SUBTRACT,     // SUBTRACT
    D3D12_BLEND_OP_REV_SUBTRACT  // REVERSE_SUBTRACT
};

static constexpr bool kBlendFactorColorValid[] = {true, true, true, true, true, true, true,
                                                  true, true, true, true, true, true, true};

static constexpr bool kBlendFactorAlphaValid[] = {true,  true, false, false, true,  true, false,
                                                  false, true, true,  true,  false, true, true};

// Map BlendOp to D3D12_BLEND_OP
static inline D3D12_BLEND_OP toD3DBlendOp(BlendOp op)
{
    auto idx = static_cast<size_t>(op);
    return kBlendOpMap[idx];
}

// For COLOR channels
static inline D3D12_BLEND toD3DBlendFactorColor(BlendFactor f)
{
    auto idx = static_cast<size_t>(f);
    return kBlendFactorColorValid[idx] ? kBlendFactorMap[idx] : D3D12_BLEND_ONE;
}

// For ALPHA channels
static inline D3D12_BLEND toD3DBlendFactorAlpha(BlendFactor f)
{
    auto idx = static_cast<size_t>(f);
    return kBlendFactorAlphaValid[idx] ? kBlendFactorMap[idx] : D3D12_BLEND_ONE;
}

static inline uintptr_t makePSOKey(const rhi::BlendDesc& blendDesc,
                                   const DepthStencilStateImpl* dsState,
                                   uint64_t programId,
                                   uint32_t vlHash,
                                   D3D12_RASTERIZER_DESC& rs,
                                   PrimitiveGroup primitiveGroup)
{
    if constexpr (sizeof(void*) == 8)
    {
        struct HashMe
        {
            rhi::BlendDesc blend{};
            uintptr_t dsHash;
            uint64_t progId;
        };
        HashMe hashMe{.blend = blendDesc, .dsHash = dsState->getHash(), .progId = programId};
        const auto rasterComp =
            ((uint32_t)primitiveGroup << 16) | (rs.CullMode << 8) | (rs.FrontCounterClockwise ? 1 : 0);
        const auto seed = (uint64_t)vlHash << 32 | (uint64_t)rasterComp;

        return tlx::hash_bytes(&hashMe, sizeof(hashMe), seed);
    }
    else
    {
        struct HashMe
        {
            rhi::BlendDesc blend{};
            uintptr_t dsHash;
            uint64_t progId;
            uint32_t vlHash;
        };
        HashMe hashMe{.blend = blendDesc, .dsHash = dsState->getHash(), .progId = programId, .vlHash = vlHash};
        const auto rasterComp =
            ((uint32_t)primitiveGroup << 16) | (rs.CullMode << 8) | (rs.FrontCounterClockwise ? 1 : 0);

        return tlx::hash_bytes(&hashMe, sizeof(hashMe), rasterComp);
    }
}

RenderPipelineImpl::RenderPipelineImpl(DriverImpl* driver) : _driver(driver)
{
    initializePipelineDefaults();
}

RenderPipelineImpl::~RenderPipelineImpl()
{
    _driver->waitForGPU();

    _activePSO.Reset();
    _activeRootSignature = nullptr;

    _psoCache.clear();
    _rootSigCache.clear();
}

void RenderPipelineImpl::initializePipelineDefaults()
{
    // Blend
    _blendDesc                        = {};
    _blendDesc.AlphaToCoverageEnable  = FALSE;
    _blendDesc.IndependentBlendEnable = FALSE;
    auto& rt                          = _blendDesc.RenderTarget[0];
    rt.BlendEnable                    = FALSE;
    rt.RenderTargetWriteMask          = D3D12_COLOR_WRITE_ENABLE_ALL;

    // Rasterizer
    _rasterDesc                       = {};
    _rasterDesc.FillMode              = D3D12_FILL_MODE_SOLID;
    _rasterDesc.CullMode              = D3D12_CULL_MODE_BACK;
    _rasterDesc.FrontCounterClockwise = TRUE;
    _rasterDesc.DepthClipEnable       = TRUE;
}

void RenderPipelineImpl::update(const RenderTarget* /*rt*/, const PipelineDesc& desc)
{
    if (!desc.programState || !desc.vertexLayout)
    {
        AXASSERT(false, "RenderPipelineImpl::update: invalid inputs");
        return;
    }

    auto program = static_cast<ProgramImpl*>(desc.programState->getProgram());

    updateBlendState(desc.blendDesc);
    updateRootSignature(program);
    updateGraphicsPipeline(desc, program);
}

void RenderPipelineImpl::updateBlendState(const BlendDesc& blendDesc)
{
    auto& rt = _blendDesc.RenderTarget[0];

    rt.BlendEnable           = blendDesc.blendEnabled ? TRUE : FALSE;
    rt.RenderTargetWriteMask = static_cast<UINT>(blendDesc.writeMask);

    // Color blend factors
    rt.SrcBlend  = toD3DBlendFactorColor(blendDesc.sourceRGBBlendFactor);
    rt.DestBlend = toD3DBlendFactorColor(blendDesc.destinationRGBBlendFactor);
    rt.BlendOp   = toD3DBlendOp(blendDesc.rgbBlendOp);

    // Alpha blend factors
    rt.SrcBlendAlpha  = toD3DBlendFactorAlpha(blendDesc.sourceAlphaBlendFactor);
    rt.DestBlendAlpha = toD3DBlendFactorAlpha(blendDesc.destinationAlphaBlendFactor);
    rt.BlendOpAlpha   = toD3DBlendOp(blendDesc.alphaBlendOp);

    // LogicOp
    rt.LogicOpEnable = FALSE;
    rt.LogicOp       = D3D12_LOGIC_OP_NOOP;
}

void RenderPipelineImpl::updateRootSignature(ProgramImpl* program)
{
    auto progId = program->getProgramId();
    if (auto it = _rootSigCache.find(progId); it != _rootSigCache.end())
    {
        _activeRootSignature = &it->second;
        return;
    }

    RootSignatureEntry entry;

    UINT rootIndex = 0;

    tlx::pod_vector<D3D12_ROOT_PARAMETER> rootParams;
    rootParams.reserve(4);  // VS UBO, FS UBO, FS SRV table, FS Sampler table

    for (auto& uboInfo : program->getActiveUniformBlockInfos())
    {
        D3D12_ROOT_PARAMETER& param     = rootParams.emplace_back();
        param.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
        param.Descriptor.ShaderRegister = uboInfo.binding;  // usually 0
        param.Descriptor.RegisterSpace  = SET_INDEX_UBO;    // map Vulkan set -> D3D12 space
        param.ShaderVisibility =
            uboInfo.stage == ShaderStage::VERTEX ? D3D12_SHADER_VISIBILITY_VERTEX : D3D12_SHADER_VISIBILITY_PIXEL;

        // hack: rootIndex must match the uboInfo.binding
        assert(rootIndex == uboInfo.binding);
        rootIndex++;
    }

    // --- FS SRVs (textures) -> descriptor table, space = SET_INDEX_SRV ---
    tlx::pod_vector<D3D12_DESCRIPTOR_RANGE> srvRanges;

    // --- Sampler descriptor table (global heap) ---
    D3D12_DESCRIPTOR_RANGE samplerRange{};

    auto& fsSamplers = program->getActiveTextureInfos();
    if (!fsSamplers.empty())
    {
        samplerRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        samplerRange.BaseShaderRegister                = 0;
        samplerRange.RegisterSpace                     = SET_INDEX_SAMPLER;
        samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        D3D12_ROOT_PARAMETER& samplerParam               = rootParams.emplace_back();
        samplerParam.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        samplerParam.DescriptorTable.NumDescriptorRanges = 1;
        samplerParam.DescriptorTable.pDescriptorRanges   = &samplerRange;
        samplerParam.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
        entry.samplerRootIndex                           = rootIndex++;

        uint16_t maxSamplerSlot = 0;
        for (auto& [_, smp] : fsSamplers)
        {
            // In Vulkan, sampler2D is a combined image sampler.
            // In D3D12 we only process texture, sampler is in global heap
            // --- SRV range (texture part) ---
            D3D12_DESCRIPTOR_RANGE& srvRange           = srvRanges.emplace_back();
            srvRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            srvRange.NumDescriptors                    = smp->count;     // number of textures
            srvRange.BaseShaderRegister                = smp->location;  // t#
            srvRange.RegisterSpace                     = SET_INDEX_SRV;  // match Vulkan set
            srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

            if (maxSamplerSlot < smp->samplerSlot)
                maxSamplerSlot = smp->samplerSlot;
        }

        samplerRange.NumDescriptors = maxSamplerSlot + 1;

        // Add SRV descriptor table root parameter
        D3D12_ROOT_PARAMETER& srvParam               = rootParams.emplace_back();
        srvParam.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        srvParam.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(srvRanges.size());
        srvParam.DescriptorTable.pDescriptorRanges   = srvRanges.data();
        srvParam.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_PIXEL;
        entry.srvRootIndex                           = rootIndex++;
    }

    // --- finalize root signature ---
    D3D12_ROOT_SIGNATURE_DESC rsDesc{};
    rsDesc.NumParameters = static_cast<UINT>(rootParams.size());
    rsDesc.pParameters   = rootParams.data();
    rsDesc.Flags         = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    Microsoft::WRL::ComPtr<ID3DBlob> sigBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errBlob;
    HRESULT hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
    AXASSERT(SUCCEEDED(hr), "Failed to serialize root signature");

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig;
    hr = _driver->getDevice()->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(),
                                                   IID_PPV_ARGS(&rootSig));
    AXASSERT(SUCCEEDED(hr), "Failed to create root signature");

    entry.rootSig        = std::move(rootSig);
    _activeRootSignature = &_rootSigCache.emplace(progId, std::move(entry)).first->second;
}

void RenderPipelineImpl::updateGraphicsPipeline(const PipelineDesc& desc, ProgramImpl* program)
{
    const auto progId = program->getProgramId();
    auto key = makePSOKey(desc.blendDesc, _dsState, progId, desc.vertexLayout->getHash(), _rasterDesc, _primitiveGroup);
    auto it  = _psoCache.find(key);
    if (it != _psoCache.end())
    {
        _activePSO = it->second;
        return;
    }

    static constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE kPrimitiveTopologyTypes[] = {D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
                                                                                D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
                                                                                D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE};
    auto vsBlob                                                              = program->getVSBlob();
    auto psBlob                                                              = program->getPSBlob();

    auto& vi = static_cast<VertexLayoutImpl*>(desc.vertexLayout)->getD3D12InputLayout();

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.pRootSignature        = _activeRootSignature->rootSig.Get();
    psoDesc.VS                    = {vsBlob.view.data(), vsBlob.view.size()};
    psoDesc.PS                    = {psBlob.view.data(), psBlob.view.size()};
    psoDesc.BlendState            = _blendDesc;
    psoDesc.RasterizerState       = _rasterDesc;
    psoDesc.DepthStencilState     = _dsState->getD3D12DepthStencilDesc();
    psoDesc.InputLayout           = vi;
    psoDesc.PrimitiveTopologyType = kPrimitiveTopologyTypes[(uint32_t)_primitiveGroup];
    psoDesc.NumRenderTargets      = 1;
    psoDesc.RTVFormats[0]         = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psoDesc.SampleDesc.Count      = 1;
    psoDesc.SampleMask            = UINT_MAX;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
    HRESULT hr = _driver->getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));
    if (FAILED(hr))
    {
        AXLOGE("Failed to create PSO, hr=0x{:08x}", static_cast<unsigned>(hr));
        AXASSERT(false, "Failed to create PSO");
    }

    _activePSO = pso;
    _psoCache.emplace(key, pso);
    _programToPSOMap.emplace(progId, key);
}

void RenderPipelineImpl::removeCachedObjects(Program* key)
{
    auto progId = key->getProgramId();
    if (auto it = _rootSigCache.find(progId); it != _rootSigCache.end())
        _rootSigCache.erase(it);

    auto range = _programToPSOMap.equal_range(progId);
    if (range.first != range.second)
    {
        for (auto it = range.first; it != range.second; ++it)
        {
            auto pipelineKey = it->second;
            auto pipelineIt  = _psoCache.find(pipelineKey);
            if (pipelineIt != _psoCache.end())
            {
                _psoCache.erase(pipelineIt);
            }
        }
        _programToPSOMap.erase(range.first, range.second);
    }
}

}  // namespace ax::rhi::d3d12
