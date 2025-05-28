#pragma once

#include <atomic>

namespace ax
{
class ConcurrentRefCountedBase
{
public:
    ConcurrentRefCountedBase() : _refCount(1) {}
    virtual ~ConcurrentRefCountedBase() {}

    void retain() { ++_refCount; }

    void release()
    {
        if (--_refCount == 0)
            delete this;
    }

    int getReferenceCount() const { return _refCount; }

protected:
    std::atomic_int _refCount;
};
}  // namespace ax
