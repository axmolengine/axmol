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

class DescriptorPool
{
public:
    size_t capacity() const { return 0; }

protected:
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
    struct PipelineLayoutState
    {
        VkPipelineLayout layout{VK_NULL_HANDLE};
        VkDescriptorSetLayoutArray descriptorSetLayouts{VK_NULL_HANDLE};

        uint32_t descriptorSetLayoutCount{0};
        uint32_t samplerDescriptorCount{0};
        uint32_t uniformDescriptorCount{0};
    };

    struct DescriptorState
    {
        VkDescriptorSetArray sets{};  // Allocated VkDescriptorSets
        uint64_t progId{0};         // progId associated with this descriptor set
        VkDescriptorPool pool{VK_NULL_HANDLE};
    };

    using DescriptorFreeList = tlx::pod_vector<DescriptorState>;

    explicit RenderPipelineImpl(DriverImpl* driver);
    ~RenderPipelineImpl();

    void prepareUpdate(DepthStencilStateImpl* ds) { _dsState = ds; }

    void update(const RenderTarget*, const PipelineDesc& desc, const ExtendedDynamicState& state);

    VkPipeline getVkPipeline() const { return _activePipeline; }
    PipelineLayoutState* getPipelineLayoutState() const { return _activeLayoutState; }

    bool acquireDescriptorState(DescriptorState& outState, int frameIndex);
    void recycleDescriptorState(DescriptorState& state);

    void removeCachedObjects(VkRenderPass key);
    void removeCachedObjects(Program* key);

private:
    void initializePipelineDefaults(DriverImpl* driver);

    void updateBlendState(const BlendDesc& blendDesc);
    void updatePipelineLayoutState(ProgramImpl* program);
    void updateGraphicsPipeline(ProgramImpl* program,
                                const PipelineDesc& desc,
                                const ExtendedDynamicState& states,
                                VkRenderPass renderPass);

    VkDescriptorPool allocateDescriptorPool();

private:
    DriverImpl* _driver{nullptr};
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

    uint64_t _activeProgId{0};

    PipelineLayoutState* _activeLayoutState{nullptr};
    VkPipeline _activePipeline{VK_NULL_HANDLE};

    tlx::hash_map<uint64_t, PipelineLayoutState> _pipelineLayoutCache;  // progId -> PipelineLayoutState
    tlx::hash_map<uintptr_t, VkPipeline> _pipelineCache;  // PSO cache
    tlx::hash_map<uint64_t, DescriptorFreeList> _descriptorCache;  // progId -> recycled desc sets

    std::multimap<uint64_t, uintptr_t> _programToPipelineMap;         // progId -> PSO id
    std::multimap<VkRenderPass, uintptr_t> _renderPassToPipelineMap;  // renderPass -> PSO id

    tlx::pod_vector<VkDescriptorPool> _descriptorPools;
};
}  // namespace ax::rhi::vk
