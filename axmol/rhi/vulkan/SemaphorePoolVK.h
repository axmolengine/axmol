/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "glad/vulkan.h"
#include "axmol/tlx/vector.hpp"

namespace ax::rhi::vk
{
class SemaphorePool
{
    static constexpr int INITIAL_POOL_SIZE = 45;

public:
    SemaphorePool(VkDevice device);
    ~SemaphorePool();
    VkSemaphore acquire();
    void recycle(VkSemaphore semaphore);

protected:
    VkDevice _device{VK_NULL_HANDLE};
    tlx::pod_vector<VkSemaphore> _pool;
};
}  // namespace ax::rhi::vk
