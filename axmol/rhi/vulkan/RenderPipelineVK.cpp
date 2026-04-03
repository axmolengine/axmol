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
static inline void toVkBlendAttachment(const BlendDesc& desc, VkPipelineColorBlendAttachmentState& outState)
{
    outState.colorWriteMask      = toVkColorMask(desc.writeMask);
    outState.blendEnable         = desc.blendEnabled ? VK_TRUE : VK_FALSE;
    outState.srcColorBlendFactor = toVkBlendFactor(desc.sourceRGBBlendFactor);
    outState.dstColorBlendFactor = toVkBlendFactor(desc.destinationRGBBlendFactor);
    outState.colorBlendOp        = toVkBlendOp(desc.rgbBlendOp);
    outState.srcAlphaBlendFactor = toVkBlendFactor(desc.sourceAlphaBlendFactor);
    outState.dstAlphaBlendFactor = toVkBlendFactor(desc.destinationAlphaBlendFactor);
    outState.alphaBlendOp        = toVkBlendOp(desc.alphaBlendOp);
}

/*
 * CLASS DescriptorPool
 */

void DescriptorPool::init(DescriptorAllocator* allocator, std::span<const VkDescriptorPoolSize> poolSizes)
{
    _allocator = allocator;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets       = DESCRIPTOR_POOL_MAX_SETS;

    auto ret = vkCreateDescriptorPool(_allocator->getDevice(), &poolInfo, nullptr, &_pool);
    VK_REQUIRE(ret, "vkCreateDescriptorPool failed");

    _freeSetCount = _maxSetCount = DESCRIPTOR_POOL_MAX_SETS;
    for (auto& poolSize : poolSizes)
    {
        if (poolSize.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            _freeUniformDescriptorCount = _maxUniformDescriptorCount = poolSize.descriptorCount;
        else if (poolSize.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            _freeSamplerDescriptorCount = _maxSamplerDescriptorCount = poolSize.descriptorCount;
    }
}

void DescriptorPool::dispose()
{
    if (_pool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(_allocator->getDevice(), _pool, nullptr);
        _pool = VK_NULL_HANDLE;
    }
    _freeSetCount = _maxSetCount = 0;
    _freeUniformDescriptorCount = _maxUniformDescriptorCount = 0;
    _freeSamplerDescriptorCount = _maxSamplerDescriptorCount = 0;
}

void DescriptorPool::allocateDescriptorSets(const PipelineLayoutState* layoutState, DescriptorState* descriptorState)
{
    // VK_REQUIRE_EXPR(canAllocate(layoutState), "The descriptor pool is full");
    VkDescriptorSetAllocateInfo ai{};
    ai.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    ai.descriptorPool     = _pool;
    ai.descriptorSetCount = layoutState->descriptorSetLayoutCount;
    ai.pSetLayouts        = layoutState->descriptorSetLayouts.data();

    VkResult result = vkAllocateDescriptorSets(_allocator->getDevice(), &ai, descriptorState->sets.data());
    VK_REQUIRE(result, "vkAllocateDescriptorSets failed");
    descriptorState->pool                   = this;
    descriptorState->uniformDescriptorCount = static_cast<uint16_t>(layoutState->uniformDescriptorCount);
    descriptorState->samplerDescriptorCount = static_cast<uint16_t>(layoutState->samplerDescriptorCount);

    _freeSetCount -= layoutState->descriptorSetLayoutCount;
    _freeUniformDescriptorCount -= layoutState->uniformDescriptorCount;
    if (layoutState->samplerDescriptorCount > 0)
        _freeSamplerDescriptorCount -= layoutState->samplerDescriptorCount;
}

void DescriptorPool::freeDescriptorSets(uint32_t descriptorSetCount, DescriptorState* descriptorState)
{
    VkResult result =
        vkFreeDescriptorSets(_allocator->getDevice(), _pool, descriptorSetCount, descriptorState->sets.data());
    VK_REQUIRE(result, "vkFreeDescriptorSets failed");
    _freeSetCount += descriptorSetCount;
    _freeUniformDescriptorCount += descriptorState->uniformDescriptorCount;
    _freeSamplerDescriptorCount += descriptorState->samplerDescriptorCount;

    descriptorState->uniformDescriptorCount = 0;
    descriptorState->samplerDescriptorCount = 0;
    descriptorState->progId                 = 0;
    descriptorState->pool                   = nullptr;
    descriptorState->sets.fill(VK_NULL_HANDLE);
}

/*
 * CLASS DescriptorAllocator
 */

void DescriptorAllocator::init(VkDevice device, std::span<const VkDescriptorPoolSize> poolSizes)
{
    _device = device;
    _poolSizes.insert(_poolSizes.end(), poolSizes.begin(), poolSizes.end());
    // pre create one pool
    spawnPool();
}

void DescriptorAllocator::dispose()
{
    for (auto pool : _pools)
    {
        pool->dispose();
        delete pool;
    }
    _pools.clear();
}

void DescriptorAllocator::allocateDescriptorSets(const PipelineLayoutState* layoutState,
                                                 DescriptorState* descriptorState)
{
    auto& pool = _pools.back();
    if (pool->canAllocate(layoutState))
        pool->allocateDescriptorSets(layoutState, descriptorState);
    else
        spawnPool()->allocateDescriptorSets(layoutState, descriptorState);
}

void DescriptorAllocator::freeDescriptorSets(DescriptorState* descriptorState)
{
    if (descriptorState->pool)
    {
#ifndef NDEBUG
        VK_REQUIRE_EXPR(descriptorState->pool->getAllocator() == this,
                        "DescriptorAllocator::freeDescriptorSets: invalid allocator");
        auto it = std::find(_pools.begin(), _pools.end(), descriptorState->pool);
        VK_REQUIRE_EXPR(it != _pools.end(), "DescriptorAllocator::freeDescriptorSets: pool not owned by allocator");
#endif
        descriptorState->pool->freeDescriptorSets(static_cast<uint32_t>(_poolSizes.size()), descriptorState);
    }
}

void DescriptorAllocator::sortPools()
{
    std::sort(_pools.begin(), _pools.end(),
              [](const DescriptorPool* a, const DescriptorPool* b) { return a->available() < b->available(); });
}

DescriptorPool* DescriptorAllocator::spawnPool()
{
    auto newPool = new DescriptorPool();
    newPool->init(this, _poolSizes);
    _pools.push_back(newPool);
    return newPool;
}

/*
 * CLASS RenderPipelineImpl
 */
RenderPipelineImpl::RenderPipelineImpl(DriverImpl* driver) : _driver(driver), _device(driver->getDevice())
{
    initializePipelineDefaults(driver);

    // For UBO only
    VkDescriptorPoolSize poolSizes1[] = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DESCRIPTOR_POOL_MAX_SETS * DESCRIPTOR_POOL_UNIFORM_MULTIPLIER},
    };
    _descriptorAllocator1.init(_device, poolSizes1);

    // For UBO + sampler
    constexpr VkDescriptorPoolSize poolSizes2[] = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DESCRIPTOR_POOL_MAX_SETS * DESCRIPTOR_POOL_UNIFORM_MULTIPLIER},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DESCRIPTOR_POOL_MAX_SETS * DESCRIPTOR_POOL_SAMPLER_MULTIPLIER},
    };
    _descriptorAllocator2.init(_device, poolSizes2);
}

