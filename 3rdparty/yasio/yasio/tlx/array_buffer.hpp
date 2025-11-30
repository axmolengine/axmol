#pragma once

#include "yasio/tlx/vector.hpp"
#include "yasio/tlx/buffer_alloc.hpp"
#include <type_traits>

namespace tlx
{
// alias: array_buffer
template <typename _Ty, typename _Alloc = tlx::crt_buffer_allocator<_Ty>>
using array_buffer = typename std::enable_if<std::is_trivially_copyable<_Ty>::value, ::tlx::vector<_Ty, _Alloc>>::type;
} // namespace tlx
