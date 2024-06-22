/****************************************************************************
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Purpose: Make bitmask operators could be use at different namespace,
          the other implementation CCEnumClass.h can't handle.

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

#if !defined(_STD)
#    define _STD ::std::
#endif

// BITMASK OPERATIONS, modified from msvc++ <type_traits> for cross-platform compiling.
#define AX_ENABLE_BITMASK_OPS(_BITMASK)                                                         \
    constexpr _BITMASK operator&(_BITMASK _Left, _BITMASK _Right) noexcept                      \
    { /* return _Left & _Right */                                                               \
        using _IntTy = _STD underlying_type<_BITMASK>::type;                                    \
        return static_cast<_BITMASK>(static_cast<_IntTy>(_Left) & static_cast<_IntTy>(_Right)); \
    }                                                                                           \
                                                                                                \
    constexpr _BITMASK operator|(_BITMASK _Left, _BITMASK _Right) noexcept                      \
    { /* return _Left | _Right */                                                               \
        using _IntTy = _STD underlying_type<_BITMASK>::type;                                    \
        return static_cast<_BITMASK>(static_cast<_IntTy>(_Left) | static_cast<_IntTy>(_Right)); \
    }                                                                                           \
                                                                                                \
    constexpr _BITMASK operator^(_BITMASK _Left, _BITMASK _Right) noexcept                      \
    { /* return _Left ^ _Right */                                                               \
        using _IntTy = _STD underlying_type<_BITMASK>::type;                                    \
        return static_cast<_BITMASK>(static_cast<_IntTy>(_Left) ^ static_cast<_IntTy>(_Right)); \
    }                                                                                           \
                                                                                                \
    constexpr _BITMASK& operator&=(_BITMASK& _Left, _BITMASK _Right) noexcept                   \
    { /* return _Left &= _Right */                                                              \
        return _Left = _Left & _Right;                                                          \
    }                                                                                           \
                                                                                                \
    constexpr _BITMASK& operator|=(_BITMASK& _Left, _BITMASK _Right) noexcept                   \
    { /* return _Left |= _Right */                                                              \
        return _Left = _Left | _Right;                                                          \
    }                                                                                           \
                                                                                                \
    constexpr _BITMASK& operator^=(_BITMASK& _Left, _BITMASK _Right) noexcept                   \
    { /* return _Left ^= _Right */                                                              \
        return _Left = _Left ^ _Right;                                                          \
    }                                                                                           \
                                                                                                \
    constexpr _BITMASK operator~(_BITMASK _Left) noexcept                                       \
    { /* return ~_Left */                                                                       \
        using _IntTy = _STD underlying_type<_BITMASK>::type;                                    \
        return static_cast<_BITMASK>(~static_cast<_IntTy>(_Left));                              \
    }                                                                                           \
                                                                                                \
    constexpr bool operator!(_BITMASK _Left) noexcept                                           \
    { /* return ~_Left */                                                                       \
        using _IntTy = _STD underlying_type<_BITMASK>::type;                                    \
        return !static_cast<_IntTy>(_Left);                                                     \
    }

// BITSHIFT OPERATIONS, inspired from msvc++ <type_traits>.
#define AX_ENABLE_BITSHIFT_OPS(_BITMASK)                                                         \
    constexpr _BITMASK operator>>(_BITMASK _Left, _BITMASK _Right) noexcept                      \
    { /* return _Left & _Right */                                                                \
        using _IntTy = _STD underlying_type<_BITMASK>::type;                                     \
        return static_cast<_BITMASK>(static_cast<_IntTy>(_Left) >> static_cast<_IntTy>(_Right)); \
    }                                                                                            \
                                                                                                 \
    constexpr _BITMASK operator<<(_BITMASK _Left, _BITMASK _Right) noexcept                      \
    { /* return _Left & _Right */                                                                \
        using _IntTy = _STD underlying_type<_BITMASK>::type;                                     \
        return static_cast<_BITMASK>(static_cast<_IntTy>(_Left) << static_cast<_IntTy>(_Right)); \
    }

namespace bitmask
{
template <typename _BITMASK>
constexpr bool none(_BITMASK _Left, _BITMASK _Elements) noexcept
{
    return !(_Left & _Elements);
}

template <typename _BITMASK>
constexpr bool any(_BITMASK _Left, _BITMASK _Elements) noexcept
{
    return !!(_Left & _Elements);
}

template <typename _BITMASK>
constexpr bool only(_BITMASK _Left, _BITMASK _Elements) noexcept
{
    return (_Left & _Elements) == _Elements;
}
}  // namespace bitmask