RenderPipelineImpl::~RenderPipelineImpl()
{
    vkDeviceWaitIdle(_device);

    for (auto& [_, state] : _pipelineLayoutCache)
    {
        if (state.descriptorSetLayoutCount == 2)
            freeDescriptorStates(_descriptorAllocator2, state.descriptorFreeList, false);
        else
            freeDescriptorStates(_descriptorAllocator1, state.descriptorFreeList, false);

        if (state.layout)
            vkDestroyPipelineLayout(_device, state.layout, nullptr);

        auto& descSetLayouts = state.descriptorSetLayouts;
        if (descSetLayouts[SET_INDEX_UBO])
            vkDestroyDescriptorSetLayout(_device, descSetLayouts[SET_INDEX_UBO], nullptr);
        if (descSetLayouts[SET_INDEX_SAMPLER])
            vkDestroyDescriptorSetLayout(_device, descSetLayouts[SET_INDEX_SAMPLER], nullptr);
    }
    _pipelineLayoutCache.clear();

    _descriptorStatePool.purge();
    _descriptorAllocator1.dispose();
    _descriptorAllocator2.dispose();

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
        VK_DYNAMIC_STATE_BLEND_CONSTANTS};

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

    auto program  = static_cast<ProgramImpl*>(desc.programState->getProgram());
    _activeProgId = program->getProgramId();
    updateBlendState(desc.blendDesc, vkRT->getColorAttachmentCount());
    updatePipelineLayoutState(program);
    updateGraphicsPipeline(program, desc, state, renderPass);
}

