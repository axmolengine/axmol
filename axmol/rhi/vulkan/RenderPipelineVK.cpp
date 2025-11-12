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

// Generate a combined ID for pipeline caching.
// You can swap to your axstd::hash_bytes if already available.
// Generate a combined ID for pipeline caching using axstd::hash_bytes
static inline uintptr_t make_pipeline_key(const PipelineDesc& desc)
{
    uintptr_t h1 = axstd::hash_bytes(&desc.blendDesc, sizeof(desc.blendDesc), 0);
    uintptr_t h2 = axstd::hash_bytes(&desc.programState, sizeof(desc.programState), h1);
    uintptr_t h3 = axstd::hash_bytes(&desc.vertexLayout, sizeof(desc.vertexLayout), h2);
    return h3;
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

// --- Core: RenderPipelineImpl::update --------------------------------------

void RenderPipelineImpl::update(const RenderTarget* rt, const PipelineDesc& desc)
{
    // Validate inputs
    if (!rt || !desc.programState || !desc.vertexLayout)
    {
        // In Axmol, prefer a centralized error/assert macro
        // AXASSERT(false, "RenderPipelineImpl::update: invalid inputs");
        return;
    }

    // 1) Retrieve VkRenderPass from RenderTargetImpl (owned/cached by RenderTarget)
    // RenderTarget is expected to provide a Vulkan render pass handle.
    // If your concrete type differs, adjust the accessor below.
    auto* vkRT              = static_cast<const RenderTargetImpl*>(rt);
    VkRenderPass renderPass = vkRT->getVkRenderPass();  // provided by RenderTargetImpl

    auto programState = desc.programState;
    auto program      = static_cast<ProgramImpl*>(programState->getProgram());

    // 2) Build lightweight pipeline states (rebuild inline; no caching needed)

    // 2.1 Color blend state (single attachment typical; extend for MRT)
    _activeAttachment = makeVkBlendAttachment(desc.blendDesc);

    _activeBlendState                 = {};
    _activeBlendState.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    _activeBlendState.logicOpEnable   = VK_FALSE;
    _activeBlendState.attachmentCount = 1;
    _activeBlendState.pAttachments    = &_activeAttachment;
    // Write fixed blend constants (same as D3D11 blendColor = {0,0,0,0})
    _activeBlendState.blendConstants[0] = 0.0f;  // R
    _activeBlendState.blendConstants[1] = 0.0f;  // G
    _activeBlendState.blendConstants[2] = 0.0f;  // B
    _activeBlendState.blendConstants[3] = 0.0f;  // A

    // 2.2 Rasterizer state (default; extend with your RasterizerDesc)
    VkPipelineRasterizationStateCreateInfo raster{};
    raster.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster.depthClampEnable        = VK_FALSE;
    raster.rasterizerDiscardEnable = VK_FALSE;
    raster.polygonMode             = VK_POLYGON_MODE_FILL;
    raster.cullMode                = VK_CULL_MODE_BACK_BIT;
    raster.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;  // depends on your coordinate system
    raster.depthBiasEnable         = VK_FALSE;
    raster.lineWidth               = 1.0f;

    raster.depthBiasEnable         = VK_FALSE;
    raster.depthBiasConstantFactor = 0.0f;
    raster.depthBiasClamp          = 0.0f;
    raster.depthBiasSlopeFactor    = 0.0f;

    // 2.3 Multisample state (default; adjust from RenderTarget if MSAA enabled)
    VkPipelineMultisampleStateCreateInfo ms{};
    ms.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    ms.sampleShadingEnable  = VK_FALSE;

    // 2.4 Depth-stencil state (default off; enable if your pipeline needs it)
    auto& ds = _dsState->getVkDepthStencilState();

    // 2.5 Input assembly (topology; may come from VertexLayout or draw command)
    VkPipelineInputAssemblyStateCreateInfo ia{};
    ia.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ia.primitiveRestartEnable = VK_FALSE;

    // 2.6 Viewport/scissor (use dynamic states to avoid baking surface size)
    VkPipelineViewportStateCreateInfo vp{};
    vp.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.viewportCount = 1;
    vp.scissorCount  = 1;

    // 2.7 Dynamic states (viewport/scissor recommended)
    VkDynamicState dynamics[] = {VK_DYNAMIC_STATE_VIEWPORT,          VK_DYNAMIC_STATE_SCISSOR,
                                 VK_DYNAMIC_STATE_STENCIL_REFERENCE, VK_DYNAMIC_STATE_BLEND_CONSTANTS,
                                 VK_DYNAMIC_STATE_DEPTH_BIAS,        VK_DYNAMIC_STATE_CULL_MODE_EXT,
                                 VK_DYNAMIC_STATE_FRONT_FACE_EXT};
    VkPipelineDynamicStateCreateInfo dyn{};
    dyn.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dyn.dynamicStateCount = static_cast<uint32_t>(std::size(dynamics));
    dyn.pDynamicStates    = dynamics;

    // 2.8 Vertex input (translate from rhi::VertexLayout)
    auto& vi = _vertexLayout->getVkCreateInfo();

    // 3) Shader stages and pipeline layout (heavyweight; use caches)
    // Assumes ProgramState can provide VkShaderModule and reflection to build descriptor set layouts.
    // Replace the placeholders below with your actual adapter.
    uintptr_t progKey = (uintptr_t)program;

    // 3.1 Descriptor set layout (per program)
    std::fill(_activeDescriptorSetLayouts.begin(), _activeDescriptorSetLayouts.end(), VK_NULL_HANDLE);
    {
        auto it = _descriptorSetLayoutCache.find(progKey);
        if (it != _descriptorSetLayoutCache.end())
        {
            _activeDescriptorSetLayouts = it->second;
        }
        else
        {
            axstd::pod_vector<VkDescriptorSetLayoutBinding> ubBindings;
            axstd::pod_vector<VkDescriptorSetLayoutBinding> samplerBindings;

            // VS uniform blocks set=0, binding=0
            auto vs = program->getVertexShader();
            for (auto& ub : vs->getActiveUniformBlockInfos())
            {
                VkDescriptorSetLayoutBinding b{};
                b.binding            = ub.binding;  // Fixed binding for VS UBO
                b.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                b.descriptorCount    = 1;
                b.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
                b.pImmutableSamplers = nullptr;
                ubBindings.push_back(b);
            }

            // FS uniform blocks -> set=0, binding=1
            auto fs = program->getFragmentShader();
            for (auto& ub : fs->getActiveUniformBlockInfos())
            {
                VkDescriptorSetLayoutBinding b{};
                b.binding            = ub.binding;  // Fixed binding for FS UBO
                b.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                b.descriptorCount    = 1;
                b.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
                b.pImmutableSamplers = nullptr;
                ubBindings.push_back(b);
            }

            // FS samplers -> set=1, binding = keep source binding
            for (auto& smp : fs->getActiveSamplerInfos())
            {
                VkDescriptorSetLayoutBinding b{};
                b.binding            = smp->location;  // Keep binding from shader source
                b.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                b.descriptorCount    = 1;
                b.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;
                b.pImmutableSamplers = nullptr;
                samplerBindings.push_back(b);
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

            // Cache pipeline layout for this program
            _descriptorSetLayoutCache.emplace(progKey, _activeDescriptorSetLayouts);
        }
    }

    // 3.2 Pipeline layout (per program)
    {
        auto it = _pipelineLayoutCache.find(progKey);
        if (it != _pipelineLayoutCache.end())
        {
            _activePipelineLayout = it->second;
        }
        else
        {
            VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};
            VkPipelineLayoutCreateInfo plc{};
            plc.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            plc.setLayoutCount         = 2;
            plc.pSetLayouts            = &_activeDescriptorSetLayouts[0];
            plc.pushConstantRangeCount = 0;
            plc.pPushConstantRanges    = nullptr;
            auto result                = vkCreatePipelineLayout(_device, &plc, nullptr, &pipelineLayout);
            if (result != VK_SUCCESS)
                AXLOGE("vkCreatePipelineLayout fail: {}", (int)result);
            _pipelineLayoutCache.emplace(progKey, pipelineLayout);

            _activePipelineLayout = pipelineLayout;
        }
    }

    // 4) Final pipeline cache (combine program + vertex layout + blend, etc.)
    const uintptr_t pipelineKey = make_pipeline_key(desc);
    {
        auto it = _pipelineCache.find(pipelineKey);
        if (it != _pipelineCache.end())
        {
            // Already created — nothing else to do for pipeline creation.
            _activePipeline = it->second;
            return;
        }

        // 4.1 Shader stages (fill from program)
        // Replace with your own getters; below is illustrative.
        std::vector<VkPipelineShaderStageCreateInfo> stages;
        {
            if (auto vs = program->getVSModule())
            {
                VkPipelineShaderStageCreateInfo s{};
                s.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                s.stage  = VK_SHADER_STAGE_VERTEX_BIT;
                s.module = vs;
                s.pName  = "main";  // entry point
                stages.push_back(s);
            }
            // Example: fragment stage
            if (auto fs = program->getFSModule())
            {
                VkPipelineShaderStageCreateInfo s{};
                s.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                s.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
                s.module = fs;
                s.pName  = "main";
                stages.push_back(s);
            }
            // Add more stages as needed (geometry, tessellation).
        }

        // 4.2 Create graphics pipeline
        VkGraphicsPipelineCreateInfo gp{};
        gp.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        gp.stageCount          = static_cast<uint32_t>(stages.size());
        gp.pStages             = stages.data();
        gp.pVertexInputState   = &vi;
        gp.pInputAssemblyState = &ia;
        gp.pViewportState      = &vp;
        gp.pRasterizationState = &raster;
        gp.pMultisampleState   = &ms;
        gp.pDepthStencilState  = &ds;
        gp.pColorBlendState    = &_activeBlendState;
        gp.pDynamicState       = &dyn;
        gp.layout              = _activePipelineLayout;
        gp.renderPass          = renderPass;
        gp.subpass             = 0;  // first subpass

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

    // After this, the current pipeline for the key is cached.
    // The caller can fetch/bind it via your higher-level RenderPipeline interface.
}

}  // namespace ax::rhi::vk
