#pragma once
#include <utility>
#include <cstring>
#include <exception>

namespace adxe
{
struct any_buffer
{
    any_buffer()                  = default;
    any_buffer(any_buffer const&) = delete;
    any_buffer& operator=(any_buffer const&) = delete;
    any_buffer(any_buffer&& o) noexcept
    {
        std::swap(buf, o.buf);
        std::swap(cap, o.cap);
    }
    any_buffer& operator=(any_buffer&& o) noexcept
    {
        std::swap(buf, o.buf);
        std::swap(cap, o.cap);
        return *this;
    }

    ~any_buffer() { clear(); }

    template <typename T>
    T* get(size_t const& len)
    {
        auto newCap = sizeof(T) * len;
        if (newCap > cap)
        {
            delete this->buf;
            this->buf = new char[newCap];
            this->cap = newCap;
        }
        return (T*)buf;
    }

    void clear()
    {
        if (cap)
        {
            delete buf;
            buf = nullptr;
            cap = 0;
        }
    }

private:
    void* buf = nullptr;
    size_t cap = 0;
};
}  // namespace adxe
