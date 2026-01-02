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
#include "axmol/rhi/vulkan/RenderPipelineVK.h"
#include "axmol/rhi/vulkan/RenderTargetVK.h"
#include "axmol/rhi/vulkan/DepthStencilStateVK.h"
#include "axmol/rhi/vulkan/VertexLayoutVK.h"
#include "axmol/rhi/vulkan/ProgramVK.h"
#include "axmol/rhi/vulkan/ShaderModuleVK.h"
#include "axmol/rhi/vulkan/DriverVK.h"
#include "axmol/rhi/vulkan/UtilsVK.h"
#include "axmol/tlx/hlookup.hpp"
#include "axmol/tlx/hash.hpp"
#include "axmol/tlx/inlined_vector.hpp"
#include <glad/vulkan.h>

namespace ax::rhi::vk
{

// --- Helpers: enum mapping and ID generation -------------------------------

static inline VkBlendOp toVkBlendOp(BlendOp op)
{
    switch (op)
    {
    case BlendOp::ADD:
        return VK_BLEND_OP_ADD;
    case BlendOp::SUBTRACT:
        return VK_BLEND_OP_SUBTRACT;
    case BlendOp::REVERSE_SUBTRACT:
        return VK_BLEND_OP_REVERSE_SUBTRACT;
    }
    return VK_BLEND_OP_ADD;
}

static inline VkBlendFactor toVkBlendFactor(BlendFactor f)
{
    switch (f)
    {
    case BlendFactor::ZERO:
        return VK_BLEND_FACTOR_ZERO;
    case BlendFactor::ONE:
        return VK_BLEND_FACTOR_ONE;
    case BlendFactor::SRC_COLOR:
        return VK_BLEND_FACTOR_SRC_COLOR;
    case BlendFactor::ONE_MINUS_SRC_COLOR:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case BlendFactor::SRC_ALPHA:
        return VK_BLEND_FACTOR_SRC_ALPHA;
    case BlendFactor::ONE_MINUS_SRC_ALPHA:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DST_COLOR:
        return VK_BLEND_FACTOR_DST_COLOR;
    case BlendFactor::ONE_MINUS_DST_COLOR:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case BlendFactor::DST_ALPHA:
        return VK_BLEND_FACTOR_DST_ALPHA;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case BlendFactor::CONSTANT_ALPHA:
        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case BlendFactor::SRC_ALPHA_SATURATE:
        return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    case BlendFactor::BLEND_COLOR:
        return VK_BLEND_FACTOR_CONSTANT_COLOR;  // maps to blend constant
    }
    return VK_BLEND_FACTOR_ONE;
}

static inline VkColorComponentFlags toVkColorMask(ColorWriteMask mask)
{
    VkColorComponentFlags flags = 0;
    // Mask is user-defined bitset; check each flag
    if ((static_cast<uint32_t>(mask) & static_cast<uint32_t>(ColorWriteMask::RED)) != 0)
        flags |= VK_COLOR_COMPONENT_R_BIT;
    if ((static_cast<uint32_t>(mask) & static_cast<uint32_t>(ColorWriteMask::GREEN)) != 0)
        flags |= VK_COLOR_COMPONENT_G_BIT;
    if ((static_cast<uint32_t>(mask) & static_cast<uint32_t>(ColorWriteMask::BLUE)) != 0)
        flags |= VK_COLOR_COMPONENT_B_BIT;
    if ((static_cast<uint32_t>(mask) & static_cast<uint32_t>(ColorWriteMask::ALPHA)) != 0)
        flags |= VK_COLOR_COMPONENT_A_BIT;
    return flags;
}

// Generate a unique key for pipeline caching.
//
// Notes:
// - _activePipelineLayout and _activeDescriptorSetLayouts are derived from the active program,
//   so they do not need to be hashed separately.
// - The pipeline cache key only needs to include states that directly affect pipeline creation:
//   blendDesc (fixed-function blend state), program (shader combination), vertexLayout,
//   and renderPass (framebuffer attachment formats).
//
// Important:
// - Axmol currently does not use dynamic rendering, so renderPass must be included in the hash
//   to ensure pipelines are correctly distinguished by their attachment formats.
// - If Axmol adopts VK_KHR_dynamic_rendering in the future, renderPass will no longer need to be
//   part of the key. In that case, attachment formats (color/depth/stencil) should be hashed instead.
//
// This design minimizes redundant PSOs while ensuring that any change in these critical states
// correctly triggers pipeline re-creation.
static inline uintptr_t makePipelineId(const rhi::BlendDesc& blendDesc,
                                       const DepthStencilStateImpl* dsState,
                                       uint64_t programId,
                                       VkRenderPass renderPass,
                                       uint32_t vlHash,
                                       uint32_t extendedDynState)
{
    struct HashMe
    {
        rhi::BlendDesc blend{};
        uintptr_t dsHash;
        uint64_t progId;
        uint64_t pass;
        uint32_t vlHash;
        uint32_t extendedDynState;
    };
    HashMe hashMe{.blend            = blendDesc,
                  .dsHash           = dsState->getHash(),
                  .progId           = programId,
                  .pass             = reinterpret_cast<uint64_t>(renderPass),
                  .vlHash           = vlHash,
                  .extendedDynState = extendedDynState};

    return tlx::hash_bytes(&hashMe, sizeof(hashMe), 0);
}

// Build the VkPipelineColorBlendAttachmentState from BlendDesc
static inline VkPipelineColorBlendAttachmentState makeVkBlendAttachment(const BlendDesc& desc)
{
    VkPipelineColorBlendAttachmentState att{};
    att.colorWriteMask      = toVkColorMask(desc.writeMask);
    att.blendEnable         = desc.blendEnabled ? VK_TRUE : VK_FALSE;
    att.srcColorBlendFactor = toVkBlendFactor(desc.sourceRGBBlendFactor);
    att.dstColorBlendFactor = toVkBlendFactor(desc.destinationRGBBlendFactor);
    att.colorBlendOp        = toVkBlendOp(desc.rgbBlendOp);
    att.srcAlphaBlendFactor = toVkBlendFactor(desc.sourceAlphaBlendFactor);
    att.dstAlphaBlendFactor = toVkBlendFactor(desc.destinationAlphaBlendFactor);
    att.alphaBlendOp        = toVkBlendOp(desc.alphaBlendOp);
    return att;
}

RenderPipelineImpl::RenderPipelineImpl(DriverImpl* driver) : _driver(driver), _device(driver->getDevice())
{
    initializePipelineDefaults(driver);
}

RenderPipelineImpl::~RenderPipelineImpl()
{
    vkDeviceWaitIdle(_device);

    for (auto& [_, res] : _pipelineLayoutCache)
    {
        if (res)
            vkDestroyPipelineLayout(_device, res, nullptr);
    }
    _pipelineLayoutCache.clear();

    for (auto& [_, state] : _descriptorLayoutCache)
    {
        auto& res = state.descriptorSetLayouts;
        if (res[SET_INDEX_UBO])
            vkDestroyDescriptorSetLayout(_device, res[SET_INDEX_UBO], nullptr);
        if (res[SET_INDEX_SAMPLER])
            vkDestroyDescriptorSetLayout(_device, res[SET_INDEX_SAMPLER], nullptr);
    }
    _descriptorLayoutCache.clear();

    for (auto pool : _descriptorPools)
        vkDestroyDescriptorPool(_device, pool, nullptr);
    _descriptorPools.clear();

    for (auto& [_, res] : _pipelineCache)
    {
        if (res)
            vkDestroyPipeline(_device, res, nullptr);
    }
    _pipelineCache.clear();
}

void RenderPipelineImpl::initializePipelineDefaults(DriverImpl* driver)
{
    // Input Assembly
    _iaState          = {};
    _iaState.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    _iaState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;  // need baked when extended dynamic state not supported
    _iaState.primitiveRestartEnable = VK_FALSE;

    // Viewport/Scissor
    _vpState               = {};
    _vpState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    _vpState.viewportCount = 1;
    _vpState.scissorCount  = 1;

    // Rasterizer
    _rasterState                         = {};
    _rasterState.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    _rasterState.depthClampEnable        = VK_FALSE;
    _rasterState.rasterizerDiscardEnable = VK_FALSE;
    _rasterState.polygonMode             = VK_POLYGON_MODE_FILL;
    _rasterState.cullMode  = VK_CULL_MODE_BACK_BIT;            // need baked when extended dynamic state not supported
    _rasterState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;  // need baked when extended dynamic state not supported
    _rasterState.lineWidth = 1.0f;

    // Multisample
    _msState                      = {};
    _msState.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    _msState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    _msState.sampleShadingEnable  = VK_FALSE;

    // Dynamic States
    static constexpr size_t MAX_DYNAMIC_STATES                                = 8;
    static tlx::inlined_vector<VkDynamicState, MAX_DYNAMIC_STATES> s_dynamics = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_STENCIL_REFERENCE,
        VK_DYNAMIC_STATE_BLEND_CONSTANTS, VK_DYNAMIC_STATE_DEPTH_BIAS};

