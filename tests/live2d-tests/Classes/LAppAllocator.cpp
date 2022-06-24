/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppAllocator.hpp"
#include <stdlib.h>

using namespace Csm;

void * LAppAllocator::Allocate(const csmSizeType  size)
{
    return malloc(size);
}

void LAppAllocator::Deallocate(void* memory)
{
    if (memory) { free(memory); }
}

void * LAppAllocator::AllocateAligned(const csmSizeType size, const csmUint32 alignment)
{
    size_t offset, shift, alignedAddress;
    void *allocation, **preamble;

    offset = alignment - 1 + sizeof(void*);

    allocation = Allocate(size + static_cast<csmUint32>(offset));

    alignedAddress = (size_t)allocation + sizeof(void*);

    shift = alignedAddress % alignment;

    if (shift)
    {
        alignedAddress += (alignment - shift);
    }

    preamble = (void**)alignedAddress;
    preamble[-1] = allocation;

    return (void*)alignedAddress;
}

void LAppAllocator::DeallocateAligned(void* alignedMemory)
{
    void** preamble;

    preamble = (void**)alignedMemory;

    Deallocate(preamble[-1]);
}
