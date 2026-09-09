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
#include "axmol/rhi/vulkan/ComputePipelineVK.h"
#include "axmol/rhi/vulkan/GraphicsDeviceVK.h"
#include "axmol/rhi/vulkan/ProgramVK.h"
#include "axmol/rhi/vulkan/ShaderModuleVK.h"
#include "axmol/rhi/vulkan/UtilsVK.h"
#include "axmol/base/Logging.h"
#include <glad/vulkan.h>

namespace ax::rhi::vk
{

ComputePipelineImpl::ComputePipelineImpl(GraphicsDeviceImpl* driver, ProgramImpl* program)
    : _driver(driver), _device(driver->getDevice())
{
    setProgram(program);
    createLayout(program);
    createPipeline(program);
}

ComputePipelineImpl::~ComputePipelineImpl()
{
    if (_pipeline != VK_NULL_HANDLE)
        vkDestroyPipeline(_device, _pipeline, nullptr);

    _descriptorAllocator.dispose();

    if (_layoutState.layout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(_device, _layoutState.layout, nullptr);
    for (auto& dsl : _layoutState.descriptorSetLayouts)
    {
        if (dsl != VK_NULL_HANDLE)
            vkDestroyDescriptorSetLayout(_device, dsl, nullptr);
    }
}

void ComputePipelineImpl::createLayout(ProgramImpl* program)
{
    tlx::pod_vector<VkDescriptorSetLayoutBinding> ubBindings;
    tlx::pod_vector<VkDescriptorSetLayoutBinding> resourceBindings;
    tlx::pod_vector<VkDescriptorSetLayoutBinding> customSamplerBindings;

    for (auto& ub : program->getActiveUniformBlockInfos())
    {
        VkDescriptorSetLayoutBinding& b = ubBindings.emplace_back();
        b.binding                       = ub.binding;
        b.descriptorType                = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        b.descriptorCount               = 1;
        b.stageFlags                    = VK_SHADER_STAGE_COMPUTE_BIT;
        b.pImmutableSamplers            = nullptr;
        ++_layoutState.uniformDescriptorCount;
    }

    // Storage buffers share the unified logical resource slot sequence with sampled images (set 1).
    for (auto& sb : program->getActiveStorageBufferInfos())
    {
        VkDescriptorSetLayoutBinding& b = resourceBindings.emplace_back();
        b.binding                       = sb.binding;
        b.descriptorType                = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        b.descriptorCount               = 1;
        b.stageFlags                    = VK_SHADER_STAGE_COMPUTE_BIT;
        b.pImmutableSamplers            = nullptr;
        ++_layoutState.storageDescriptorCount;
    }

    const bool separateSamplers = !program->getActiveSamplerInfos().empty();

    for (auto& [_, smp] : program->getActiveTextureInfos())
    {
        VkDescriptorSetLayoutBinding& b = resourceBindings.emplace_back();
        b.binding                       = smp->location;
        b.descriptorType =
            separateSamplers ? VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        b.descriptorCount    = smp->count;
        b.stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT;
        b.pImmutableSamplers = nullptr;

        if (separateSamplers)
            _layoutState.imageDescriptorCount += smp->count;
        else
            _layoutState.combinedDescriptorCount += smp->count;
    }

    if (separateSamplers)
    {
        for (const auto& smp : program->getActiveSamplerInfos())
        {
            if (!smp.samplerId)
                continue;

            if (smp.presetIndex >= 0)
            {
                AXASSERT(smp.space == axslc::kPresetSamplerDescriptorSet, "Preset sampler must use descriptor set 1");
                AXASSERT(smp.binding >= axslc::kVulkanSamplerBindingShift,
                         "Preset sampler binding must include the sampler binding shift");

                VkDescriptorSetLayoutBinding& b = resourceBindings.emplace_back();
                b.binding                       = smp.binding;
                b.descriptorType                = VK_DESCRIPTOR_TYPE_SAMPLER;
                b.descriptorCount               = smp.count;
                b.stageFlags                    = VK_SHADER_STAGE_COMPUTE_BIT;
                b.pImmutableSamplers            = nullptr;
                _layoutState.samplerDescriptorCount += smp.count;
            }
            else
            {
                AXASSERT(smp.binding >= axslc::kVulkanSamplerBindingShift,
                         "Custom sampler binding must include the sampler binding shift");

                VkDescriptorSetLayoutBinding& b = customSamplerBindings.emplace_back();
                b.binding                       = smp.binding;
                b.descriptorType                = VK_DESCRIPTOR_TYPE_SAMPLER;
                b.descriptorCount               = smp.count;
                b.stageFlags                    = VK_SHADER_STAGE_COMPUTE_BIT;
                b.pImmutableSamplers            = nullptr;
                _layoutState.samplerDescriptorCount += smp.count;
            }
        }
    }

    if (!customSamplerBindings.empty())
    {
        VkDescriptorSetLayoutCreateInfo dslCustom{};
        dslCustom.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        dslCustom.bindingCount = static_cast<uint32_t>(customSamplerBindings.size());
        dslCustom.pBindings    = customSamplerBindings.data();
        auto vr                = vkCreateDescriptorSetLayout(_device, &dslCustom, nullptr,
                                                             &_layoutState.descriptorSetLayouts[SET_INDEX_CUSTOM_SAMPLER]);
        VK_REQUIRE(vr, "vkCreateDescriptorSetLayout for compute custom samplers (set 2) failed");
    }

    VkDescriptorSetLayoutCreateInfo dsl0{};
    dsl0.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dsl0.bindingCount = static_cast<uint32_t>(ubBindings.size());
    dsl0.pBindings    = ubBindings.data();
    vkCreateDescriptorSetLayout(_device, &dsl0, nullptr, &_layoutState.descriptorSetLayouts[SET_INDEX_UBO]);

    if (!resourceBindings.empty())
    {
        VkDescriptorSetLayoutCreateInfo dsl1{};
        dsl1.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        dsl1.bindingCount = static_cast<uint32_t>(resourceBindings.size());
        dsl1.pBindings    = resourceBindings.data();
        auto vr           = vkCreateDescriptorSetLayout(_device, &dsl1, nullptr,
                                                        &_layoutState.descriptorSetLayouts[SET_INDEX_RESOURCE]);
        VK_REQUIRE(vr, "vkCreateDescriptorSetLayout for compute resources (set 1) failed");
    }

    const bool hasCustomSet               = !customSamplerBindings.empty();
    _layoutState.descriptorSetLayoutCount = hasCustomSet ? 3u : !resourceBindings.empty() ? 2u : 1u;

    VkPipelineLayoutCreateInfo plc{};
    plc.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plc.setLayoutCount         = _layoutState.descriptorSetLayoutCount;
    plc.pSetLayouts            = _layoutState.descriptorSetLayouts.data();
    plc.pushConstantRangeCount = 0;
    plc.pPushConstantRanges    = nullptr;

    VkResult result = vkCreatePipelineLayout(_device, &plc, nullptr, &_layoutState.layout);
    VK_REQUIRE(result, "vkCreatePipelineLayout for compute failed");

    // Compute descriptor pool: UBO + storage + sampled image + sampler.
    constexpr VkDescriptorPoolSize poolSizes[] = {
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DESCRIPTOR_POOL_MAX_SETS * DESCRIPTOR_POOL_UNIFORM_MULTIPLIER},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, DESCRIPTOR_POOL_MAX_SETS * DESCRIPTOR_POOL_SAMPLER_MULTIPLIER},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, DESCRIPTOR_POOL_MAX_SETS * DESCRIPTOR_POOL_SAMPLER_MULTIPLIER},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DESCRIPTOR_POOL_MAX_SETS * DESCRIPTOR_POOL_SAMPLER_MULTIPLIER},
        {VK_DESCRIPTOR_TYPE_SAMPLER, DESCRIPTOR_POOL_MAX_SETS * DESCRIPTOR_POOL_SAMPLER_MULTIPLIER},
    };
    _descriptorAllocator.init(_device, poolSizes);
}

