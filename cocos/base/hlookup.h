// C++20 demo: Heterogeneous lookup for unordered containers (transparent hashing)
// https://en.cppreference.com/w/cpp/container/unordered_map/find
#pragma once
#include <string>
#include <string_view>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include "tsl/robin_map.h"
#include "tsl/robin_set.h"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace hlookup
{
struct string_hash
{
    using hash_type      = std::hash<std::string_view>;
    using is_transparent = void;

    size_t operator()(const char* str) const { return hash_type{}(str); }
    size_t operator()(std::string_view str) const { return hash_type{}(str); }
    size_t operator()(std::string const& str) const { return hash_type{}(str); }
};

struct equal_to
{
    template <class _Ty1, class _Ty2>
    constexpr auto operator()(_Ty1&& _Left, _Ty2&& _Right) const
        noexcept(noexcept(static_cast<_Ty1&&>(_Left) == static_cast<_Ty2&&>(_Right)))  // strengthened
        -> decltype(static_cast<_Ty1&&>(_Left) == static_cast<_Ty2&&>(_Right))
    {
        return static_cast<_Ty1&&>(_Left) == static_cast<_Ty2&&>(_Right);
    }

    using is_transparent = int;
};

template <typename _Valty>
using stl_string_map = std::map<std::string, _Valty, std::less<>>;
using stl_string_set = std::set<std::string, std::less<>>;

template <typename _Valty>
using string_map = tsl::robin_map<std::string, _Valty, string_hash, equal_to>;
using string_set = tsl::robin_set<std::string, string_hash, equal_to>;

template <typename _Cont, typename _Valty>
inline auto set_item(_Cont& cont, std::string_view key, _Valty&& _Val)
{
    typename _Cont::iterator it = cont.find(key);
    if (it != cont.end())
        it.value() = std::move(_Val);
    else
        it = cont.emplace(std::string{key}, std::forward<_Valty>(_Val)).first;
    return it;
}

constexpr auto empty_sv = ""sv;

}  // namespace hlookup
