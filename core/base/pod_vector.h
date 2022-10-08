/****************************************************************************
 Copyright (c) 2022 Bytedance Inc.

 https://axmolengine.github.io/

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

 The pod_vector concepts:
   a. only accept pod type
   b. support release memory ownership with `release_pointer`
   c. support pod_allocater with C(realloc) or C++(new/delete)
   b. resize operation no fill
*/
#pragma once
#include <string.h>
#include <utility>
#include <new>
#include <type_traits>

namespace ax
{

template <typename _Ty, bool /*_use_crt_alloc*/ = false>
struct pod_allocator
{
    static void* reallocate(void* old_block, size_t old_count, size_t new_count)
    {
        if (old_count != new_count)
        {
            void* new_block = nullptr;
            if (new_count)
            {
                new_block = ::operator new(new_count * sizeof(_Ty));
                if (old_block)
                    memcpy(new_block, old_block, (std::min)(old_count, new_count) * sizeof(_Ty));
            }

            ::operator delete(old_block);
            return new_block;
        }
        return old_block;
    }
};

template <typename _Ty>
struct pod_allocator<_Ty, true>
{
    static void* reallocate(void* old_block, size_t /*old_count*/, size_t new_count)
    {
        return ::realloc(old_block, new_count * sizeof(_Ty));
    }
};

template <typename _Ty,
          typename _Alty                                               = pod_allocator<_Ty>,
          std::enable_if_t<std::is_trivially_destructible_v<_Ty>, int> = 0>
class pod_vector
{
public:
    using pointer                            = _Ty*;
    using value_type                         = _Ty;
    pod_vector()                             = default;
    pod_vector(pod_vector const&)            = delete;
    pod_vector& operator=(pod_vector const&) = delete;

    pod_vector(pod_vector&& o) noexcept
    {
        std::swap(_Myfirst, o._Myfirst);
        std::swap(_Mylast, o._Mylast);
        std::swap(_Myend, o._Myend);
    }
    pod_vector& operator=(pod_vector&& o) noexcept
    {
        std::swap(_Myfirst, o._Myfirst);
        std::swap(_Mylast, o._Mylast);
        std::swap(_Myend, o._Myend);
        return *this;
    }

    ~pod_vector() { shrink_to_fit(0); }

    template <typename... _Args>
    _Ty& emplace(_Args&&... args) noexcept
    {
        return *new (resize(this->size() + 1)) _Ty(std::forward<_Args>(args)...);
    }

    void reserve(size_t new_cap)
    {
        if (this->capacity() < new_cap)
        {
            auto cur_size = this->size();
            _Reallocate_exactly(new_cap);
            _Mylast = _Myfirst + cur_size;
        }
    }

    // return address of new last element
    pointer resize(size_t new_size)
    {
        auto old_cap = this->capacity();
        if (old_cap < new_size)
            _Reallocate_exactly((std::max)(old_cap + old_cap / 2, new_size));
        _Mylast = _Myfirst + new_size;
        return _Mylast - 1;
    }

    _Ty& operator[](size_t idx) { return _Myfirst[idx]; }
    const _Ty& operator[](size_t idx) const { return _Myfirst[idx]; }

    size_t capacity() const noexcept { return _Myend - _Myfirst; }
    size_t size() const noexcept { return _Mylast - _Myfirst; }
    void clear() noexcept { _Mylast = _Myfirst; }

    void shrink_to_fit() { shrink_to_fit(this->size()); }
    void shrink_to_fit(size_t new_size)
    {
        if (this->capacity() != new_size)
            _Reallocate_exactly(new_size);
        _Mylast = _Myfirst + new_size;
    }

    // release memmory ownership
    pointer release_pointer() noexcept
    {
        auto ptr = _Myfirst;
        memset(this, 0, sizeof(*this));
        return ptr;
    }

private:
    void _Reallocate_exactly(size_t new_cap)
    {
        auto new_block = (pointer)_Alty::reallocate(_Myfirst, _Myend - _Myfirst, new_cap);
        if (new_block || 0 == new_cap)
        {
            _Myfirst = new_block;
            _Myend   = _Myfirst + new_cap;
        }
        else
            throw std::bad_alloc{};
    }

    pointer _Myfirst = nullptr;
    pointer _Mylast  = nullptr;
    pointer _Myend   = nullptr;
};
}  // namespace ax
