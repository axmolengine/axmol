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
#include "axmol/rhi/ProgramState.h"
#include "axmol/tlx/hlookup.hpp"
#include "axmol/tlx/hash.hpp"
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
static inline uintptr_t makePipelineKey(const rhi::BlendDesc& blendDesc,
                                        void* program,
                                        void* vertexLayout,
                                        void* renderPass)
{
    struct HashMe
    {
        rhi::BlendDesc blend{};
        void* prog;
        void* vl;
        void* pass;
    };
    HashMe hashMe{.blend = blendDesc, .prog = program, .vl = vertexLayout, .pass = renderPass};

    return axstd::hash_bytes(&hashMe, sizeof(hashMe), 0);
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

RenderPipelineImpl::RenderPipelineImpl(VkDevice device) : _device(device)
{
    initializePipelineDefaults();
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

    for (auto& [_, res] : _descriptorSetLayoutCache)
    {
        if (res[0])
            vkDestroyDescriptorSetLayout(_device, res[0], nullptr);
        if (res[1])
            vkDestroyDescriptorSetLayout(_device, res[1], nullptr);
    }
    _descriptorSetLayoutCache.clear();

    for (auto& [_, res] : _pipelineCache)
    {
        if (res)
            vkDestroyPipeline(_device, res, nullptr);
    }
    _pipelineCache.clear();
}

void RenderPipelineImpl::initializePipelineDefaults()
{
    // Input Assembly
    _iaState                        = {};
    _iaState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    _iaState.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
    _rasterState.cullMode                = VK_CULL_MODE_BACK_BIT;
    _rasterState.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    _rasterState.lineWidth               = 1.0f;

    // Multisample
    _msState                      = {};
    _msState.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    _msState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    _msState.sampleShadingEnable  = VK_FALSE;

    // Dynamic States
    static VkDynamicState dynamics[] = {VK_DYNAMIC_STATE_VIEWPORT,          VK_DYNAMIC_STATE_SCISSOR,
                                        VK_DYNAMIC_STATE_STENCIL_REFERENCE, VK_DYNAMIC_STATE_BLEND_CONSTANTS,
                                        VK_DYNAMIC_STATE_DEPTH_BIAS,        VK_DYNAMIC_STATE_CULL_MODE_EXT,
                                        VK_DYNAMIC_STATE_FRONT_FACE_EXT,    VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT};
    _dynState                        = {};
    _dynState.sType                  = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    _dynState.dynamicStateCount      = static_cast<uint32_t>(std::size(dynamics));
    _dynState.pDynamicStates         = dynamics;
}

void RenderPipelineImpl::update(const RenderTarget* rt, const PipelineDesc& desc)
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
    updateGraphicsPipeline(desc, renderPass, program);
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
    uintptr_t progKey = (uintptr_t)program;
    std::fill(_activeDescriptorSetLayouts.begin(), _activeDescriptorSetLayouts.end(), VK_NULL_HANDLE);

    auto it = _descriptorSetLayoutCache.find(progKey);
    if (it != _descriptorSetLayoutCache.end())
    {
        _activeDescriptorSetLayouts = it->second;
        return;
    }

    axstd::pod_vector<VkDescriptorSetLayoutBinding> ubBindings;
    axstd::pod_vector<VkDescriptorSetLayoutBinding> samplerBindings;

    // VS uniform blocks -> set=0
    auto vs = program->getVertexShader();
    for (auto& ub : vs->getActiveUniformBlockInfos())
    {
        VkDescriptorSetLayoutBinding& b = ubBindings.emplace_back();
        b.binding                       = ub.binding;
        b.descriptorType                = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        b.descriptorCount               = 1;
        b.stageFlags                    = VK_SHADER_STAGE_VERTEX_BIT;
        b.pImmutableSamplers            = nullptr;
    }

    // FS uniform blocks -> set=0
    auto fs = program->getFragmentShader();
    for (auto& ub : fs->getActiveUniformBlockInfos())
    {
        VkDescriptorSetLayoutBinding& b = ubBindings.emplace_back();
        b.binding                       = ub.binding;
        b.descriptorType                = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        b.descriptorCount               = 1;
        b.stageFlags                    = VK_SHADER_STAGE_FRAGMENT_BIT;
        b.pImmutableSamplers            = nullptr;
    }

    // FS samplers -> set=1
    for (auto& smp : fs->getActiveSamplerInfos())
    {
        VkDescriptorSetLayoutBinding& b = samplerBindings.emplace_back();
        b.binding                       = smp->location;
        b.descriptorType                = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        b.descriptorCount               = 1;
        b.stageFlags                    = VK_SHADER_STAGE_FRAGMENT_BIT;
        b.pImmutableSamplers            = nullptr;
    }

    // Create DescriptorSetLayout for UBOs (set=0)
    VkDescriptorSetLayoutCreateInfo dsl0{};
    dsl0.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dsl0.bindingCount = static_cast<uint32_t>(ubBindings.size());
    dsl0.pBindings    = ubBindings.data();
    vkCreateDescriptorSetLayout(_device, &dsl0, nullptr, &_activeDescriptorSetLayouts[0]);

    // Create DescriptorSetLayout for samplers (set=1)
    VkDescriptorSetLayoutCreateInfo dsl1{};
    dsl1.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dsl1.bindingCount = static_cast<uint32_t>(samplerBindings.size());
    dsl1.pBindings    = samplerBindings.data();
    vkCreateDescriptorSetLayout(_device, &dsl1, nullptr, &_activeDescriptorSetLayouts[1]);

    _descriptorSetLayoutCache.emplace(progKey, _activeDescriptorSetLayouts);
}