    if (s_dynamics.size() < MAX_DYNAMIC_STATES && driver->isExtendedDynamicStateSupported())
    {
        s_dynamics.push_back(VK_DYNAMIC_STATE_CULL_MODE_EXT);
        s_dynamics.push_back(VK_DYNAMIC_STATE_FRONT_FACE_EXT);
        s_dynamics.push_back(VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT);
    }

    _dynState                   = {};
    _dynState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    _dynState.dynamicStateCount = static_cast<uint32_t>(s_dynamics.size());
    _dynState.pDynamicStates    = &s_dynamics[0];

    // preallocate 1 descriptor pool
    allocateDescriptorPool();
}

void RenderPipelineImpl::update(const RenderTarget* rt, const PipelineDesc& desc, const ExtendedDynamicState& state)
{
    // Validate inputs
    if (!rt || !desc.programState || !desc.vertexLayout)
    {
        AXASSERT(false, "RenderPipelineImpl::update: invalid inputs");
        return;
    }

    // 1) Retrieve VkRenderPass from RenderTargetImpl (owned/cached by RenderTarget)
    // RenderTarget is expected to provide a Vulkan render pass handle.
    // If your concrete type differs, adjust the accessor below.
    auto* vkRT              = static_cast<const RenderTargetImpl*>(rt);
    VkRenderPass renderPass = vkRT->getVkRenderPass();  // provided by RenderTargetImpl

    auto program = static_cast<ProgramImpl*>(desc.programState->getProgram());

    updateBlendState(desc.blendDesc);
    updateDescriptorSetLayouts(program);
    updatePipelineLayout(program);
    updateGraphicsPipeline(desc, state, renderPass, program);
}

