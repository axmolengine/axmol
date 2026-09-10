/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once
#include <type_traits>

namespace tlx
{
// enable_if_convertible_t
template <typename From, typename To, typename R = bool>
using enable_if_convertible_t = std::enable_if_t<std::is_convertible_v<From, To>, R>;

#pragma region is_ref_counted
template <typename _Ty, typename Enable = void>
struct is_ref_counted : std::false_type
{};

template <typename... _Types>
struct is_ref_counted_helper
{};

template <typename _Ty>
struct is_ref_counted<_Ty,
                      std::conditional_t<false,
                                         is_ref_counted_helper<decltype(std::declval<_Ty>().retain()),
                                                               decltype(std::declval<_Ty>().release()),
                                                               decltype(std::declval<_Ty>().getReferenceCount())>,
                                         void>> : public std::true_type
{};

template <typename _Ty>
inline constexpr bool is_ref_counted_v = is_ref_counted<std::remove_pointer_t<_Ty>>::value;
#pragma endregion

#pragma region is_resizable_container
template <typename T, typename Enable = void>
struct is_resizable_container : std::false_type
{};

template <typename... Ts>
struct is_resizable_container_helper
{};

template <typename T>
struct is_resizable_container<T,
                              std::conditional_t<false,
                                                 is_resizable_container_helper<typename T::value_type,
                                                                               decltype(std::declval<T>().size()),
                                                                               decltype(std::declval<T>().resize(0)),
                                                                               decltype(std::declval<T>().data())>,
                                                 void>> : public std::true_type
{};

template <typename _T>
inline constexpr bool is_resizable_container_v = is_resizable_container<_T>::value;
#pragma endregion

template <typename... Ts>
struct select1st;

template <typename First, typename... Rest>
struct select1st<First, Rest...>
{
    using type = First;
};

template <typename... Ts>
using select1st_t = typename select1st<Ts...>::type;

}  // namespace tlx
