/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/rhi/d3d12/ComputePipeline12.h"
#include "axmol/rhi/d3d12/GraphicsDevice12.h"
#include "axmol/rhi/d3d12/Program12.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/base/Logging.h"
#include <string_view>

namespace ax::rhi::d3d12
{

ComputePipelineImpl::ComputePipelineImpl(GraphicsDeviceImpl* driver, ProgramImpl* program) : _driver(driver)
{
    setProgram(program);
    if (createRootSignature(program))
        createPipeline(program);
}

ComputePipelineImpl::~ComputePipelineImpl() {}

bool ComputePipelineImpl::createRootSignature(ProgramImpl* program)
{
    tlx::pod_vector<D3D12_ROOT_PARAMETER> rootParams;
    rootParams.reserve(6);

    UINT rootIndex = 0;

    for (auto& uboInfo : program->getActiveUniformBlockInfos())
    {
        D3D12_ROOT_PARAMETER& param     = rootParams.emplace_back();
        param.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
        param.Descriptor.ShaderRegister = uboInfo.binding;
        param.Descriptor.RegisterSpace  = 0;
        param.ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
        _cbvRootIndices.emplace(uboInfo.binding, rootIndex);
        ++rootIndex;
    }

    // Storage buffers and textures share the unified logical slot sequence (space 1).
    tlx::pod_vector<D3D12_DESCRIPTOR_RANGE> srvRanges;
    tlx::pod_vector<D3D12_DESCRIPTOR_RANGE> uavRanges;
    tlx::pod_vector<D3D12_DESCRIPTOR_RANGE> customSamplerRanges;
    D3D12_DESCRIPTOR_RANGE samplerRange{};

    for (auto& sb : program->getActiveStorageBufferInfos())
    {
        if (sb.access == BufferAccess::READ_WRITE)
        {
            D3D12_DESCRIPTOR_RANGE& r           = uavRanges.emplace_back();
            r.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
            r.NumDescriptors                    = 1;
            r.BaseShaderRegister                = sb.binding;
            r.RegisterSpace                     = 1;
            r.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        }
        else
        {
            D3D12_DESCRIPTOR_RANGE& r           = srvRanges.emplace_back();
            r.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            r.NumDescriptors                    = 1;
            r.BaseShaderRegister                = sb.binding;
            r.RegisterSpace                     = 1;
            r.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        }
    }

    const auto& textures = program->getActiveTextureInfos();
    if (!textures.empty())
    {
        samplerRange.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        samplerRange.BaseShaderRegister                = 0;
        samplerRange.RegisterSpace                     = 1;
        samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        samplerRange.NumDescriptors                    = 22;

        D3D12_ROOT_PARAMETER& samplerParam               = rootParams.emplace_back();
        samplerParam.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        samplerParam.DescriptorTable.NumDescriptorRanges = 1;
        samplerParam.DescriptorTable.pDescriptorRanges   = &samplerRange;
        samplerParam.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
        _samplerRootIndex                                = rootIndex++;

        for (auto& [_, smp] : textures)
        {
            D3D12_DESCRIPTOR_RANGE& r           = srvRanges.emplace_back();
            r.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            r.NumDescriptors                    = smp->count;
            r.BaseShaderRegister                = smp->location;
            r.RegisterSpace                     = 1;
            r.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
        }

        uint32_t customSamplerCount = 0;
        for (const auto& smp : program->getActiveSamplerInfos())
        {
            if (smp.presetIndex < 0)
                customSamplerCount += smp.count;
        }

        if (customSamplerCount > 0)
        {
            customSamplerRanges.reserve(program->getActiveSamplerInfos().size());
            for (const auto& sampler : program->getActiveSamplerInfos())
            {
                if (sampler.presetIndex >= 0)
                    continue;
                auto& range                             = customSamplerRanges.emplace_back();
                range.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
                range.NumDescriptors                    = sampler.count;
                range.BaseShaderRegister                = sampler.binding;
                range.RegisterSpace                     = 2;
                range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
            }

            D3D12_ROOT_PARAMETER& customParam               = rootParams.emplace_back();
            customParam.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            customParam.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(customSamplerRanges.size());
            customParam.DescriptorTable.pDescriptorRanges   = customSamplerRanges.data();
            customParam.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
            _customSamplerRootIndex                         = rootIndex++;
        }
    }

    // The SRV table must be created even when the compute shader only uses
    // read-only storage buffers (no sampled textures).
    if (!srvRanges.empty())
    {
        D3D12_ROOT_PARAMETER& srvParam               = rootParams.emplace_back();
        srvParam.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        srvParam.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(srvRanges.size());
        srvParam.DescriptorTable.pDescriptorRanges   = srvRanges.data();
        srvParam.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
        _srvRootIndex                                = rootIndex++;
    }

    if (!uavRanges.empty())
    {
        D3D12_ROOT_PARAMETER& uavParam               = rootParams.emplace_back();
        uavParam.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        uavParam.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(uavRanges.size());
        uavParam.DescriptorTable.pDescriptorRanges   = uavRanges.data();
        uavParam.ShaderVisibility                    = D3D12_SHADER_VISIBILITY_ALL;
        _uavRootIndex                                = rootIndex++;
    }

    D3D12_ROOT_SIGNATURE_DESC rsDesc{};
    rsDesc.NumParameters = static_cast<UINT>(rootParams.size());
    rsDesc.pParameters   = rootParams.data();
    rsDesc.Flags         = D3D12_ROOT_SIGNATURE_FLAG_NONE;

    Microsoft::WRL::ComPtr<ID3DBlob> sigBlob, errBlob;
    HRESULT hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
    if (FAILED(hr) || !sigBlob)
    {
        const std::string_view detail =
            errBlob ? std::string_view(static_cast<const char*>(errBlob->GetBufferPointer()), errBlob->GetBufferSize())
                    : std::string_view("unknown serialization error");
        AXLOGE("Failed to serialize compute root signature, hr:{}, {}", hr, detail);
        AXASSERT(false, "Failed to serialize compute root signature");
        return false;
    }

    hr = _driver->getDevice()->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(),
                                                   IID_PPV_ARGS(&_rootSig));
    if (FAILED(hr) || !_rootSig)
    {
        AXLOGE("Failed to create compute root signature, hr:{}", hr);
        AXASSERT(false, "Failed to create compute root signature");
        return false;
    }

    uint32_t customSamplerCount = 0;
    for (const auto& smp : program->getActiveSamplerInfos())
    {
        if (smp.presetIndex < 0)
            customSamplerCount += smp.count;
    }
    _customSamplerBatches.initialize(_driver, customSamplerCount);
    return true;
}

const DescriptorHandle* ComputePipelineImpl::getCustomSamplerBatch(const ::ax::rhi::ProgramState* programState)
{
    return _customSamplerBatches.get(programState);
}

void ComputePipelineImpl::createPipeline(ProgramImpl* program)
{
    auto csBlob = program->getCSBlob();
    if (csBlob.empty())
        return;

    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.pRootSignature     = _rootSig.Get();
    psoDesc.CS.pShaderBytecode = csBlob.data();
    psoDesc.CS.BytecodeLength  = csBlob.size();

    HRESULT hr = _driver->getDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&_pso));
    AXASSERT(SUCCEEDED(hr), "Failed to create compute PSO");
}

}  // namespace ax::rhi::d3d12