void RenderPipelineImpl::updateBlendState(const BlendDesc& blendDesc)
{
    _activeAttachment                 = makeVkBlendAttachment(blendDesc);
    _activeBlendState                 = {};
    _activeBlendState.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    _activeBlendState.logicOpEnable   = VK_FALSE;
    _activeBlendState.attachmentCount = 1;
    _activeBlendState.pAttachments    = &_activeAttachment;
    std::fill(std::begin(_activeBlendState.blendConstants), std::end(_activeBlendState.blendConstants), 0.0f);
}

void RenderPipelineImpl::updateDescriptorSetLayouts(ProgramImpl* program)
{
    auto progId = program->getProgramId();
    auto it     = _descriptorLayoutCache.find(progId);
    if (it != _descriptorLayoutCache.end())
    {
        _activeDSL = &it->second;
        return;
    }

    tlx::pod_vector<VkDescriptorSetLayoutBinding> ubBindings;
    tlx::pod_vector<VkDescriptorSetLayoutBinding> samplerBindings;

    DescriptorSetLayoutState dslState{};

    for (auto& ub : program->getActiveUniformBlockInfos())
    {
        const auto stageFlags =
            ub.stage == ShaderStage::VERTEX ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
        VkDescriptorSetLayoutBinding& b = ubBindings.emplace_back();
        b.binding                       = ub.binding;
        b.descriptorType                = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        b.descriptorCount               = 1;
        b.stageFlags                    = stageFlags;
        b.pImmutableSamplers            = nullptr;

        ++dslState.uniformDescriptorCount;
    }

    // FS samplers -> set=1
    for (auto& [_, smp] : program->getActiveTextureInfos())
    {
        VkDescriptorSetLayoutBinding& b = samplerBindings.emplace_back();
        b.binding                       = smp->location;
        b.descriptorType                = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        b.descriptorCount               = smp->count;
        b.stageFlags                    = VK_SHADER_STAGE_FRAGMENT_BIT;
        b.pImmutableSamplers            = nullptr;

        dslState.samplerDescriptorCount += smp->count;
    }

    // Create DescriptorSetLayout for UBOs (set=0)
    VkDescriptorSetLayoutCreateInfo dsl0{};
    dsl0.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dsl0.bindingCount = static_cast<uint32_t>(ubBindings.size());
    dsl0.pBindings    = ubBindings.data();
    vkCreateDescriptorSetLayout(_device, &dsl0, nullptr, &dslState.descriptorSetLayouts[SET_INDEX_UBO]);

    dslState.descriptorSetLayoutCount = 1;
    if (!samplerBindings.empty())
    {
        // Create DescriptorSetLayout for samplers (set=1)
        VkDescriptorSetLayoutCreateInfo dsl1{};
        dsl1.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        dsl1.bindingCount = static_cast<uint32_t>(samplerBindings.size());
        dsl1.pBindings    = samplerBindings.data();
        auto vr =
            vkCreateDescriptorSetLayout(_device, &dsl1, nullptr, &dslState.descriptorSetLayouts[SET_INDEX_SAMPLER]);
        assert(vr == VK_SUCCESS);
        ++dslState.descriptorSetLayoutCount;
    }

    _activeDSL = &_descriptorLayoutCache.emplace(progId, dslState).first->second;
}

