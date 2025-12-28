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
#include <unordered_map>

namespace ax::rhi::vk
{
class DepthStencilStateImpl;
class VertexLayoutImpl;
class ProgramImpl;
class DriverImpl;

struct ExtendedDynamicState
{
    VkCullModeFlags cullMode : 4              = VK_CULL_MODE_NONE;
    VkFrontFace frontFace : 4                 = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    VkPrimitiveTopology primitiveTopology : 8 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    uint32_t reserved : 16                    = 0;
};

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
    static constexpr int MAX_DESCRIPTOR_SETS = 2;
    static constexpr int SET_INDEX_UBO       = 0;
    static constexpr int SET_INDEX_SAMPLER   = 1;

    static constexpr uint32_t DEFAULT_DESCRIPTOR_POOL_UNIFORM_COUNT = 64;
    static constexpr uint32_t DEFAULT_DESCRIPTOR_POOL_SAMPLER_COUNT = 64;
    static constexpr uint32_t DEFAULT_DESCRIPTOR_POOL_MAX_SETS      = 128;

    using VkDescriptorSetArray       = std::array<VkDescriptorSet, MAX_DESCRIPTOR_SETS>;
    using VkDescriptorSetLayoutArray = std::array<VkDescriptorSetLayout, MAX_DESCRIPTOR_SETS>;
    struct DescriptorSetLayoutState
    {
        VkDescriptorSetLayoutArray descriptorSetLayouts{VK_NULL_HANDLE};
        uint32_t descriptorSetLayoutCount{0};
        uint32_t samplerDescriptorCount{0};
        uint32_t uniformDescriptorCount{0};
    };

    struct DescriptorState
    {
        VkDescriptorSetArray sets;     // Allocated VkDescriptorSets
        VkPipelineLayout ownerLayout;  // PipelineLayout
        int frameIndex;                // Frame index (for multi-frame in flight)
    };

    using DescriptorPool = std::array<tlx::pod_vector<DescriptorState>, MAX_FRAMES_IN_FLIGHT>;

    explicit RenderPipelineImpl(DriverImpl* driver);
    ~RenderPipelineImpl();

    void prepareUpdate(DepthStencilStateImpl* ds) { _dsState = ds; }

    void update(const RenderTarget*, const PipelineDesc& desc, const ExtendedDynamicState& state);

    VkPipeline getVkPipeline() const { return _activePipeline; }
    VkPipelineLayout getVkPipelineLayout() const { return _activePipelineLayout; }
    DescriptorSetLayoutState* getDescriptorSetLayoutState() const { return _activeDSL; }

    bool acquireDescriptorState(DescriptorState& outState, int frameIndex);
    void recycleDescriptorState(DescriptorState& state);

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

    void removeCachedPipelines(VkRenderPass rp);

private:
    void initializePipelineDefaults(DriverImpl* driver);

    void updateBlendState(const BlendDesc& blendDesc);
    void updateDescriptorSetLayouts(ProgramImpl* program);
    void updatePipelineLayout(ProgramImpl* program);
    void updateGraphicsPipeline(const PipelineDesc& desc,
                                const ExtendedDynamicState& states,
                                VkRenderPass renderPass,
                                ProgramImpl* program);

    VkDescriptorPool allocateDescriptorPool();

private:
    DriverImpl* _driverImpl{nullptr};
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
    DescriptorSetLayoutState* _activeDSL{nullptr};

    VkPipeline _activePipeline{VK_NULL_HANDLE};

    tlx::pod_vector<VkDescriptorPool> _descriptorPools;

    tlx::hash_map<uintptr_t, DescriptorSetLayoutState> _descriptorLayoutCache;
    tlx::hash_map<uintptr_t, VkPipelineLayout> _pipelineLayoutCache;
    tlx::hash_map<uintptr_t, VkPipeline> _pipelineCache;  // PSO cache
    tlx::hash_map<VkPipelineLayout, DescriptorPool> _descriptorCache;

    // TODO:
    std::multimap<ProgramImpl*, uintptr_t> _programToPipelineMap;
    std::multimap<VkRenderPass, uintptr_t> _renderPassToPipelineMap;
};
}  // namespace ax::rhi::vk
