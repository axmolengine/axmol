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
#include "yasio/object_pool.hpp"

namespace ax::rhi::vk
{
class DepthStencilStateImpl;
class VertexLayoutImpl;
class ProgramImpl;
class DriverImpl;

static constexpr int MAX_DESCRIPTOR_SETS = 2;
static constexpr int SET_INDEX_UBO       = 0;
static constexpr int SET_INDEX_SAMPLER   = 1;

static constexpr uint32_t DESCRIPTOR_POOL_ELEMENT_COUNT = 64;

struct ExtendedDynamicState
{
    VkCullModeFlags cullMode : 4              = VK_CULL_MODE_NONE;
    VkFrontFace frontFace : 4                 = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    VkPrimitiveTopology primitiveTopology : 8 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    uint32_t reserved : 16                    = 0;
};

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

class DescriptorPool;
class DescriptorAllocator;
struct DescriptorState
{
    DescriptorAllocator* allocator{nullptr};
    DescriptorPool* pool{nullptr};
    VkDescriptorSetArray sets{};  // Allocated VkDescriptorSets
    uint64_t progId{0};           // progId associated with this descriptor set
};

using DescriptorFreeList = tlx::pod_vector<DescriptorState*>;

class DescriptorPool
{
public:
    void init(VkDevice device, std::span<const VkDescriptorPoolSize> poolSizes);
    void dispose(VkDevice device);

    int available() const { return _capacity - _allocated; }
    int capacity() const { return _capacity; }

    void allocateDescriptorSets(VkDevice device, const PipelineLayoutState* layoutState, DescriptorState* descriptor);
    void freeDescriptorSets(VkDevice device, uint32_t descriptorCount, DescriptorState* descriptor);

protected:
    VkDescriptorPool _pool{VK_NULL_HANDLE};
    int _capacity{0};
    int _allocated{0};
};

class DescriptorAllocator
{
public:
    void init(VkDevice device, std::span<const VkDescriptorPoolSize> poolSizes);
    void dispose();

    void allocateDescriptorSets(const PipelineLayoutState* layoutState, DescriptorState* descriptor);
    void freeDescriptorSets(DescriptorState* descriptor);

    void sortPools();

protected:
    DescriptorPool* spawnPool();

    VkDevice _device{VK_NULL_HANDLE};
    tlx::pod_vector<VkDescriptorPoolSize> _poolSizes;
    tlx::pod_vector<DescriptorPool*> _pools;
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
    explicit RenderPipelineImpl(DriverImpl* driver);
    ~RenderPipelineImpl();

    void prepareUpdate(DepthStencilStateImpl* ds) { _dsState = ds; }

    void update(const RenderTarget*, const PipelineDesc& desc, const ExtendedDynamicState& state);

    VkPipeline getVkPipeline() const { return _activePipeline; }
    PipelineLayoutState* getPipelineLayoutState() const { return _activeLayoutState; }

    DescriptorState* acquireDescriptorState();
    void recycleDescriptorState(DescriptorState* state);

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
    tlx::hash_map<uint64_t, DescriptorFreeList> _descriptorCache;       // progId -> recycled desc sets

    tlx::hash_map<uintptr_t, VkPipeline> _pipelineCache;  // PSO cache

    std::multimap<uint64_t, uintptr_t> _programToPipelineMap;         // progId -> PSO id
    std::multimap<VkRenderPass, uintptr_t> _renderPassToPipelineMap;  // renderPass -> PSO id

    DescriptorAllocator _descriptorAllocator1;  // ubo descriptor allocator
    DescriptorAllocator _descriptorAllocator2;  // ubo + sampler descriptor allocator

    yasio::object_pool<DescriptorState> _descriptorStatePool;
};
}  // namespace ax::rhi::vk
