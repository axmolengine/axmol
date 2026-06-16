/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Type/CubismBasicType.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * An interface to implement memory allocation and deallocation processes<br>
 * on the platform side and call from the Framework.
 */
class ICubismAllocator
{
public:
    /**
     * Destructor
     */
    virtual ~ICubismAllocator() {}

    /**
     * Allocates the memory.
     *
     * @param size Desired amount of memory in bytes
     *
     * @return Pointer to the allocated memory if succeeded; otherwise `0`
     */
    virtual void* Allocate(const csmSizeType size) = 0;

    /**
     * Deallocates the memory.
     *
     * @param memory Pointer to allocated memory to be deallocated
     */
    virtual void Deallocate(void* memory) = 0;


    /**
     * Allocates the memory with specified alignment.
     *
     * @param size Desired amount of memory in bytes
     * @param alignment Desired alignment of memory in bytes
     *
     * @return Pointer to the allocated memory if succeeded; otherwise `0`
     */
    virtual void* AllocateAligned(const csmSizeType size, const csmUint32 alignment) = 0;

    /**
     * Deallocates the aligned memory.
     *
     * @param alignedMemory Pointer to allocated memory to be deallocated
     */
    virtual void DeallocateAligned(void* alignedMemory) = 0;

};
}}}