void RenderPipelineImpl::updatePipelineLayout(ProgramImpl* program)
{
    uintptr_t progKey = (uintptr_t)program;
    auto it           = _pipelineLayoutCache.find(progKey);
    if (it != _pipelineLayoutCache.end())
    {
        _activePipelineLayout = it->second;
        return;
    }

    VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};
    VkPipelineLayoutCreateInfo plc{};
    plc.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plc.setLayoutCount         = static_cast<uint32_t>(_activeDescriptorSetLayouts.size());
    plc.pSetLayouts            = _activeDescriptorSetLayouts.data();
    plc.pushConstantRangeCount = 0;
    plc.pPushConstantRanges    = nullptr;

    VkResult result = vkCreatePipelineLayout(_device, &plc, nullptr, &pipelineLayout);
    if (result == VK_SUCCESS)
    {
        _pipelineLayoutCache.emplace(progKey, pipelineLayout);
        _activePipelineLayout = pipelineLayout;
    }
    else
    {
        AXLOGE("vkCreatePipelineLayout fail: {}", (int)result);
    }
}

void RenderPipelineImpl::updateGraphicsPipeline(const PipelineDesc& desc, VkRenderPass renderPass, ProgramImpl* program)
{
    const uintptr_t pipelineKey = makePipelineKey(desc.blendDesc, program, desc.vertexLayout, renderPass);
    auto it                     = _pipelineCache.find(pipelineKey);
    if (it != _pipelineCache.end())
    {
        _activePipeline = it->second;
        return;
    }

    // Shader stages
    axstd::pod_vector<VkPipelineShaderStageCreateInfo> stages;
    if (auto vs = program->getVSModule())
    {
        VkPipelineShaderStageCreateInfo& s = stages.emplace_back();
        s.sType                            = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        s.stage                            = VK_SHADER_STAGE_VERTEX_BIT;
        s.module                           = vs;
        s.pName                            = "main";
    }
    if (auto fs = program->getFSModule())
    {
        VkPipelineShaderStageCreateInfo& s = stages.emplace_back();
        s.sType                            = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        s.stage                            = VK_SHADER_STAGE_FRAGMENT_BIT;
        s.module                           = fs;
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
    if (res == VK_SUCCESS)
    {
        _activePipeline = pipeline;
        _pipelineCache.emplace(pipelineKey, pipeline);
    }
    else
    {
        AXLOGE("vkCreateGraphicsPipelines fail: {}", (int)res);
    }
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
