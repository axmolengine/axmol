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
#include <stdint.h>
#include <type_traits>
#include <vector>

#include "axmol/base/RefPtr.h"
#include "axmol/platform/PlatformDefine.h"

namespace ax
{

class Object;

template <typename _Ty>
class WeakPtr;

struct WeakObjectItem
{
    Object* object{nullptr};
    uint32_t serialNumber{1};  // Initial generation starts at 1
    int nextFreeIndex{-1};     // Pointer to the next free item in the implicit free list
};

/**
 * WeakObjectRegistry
 *
 * A global registry that manages weak references to ax::Object instances.
 * It assigns and tracks indices and serial numbers to ensure that WeakPtr
 * becomes expired when the target Object is destroyed.
 *
 * Important:
 * - Not designed for multithreading. No synchronization is provided.
 * - ax::Object reference counting itself is non-atomic, so the entire
 *   lifecycle management is single-threaded by design.
 * - Callers must ensure external synchronization if used in a concurrent context.
 *
 * Typical usage:
 * - Internal engine mechanism to support WeakPtr.
 * - Provides allocate/free index and lookup by ID/serial number.
 */
class AX_DLL WeakObjectRegistry
{
    friend class Object;
    template <typename>
    friend class WeakPtr;

public:
    static constexpr int CHUNK_SIZE = 32768;  // 32K objects per chunk

    static WeakObjectRegistry& getInstance();

    virtual ~WeakObjectRegistry();

protected:
    // Core interface for lifecycle management
    void allocateIndex(ax::Object* obj);
    void freeIndex(ax::Object* obj);

    // O(1) safe retrieval of the actual object
    ax::Object* getObject(int index, int expectedSerialNumber) const;

    // Get the current serial number for a specific index
    uint32_t getSerialNumber(int index) const;

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
};

/**
 * WeakPtr
 *
 * A non-owning smart pointer to an ax::Object.
 * Unlike RefPtr, it does not increase the reference count.
 * It becomes expired when the target Object is destroyed or recycled.
 *
 * Key points:
 * - Use expired() to check if the pointer has expired.
 * - Use lock() to obtain a RefPtr if the object is still alive.
 * - Supports comparison with raw pointers and nullptr for convenience.
 * - Does not guarantee thread safety (same as ax::Object reference counting).
 *
 * Typical usage:
 * - Store a WeakPtr when you need to reference an Object without extending its lifetime.
 * - Always check expired() before dereferencing or locking.
 */
template <typename _Ty>
class WeakPtr
{
public:
    WeakPtr() noexcept : _index(-1), _serialNumber(0) {}

    explicit WeakPtr(_Ty* p) { assign(p); }
    explicit WeakPtr(std::nullptr_t) { reset(); }

    WeakPtr(const WeakPtr& other) noexcept            = default;
    WeakPtr& operator=(const WeakPtr& other) noexcept = default;

    WeakPtr(WeakPtr&& other) noexcept : _index(other._index), _serialNumber(other._serialNumber) { other.reset(); }
    WeakPtr& operator=(WeakPtr&& other) noexcept
    {
        if (this != &other)
        {
            _index        = other._index;
            _serialNumber = other._serialNumber;
            other.reset();
        }
        return *this;
    }

    WeakPtr& operator=(_Ty* p)
    {
        assign(p);
        return *this;
    }

    WeakPtr& operator=(std::nullptr_t)
    {
        reset();
        return *this;
    }

    ax::RefPtr<_Ty> lock() const
    {
        _Ty* p = get();
        return ax::RefPtr<_Ty>(p);
    }

    // Attempt to safely retrieve the underlying object
    _Ty* get() const
    {
        static_assert(std::is_base_of<Object, std::remove_const_t<_Ty>>::value, "_Ty must inherit from Object");
        if (_index == -1)
            return nullptr;
        return static_cast<_Ty*>(WeakObjectRegistry::getInstance().getObject(_index, _serialNumber));
    }

    // Returns true if the pointer has expired (object destroyed or no longer valid)
    bool expired() const { return !get(); }

    // Overloaded operators for convenience
    _Ty* operator->() const
    {
        _Ty* p = get();
        assert(p != nullptr && "Dereferencing an invalid WeakPtr!");
        return p;
    }

    _Ty& operator*() const
    {
        _Ty* p = get();
        assert(p != nullptr && "Dereferencing an invalid WeakPtr!");
        return *p;
    }

    explicit operator bool() const { return !expired(); }

    template <typename _Other>
    bool operator==(const _Other* other) const
    {
        return get() == other;
    }

    template <typename _Uty>
    bool operator==(const WeakPtr<_Uty>& other) const
    {
        return static_cast<Object*>(get()) == static_cast<Object*>(other.get());
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
    void assign(_Ty* p)
    {
        using NonConstType = std::remove_const_t<_Ty>;
        static_assert(std::is_base_of<ax::Object, NonConstType>::value, "_Ty must inherit from ax::Object");

        if (p)
        {
            // Lazy-load registration: register the object in the global table ONLY
            // when a WeakPtr is tracking it for the first time.
            if (p->_internalIndex == -1)
                WeakObjectRegistry::getInstance().allocateIndex(const_cast<NonConstType*>(p));
            _index        = p->_internalIndex;
            _serialNumber = WeakObjectRegistry::getInstance().getSerialNumber(_index);
        }
        else
        {
            reset();
        }
    }

    int _index;
    uint32_t _serialNumber;
};

}  // namespace ax
