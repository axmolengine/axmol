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

#include <assert.h>
#include <type_traits>
#include <vector>
#include <mutex>

#include "axmol/base/RefPtr.h"
#include "axmol/platform/PlatformDefine.h"

namespace ax
{

class Object;

struct WeakObjectItem
{
    Object* object{nullptr};
    int serialNumber{1};    // Initial generation starts at 1
    int nextFreeIndex{-1};  // Pointer to the next free item in the implicit free list
};

class AX_DLL WeakObjectRegistry
{
public:
    static constexpr int CHUNK_SIZE = 65536;  // 64K objects per chunk

    static WeakObjectRegistry& getInstance();

    ~WeakObjectRegistry();

    // Core interface for lifecycle management
    void allocateIndex(ax::Object* obj);
    void freeIndex(ax::Object* obj);

    // O(1) safe retrieval of the actual object
    ax::Object* getObject(int index, int expectedSerialNumber) const;

    // Get the current serial number for a specific index
    int getSerialNumber(int index) const;

private:
    WeakObjectRegistry() = default;

    // O(1) Chunked array addressing.
    // Must remain inline in the header for compiler optimization (bitwise shift/masking).
    inline WeakObjectItem& getItem(int index) const
    {
        const int chunkIndex  = index / CHUNK_SIZE;
        const int withinIndex = index % CHUNK_SIZE;
        return _chunks[chunkIndex][withinIndex];
    }

    // Expand capacity by allocating a new chunk
    void ensureCapacity(int index);

    std::vector<WeakObjectItem*> _chunks;  // Master array containing pointers to each Chunk
    int _firstFreeIndex = -1;              // Head of the implicit free list
    int _numElements    = 0;               // High water mark of allocated elements
    mutable std::mutex _mutex;             // Thread safety for async operations
};

//-------------------------------------------------------------------------
// CLASS Template: WeakPtr
//-------------------------------------------------------------------------
template <typename _Ty>
class WeakPtr
{
public:
    WeakPtr() noexcept : _index(-1), _serialNumber(0) {}

    WeakPtr(_Ty* obj) { assign(obj); }

    WeakPtr(const WeakPtr& other) noexcept            = default;
    WeakPtr& operator=(const WeakPtr& other) noexcept = default;

    WeakPtr(WeakPtr&& other) noexcept : _index(other._index), _serialNumber(other._serialNumber) { other.reset(); }
    WeakPtr& operator=(WeakPtr&& other) noexcept
    {
        _index        = other._index;
        _serialNumber = other._serialNumber;
        other.reset();
        return *this;
    }

    WeakPtr& operator=(_Ty* obj)
    {
        assign(obj);
        return *this;
    }

    ax::RefPtr<_Ty> lock() const
    {
        _Ty* obj = get();
        return ax::RefPtr<_Ty>(obj);
    }

    // Attempt to safely retrieve the underlying object
    _Ty* get() const
    {
        static_assert(std::is_base_of<Object, _Ty>::value, "_Ty must inherit from Object");
        if (_index == -1)
            return nullptr;
        return static_cast<_Ty*>(WeakObjectRegistry::getInstance().getObject(_index, _serialNumber));
    }

    // Check if the pointer is still valid
    bool expired() const { return !get(); }

    // Overloaded operators for convenience
    _Ty* operator->() const
    {
        _Ty* obj = get();
        assert(obj != nullptr && "Dereferencing an invalid WeakPtr!");
        return obj;
    }

    _Ty& operator*() const
    {
        _Ty* obj = get();
        assert(obj != nullptr && "Dereferencing an invalid WeakPtr!");
        return *obj;
    }

    explicit operator bool() const { return !expired(); }

    template <typename _Other>
    bool operator==(const _Other* other) const
    {
        return get() == other;
    }

    bool operator==(std::nullptr_t) const { return expired(); }
    bool operator!=(std::nullptr_t) const { return !expired(); }

    // Clear the weak pointer locally
    void reset()
    {
        _index        = -1;
        _serialNumber = 0;
    }

private:
    void assign(_Ty* obj)
    {
        static_assert(std::is_base_of<ax::Object, _Ty>::value, "_Ty must inherit from ax::Object");

        if (obj)
        {
            // Lazy-load registration: register the object in the global table ONLY
            // when a WeakPtr is tracking it for the first time.
            if (static_cast<Object*>(obj)->_internalIndex == -1)
                WeakObjectRegistry::getInstance().allocateIndex(obj);
            _index        = static_cast<Object*>(obj)->_internalIndex;
            _serialNumber = WeakObjectRegistry::getInstance().getSerialNumber(_index);
        }
        else
        {
            reset();
        }
    }

    int _index;
    int _serialNumber;
};

}  // namespace ax