void RenderPipelineImpl::updatePipelineLayout(ProgramImpl* program)
{
    auto progId = program->getProgramId();
    auto it     = _pipelineLayoutCache.find(progId);
    if (it != _pipelineLayoutCache.end())
    {
        _activePipelineLayout = it->second;
        return;
    }

    VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};
    VkPipelineLayoutCreateInfo plc{};
    plc.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plc.setLayoutCount         = _activeDSL->descriptorSetLayoutCount;
    plc.pSetLayouts            = _activeDSL->descriptorSetLayouts.data();
    plc.pushConstantRangeCount = 0;
    plc.pPushConstantRanges    = nullptr;

    VkResult result = vkCreatePipelineLayout(_device, &plc, nullptr, &pipelineLayout);
    VK_VERIFY_RESULT(result, "vkCreatePipelineLayout fail");
    _pipelineLayoutCache.emplace(progId, pipelineLayout);
    _activePipelineLayout = pipelineLayout;
}

void RenderPipelineImpl::updateGraphicsPipeline(const PipelineDesc& desc,
                                                const ExtendedDynamicState& state,
                                                VkRenderPass renderPass,
                                                ProgramImpl* program)
{
    static_assert(sizeof(state) == 4, "ExtendedDynamicState size must be 4 bytes");

    uint32_t extendedDynState;
    if (!_driver->isExtendedDynamicStateSupported())
    {  // bake all
        _rasterState.cullMode  = state.cullMode;
        _rasterState.frontFace = state.frontFace;
        _iaState.topology      = state.primitiveTopology;
        extendedDynState       = std::bit_cast<uint32_t>(state);
    }
    else if (!_driver->isDynamicPrimitiveTopologyUnrestricted())
    {
        // bake topology only
        _iaState.topology = state.primitiveTopology;
        extendedDynState  = state.primitiveTopology;
    }
    else
        extendedDynState = 0;  // all dynamic

    const uintptr_t pipelineId = makePipelineId(desc.blendDesc, _dsState, program->getProgramId(), renderPass,
                                                desc.vertexLayout->getHash(), extendedDynState);
    auto it                    = _pipelineCache.find(pipelineId);
    if (it != _pipelineCache.end())
    {
        _activePipeline = it->second;
        return;
    }

    // Shader stages
    tlx::pod_vector<VkPipelineShaderStageCreateInfo> stages;
    {
        VkPipelineShaderStageCreateInfo& s = stages.emplace_back();
        s.sType                            = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        s.stage                            = VK_SHADER_STAGE_VERTEX_BIT;
        s.module                           = program->getNativeVSModule();
        s.pName                            = "main";
    }

    {
        VkPipelineShaderStageCreateInfo& s = stages.emplace_back();
        s.sType                            = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        s.stage                            = VK_SHADER_STAGE_FRAGMENT_BIT;
        s.module                           = program->getNativeFSModule();
        s.pName                            = "main";
    }

    auto& viState = static_cast<VertexLayoutImpl*>(desc.vertexLayout)->getVkCreateInfo();
    VkGraphicsPipelineCreateInfo gp{};
    gp.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    gp.stageCount          = static_cast<uint32_t>(stages.size());
    gp.pStages             = stages.data();
    gp.pVertexInputState   = &viState;
    gp.pInputAssemblyState = &_iaState;
    gp.pViewportState      = &_vpState;
    gp.pRasterizationState = &_rasterState;
    gp.pMultisampleState   = &_msState;
    gp.pDepthStencilState  = &_dsState->getVkDepthStencilState();
    gp.pColorBlendState    = &_activeBlendState;
    gp.pDynamicState       = &_dynState;
    gp.layout              = _activePipelineLayout;
    gp.renderPass          = renderPass;
    gp.subpass             = 0;

    VkPipeline pipeline = VK_NULL_HANDLE;
    VkResult res        = vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &gp, nullptr, &pipeline);
    VK_VERIFY_RESULT(res, "vkCreateGraphicsPipelines fail");
    _renderPassToPipelineMap.emplace(renderPass, pipelineId);
    _pipelineCache.emplace(pipelineId, pipeline);
    _activePipeline = pipeline;
}

