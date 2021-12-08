
/****************************************************************************
 Copyright (c) 2021 @denghe
 Copyright (c) 2021 Bytedance Inc.
 
 https://adxe.org
 
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