void RenderPipelineImpl::updateBlendState(const BlendDesc& blendDesc, uint32_t colorAttachmentCount)
{
    _activeBlendAttachmentStates.clear();
    for (uint32_t i = 0; i < colorAttachmentCount; ++i)
    {
        auto& outState = _activeBlendAttachmentStates.emplace_back();
        toVkBlendAttachment(blendDesc, outState);
    }
    _activeBlendState                 = {};
    _activeBlendState.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    _activeBlendState.logicOpEnable   = VK_FALSE;
    _activeBlendState.attachmentCount = colorAttachmentCount;
    _activeBlendState.pAttachments    = _activeBlendAttachmentStates.data();
    std::fill(std::begin(_activeBlendState.blendConstants), std::end(_activeBlendState.blendConstants), 0.0f);
}

void RenderPipelineImpl::updatePipelineLayoutState(ProgramImpl* program)
{
    auto it = _pipelineLayoutCache.find(_activeProgId);
    if (it != _pipelineLayoutCache.end())
    {
        _activeLayoutState = &it->second;
        return;
    }

    tlx::pod_vector<VkDescriptorSetLayoutBinding> ubBindings;
    tlx::pod_vector<VkDescriptorSetLayoutBinding> samplerBindings;

    auto& state = _pipelineLayoutCache.emplace(_activeProgId, PipelineLayoutState{}).first->second;

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

        ++state.uniformDescriptorCount;
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

        state.samplerDescriptorCount += smp->count;
    }

    // Create DescriptorSetLayout for UBOs (set=0)
    VkDescriptorSetLayoutCreateInfo dsl0{};
    dsl0.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dsl0.bindingCount = static_cast<uint32_t>(ubBindings.size());
    dsl0.pBindings    = ubBindings.data();
    vkCreateDescriptorSetLayout(_device, &dsl0, nullptr, &state.descriptorSetLayouts[SET_INDEX_UBO]);

    if (!samplerBindings.empty())
    {
        // Create DescriptorSetLayout for samplers (set=1)

        state.descriptorSetLayoutCount = 2;

        VkDescriptorSetLayoutCreateInfo dsl1{};
        dsl1.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        dsl1.bindingCount = static_cast<uint32_t>(samplerBindings.size());
        dsl1.pBindings    = samplerBindings.data();
        auto vr = vkCreateDescriptorSetLayout(_device, &dsl1, nullptr, &state.descriptorSetLayouts[SET_INDEX_SAMPLER]);
        VK_REQUIRE(vr, "vkCreateDescriptorSetLayout failed");
    }
    else
        state.descriptorSetLayoutCount = 1;

    VkPipelineLayoutCreateInfo plc{};
    plc.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plc.setLayoutCount         = state.descriptorSetLayoutCount;
    plc.pSetLayouts            = state.descriptorSetLayouts.data();
    plc.pushConstantRangeCount = 0;
    plc.pPushConstantRanges    = nullptr;

    VkResult result = vkCreatePipelineLayout(_device, &plc, nullptr, &state.layout);
    VK_REQUIRE(result, "vkCreatePipelineLayout fail");
    _activeLayoutState = &state;
}

void RenderPipelineImpl::updateGraphicsPipeline(ProgramImpl* program,
                                                const PipelineDesc& desc,
                                                const ExtendedDynamicState& state,
                                                VkRenderPass renderPass)
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

    const uintptr_t pipelineId = makePipelineId(desc.blendDesc, _dsState, _activeProgId, renderPass,
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
    gp.layout              = _activeLayoutState->layout;
    gp.renderPass          = renderPass;
    gp.subpass             = 0;

    VkPipeline pipeline = VK_NULL_HANDLE;
    VkResult res        = vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &gp, nullptr, &pipeline);
    VK_REQUIRE(res, "vkCreateGraphicsPipelines fail");
    _renderPassToPipelineMap.emplace(renderPass, pipelineId);
    _pipelineCache.emplace(pipelineId, pipeline);
    _activePipeline = pipeline;
}

