//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any 
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)
Copyright (c) 2012-2021 HALX99
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef YASIO__SOL_HPP
#define YASIO__SOL_HPP
#include "yasio/detail/fp16.hpp"

#if YASIO__HAS_CXX14

#  if !YASIO__HAS_CXX20
#    include "sol/sol.hpp"
#  else
#    include "sol3/sol.hpp"
#  endif

#  if !YASIO__HAS_CXX17
namespace sol
{
namespace stack
{
template <> struct pusher<cxx17::string_view> {
  static int push(lua_State* L, const cxx17::string_view& str)
  {
    lua_pushlstring(L, !str.empty() ? str.c_str() : "", str.length());
    return 1;
  }
};
template <> struct getter<cxx17::string_view> {
  static cxx17::string_view get(lua_State* L, int index, record& tracking)
  {
    tracking.use(1); // THIS IS THE ONLY BIT THAT CHANGES
    size_t len    = 0;
    const char* s = lua_tolstring(L, index, &len);
    return cxx17::string_view(s, len);
  }
};
} // namespace stack
template <> struct lua_type_of<cxx17::string_view> : std::integral_constant<type, type::string> {};
} // namespace sol
#  endif

namespace sol
{
namespace stack
{
#  if defined(YASIO_HAVE_HALF_FLOAT)
template <> struct pusher<fp16_t> {
  static int push(lua_State* L, const fp16_t& value)
  {
    lua_pushnumber(L, static_cast<float>(value));
    return 1;
  }
};
template <> struct getter<fp16_t> {
  static fp16_t get(lua_State* L, int index, record& tracking)
  {
    tracking.use(1); // THIS IS THE ONLY BIT THAT CHANGES
    return fp16_t{static_cast<float>(lua_tonumber(L, index))};
  }
};
#  endif
} // namespace stack
#  if defined(YASIO_HAVE_HALF_FLOAT)
template <> struct lua_type_of<fp16_t> : std::integral_constant<type, type::number> {};
#  endif
} // namespace sol

#endif

#endif