void ComputePipelineImpl::createPipeline(ProgramImpl* program)
{
    auto csModule = static_cast<ShaderModuleImpl*>(program->getCSModule());
    if (!csModule)
        return;

    VkShaderModule csShader = csModule->internalHandle();
    if (csShader == VK_NULL_HANDLE)
        return;

    VkPipelineShaderStageCreateInfo stage{};
    stage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
    stage.module = csShader;
    stage.pName  = "main";

    VkComputePipelineCreateInfo info{};
    info.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    info.stage  = stage;
    info.layout = _layoutState.layout;

    VkResult result = vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &info, nullptr, &_pipeline);
    VK_REQUIRE(result, "vkCreateComputePipelines failed");
}

DescriptorState* ComputePipelineImpl::acquireDescriptorState()
{
    if (!_layoutState.descriptorFreeList.empty())
    {
        auto descriptorState = _layoutState.descriptorFreeList.back();
        _layoutState.descriptorFreeList.pop_back();
        return descriptorState;
    }

    auto descriptorState             = _descriptorStatePool.construt();
    descriptorState->progId          = getProgram()->getProgramId();
    descriptorState->computePipeline = this;
    _descriptorAllocator.allocateDescriptorSets(&_layoutState, descriptorState);
    return descriptorState;
}

void ComputePipelineImpl::recycleDescriptorState(DescriptorState* descriptorState)
{
    if (!descriptorState)
        return;
    if (descriptorState->computePipeline == this)
    {
        _layoutState.descriptorFreeList.push_back(descriptorState);
        return;
    }
    if (descriptorState->pool)
        descriptorState->pool->getAllocator()->freeDescriptorSets(descriptorState);
    _descriptorStatePool.destroy(descriptorState);
}

}  // namespace ax::rhi::vk
