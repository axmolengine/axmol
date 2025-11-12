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

#include "axmol/rhi/RenderPipeline.h"
#include "axmol/tlx/hlookup.hpp"
#include <glad/vulkan.h>

namespace ax::rhi::vk
{
class DepthStencilStateImpl;
class VertexLayoutImpl;

/**
 * @brief Vulkan-based RenderPipeline implementation
 *
 * This class manages pipeline creation and caching for Vulkan.
 * It converts Axmol's PipelineDesc into Vulkan pipeline states,
 * and caches intermediate objects (blend, depth-stencil, rasterizer, etc.)
 * to avoid redundant Vulkan object creation.
 */
class RenderPipelineImpl : public RenderPipeline
{
public:
    // match with axslcc-1.5.1
    static constexpr int MAX_DESCRIPTOR_SET_COUNT = 2;
    static constexpr int DESCRIPTOR_SET_UBO       = 0;
    static constexpr int DESCRIPTOR_SET_SAMPLER   = 1;
    static constexpr int VS_UBO_BINDING_INDEX     = 0;
    static constexpr int FS_UBO_BINDING_INDEX     = 1;

    using VkDescriptorSetLayoutArray = std::array<VkDescriptorSetLayout, MAX_DESCRIPTOR_SET_COUNT>;

    explicit RenderPipelineImpl(VkDevice device) : _device(device) {}
    ~RenderPipelineImpl();

    void prepareUpdate(DepthStencilStateImpl* ds, VertexLayoutImpl* vl)
    {
        _dsState      = ds;
        _vertexLayout = vl;
    }

    /**
     * @brief Update pipeline state based on the given description.
     *
     * This function checks caches for existing states and pipelines.
     * If not found, it creates new Vulkan objects and stores them.
     */
    void update(const RenderTarget*, const PipelineDesc& desc) override;

    VkPipeline getVkPipeline() const { return _activePipeline; }

    VkPipelineLayout getVkPipelineLayout() const { return _activePipelineLayout; }

    VkDescriptorSetLayout getDescriptorSetLayout(int index) { return _activeDescriptorSetLayouts[index]; }

private:
    VkDevice _device{VK_NULL_HANDLE};

    // Active blend attachment and state used in the current pipeline
    DepthStencilStateImpl* _dsState{nullptr};
    VertexLayoutImpl* _vertexLayout{nullptr};
    VkPipelineColorBlendAttachmentState _activeAttachment{};
    VkPipelineColorBlendStateCreateInfo _activeBlendState{};

    VkPipeline _activePipeline{nullptr};
    VkPipelineLayout _activePipelineLayout{nullptr};
    VkDescriptorSetLayoutArray _activeDescriptorSetLayouts{};

    // === Cache layers ===

    /// Pipeline layout cache: ProgramState hash -> VkPipelineLayout
    axstd::hash_map<uintptr_t, VkPipelineLayout> _pipelineLayoutCache;

    /// Descriptor set layout cache: ProgramState hash -> VkDescriptorSetLayout
    axstd::hash_map<uintptr_t, VkDescriptorSetLayoutArray> _descriptorSetLayoutCache;

    /// Final pipeline cache: combined PipelineDesc hash -> VkPipeline
    axstd::hash_map<uintptr_t, VkPipeline> _pipelineCache;
};
}  // namespace ax::rhi::vk
