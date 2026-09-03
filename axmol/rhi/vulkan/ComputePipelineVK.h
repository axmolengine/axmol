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
