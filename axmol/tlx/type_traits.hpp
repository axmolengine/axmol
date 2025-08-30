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
#include <type_traits>

namespace axstd
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
}  // namespace axstd
