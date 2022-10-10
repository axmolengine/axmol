#pragma once

#include <type_traits>
#include <memory>

namespace axstd
{
/* make_unique_for_overwrite since c++20, but not all platformm support */
template <class _Ty, std::enable_if_t<!std::is_array_v<_Ty>, int> = 0>
inline std::unique_ptr<_Ty> make_unique_for_overwrite()
{
    // make a unique_ptr with default initialization
    return std::unique_ptr<_Ty>(new _Ty);
}

template <class _Ty, std::enable_if_t<std::is_array_v<_Ty> && std::is_trivially_copyable_v<_Ty>, int> = 0>
inline std::unique_ptr<_Ty> make_unique_for_overwrite(const size_t _Size)
{
    // make a unique_ptr with default initialization
    using _Elem = std::remove_extent_t<_Ty>;
    return std::unique_ptr<_Ty>(new _Elem[_Size]);
}
}