DescriptorState* RenderPipelineImpl::acquireDescriptorState()
{
    AXASSERT(_activeLayoutState, "PipelineLayoutState must be valid");
    DescriptorState* descriptorState{nullptr};
    // 1) Try to reuse a recycled allocation from the cache
    if (_activeLayoutState)
    {
        auto& freeList = _activeLayoutState->descriptorFreeList;
        if (!freeList.empty())
        {
            descriptorState = freeList.back();
            freeList.pop_back();
            return descriptorState;
        }
    }

    descriptorState         = _descriptorStatePool.construt();
    descriptorState->progId = _activeProgId;

    if (_activeLayoutState->descriptorSetLayoutCount == 2) [[likely]]
    {
        _descriptorAllocator2.allocateDescriptorSets(_activeLayoutState, descriptorState);
    }
    else
    {
        AXASSERT(_activeLayoutState->descriptorSetLayoutCount == 1, "DescriptorSetLayoutCount must be 1 or 2");
        _descriptorAllocator1.allocateDescriptorSets(_activeLayoutState, descriptorState);
    }
    return descriptorState;
}

void RenderPipelineImpl::recycleDescriptorStates(std::span<DescriptorState*> descriptorStates, bool needResort)
{
    int allocatorMods{0};
    for (auto descriptorState : descriptorStates)
    {
        // Find the cache entry for this layout state
        auto it = _pipelineLayoutCache.find(descriptorState->progId);
        if (it != _pipelineLayoutCache.end()) [[likely]]
        {
            // Push the allocation back into the free list for reuse directly
            auto& freeList = it->second.descriptorFreeList;
            freeList.push_back(descriptorState);
        }
        else
        {
            // means the relative program unloaded, just free to driver pool
            if (descriptorState->pool)
            {
                auto allocator = descriptorState->pool->getAllocator();
                allocator->freeDescriptorSets(descriptorState);

                if (allocator == &_descriptorAllocator2)
                    allocatorMods |= 2;
                else
                    allocatorMods |= 1;
            }

            _descriptorStatePool.destroy(descriptorState);
        }
    }

    if (needResort && allocatorMods) [[unlikely]]
    {
        if (allocatorMods & 2)
            _descriptorAllocator2.sortPools();
        if (allocatorMods & 1)
            _descriptorAllocator1.sortPools();
    }
}

void RenderPipelineImpl::removeCachedObjects(VkRenderPass key)
{
    auto range = _renderPassToPipelineMap.equal_range(key);

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

void RenderPipelineImpl::removeCachedObjects(Program* key)
{
    auto progId = key->getProgramId();

    _driver->waitForGPU();

    // remove layout state
    auto layoutIt = _pipelineLayoutCache.find(progId);
    if (layoutIt != _pipelineLayoutCache.end())
    {
        auto& layoutState = layoutIt->second;

        // remove descriptor sets cache
        if (layoutState.descriptorSetLayoutCount == 2) [[likely]]
            freeDescriptorStates(_descriptorAllocator2, layoutState.descriptorFreeList, true);
        else
            freeDescriptorStates(_descriptorAllocator1, layoutState.descriptorFreeList, true);

        if (layoutState.layout)
            vkDestroyPipelineLayout(_device, layoutState.layout, nullptr);

        auto& descSetLayouts = layoutState.descriptorSetLayouts;
        if (descSetLayouts[SET_INDEX_UBO])
            vkDestroyDescriptorSetLayout(_device, descSetLayouts[SET_INDEX_UBO], nullptr);
        if (descSetLayouts[SET_INDEX_SAMPLER])
            vkDestroyDescriptorSetLayout(_device, descSetLayouts[SET_INDEX_SAMPLER], nullptr);
        _pipelineLayoutCache.erase(layoutIt);
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

void RenderPipelineImpl::freeDescriptorStates(DescriptorAllocator& allocator,
                                              DescriptorList& descriptorStates,
                                              bool needResortPools)
{
    for (auto descriptorState : descriptorStates)
    {
        allocator.freeDescriptorSets(descriptorState);
        _descriptorStatePool.destroy(descriptorState);
    }
    descriptorStates.clear();
    if (needResortPools)
        allocator.sortPools();
}

}  // namespace ax::rhi::vk
