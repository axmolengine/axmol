/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/rhi/vulkan/SemaphorePoolVK.h"

namespace ax::rhi::vk
{
namespace
{
VkSemaphore createSemaphore(VkDevice device)
{
    VkSemaphore semaphore;
    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };
    vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore);
    return semaphore;
}

}  // namespace
SemaphorePool::SemaphorePool(VkDevice device) : _device(device)
{
    for (size_t i = 0; i < INITIAL_POOL_SIZE; ++i)
    {
        _pool.push_back(createSemaphore(_device));
    }
}

SemaphorePool::~SemaphorePool()
{
    for (auto semaphore : _pool)
    {
        vkDestroySemaphore(_device, semaphore, nullptr);
    }
    _pool.clear();
}

VkSemaphore SemaphorePool::acquire()
{
    VkSemaphore semaphore;
    if (!_pool.empty())
    {
        semaphore = _pool.back();
        _pool.pop_back();
    }
    else
    {
        semaphore = createSemaphore(_device);
    }

    return semaphore;
}

void SemaphorePool::recycle(VkSemaphore semaphore)
{
    _pool.push_back(semaphore);
}

}  // namespace ax::rhi::vk
