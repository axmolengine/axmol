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