bool RenderPipelineImpl::acquireDescriptorState(DescriptorState& state, int frameIndex)
{
    AXASSERT(_activeDSL, "DescriptorSetState must be valid");
    auto* dss = _activeDSL;

    // 1) Try to reuse a recycled allocation from the cache
    auto it = _descriptorCache.find(_activePipelineLayout);
    if (it != _descriptorCache.end())
    {
        auto& pool     = it->second;
        auto& freeList = pool[frameIndex];
        if (!freeList.empty())
        {
            state = freeList.back();
            freeList.pop_back();

            assert(state.frameIndex == -1);
            state.frameIndex = frameIndex;
            return true;
        }
    }

    auto& outSets     = state.sets;
    state.frameIndex  = frameIndex;
    state.ownerLayout = _activePipelineLayout;

    VkDescriptorSetAllocateInfo ai{};
    ai.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    ai.descriptorPool     = _descriptorPools.back();
    ai.descriptorSetCount = dss->descriptorSetLayoutCount;
    ai.pSetLayouts        = dss->descriptorSetLayouts.data();

    VkResult vr = vkAllocateDescriptorSets(_device, &ai, outSets.data());
    if (vr == VK_SUCCESS)
        return true;

    // 3) If the pool is exhausted or fragmented, create a new pool and retry
    if (vr == VK_ERROR_OUT_OF_POOL_MEMORY || vr == VK_ERROR_FRAGMENTED_POOL)
    {
        ai.descriptorPool = allocateDescriptorPool();
        vr                = vkAllocateDescriptorSets(_device, &ai, outSets.data());
        return vr == VK_SUCCESS;
    }

    AXLOGE("vkAllocateDescriptorSets failed: {}", (int)vr);
    return false;
}

void RenderPipelineImpl::recycleDescriptorState(DescriptorState& state)
{
    // Find the cache entry for this layout state
    auto it = _descriptorCache.find(state.ownerLayout);
    if (it == _descriptorCache.end())
    {
        AXLOGD("DescriptorSetCache miss: no pool found for pipelineLayout=0x{:016x}, creating new pool",
               reinterpret_cast<uint64_t>(state.ownerLayout));
        it = _descriptorCache.emplace(state.ownerLayout, DescriptorPool()).first;
    }

    // Push the allocation back into the free list for the given frame index
    auto& pool       = it->second;
    auto& freeList   = pool[state.frameIndex];
    state.frameIndex = -1;  // mark it's free to reuse
    freeList.push_back(state);
}

VkDescriptorPool RenderPipelineImpl::allocateDescriptorPool()
{
    constexpr VkDescriptorPoolSize poolSizes[] = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, RenderPipelineImpl::DEFAULT_DESCRIPTOR_POOL_UNIFORM_COUNT},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, RenderPipelineImpl::DEFAULT_DESCRIPTOR_POOL_SAMPLER_COUNT},
        /*{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 32},*/  // SSBO, unused currently
    };

    VkDescriptorPoolCreateInfo pci{};
    pci.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pci.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pci.maxSets       = DEFAULT_DESCRIPTOR_POOL_MAX_SETS;
    pci.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
    pci.pPoolSizes    = poolSizes;

    VkDescriptorPool pool = VK_NULL_HANDLE;
    VkResult vr           = vkCreateDescriptorPool(_device, &pci, nullptr, &pool);
    VK_VERIFY_RESULT(vr, "Failed to create descriptor pool");

    _descriptorPools.push_back(pool);

    return pool;
}

