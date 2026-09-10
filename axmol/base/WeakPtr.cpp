/****************************************************************************
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "axmol/base/WeakPtr.h"
#include "axmol/base/Object.h"

namespace ax
{

WeakObjectRegistry& WeakObjectRegistry::getInstance()
{
    static WeakObjectRegistry instance;
    return instance;
}

WeakObjectRegistry::~WeakObjectRegistry()
{
    for (auto chunk : _chunks)
        delete[] chunk;
}

void WeakObjectRegistry::allocateIndex(ax::Object* obj)
{
    if (!obj || obj->_internalIndex != -1)
        return;  // Ignore null or already registered objects

    int allocatedIndex = -1;

    if (_firstFreeIndex != -1)
    {
        // Retrieve a recycled slot from the head of the implicit free list
        allocatedIndex  = _firstFreeIndex;
        auto& item      = getItem(allocatedIndex);
        _firstFreeIndex = item.nextFreeIndex;
        item.object     = obj;
        // Do NOT reset serialNumber; it must keep incrementing to invalidate old weak pointers.
    }
    else
    {
        // Expand capacity if the free list is empty
        allocatedIndex = _numElements++;
        ensureCapacity(allocatedIndex);

        auto& item         = getItem(allocatedIndex);
        item.object        = obj;
        item.serialNumber  = 1;
        item.nextFreeIndex = -1;
    }

    obj->_internalIndex = allocatedIndex;
}

void WeakObjectRegistry::freeIndex(ax::Object* obj)
{
    if (!obj || obj->_internalIndex == -1)
        return;

    auto& item = getItem(obj->_internalIndex);

    // Clear the pointer and increment the serial number to invalidate all existing WeakPtrs
    item.object = nullptr;
    ++item.serialNumber;

    if (item.serialNumber == 0)
        item.serialNumber = 1;

    // Push the recycled slot to the head of the free list
    item.nextFreeIndex = _firstFreeIndex;
    _firstFreeIndex    = obj->_internalIndex;

    // Unbind the object
    obj->_internalIndex = -1;
}

ax::Object* WeakObjectRegistry::getObject(int index, int expectedSerialNumber) const
{
    if (index < 0 || index >= _numElements)
        return nullptr;

    auto& item = getItem(index);

    // Core validation: The object is valid ONLY if the serial numbers match exactly
    if (item.serialNumber == expectedSerialNumber)
        return item.object;
    return nullptr;
}

uint32_t WeakObjectRegistry::getSerialNumber(int index) const
{
    if (index < 0 || index >= _numElements)
        return 0;
    return getItem(index).serialNumber;
}

void WeakObjectRegistry::ensureCapacity(int index)
{
    while (index >= _chunks.size() * CHUNK_SIZE)
    {
        // Allocate a continuous fixed-size memory block
        _chunks.push_back(new WeakObjectItem[CHUNK_SIZE]);
    }
}

}  // namespace ax
