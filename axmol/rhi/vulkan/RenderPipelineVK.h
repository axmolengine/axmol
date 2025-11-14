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
class ProgramImpl;

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
    static constexpr int MAX_DESCRIPTOR_SET_COUNT = 2;
    static constexpr int DESCRIPTOR_SET_UBO       = 0;
    static constexpr int DESCRIPTOR_SET_SAMPLER   = 1;

    using VkDescriptorSetLayoutArray = std::array<VkDescriptorSetLayout, MAX_DESCRIPTOR_SET_COUNT>;

    explicit RenderPipelineImpl(VkDevice device);
    ~RenderPipelineImpl();

    void prepareUpdate(DepthStencilStateImpl* ds) { _dsState = ds; }

    void update(const RenderTarget*, const PipelineDesc& desc) override;

    VkPipeline getVkPipeline() const { return _activePipeline; }
    VkPipelineLayout getVkPipelineLayout() const { return _activePipelineLayout; }
    VkDescriptorSetLayout getDescriptorSetLayout(int index) { return _activeDescriptorSetLayouts[index]; }

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
    // void updateInputAssemblyState(PrimitiveType primitiveType);

private:
    void initializePipelineDefaults();

    void updateBlendState(const BlendDesc& blendDesc);
    void updateDescriptorSetLayouts(ProgramImpl* program);
    void updatePipelineLayout(ProgramImpl* program);
    void updateGraphicsPipeline(const PipelineDesc& desc, VkRenderPass renderPass, ProgramImpl* program);

private:
    VkDevice _device{VK_NULL_HANDLE};

    const DepthStencilStateImpl* _dsState{nullptr};

    VkPipelineInputAssemblyStateCreateInfo _iaState{};

#pragma region pipeline constant states
    VkPipelineViewportStateCreateInfo _vpState{};
    VkPipelineRasterizationStateCreateInfo _rasterState{};
    VkPipelineMultisampleStateCreateInfo _msState{};
    VkPipelineDynamicStateCreateInfo _dynState{};
#pragma endregion

    VkPipelineColorBlendAttachmentState _activeAttachment{};
    VkPipelineColorBlendStateCreateInfo _activeBlendState{};

    VkPipelineLayout _activePipelineLayout{nullptr};
    VkDescriptorSetLayoutArray _activeDescriptorSetLayouts{};

    VkPipeline _activePipeline{nullptr};

    axstd::hash_map<uintptr_t, VkPipelineLayout> _pipelineLayoutCache;
    axstd::hash_map<uintptr_t, VkDescriptorSetLayoutArray> _descriptorSetLayoutCache;
    axstd::hash_map<uintptr_t, VkPipeline> _pipelineCache; // PSO cache
};
}  // namespace ax::rhi::vk