void RenderPipelineImpl::removeCachedPSOsByRenderPass(VkRenderPass rp)
{
    auto range = _renderPassToPipelineMap.equal_range(rp);

    if (range.first != range.second)
    {
        for (auto it = range.first; it != range.second; ++it)
        {
            auto pipelineKey = it->second;
            auto pipelineIt  = _pipelineCache.find(pipelineKey);
            if (pipelineIt != _pipelineCache.end())
            {
                vkDestroyPipeline(_device, pipelineIt->second, nullptr);
                _pipelineCache.erase(pipelineIt);
            }
        }
        _renderPassToPipelineMap.erase(range.first, range.second);
    }
}

void RenderPipelineImpl::removeCachedObjectsByProgram(Program* program)
{
    auto progId = program->getProgramId();

    _driver->waitForGPU();

    // remove descriptor set layouts
    auto dslIt = _descriptorLayoutCache.find(progId);
    if (dslIt != _descriptorLayoutCache.end())
    {
        auto& res = dslIt->second.descriptorSetLayouts;
        if (res[SET_INDEX_UBO])
            vkDestroyDescriptorSetLayout(_device, res[SET_INDEX_UBO], nullptr);
        if (res[SET_INDEX_SAMPLER])
            vkDestroyDescriptorSetLayout(_device, res[SET_INDEX_SAMPLER], nullptr);
        _descriptorLayoutCache.erase(dslIt);
    }

    // remove pipeline layout
    auto plIt = _pipelineLayoutCache.find(progId);
    if (plIt != _pipelineLayoutCache.end())
    {
        vkDestroyPipelineLayout(_device, plIt->second, nullptr);
        _pipelineLayoutCache.erase(plIt);
    }

    // remove pipeline(s)
    auto range = _programToPipelineMap.equal_range(progId);
    for (auto it = range.first; it != range.second; ++it)
    {
        auto pipelineKey = it->second;
        auto pipelineIt  = _pipelineCache.find(pipelineKey);
        if (pipelineIt != _pipelineCache.end())
        {
            vkDestroyPipeline(_device, pipelineIt->second, nullptr);
            _pipelineCache.erase(pipelineIt);
        }
    }
    _programToPipelineMap.erase(range.first, range.second);
}

/**
 * @brief Updates input assembly state for dynamic primitive type handling
 * Axmol engine uses dynamic primitive types which provides flexibility for most rendering scenarios.
 * Current limitation: LINE_LOOP primitive type is not supported in the dynamic implementation.
 * This implementation covers the majority of use cases efficiently. If LINE_LOOP support is required
 * in the future:
 * Uncomment and implement this function
 * Call it at appropriate locations in the rendering pipeline
 * Include primitive type in pipeline key generation to ensure proper state management
 * The dynamic approach balances performance and flexibility while maintaining compatibility
 * with modern graphics APIs.
 */
// void RenderPipelineImpl::updateInputAssemblyState(PrimitiveType primType)
//{
//     switch (primType)
//     {
//     case PrimitiveType::POINT:
//         _iaState.topology               = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
//         _iaState.primitiveRestartEnable = VK_FALSE;
//         break;
//     case PrimitiveType::LINE:
//         _iaState.topology               = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
//         _iaState.primitiveRestartEnable = VK_FALSE;
//         break;
//     case PrimitiveType::LINE_LOOP:
//         _iaState.topology               = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
//         _iaState.primitiveRestartEnable = VK_TRUE;  // simulate restart index loop
//         break;
//     case PrimitiveType::LINE_STRIP:
//         _iaState.topology               = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
//         _iaState.primitiveRestartEnable = VK_FALSE;
//         break;
//     case PrimitiveType::TRIANGLE:
//         _iaState.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//         _iaState.primitiveRestartEnable = VK_FALSE;
//         break;
//     case PrimitiveType::TRIANGLE_STRIP:
//         _iaState.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
//         _iaState.primitiveRestartEnable = VK_FALSE;
//         break;
//     default:
//         _iaState.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//         _iaState.primitiveRestartEnable = VK_FALSE;
//         break;
//     }
// }

}  // namespace ax::rhi::vk
