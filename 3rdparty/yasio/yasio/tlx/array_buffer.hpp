#pragma once

#include "yasio/tlx/vector.hpp"
#include "yasio/tlx/buffer_alloc.hpp"
#include <type_traits>

namespace tlx
{
// alias: array_buffer
// Usage restriction: only valid for types that meet ALL of the following:
// 1. Trivially default constructible
// 2. Trivially destructible
// 3. Trivially copyable
//
// In other words, this alias is intended only for POD or standard-layout types,
// ensuring that zero-initialization and raw memory operations (e.g. memcpy, memset)
// are safe and well-defined.
template <typename _Ty, typename _Alloc = tlx::crt_buffer_allocator<_Ty>>
using array_buffer = typename std::enable_if<std::is_trivially_copyable<_Ty>::value, ::tlx::vector<_Ty, _Alloc, fill_policy::no_fill_trivial>>::type;
} // namespace tlx
