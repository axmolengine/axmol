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
#include <functional>
#include <utility>
#include <memory>
#include "axmol/tlx/type_traits.hpp"

namespace axstd
{

/**
 * Wrapper class which maintains a strong reference to a axmol ax::Object* type object.
 * Similar in concept to a boost smart pointer.
 *
 * Enables the use of the RAII idiom with axmol objects and helps automate some of the more
 * mundane tasks of pointer initialization and cleanup.
 *
 * The class itself is modelled on C++ 11 std::shared_ptr, and trys to keep some of the methods
 * and functionality consistent with std::shared_ptr.
 */

struct retain_object_t
{};
struct adopt_object_t
{};

inline constexpr retain_object_t retain_object{};
inline constexpr adopt_object_t adopt_object{};

template <typename T>
struct retain_traits final
{
    static constexpr auto default_action() noexcept { return retain_object; }

    static void retain(T* p) noexcept
    {
        if (p)
            p->retain();
    }

    static void release(T* p) noexcept
    {
        if (p)
            p->release();
    }
};

template <typename T, typename Traits = retain_traits<T>>
class retain_ptr
{
private:
    typedef retain_ptr this_type;

public:
    typedef T element_type;
    typedef Traits traits_type;

    constexpr retain_ptr() noexcept : px(nullptr) {}
    retain_ptr(std::nullptr_t) noexcept : px(nullptr) {}
    explicit retain_ptr(T* p) noexcept : this_type(p, traits_type::default_action()) {}

    retain_ptr(T* p, retain_object_t) noexcept : this_type(p, adopt_object) { traits_type::retain(p); }
    retain_ptr(T* p, adopt_object_t) noexcept : px(p) {}

    template <typename U, typename E = enable_if_convertible_t<U*, T*>>
    retain_ptr(retain_ptr<U, traits_type> const& rhs) noexcept : px(rhs.get())
    {
        traits_type::retain(px);
    }

    retain_ptr(retain_ptr const& rhs) noexcept : px(rhs.px) { traits_type::retain(px); }

    ~retain_ptr() noexcept { traits_type::release(px); }

    template <typename U>
    retain_ptr& operator=(retain_ptr<U, traits_type> const& rhs) noexcept
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    retain_ptr(retain_ptr&& rhs) noexcept : px(rhs.px) { rhs.px = nullptr; }

    retain_ptr& operator=(retain_ptr&& rhs) noexcept
    {
        this_type(std::move(rhs)).swap(*this);
        return *this;
    }

    template <typename U, typename R>
    friend class retain_ptr;

    template <typename U, typename E = enable_if_convertible_t<U*, T*>>
    retain_ptr(retain_ptr<U, traits_type>&& rhs) noexcept : px(rhs.px)
    {
        rhs.px = nullptr;
    }

    template <typename U>
    retain_ptr& operator=(retain_ptr<U, traits_type>&& rhs) noexcept
    {
        this_type(std::move(rhs)).swap(*this);
        return *this;
    }

    retain_ptr& operator=(retain_ptr const& rhs) noexcept
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    retain_ptr& operator=(T* rhs) noexcept
    {
        this_type(rhs).swap(*this);
        return *this;
    }

    void reset() noexcept { this_type().swap(*this); }
    void reset(T* rhs) noexcept { reset(rhs, traits_type::default_action()); }
    void reset(T* rhs, retain_object_t) noexcept { this_type(rhs, retain_object).swap(*this); }
    void reset(T* rhs, adopt_object_t) noexcept { this_type(rhs, adopt_object).swap(*this); }

    T* get() const noexcept { return px; }
    operator T*() const noexcept { return px; }

    T* detach() noexcept
    {
        T* ret = px;
        px     = nullptr;
        return ret;
    }

    T& operator*() const noexcept
    {
        assert(px != nullptr);
        return *px;
    }

    T* operator->() const noexcept
    {
        assert(px != nullptr);
        return px;
    }

    explicit operator bool() const noexcept { return px != nullptr; }

    void swap(retain_ptr& rhs) noexcept
    {
        T* tmp = px;
        px     = rhs.px;
        rhs.px = tmp;
    }

private:
    T* px;
};

template <typename T, typename U, typename R>
inline bool operator==(retain_ptr<T, R> const& a, retain_ptr<U, R> const& b) noexcept
{
    return a.get() == b.get();
}

template <typename T, typename U, typename R>
inline bool operator!=(retain_ptr<T, R> const& a, retain_ptr<U, R> const& b) noexcept
{
    return a.get() != b.get();
}

template <typename T, typename U, typename R>
inline bool operator==(retain_ptr<T, R> const& a, U* b) noexcept
{
    return a.get() == b;
}

template <typename T, typename U, typename R>
inline bool operator!=(retain_ptr<T, R> const& a, U* b) noexcept
{
    return a.get() != b;
}

template <typename T, typename U, typename R>
inline bool operator==(T* a, retain_ptr<U, R> const& b) noexcept
{
    return a == b.get();
}

template <typename T, typename U, typename R>
inline bool operator!=(T* a, retain_ptr<U, R> const& b) noexcept
{
    return a != b.get();
}

template <typename T, typename U, typename R>
inline bool operator<(retain_ptr<T, R> const& a, retain_ptr<U, R> const& b) noexcept
{
    return std::less<>()(a.get(), b.get());
}

template <typename T, typename R>
inline void swap(retain_ptr<T, R>& lhs, retain_ptr<T, R>& rhs) noexcept
{
    lhs.swap(rhs);
}

// mem_fn support

template <typename T, typename R>
inline T* get_pointer(retain_ptr<T, R> const& p) noexcept
{
    return p.get();
}

// pointer casts

template <typename T, typename U, typename R>
inline retain_ptr<T, R> static_pointer_cast(retain_ptr<U, R> const& p)
{
    return static_cast<T*>(p.get());
}

template <typename T, typename U, typename R>
inline retain_ptr<T, R> const_pointer_cast(retain_ptr<U, R> const& p)
{
    return const_cast<T*>(p.get());
}

template <typename T, typename U, typename R>
inline retain_ptr<T, R> dynamic_pointer_cast(retain_ptr<U, R> const& p)
{
    return dynamic_cast<T*>(p.get());
}

template <typename T, typename U, typename R>
inline retain_ptr<T, R> static_pointer_cast(retain_ptr<U, R>&& p) noexcept
{
    return retain_ptr<T, R>(static_cast<T*>(p.detach()), adopt_object);
}

template <typename T, typename U, typename R>
inline retain_ptr<T, R> const_pointer_cast(retain_ptr<U, R>&& p) noexcept
{
    return retain_ptr<T, R>(const_cast<T*>(p.detach()), adopt_object);
}

template <typename T, typename U, typename R>
inline retain_ptr<T, R> dynamic_pointer_cast(retain_ptr<U, R>&& p) noexcept
{
    T* p2 = dynamic_cast<T*>(p.get());

    retain_ptr<T, R> r(p2, adopt_object);

    if (p2)
        p.detach();

    return r;
}

}  // namespace axstd

// std::hash

namespace std
{

template <typename T, typename R>
struct hash<::axstd::retain_ptr<T, R>>
{
    std::size_t operator()(::axstd::retain_ptr<T, R> const& p) const noexcept { return std::hash<T*>()(p.get()); }
};

}  // namespace std
