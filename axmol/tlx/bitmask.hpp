/****************************************************************************
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 Purpose: Make bitmask operators could be use at different namespace,
          the other implementation CCEnumClass.h can't handle.

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include <stdint.h>
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

template <typename _BITMASK>
inline void set(_BITMASK& _Left, _BITMASK _Elements) noexcept
{
    _Left |= _Elements;
}

template <typename _BITMASK>
inline void clear(_BITMASK& _Left, _BITMASK _Elements) noexcept
{
    _Left &= ~_Elements;
}

template <typename _Ty>
static inline constexpr _Ty low_bits(uint32_t n)
{
    return (n >= sizeof(_Ty) * 8) ? ~(_Ty(0)) : (_Ty(1) << n) - _Ty(1);
}
}  // namespace bitmask
