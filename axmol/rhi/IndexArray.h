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
#include "axmol/rhi/RHITypes.h"
#include "axmol/tlx/vector.hpp"
#include <assert.h>
#include <variant>

namespace ax::rhi
{

// Keep original helpers
using ilist_u16_t = std::initializer_list<uint16_t>;
using ilist_u32_t = std::initializer_list<uint32_t>;

template <typename _T>
inline constexpr bool is_index_format_type_v =
    std::is_integral_v<_T> && (std::is_same_v<_T, uint16_t> || std::is_same_v<_T, uint32_t>);

/**
 * @class IndexArray
 * @brief Container for index data used in graphics rendering.
 *
 * Provides storage for index arrays with runtime-selectable format.
 * The default format is rhi::IndexFormat::U_SHORT.
 */
class IndexArray
{
public:
    // Public aliases preserved (iterator types were byte_buffer iterators in original).
    // Here, expose typed iterators via getters instead of typedefs to raw byte iterators.
    using size_type = std::size_t;

    static constexpr unsigned int formatToStride(rhi::IndexFormat format)
    {
        return (format == rhi::IndexFormat::U_SHORT) ? 2u : 4u;
    }
    static constexpr rhi::IndexFormat strideToFormat(unsigned int stride)
    {
        return stride == 2 ? rhi::IndexFormat::U_SHORT : rhi::IndexFormat::U_INT;
    }

    // variant container_type
    using index16_array_t = tlx::pod_vector<uint16_t>;
    using index32_array_t = tlx::pod_vector<uint32_t>;
    using container_type  = std::variant<index16_array_t, index32_array_t>;

    // Constructors
    IndexArray() : _cont(index16_array_t{}) {}
    IndexArray(rhi::IndexFormat indexFormat)
    {
        if (indexFormat == rhi::IndexFormat::U_SHORT)
            _cont.emplace<index16_array_t>();
        else
            _cont.emplace<index32_array_t>();
    }

    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    IndexArray(std::initializer_list<_Ty> rhs)
    {
        if constexpr (std::is_same_v<_Ty, uint16_t>)
            _cont = index16_array_t(rhs.begin(), rhs.end());
        else
            _cont = index32_array_t(rhs.begin(), rhs.end());
    }

    IndexArray(const IndexArray&)     = default;
    IndexArray(IndexArray&&) noexcept = default;

    IndexArray& operator=(const IndexArray&)     = default;
    IndexArray& operator=(IndexArray&&) noexcept = default;

    void swap(IndexArray& rhs) { std::swap(_cont, rhs._cont); }

    // Format
    rhi::IndexFormat format() const
    {
        return std::holds_alternative<index16_array_t>(_cont) ? rhi::IndexFormat::U_SHORT : rhi::IndexFormat::U_INT;
    }

    // Clear
    void clear()
    {
        std::visit([](auto& v) { v.clear(); }, _cont);
    }

    void reserve(size_type count)
    {
        std::visit([count](auto& v) { v.reserve(count); }, _cont);
    }

    // Emplace back (typed)
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    void emplace_back(const _Ty& val)
    {
        push_back(val);
    }

    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    void push_back(const _Ty& val)
    {
        auto& vec = std::get<tlx::pod_vector<_Ty>>(_cont);
        vec.push_back(val);
    }

    // Insert range [first,last) at position
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    void insert(_Ty* position, _Ty* first, _Ty* last)
    {
        // Note: position/first/last are expected to come from us; if external, make sure they match format.
        auto& vec      = std::get<tlx::pod_vector<_Ty>>(_cont);
        auto pos_index = static_cast<std::size_t>(position - vec.data());
        vec.insert(vec.begin() + pos_index, first, last);
    }

    // Begin/End (typed) — returns typed pointers, matching original intent
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    const _Ty* begin() const
    {
        auto& vec = std::get<tlx::pod_vector<_Ty>>(_cont);
        return vec.data();
    }
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    _Ty* begin()
    {
        auto& vec = std::get<tlx::pod_vector<_Ty>>(_cont);
        return vec.data();
    }

    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    const _Ty* end() const
    {
        return begin<_Ty>() + size();
    }
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    _Ty* end()
    {
        return begin<_Ty>() + size();
    }

    // Erase single element at position (typed pointer)
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    _Ty* erase(_Ty* position)
    {
        auto& vec = std::get<tlx::pod_vector<_Ty>>(_cont);
        auto idx  = static_cast<std::size_t>(position - vec.data());
        auto it   = vec.erase(vec.begin() + idx);
        return it != vec.end() ? vec.data() + (it - vec.begin()) : vec.data() + vec.size();
    }

    // Erase range [first,last) (typed pointers)
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    _Ty* erase(_Ty* first, _Ty* last)
    {
        auto& vec = std::get<tlx::pod_vector<_Ty>>(_cont);
        auto s    = static_cast<std::size_t>(first - vec.data());
        auto e    = static_cast<std::size_t>(last - vec.data());
        auto it   = vec.erase(vec.begin() + s, vec.begin() + e);
        return it != vec.end() ? vec.data() + (it - vec.begin()) : vec.data() + vec.size();
    }

    // at — reference return, safe (no memcpy)
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    _Ty& at(size_t idx)
    {
        return std::get<tlx::pod_vector<_Ty>>(_cont).at(idx);
    }
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    const _Ty& at(size_t idx) const
    {
        return std::get<tlx::pod_vector<_Ty>>(_cont).at(idx);
    }

    // operator[] — reference return
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    _Ty& operator[](size_t idx)
    {
        return std::get<tlx::pod_vector<_Ty>>(_cont)[idx];
    }
    template <typename _Ty = uint16_t, std::enable_if_t<is_index_format_type_v<_Ty>, int> = 0>
    const _Ty& operator[](size_t idx) const
    {
        return std::get<tlx::pod_vector<_Ty>>(_cont)[idx];
    }

    // data — typed pointer, matching format
    uint8_t* data() noexcept
    {
        // If you truly need raw bytes, reinterpret is safe because the underlying storage is properly aligned.
        // But better expose typed data() to avoid confusion:
        return std::visit([](auto& vec) { return reinterpret_cast<uint8_t*>(vec.data()); }, _cont);
    }
    const uint8_t* data() const noexcept
    {
        return std::visit([](auto const& vec) { return reinterpret_cast<const uint8_t*>(vec.data()); }, _cont);
    }

    // size/count and size_bytes
    size_t size() const
    {
        return std::visit([](auto const& vec) { return vec.size(); }, _cont);
    }
    size_t size_bytes() const
    {
        return std::visit([](auto const& vec) {
            using value_type = typename std::decay_t<decltype(vec)>::value_type;
            return vec.size() * sizeof(value_type);
        }, _cont);
    }

    // resize by count
    void resize(size_type count)
    {
        std::visit([&](auto& vec) { vec.resize(count); }, _cont);
    }

    bool empty() const { return size() == 0; }

    // for_each — keep unified callback with uint32_t values
    template <typename _Fty>
    void for_each(_Fty&& cb) const
    {
        std::visit([&](auto const& vec) {
            for (const auto& v : vec)
                cb(static_cast<uint32_t>(v));
        }, _cont);
    }

    // Convenience: stride query to match original helpers
    unsigned char stride() const
    {
        return std::visit([](auto const& vec) -> unsigned char {
            using value_type = typename std::decay_t<decltype(vec)>::value_type;
            return static_cast<unsigned char>(sizeof(value_type));
        }, _cont);
    }

protected:
    container_type _cont;
};

}  // namespace ax::rhi
