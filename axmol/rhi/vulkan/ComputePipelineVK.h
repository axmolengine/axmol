/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/ComputePipeline.h"
#include "axmol/rhi/vulkan/GraphicsPipelineVK.h"
#include <glad/vulkan.h>

namespace ax::rhi::vk
{
class ProgramImpl;
class GraphicsDeviceImpl;

/**
 * @brief A Vulkan compute pipeline with its own descriptor layout.
 *
 * Descriptor sets mirror the graphics model:
 *   set 0: uniform buffers
 *   set 1: storage buffers (unshifted binding) + sampled images (unshifted) + preset samplers (shifted)
 *   set 2: custom samplers (shifted)
 */
class ComputePipelineImpl : public ComputePipeline
{
public:
    ComputePipelineImpl(GraphicsDeviceImpl* driver, ProgramImpl* program);
    ~ComputePipelineImpl();

    bool isValid() const override { return _pipeline != VK_NULL_HANDLE; }

    VkPipeline getPipeline() const { return _pipeline; }
    PipelineLayoutState* getLayoutState() { return &_layoutState; }

    DescriptorState* acquireDescriptorState();
    void recycleDescriptorState(DescriptorState* descriptorState);

private:
    void createLayout(ProgramImpl* program);
    void createPipeline(ProgramImpl* program);

    GraphicsDeviceImpl* _driver{nullptr};
    VkDevice _device{VK_NULL_HANDLE};
    PipelineLayoutState _layoutState{};
    DescriptorAllocator _descriptorAllocator{};
    yasio::object_pool<DescriptorState> _descriptorStatePool;
    VkPipeline _pipeline{VK_NULL_HANDLE};
};

}  // namespace ax::rhi::vk
