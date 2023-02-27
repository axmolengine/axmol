//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any 
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2023 HALX99
Copyright (c) 2016 Matthew Rodusek(matthew.rodusek@gmail.com) <http://rodusek.me>

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
See: https://github.com/bitwizeshift/string_view-standalone
*/
#ifndef YASIO__STRING_VIEW_HPP
#define YASIO__STRING_VIEW_HPP
#include <ctype.h>
#include <string.h>
#include <wchar.h>
#include <string>
#include "yasio/compiler/feature_test.hpp"

/// wcsncasecmp workaround for android API level < 23, copy from msvc ucrt 10.0.18362.0 'wcsnicmp'
#if (defined(__ANDROID_API__) && __ANDROID_API__ < 23) || defined(__MINGW32__)
inline int wcsncasecmp(wchar_t const* const lhs, wchar_t const* const rhs, size_t const count)
{
  if (count == 0)
  {
    return 0;
  }

  wchar_t const* lhs_ptr = reinterpret_cast<wchar_t const*>(lhs);
  wchar_t const* rhs_ptr = reinterpret_cast<wchar_t const*>(rhs);

  int result;
  int lhs_value;
  int rhs_value;
  size_t remaining = count;
  do
  {
    lhs_value = ::towlower(*lhs_ptr++);
    rhs_value = ::towlower(*rhs_ptr++);
    result    = lhs_value - rhs_value;
  } while (result == 0 && lhs_value != 0 && --remaining != 0);

  return result;
}
#endif

/// string_view workaround on c++11
#if YASIO__HAS_CXX17
#  if __has_include(<string_view>)
#    include <string_view>
#  endif
#else
#  include <algorithm>
#  include <exception>
#  include <stdexcept>
namespace cxx17
{
template <class _Traits>
inline size_t __xxtraits_find(const typename _Traits::char_type* _Haystack, const size_t _Hay_size,
                              const size_t _Start_at, const typename _Traits::char_type* _Needle,
                              const size_t _Needle_size)
{ // search [_Haystack, _Haystack +
  // _Hay_size) for [_Needle, _Needle +
  // _Needle_size), at/after _Start_at
  if (_Needle_size > _Hay_size || _Start_at > _Hay_size - _Needle_size)
  { // xpos cannot exist, report failure
    // N4659 24.3.2.7.2 [string.find]/1 says:
    // 1. _Start_at <= xpos
    // 2. xpos + _Needle_size <= _Hay_size;
    // therefore:
    // 3. _Needle_size <= _Hay_size (by 2)
    // (checked above)
    // 4. _Start_at + _Needle_size <=
    // _Hay_size (substitute 1 into 2)
    // 5. _Start_at <= _Hay_size -
    // _Needle_size (4, move _Needle_size to
    // other side) (also checked above)
    return static_cast<size_t>(-1);
  }

  if (_Needle_size == 0)
  { // empty string always matches if xpos is possible
    return _Start_at;
  }

  const auto _Possible_matches_end = _Haystack + (_Hay_size - _Needle_size) + 1;
  for (auto _Match_try = _Haystack + _Start_at;; ++_Match_try)
  {
    _Match_try = _Traits::find(_Match_try, static_cast<size_t>(_Possible_matches_end - _Match_try),
                               *_Needle);
    if (!_Match_try)
    { // didn't find first character; report failure
      return static_cast<size_t>(-1);
    }

    if (_Traits::compare(_Match_try, _Needle, _Needle_size) == 0)
    { // found match
      return static_cast<size_t>(_Match_try - _Haystack);
    }
  }
}

template <class _Traits>
inline size_t __xxtraits_find_ch(const typename _Traits::char_type* _Haystack,
                                 const size_t _Hay_size, const size_t _Start_at,
                                 const typename _Traits::char_type _Ch)
{ // search [_Haystack, _Haystack + _Hay_size)
  // for _Ch, at/after _Start_at
  if (_Start_at < _Hay_size)
  {
    const auto _Found_at = _Traits::find(_Haystack + _Start_at, _Hay_size - _Start_at, _Ch);
    if (_Found_at)
    {
      return static_cast<size_t>(_Found_at - _Haystack);
    }
  }

  return static_cast<size_t>(-1); // (npos) no match
}

template <class _Traits>
inline size_t __xxtraits_rfind(const typename _Traits::char_type* _Haystack, const size_t _Hay_size,
                               const size_t _Start_at, const typename _Traits::char_type* _Needle,
                               const size_t _Needle_size)
{ // search [_Haystack, _Haystack + _Hay_size)
  // for [_Needle, _Needle + _Needle_size)
  // beginning before _Start_at
  if (_Needle_size == 0)
  {
    return (std::min)(_Start_at, _Hay_size); // empty string always matches
  }

  if (_Needle_size <= _Hay_size)
  { // room for match, look for it
    for (auto _Match_try = _Haystack + (std::min)(_Start_at, _Hay_size - _Needle_size);;
         --_Match_try)
    {
      if (_Traits::eq(*_Match_try, *_Needle) &&
          _Traits::compare(_Match_try, _Needle, _Needle_size) == 0)
      {
        return static_cast<size_t>(_Match_try - _Haystack); // found a match
      }

      if (_Match_try == _Haystack)
      {
        break; // at beginning, no more chance for match
      }
    }
  }

  return static_cast<size_t>(-1); // no match
}

template <class _Traits>
inline size_t __xxtraits_rfind_ch(const typename _Traits::char_type* _Haystack,
                                  const size_t _Hay_size, const size_t _Start_at,
                                  const typename _Traits::char_type _Ch)
{ // search [_Haystack, _Haystack +
  // _Hay_size) for _Ch before _Start_at
  if (_Hay_size != 0)
  { // room for match, look for it
    for (auto _Match_try = _Haystack + (std::min)(_Start_at, _Hay_size - 1);; --_Match_try)
    {
      if (_Traits::eq(*_Match_try, _Ch))
      {
        return static_cast<size_t>(_Match_try - _Haystack); // found a match
      }

      if (_Match_try == _Haystack)
      {
        break; // at beginning, no more chance for match
      }
    }
  }

  return static_cast<size_t>(-1); // no match
}

template <class _Traits>
inline size_t __xxtraits_find_last_of(const typename _Traits::char_type* _Haystack,
                                      const size_t _Hay_size, const size_t _Start_at,
                                      const typename _Traits::char_type* _Needle,
                                      const size_t _Needle_size)
{ // in [_Haystack, _Haystack + _Hay_size), look for
  // last of [_Needle, _Needle + _Needle_size), before
  // _Start_at general algorithm
  if (_Needle_size != 0 && _Hay_size != 0)
  { // worth searching, do it
    for (auto _Match_try = _Haystack + (std::min)(_Start_at, _Hay_size - 1);; --_Match_try)
    {
      if (_Traits::find(_Needle, _Needle_size, *_Match_try))
      {
        return static_cast<size_t>(_Match_try - _Haystack); // found a match
      }

      if (_Match_try == _Haystack)
      {
        break; // at beginning, no more chance for match
      }
    }
  }

  return static_cast<size_t>(-1); // no match
}

template <class _Traits>
inline size_t _xxtraits_find_last_not_of(const typename _Traits::char_type* _Haystack,
                                         const size_t _Hay_size, const size_t _Start_at,
                                         const typename _Traits::char_type* _Needle,
                                         const size_t _Needle_size)
{ // in [_Haystack, _Haystack + _Hay_size), look for
  // none of [_Needle, _Needle + _Needle_size), before
  // _Start_at general algorithm
  if (_Hay_size != 0)
  { // worth searching, do it
    for (auto _Match_try = _Haystack + (std::min)(_Start_at, _Hay_size - 1);; --_Match_try)
    {
      if (!_Traits::find(_Needle, _Needle_size, *_Match_try))
      {
        return static_cast<size_t>(_Match_try - _Haystack); // found a match
      }

      if (_Match_try == _Haystack)
      {
        break; // at beginning, no more chance for match
      }
    }
  }

  return static_cast<size_t>(-1); // no match
}

template <class _Traits>
inline size_t __xxtraits_find_first_of(const typename _Traits::char_type* _Haystack,
                                       const size_t _Hay_size, const size_t _Start_at,
                                       const typename _Traits::char_type* _Needle,
                                       const size_t _Needle_size)
{ // in [_Haystack, _Haystack + _Hay_size), look for
  // one of [_Needle, _Needle + _Needle_size), at/after
  // _Start_at general algorithm
  if (_Needle_size != 0 && _Start_at < _Hay_size)
  { // room for match, look for it
    const auto _End = _Haystack + _Hay_size;
    for (auto _Match_try = _Haystack + _Start_at; _Match_try < _End; ++_Match_try)
    {
      if (_Traits::find(_Needle, _Needle_size, *_Match_try))
      {
        return (static_cast<size_t>(_Match_try - _Haystack)); // found a match
      }
    }
  }

  return (static_cast<size_t>(-1)); // no match
}

template <class _Traits>
inline size_t __xxtraits_find_first_not_of(const typename _Traits::char_type* _Haystack,
                                           const size_t _Hay_size, const size_t _Start_at,
                                           const typename _Traits::char_type* _Needle,
                                           const size_t _Needle_size)
{ // in [_Haystack, _Haystack + _Hay_size), look for none
  // of [_Needle, _Needle + _Needle_size), at/after
  // _Start_at general algorithm
  if (_Start_at < _Hay_size)
  { // room for match, look for it
    const auto _End = _Haystack + _Hay_size;
    for (auto _Match_try = _Haystack + _Start_at; _Match_try < _End; ++_Match_try)
    {
      if (!_Traits::find(_Needle, _Needle_size, *_Match_try))
      {
        return (static_cast<size_t>(_Match_try - _Haystack)); // found a match
      }
    }
  }

  return (static_cast<size_t>(-1)); // no match
}

template <class _Traits>
inline size_t __xxtraits_find_not_ch(const typename _Traits::char_type* _Haystack,
                                     const size_t _Hay_size, const size_t _Start_at,
                                     const typename _Traits::char_type _Ch)
{ // search [_Haystack, _Haystack + _Hay_size) for any value other
  // than _Ch, at/after _Start_at
  if (_Start_at < _Hay_size)
  { // room for match, look for it
    const auto _End = _Haystack + _Hay_size;
    for (auto _Match_try = _Haystack + _Start_at; _Match_try < _End; ++_Match_try)
    {
      if (!_Traits::eq(*_Match_try, _Ch))
      {
        return (static_cast<size_t>(_Match_try - _Haystack)); // found a match
      }
    }
  }

  return (static_cast<size_t>(-1)); // no match
}

template <class _Traits>
inline size_t __xxtraits_rfind_not_ch(const typename _Traits::char_type* _Haystack,
                                      const size_t _Hay_size, const size_t _Start_at,
                                      const typename _Traits::char_type _Ch)
{ // search [_Haystack, _Haystack + _Hay_size) for any value other than _Ch before _Start_at
  if (_Hay_size != 0)
  { // room for match, look for it
    for (auto _Match_try = _Haystack + (std::min)(_Start_at, _Hay_size - 1);; --_Match_try)
    {
      if (!_Traits::eq(*_Match_try, _Ch))
      {
        return (static_cast<size_t>(_Match_try - _Haystack)); // found a match
      }

      if (_Match_try == _Haystack)
      {
        break; // at beginning, no more chance for match
      }
    }
  }

  return (static_cast<size_t>(-1)); // no match
}

////////////////////////////////////////////////////////////////////////////
/// \brief A wrapper around non-owned strings.
///
/// This is an implementation of the C++17 string_view proposal
///
/// \ingroup core
////////////////////////////////////////////////////////////////////////////
template <typename _CharT, typename _Traits = std::char_traits<_CharT>> class basic_string_view;

template <typename _CharT, typename _Traits> class basic_string_view
{
  //------------------------------------------------------------------------
  // Public Member Types
  //------------------------------------------------------------------------
public:
  using char_type   = _CharT;
  using traits_type = _Traits;
  using size_type   = size_t;

  using value_type      = _CharT;
  using reference       = value_type&;
  using const_reference = const value_type&;
  using pointer         = value_type*;
  using const_pointer   = const value_type*;

  using iterator       = const _CharT*;
  using const_iterator = const _CharT*;

  //------------------------------------------------------------------------
  // Public Members
  //------------------------------------------------------------------------
public:
  static const size_type npos = size_type(-1);

  //------------------------------------------------------------------------
  // Constructors
  //------------------------------------------------------------------------
public:
  /// \brief Default constructs a basic_string_view without any content
  basic_string_view();

  /// \brief Constructs a basic_string_view from a std::basic_string
  ///
  /// \param str the string to view
  template <typename Allocator>
  basic_string_view(const std::basic_string<_CharT, _Traits, Allocator>& str);

  /// \brief Constructs a basic_string_view from an ansi-string
  ///
  /// \param str the string to view
  basic_string_view(const char_type* str);

  /// \brief Constructs a basic_string_view from an ansi string of a given size
  ///
  /// \param str the string to view
  /// \param count the size of the string
  basic_string_view(const char_type* str, size_type count);

  //------------------------------------------------------------------------
  // Assignment
  //------------------------------------------------------------------------
public:
  //------------------------------------------------------------------------
  // Capacity
  //------------------------------------------------------------------------
public:
  /// \brief Returns the length of the string, in terms of bytes
  ///
  /// \return the length of the string, in terms of bytes
  size_type size() const;

  /// \copydoc basic_string_view::size
  size_type length() const;

  /// \brief The largest possible number of char-like objects that can be
  ///        referred to by a basic_string_view.
  /// \return Maximum number of characters
  size_type max_size() const;

  /// \brief Returns whether the basic_string_view is empty
  ///        (i.e. whether its length is 0).
  ///
  /// \return whether the basic_string_view is empty
  bool empty() const;

  //------------------------------------------------------------------------
  // Element Access
  //------------------------------------------------------------------------
public:
  /// \brief Gets the ansi-string of the current basic_string_view
  ///
  /// \return the ansi-string pointer
  const char_type* c_str() const;

  /// \brief Gets the data of the current basic_string_view
  ///
  /// \note This is an alias of #c_str
  ///
  /// \return the data this basic_string_view contains
  const char_type* data() const;

  /// \brief Accesses the element at index \p pos
  ///
  /// \param pos the index to access
  /// \return const reference to the character
  const_reference operator[](size_t pos) const;

  /// \brief Accesses the element at index \p pos
  ///
  /// \param pos the index to access
  /// \return const reference to the character
  const_reference at(size_t pos) const;

  /// \brief Access the first character of the string
  ///
  /// \note Undefined behavior if basic_string_view is empty
  ///
  /// \return reference to the first character of the string
  const_reference front() const;

  /// \brief References the last character of the string
  ///
  /// \note Undefined behavior if basic_string_view is empty
  ///
  /// \return reference to the last character of the string
  const_reference back() const;

  //------------------------------------------------------------------------
  // Modifiers
  //------------------------------------------------------------------------
public:
  /// \brief Moves the start of the view forward by n characters.
  ///
  /// The behavior is undefined if n > size().
  ///
  /// \param n number of characters to remove from the start of the view
  void remove_prefix(size_type n);

  /// \brief Moves the end of the view back by n characters.
  ///
  /// The behavior is undefined if n > size().
  ///
  /// \param n number of characters to remove from the end of the view
  void remove_suffix(size_type n);

  /// \brief Exchanges the view with that of v.
  ///
  /// \param v view to swap with
  void swap(basic_string_view& v);

  //------------------------------------------------------------------------
  // Conversions
  //------------------------------------------------------------------------
public:
  /// \brief Creates a basic_string with a copy of the content of the current
  /// view.
  ///
  /// \tparam Allocator type used to allocate internal storage
  /// \param a Allocator instance to use for allocating the new string
  ///
  /// \return A basic_string containing a copy of the characters of the current
  /// view.
  template <class Allocator = std::allocator<_CharT>>
  std::basic_string<_CharT, _Traits, Allocator> to_string(const Allocator& a = Allocator()) const;

  /// \copydoc basic_string_view::to_string
  template <class Allocator>
  explicit operator std::basic_string<_CharT, _Traits, Allocator>() const;

  //------------------------------------------------------------------------
  // Operations
  //------------------------------------------------------------------------
public:
  /// \brief Copies the substring [pos, pos + rcount) to the character string
  /// pointed
  ///        to by dest, where rcount is the smaller of count and size() - pos.
  ///
  /// \param dest pointer to the destination character string
  /// \param count requested substring length
  /// \param pos position of the first character
  size_type copy(char_type* dest, size_type count = npos, size_type pos = 0) const;

  /// \brief Returns a substring of this viewed string
  ///
  /// \param pos the position of the first character in the substring
  /// \param len the length of the substring
  /// \return the created substring
  basic_string_view substr(size_t pos = 0, size_t len = npos) const;

  //------------------------------------------------------------------------

  /// \brief Compares two character sequences
  ///
  /// \param v view to compare
  /// \return negative value if this view is less than the other character
  ///         sequence, zero if the both character sequences are equal, positive
  ///         value if this view is greater than the other character sequence.
  int compare(basic_string_view v) const;

  /// \brief Compares two character sequences
  ///
  /// \param pos   position of the first character in this view to compare
  /// \param count number of characters of this view to compare
  /// \param v     view to compare
  /// \return negative value if this view is less than the other character
  ///         sequence, zero if the both character sequences are equal, positive
  ///         value if this view is greater than the other character sequence.
  int compare(size_type pos, size_type count, basic_string_view v) const;

  /// \brief Compares two character sequences
  ///
  /// \param pos1   position of the first character in this view to compare
  /// \param count1 number of characters of this view to compare
  /// \param v      view to compare
  /// \param pos2   position of the second character in this view to compare
  /// \param count2 number of characters of the given view to compare
  /// \return negative value if this view is less than the other character
  ///         sequence, zero if the both character sequences are equal, positive
  ///         value if this view is greater than the other character sequence.
  int compare(size_type pos1, size_type count1, basic_string_view v, size_type pos2,
              size_type count2) const;

  /// \brief Compares two character sequences
  ///
  /// \param s pointer to the character string to compare to
  /// \return negative value if this view is less than the other character
  ///         sequence, zero if the both character sequences are equal, positive
  ///         value if this view is greater than the other character sequence.
  int compare(const char_type* s) const;

  /// \brief Compares two character sequences
  ///
  /// \param pos   position of the first character in this view to compare
  /// \param count number of characters of this view to compare
  /// \param s pointer to the character string to compare to
  /// \return negative value if this view is less than the other character
  ///         sequence, zero if the both character sequences are equal, positive
  ///         value if this view is greater than the other character sequence.
  int compare(size_type pos, size_type count, const char_type* s) const;

  /// \brief Compares two character sequences
  ///
  /// \param pos   position of the first character in this view to compare
  /// \param count1 number of characters of this view to compare
  /// \param s pointer to the character string to compare to
  /// \param count2 number of characters of the given view to compare
  /// \return negative value if this view is less than the other character
  ///         sequence, zero if the both character sequences are equal, positive
  ///         value if this view is greater than the other character sequence.
  int compare(size_type pos, size_type count1, const char_type* s, size_type count2) const;

  //------------------------------------------------------------------------

  size_type find(basic_string_view v, size_type pos = 0) const;

  size_type find(char_type c, size_type pos = 0) const;

  size_type find(const char_type* s, size_type pos, size_type count) const;

  size_type find(const char_type* s, size_type pos = 0) const;

  //------------------------------------------------------------------------

  size_type rfind(basic_string_view v, size_type pos = npos) const;

  size_type rfind(char_type c, size_type pos = npos) const;

  size_type rfind(const char_type* s, size_type pos, size_type count) const;

  size_type rfind(const char_type* s, size_type pos = npos) const;

  //------------------------------------------------------------------------

  size_type find_first_of(basic_string_view v, size_type pos = 0) const;

  size_type find_first_of(char_type c, size_type pos = 0) const;

  size_type find_first_of(const char_type* s, size_type pos, size_type count) const;

  size_type find_first_of(const char_type* s, size_type pos = 0) const;

  //------------------------------------------------------------------------

  size_type find_last_of(basic_string_view v, size_type pos = npos) const;

  size_type find_last_of(char_type c, size_type pos = npos) const;

  size_type find_last_of(const char_type* s, size_type pos, size_type count) const;

  size_type find_last_of(const char_type* s, size_type pos = npos) const;

  //------------------------------------------------------------------------

  size_type find_first_not_of(basic_string_view v, size_type pos = 0) const;

  size_type find_first_not_of(char_type c, size_type pos = 0) const;

  size_type find_first_not_of(const char_type* s, size_type pos, size_type count) const;

  size_type find_first_not_of(const char_type* s, size_type pos = 0) const;

  //------------------------------------------------------------------------

  size_type find_last_not_of(basic_string_view v, size_type pos = npos) const;

  size_type find_last_not_of(char_type c, size_type pos = npos) const;

  size_type find_last_not_of(const char_type* s, size_type pos, size_type count) const;

  size_type find_last_not_of(const char_type* s, size_type pos = npos) const;

  //------------------------------------------------------------------------
  // Iterators
  //------------------------------------------------------------------------
public:
  /// \brief Retrieves the begin iterator for this basic_string_view
  ///
  /// \return the begin iterator
  const_iterator begin() const;

  /// \brief Retrieves the end iterator for this basic_string_view
  ///
  /// \return the end iterator
  const_iterator end() const;

  /// \copydoc basic_string_view::begin()
  const_iterator cbegin() const;

  /// \copydoc basic_string_view::end()
  const_iterator cend() const;

  //------------------------------------------------------------------------
  // Private Member
  //------------------------------------------------------------------------
private:
  const char_type* m_str; ///< The internal string type
  size_type m_size;       ///< The size of this string
};

//--------------------------------------------------------------------------
// Type Aliases
//--------------------------------------------------------------------------

using string_view    = basic_string_view<char>;
using wstring_view   = basic_string_view<wchar_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;

//--------------------------------------------------------------------------
// Inline Definitions
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline basic_string_view<_CharT, _Traits>::basic_string_view() : m_str(nullptr), m_size(0)
{}

template <typename _CharT, typename _Traits>
template <typename Allocator>
inline basic_string_view<_CharT, _Traits>::basic_string_view(
    const std::basic_string<_CharT, _Traits, Allocator>& str)
    : m_str(str.c_str()), m_size(str.size())
{}

template <typename _CharT, typename _Traits>
inline basic_string_view<_CharT, _Traits>::basic_string_view(const char_type* str)
    : m_str(str), m_size(traits_type::length(str))
{}

template <typename _CharT, typename _Traits>
inline basic_string_view<_CharT, _Traits>::basic_string_view(const char_type* str, size_type count)
    : m_str(str), m_size(count)
{}

//--------------------------------------------------------------------------
// Capacity
//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::size() const
{
  return m_size;
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::length() const
{
  return size();
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::max_size() const
{
  return npos - 1;
}

template <typename _CharT, typename _Traits>
inline bool basic_string_view<_CharT, _Traits>::empty() const
{
  return m_size == 0;
}

//--------------------------------------------------------------------------
// Element Access
//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline const typename basic_string_view<_CharT, _Traits>::char_type*
basic_string_view<_CharT, _Traits>::c_str() const
{
  return m_str;
}

template <typename _CharT, typename _Traits>
inline const typename basic_string_view<_CharT, _Traits>::char_type*
basic_string_view<_CharT, _Traits>::data() const
{
  return m_str;
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::const_reference
basic_string_view<_CharT, _Traits>::operator[](size_t pos) const
{
  return m_str[pos];
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::const_reference
basic_string_view<_CharT, _Traits>::at(size_t pos) const
{
  if (yasio__unlikely(pos >= m_size))
    YASIO__THROW(std::out_of_range("Input out of range in basic_string_view::at"), 0);
  return m_str[pos];
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::const_reference
basic_string_view<_CharT, _Traits>::front() const
{
  return *m_str;
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::const_reference
basic_string_view<_CharT, _Traits>::back() const
{
  return m_str[m_size - 1];
}

//--------------------------------------------------------------------------
// Modifiers
//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline void basic_string_view<_CharT, _Traits>::remove_prefix(size_type n)
{
  m_str += n, m_size -= n;
}

template <typename _CharT, typename _Traits>
inline void basic_string_view<_CharT, _Traits>::remove_suffix(size_type n)
{
  m_size -= n;
}

template <typename _CharT, typename _Traits>
inline void basic_string_view<_CharT, _Traits>::swap(basic_string_view& v)
{
  using std::swap;
  swap(m_size, v.m_size);
  swap(m_str, v.m_str);
}

//--------------------------------------------------------------------------
// Conversions
//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
template <class Allocator>
inline std::basic_string<_CharT, _Traits, Allocator>
basic_string_view<_CharT, _Traits>::to_string(const Allocator& a) const
{
  return std::basic_string<_CharT, _Traits, Allocator>(m_str, m_size, a);
}

template <typename _CharT, typename _Traits>
template <class Allocator>
inline basic_string_view<_CharT, _Traits>::operator std::basic_string<_CharT, _Traits, Allocator>()
    const
{
  return std::basic_string<_CharT, _Traits, Allocator>(m_str, m_size);
}

//--------------------------------------------------------------------------
// String Operations
//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::copy(char_type* dest, size_type count, size_type pos) const
{
  if (yasio__unlikely(pos >= m_size))
    YASIO__THROW(std::out_of_range("Index out of range in basic_string_view::copy"), 0);

  const size_type rcount = (std::min)(m_size - pos, count + 1);
  std::copy(m_str + pos, m_str + pos + rcount, dest);
  return rcount;
}

template <typename _CharT, typename _Traits>
inline basic_string_view<_CharT, _Traits>
basic_string_view<_CharT, _Traits>::substr(size_t pos, size_t len) const
{
  const size_type max_length = pos > m_size ? 0 : m_size - pos;
  if (yasio__unlikely(pos >= m_size))
    YASIO__THROW(std::out_of_range("Index out of range in basic_string_view::substr"), (basic_string_view<_CharT, _Traits>{}));
  return basic_string_view<_CharT, _Traits>(m_str + pos, len > max_length ? max_length : len);
}

//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline int basic_string_view<_CharT, _Traits>::compare(basic_string_view v) const
{
  const size_type rlen = (std::min)(m_size, v.m_size);
  const int compare    = _Traits::compare(m_str, v.m_str, rlen);

  return (compare ? compare : (m_size < v.m_size ? -1 : (m_size > v.m_size ? 1 : 0)));
}

template <typename _CharT, typename _Traits>
inline int basic_string_view<_CharT, _Traits>::compare(size_type pos, size_type count,
                                                       basic_string_view v) const
{
  return substr(pos, count).compare(v);
}

template <typename _CharT, typename _Traits>
inline int basic_string_view<_CharT, _Traits>::compare(size_type pos1, size_type count1,
                                                       basic_string_view v, size_type pos2,
                                                       size_type count2) const
{
  return substr(pos1, count1).compare(v.substr(pos2, count2));
}

template <typename _CharT, typename _Traits>
inline int basic_string_view<_CharT, _Traits>::compare(const char_type* s) const
{
  return compare(basic_string_view<_CharT, _Traits>(s));
}

template <typename _CharT, typename _Traits>
inline int basic_string_view<_CharT, _Traits>::compare(size_type pos, size_type count,
                                                       const char_type* s) const
{
  return substr(pos, count).compare(basic_string_view<_CharT, _Traits>(s));
}

template <typename _CharT, typename _Traits>
inline int basic_string_view<_CharT, _Traits>::compare(size_type pos, size_type count1,
                                                       const char_type* s, size_type count2) const
{
  return substr(pos, count1).compare(basic_string_view<_CharT, _Traits>(s, count2));
}

//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find(basic_string_view v, size_type pos) const
{
  return __xxtraits_find<_Traits>(m_str, m_size, pos, v.m_str, v.m_size);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find(char_type c, size_type pos) const
{
  return find(basic_string_view<_CharT, _Traits>(&c, 1), pos);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find(const char_type* s, size_type pos, size_type count) const
{
  return find(basic_string_view<_CharT, _Traits>(s, count), pos);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find(const char_type* s, size_type pos) const
{
  return find(basic_string_view<_CharT, _Traits>(s), pos);
}

//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::rfind(basic_string_view v, size_type pos) const
{
  return __xxtraits_rfind<_Traits>(m_str, m_size, pos, v.m_str, v.m_size);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::rfind(char_type c, size_type pos) const
{
  return rfind(basic_string_view<_CharT, _Traits>(&c, 1), pos);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::rfind(const char_type* s, size_type pos, size_type count) const
{
  return rfind(basic_string_view<_CharT, _Traits>(s, count), pos);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::rfind(const char_type* s, size_type pos) const
{
  return rfind(basic_string_view<_CharT, _Traits>(s), pos);
}

//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_of(basic_string_view v, size_type pos) const
{
  return __xxtraits_find_first_of<_Traits>(m_str, m_size, pos, v.m_str, v.m_size);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_of(char_type c, size_type pos) const
{
  return __xxtraits_find_ch<_Traits>(m_str, m_size, pos, c);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_of(const char_type* s, size_type pos,
                                                  size_type count) const
{
  return find_first_of(basic_string_view<_CharT, _Traits>(s, count), pos);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_of(const char_type* s, size_type pos) const
{
  return find_first_of(basic_string_view<_CharT, _Traits>(s), pos);
}

//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_of(basic_string_view v, size_type pos) const
{
  return __xxtraits_find_last_of<_Traits>(m_str, m_size, pos, v.m_str, v.m_size);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_of(char_type c, size_type pos) const
{
  return __xxtraits_rfind_ch<_Traits>(m_str, m_size, pos, c);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_of(const char_type* s, size_type pos,
                                                 size_type count) const
{
  return find_last_of(basic_string_view<_CharT, _Traits>(s, count), pos);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_of(const char_type* s, size_type pos) const
{
  return find_last_of(basic_string_view<_CharT, _Traits>(s), pos);
}

//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_not_of(basic_string_view v, size_type pos) const
{
  return __xxtraits_find_first_not_of<_Traits>(m_str, m_size, pos, v.m_str, v.m_size, pos);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_not_of(char_type c, size_type pos) const
{
  return __xxtraits_find_not_ch<_Traits>(m_str, m_size, pos, c);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_not_of(const char_type* s, size_type pos,
                                                      size_type count) const
{
  return find_first_not_of(basic_string_view<_CharT, _Traits>(s, count), pos);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_first_not_of(const char_type* s, size_type pos) const
{
  return find_first_not_of(basic_string_view<_CharT, _Traits>(s), pos);
}

//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_not_of(basic_string_view v, size_type pos) const
{
  return _xxtraits_find_last_not_of(m_str, m_size, pos, v.m_str, v.m_size);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_not_of(char_type c, size_type pos) const
{
  return __xxtraits_rfind_not_ch<_Traits>(m_str, m_size, pos, c);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_not_of(const char_type* s, size_type pos,
                                                     size_type count) const
{
  return find_last_not_of(basic_string_view<_CharT, _Traits>(s, count), pos);
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::size_type
basic_string_view<_CharT, _Traits>::find_last_not_of(const char_type* s, size_type pos) const
{
  return find_last_not_of(basic_string_view<_CharT, _Traits>(s), pos);
}

//--------------------------------------------------------------------------
// Iterator
//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::const_iterator
basic_string_view<_CharT, _Traits>::begin() const
{
  return m_str;
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::const_iterator
basic_string_view<_CharT, _Traits>::end() const
{
  return m_str + m_size;
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::const_iterator
basic_string_view<_CharT, _Traits>::cbegin() const
{
  return m_str;
}

template <typename _CharT, typename _Traits>
inline typename basic_string_view<_CharT, _Traits>::const_iterator
basic_string_view<_CharT, _Traits>::cend() const
{
  return m_str + m_size;
}

//--------------------------------------------------------------------------
// Public Functions
//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
std::basic_ostream<_CharT, _Traits>& operator<<(std::basic_ostream<_CharT, _Traits>& o,
                                                const basic_string_view<_CharT, _Traits>& str)
{
  o.write(str.data(), str.size());
  return o;
}

template <typename _CharT, typename _Traits>
inline void swap(basic_string_view<_CharT, _Traits>& lhs, basic_string_view<_CharT, _Traits>& rhs)
{
  lhs.swap(rhs);
}

//--------------------------------------------------------------------------
// Comparison Functions
//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline bool operator==(const basic_string_view<_CharT, _Traits>& lhs,
                       const basic_string_view<_CharT, _Traits>& rhs)
{
  return lhs.compare(rhs) == 0;
}

template <typename _CharT, typename _Traits>
inline bool operator==(basic_string_view<_CharT, _Traits> lhs, const _CharT* rhs)
{
  return lhs == basic_string_view<_CharT, _Traits>(rhs);
}

template <typename _CharT, typename _Traits>
inline bool operator==(const _CharT* lhs, const basic_string_view<_CharT, _Traits>& rhs)
{
  return basic_string_view<_CharT, _Traits>(lhs) == rhs;
}

template <typename _CharT, typename _Traits, typename Allocator>
inline bool operator==(const std::basic_string<_CharT, _Traits, Allocator>& lhs,
                       const basic_string_view<_CharT, _Traits>& rhs)
{
  return basic_string_view<_CharT, _Traits>(lhs) == rhs;
}

template <typename _CharT, typename _Traits, typename Allocator>
inline bool operator==(const basic_string_view<_CharT, _Traits>& lhs,
                       const std::basic_string<_CharT, _Traits, Allocator>& rhs)
{
  return lhs == basic_string_view<_CharT, _Traits>(rhs);
}

//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline bool operator!=(const basic_string_view<_CharT, _Traits>& lhs,
                       const basic_string_view<_CharT, _Traits>& rhs)
{
  return lhs.compare(rhs) != 0;
}

template <typename _CharT, typename _Traits>
inline bool operator!=(const basic_string_view<_CharT, _Traits>& lhs, const _CharT* rhs)
{
  return lhs != basic_string_view<_CharT, _Traits>(rhs);
}

template <typename _CharT, typename _Traits>
inline bool operator!=(const _CharT* lhs, const basic_string_view<_CharT, _Traits>& rhs)
{
  return basic_string_view<_CharT, _Traits>(lhs) != rhs;
}

template <typename _CharT, typename _Traits, typename Allocator>
inline bool operator!=(const std::basic_string<_CharT, _Traits, Allocator>& lhs,
                       const basic_string_view<_CharT, _Traits>& rhs)
{
  return basic_string_view<_CharT, _Traits>(lhs) != rhs;
}

template <typename _CharT, typename _Traits, typename Allocator>
inline bool operator!=(const basic_string_view<_CharT, _Traits>& lhs,
                       const std::basic_string<_CharT, _Traits, Allocator>& rhs)
{
  return lhs != basic_string_view<_CharT, _Traits>(rhs);
}
//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline bool operator<(const basic_string_view<_CharT, _Traits>& lhs,
                      const basic_string_view<_CharT, _Traits>& rhs)
{
  return lhs.compare(rhs) < 0;
}

template <typename _CharT, typename _Traits>
inline bool operator<(const basic_string_view<_CharT, _Traits>& lhs, const _CharT* rhs)
{
  return lhs < basic_string_view<_CharT, _Traits>(rhs);
}

template <typename _CharT, typename _Traits>
inline bool operator<(const _CharT* lhs, const basic_string_view<_CharT, _Traits>& rhs)
{
  return basic_string_view<_CharT, _Traits>(lhs) < rhs;
}

template <typename _CharT, typename _Traits, typename Allocator>
inline bool operator<(const std::basic_string<_CharT, _Traits, Allocator>& lhs,
                      const basic_string_view<_CharT, _Traits>& rhs)
{
  return basic_string_view<_CharT, _Traits>(lhs) < rhs;
}

template <typename _CharT, typename _Traits, typename Allocator>
inline bool operator<(const basic_string_view<_CharT, _Traits>& lhs,
                      const std::basic_string<_CharT, _Traits, Allocator>& rhs)
{
  return lhs < basic_string_view<_CharT, _Traits>(rhs);
}

//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline bool operator>(const basic_string_view<_CharT, _Traits>& lhs,
                      const basic_string_view<_CharT, _Traits>& rhs)
{
  return lhs.compare(rhs) > 0;
}

template <typename _CharT, typename _Traits>
inline bool operator>(const basic_string_view<_CharT, _Traits>& lhs, const _CharT* rhs)
{
  return lhs > basic_string_view<_CharT, _Traits>(rhs);
}

template <typename _CharT, typename _Traits>
inline bool operator>(const _CharT* lhs, const basic_string_view<_CharT, _Traits>& rhs)
{
  return basic_string_view<_CharT, _Traits>(lhs) > rhs;
}

template <typename _CharT, typename _Traits, typename Allocator>
inline bool operator>(const std::basic_string<_CharT, _Traits, Allocator>& lhs,
                      const basic_string_view<_CharT, _Traits>& rhs)
{
  return basic_string_view<_CharT, _Traits>(lhs) > rhs;
}

template <typename _CharT, typename _Traits, typename Allocator>
inline bool operator>(const basic_string_view<_CharT, _Traits>& lhs,
                      const std::basic_string<_CharT, _Traits, Allocator>& rhs)
{
  return lhs > basic_string_view<_CharT, _Traits>(rhs);
}

//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline bool operator<=(const basic_string_view<_CharT, _Traits>& lhs,
                       const basic_string_view<_CharT, _Traits>& rhs)
{
  return lhs.compare(rhs) <= 0;
}

template <typename _CharT, typename _Traits>
inline bool operator<=(const basic_string_view<_CharT, _Traits>& lhs, const _CharT* rhs)
{
  return lhs <= basic_string_view<_CharT, _Traits>(rhs);
}

template <typename _CharT, typename _Traits>
inline bool operator<=(const _CharT* lhs, const basic_string_view<_CharT, _Traits>& rhs)
{
  return basic_string_view<_CharT, _Traits>(lhs) <= rhs;
}

template <typename _CharT, typename _Traits, typename Allocator>
inline bool operator<=(const std::basic_string<_CharT, _Traits, Allocator>& lhs,
                       const basic_string_view<_CharT, _Traits>& rhs)
{
  return basic_string_view<_CharT, _Traits>(lhs) <= rhs;
}

template <typename _CharT, typename _Traits, typename Allocator>
inline bool operator<=(const basic_string_view<_CharT, _Traits>& lhs,
                       const std::basic_string<_CharT, _Traits, Allocator>& rhs)
{
  return lhs <= basic_string_view<_CharT, _Traits>(rhs);
}

//--------------------------------------------------------------------------

template <typename _CharT, typename _Traits>
inline bool operator>=(const basic_string_view<_CharT, _Traits>& lhs,
                       const basic_string_view<_CharT, _Traits>& rhs)
{
  return lhs.compare(rhs) >= 0;
}

template <typename _CharT, typename _Traits>
inline bool operator>=(const basic_string_view<_CharT, _Traits>& lhs, const _CharT* rhs)
{
  return lhs >= basic_string_view<_CharT, _Traits>(rhs);
}

template <typename _CharT, typename _Traits>
inline bool operator>=(const _CharT* lhs, const basic_string_view<_CharT, _Traits>& rhs)
{
  return basic_string_view<_CharT, _Traits>(lhs) >= rhs;
}

template <typename _CharT, typename _Traits, typename Allocator>
inline bool operator>=(const std::basic_string<_CharT, _Traits, Allocator>& lhs,
                       const basic_string_view<_CharT, _Traits>& rhs)
{
  return basic_string_view<_CharT, _Traits>(lhs) >= rhs;
}

template <typename _CharT, typename _Traits, typename Allocator>
inline bool operator>=(const basic_string_view<_CharT, _Traits>& lhs,
                       const std::basic_string<_CharT, _Traits, Allocator>& rhs)
{
  return lhs >= basic_string_view<_CharT, _Traits>(rhs);
}
//-----------------------------------------------------------------
// FNV1a hash from msvc++
#  if YASIO__64BITS
static YASIO__CONSTEXPR size_t _FNV_offset_basis = 14695981039346656037ULL;
static YASIO__CONSTEXPR size_t _FNV_prime = 1099511628211ULL;
#  else  /* defined(_M_X64), etc. */
static YASIO__CONSTEXPR size_t _FNV_offset_basis = 2166136261U;
static YASIO__CONSTEXPR size_t _FNV_prime = 16777619U;
#  endif /* defined(_M_X64), etc. */
inline size_t _FNV1a_hash(const void* _First, size_t _Count)
{ // FNV-1a hash function for bytes in [_First, _First+_Count)
  size_t _Val = _FNV_offset_basis;
  for (size_t _Next = 0; _Next < _Count; ++_Next)
  { // fold in another byte
    _Val ^= (size_t) static_cast<const unsigned char*>(_First)[_Next];
    _Val *= _FNV_prime;
  }

  return (_Val);
}
} // namespace cxx17
namespace std
{
template <class _Elem> struct hash<cxx17::basic_string_view<_Elem>>
{ // hash functor for basic_string_view
  typedef cxx17::basic_string_view<_Elem> _Kty;

  size_t operator()(const _Kty& _Keyval) const
  { // hash _Keyval to size_t value by pseudorandomizing transform
#  if defined(_LIBCPP_VERSION)
    return __do_string_hash(_Keyval.data(), _Keyval.data() + _Keyval.size());
#  elif defined(__GLIBCXX__)
    return _Hash_impl::hash(_Keyval.data(), _Keyval.size() * sizeof(_Elem));
#  else // msvc++ or other compiler without stable hash bytes function exists
    return ::cxx17::_FNV1a_hash(_Keyval.data(), _Keyval.size() * sizeof(_Elem));
#  endif
  }
};
} // namespace std
#endif

#if YASIO__HAS_CXX14
namespace cxx17
{
// basic_string_view LITERALS
inline namespace literals
{
inline namespace string_view_literals
{
inline cxx17::string_view operator"" _sv(const char* _Str, size_t _Len)
{
  return cxx17::string_view(_Str, _Len);
}
inline cxx17::wstring_view operator"" _sv(const wchar_t* _Str, size_t _Len)
{
  return cxx17::wstring_view(_Str, _Len);
}
inline cxx17::u16string_view operator"" _sv(const char16_t* _Str, size_t _Len)
{
  return cxx17::u16string_view(_Str, _Len);
}
inline cxx17::u32string_view operator"" _sv(const char32_t* _Str, size_t _Len)
{
  return cxx17::u32string_view(_Str, _Len);
}
} // namespace string_view_literals
} // namespace literals
} // namespace cxx17
#endif

namespace cxx17
{
template <typename _CharT, typename _Traits, typename Allocator>
inline std::basic_string<_CharT, _Traits, Allocator>& assign(std::basic_string<_CharT, _Traits, Allocator>& lhs, const basic_string_view<_CharT, _Traits>& rhs)
{
  if (!rhs.empty())
    lhs.assign(rhs.data(), rhs.size());
  else
    lhs.clear();
  return lhs;
}
template <typename _CharT, typename _Traits, typename Allocator>
inline std::basic_string<_CharT, _Traits, Allocator>& append(std::basic_string<_CharT, _Traits, Allocator>& lhs, const basic_string_view<_CharT, _Traits>& rhs)
{
  if (!rhs.empty())
    lhs.append(rhs.data(), rhs.size());
  else
    lhs.clear();
  return lhs;
}
template <typename _CharT, typename _Traits, typename Allocator = std::allocator<_CharT>>
inline std::basic_string<_CharT, _Traits, Allocator> svtos(const basic_string_view<_CharT, _Traits>& value)
{
    using string_type = std::basic_string<_CharT, _Traits, Allocator>;
    return !value.empty() ? string_type(value.data(), value.size()) : string_type{};
}
} // namespace cxx17

namespace cxx20
{
template< class T >
using decay_t = typename std::decay<T>::type;
template< class T >
using remove_const_t = typename std::remove_const<T>::type;
namespace char_ranges
{ // allow get char type from char*, wchar_t*, std::string, std::wstring
template <typename _Ty> struct value_type {
  using type = typename _Ty::value_type;
};

template <typename _Ty> struct value_type<_Ty&> {
  using type = remove_const_t<_Ty>;
};

template <typename _Ty> struct value_type<_Ty*> {
  using type = remove_const_t<_Ty>;
};
} // namespace char_ranges

// starts_with(), since C++20:
template <typename _CharT>
inline bool starts_with(cxx17::basic_string_view<_CharT> lhs,
                        cxx17::basic_string_view<_CharT> v) // (1)
{
  return lhs.size() >= v.size() && lhs.compare(0, v.size(), v) == 0;
}

template <typename _T1, typename _T2> 
inline bool starts_with(_T1&& lhs, _T2&& v) // (2)
{
  using char_type = typename char_ranges::value_type<decay_t<_T1>>::type;
  return starts_with(cxx17::basic_string_view<char_type>{lhs}, cxx17::basic_string_view<char_type>{v});
}

template <typename _CharT> 
inline bool starts_with(cxx17::basic_string_view<_CharT> lhs, int c) // (3)
{
  return !lhs.empty() && lhs.front() == c;
}

template <typename _Ty> 
inline bool starts_with(_Ty&& lhs, int c) // (4)
{
  using char_type = typename char_ranges::value_type<decay_t<_Ty>>::type;
  return starts_with(cxx17::basic_string_view<char_type>{lhs}, c);
}

// ends_with(), since C++20:
template <typename _CharT>
inline bool ends_with(cxx17::basic_string_view<_CharT> lhs,
                      cxx17::basic_string_view<_CharT> v) // (1)
{
  return lhs.size() >= v.size() && lhs.compare(lhs.size() - v.size(), lhs.npos, v) == 0;
}

template <typename _T1, typename _T2> 
inline bool ends_with(_T1&& lhs, _T2&& v) // (2)
{
  using char_type = typename char_ranges::value_type<decay_t<_T1>>::type;
  return ends_with(cxx17::basic_string_view<char_type>{lhs}, cxx17::basic_string_view<char_type>{v});
}

template <typename _CharT>
inline bool ends_with(cxx17::basic_string_view<_CharT> lhs, int c) // (3)
{
  return !lhs.empty() && lhs.back() == c;
}

template <typename _Ty> inline bool ends_with(_Ty&& lhs, int c) // (4)
{
  using char_type = typename char_ranges::value_type<decay_t<_Ty>>::type;
  return ends_with(cxx17::basic_string_view<char_type>{lhs}, c);
}

/// The case insensitive implementation of starts_with, ends_with
namespace ic
{
template <typename _CharT>
inline bool iequals(cxx17::basic_string_view<_CharT> lhs, cxx17::basic_string_view<_CharT> v);
#if defined(_MSC_VER)
template <>
inline bool iequals<char>(cxx17::basic_string_view<char> lhs, cxx17::basic_string_view<char> v)
{
  return lhs.size() == v.size() && ::_strnicmp(lhs.data(), v.data(), v.size()) == 0;
}
template <>
inline bool iequals<wchar_t>(cxx17::basic_string_view<wchar_t> lhs,
                             cxx17::basic_string_view<wchar_t> v)
{
  return lhs.size() == v.size() && ::_wcsnicmp(lhs.data(), v.data(), v.size()) == 0;
}
#else
template <>
inline bool iequals<char>(cxx17::basic_string_view<char> lhs, cxx17::basic_string_view<char> v)
{
  return lhs.size() == v.size() && ::strncasecmp(lhs.data(), v.data(), v.size()) == 0;
}
template <>
inline bool iequals<wchar_t>(cxx17::basic_string_view<wchar_t> lhs,
                             cxx17::basic_string_view<wchar_t> v)
{
  return lhs.size() == v.size() && ::wcsncasecmp(lhs.data(), v.data(), v.size()) == 0;
}
#endif
template <typename _T1, typename _T2> inline bool iequals(_T1&& lhs, _T2&& v)
{
  using char_type = typename char_ranges::value_type<decay_t<_T1>>::type;
  return iequals(cxx17::basic_string_view<char_type>{lhs}, cxx17::basic_string_view<char_type>{v});
}
// starts_with(), since C++20:
template <typename _CharT>
inline bool starts_with(cxx17::basic_string_view<_CharT> lhs,
                        cxx17::basic_string_view<_CharT> v) // (1)
{
  return lhs.size() >= v.size() && iequals(lhs.substr(0, v.size()), v);
}

template <typename _T1, typename _T2> inline bool starts_with(_T1&& lhs, _T2&& v) // (2)
{
  using char_type = typename char_ranges::value_type<decay_t<_T1>>::type;
  return starts_with(cxx17::basic_string_view<char_type>{lhs}, cxx17::basic_string_view<char_type>{v});
}

template <typename _CharT>
inline bool starts_with(cxx17::basic_string_view<_CharT> lhs, int c) // (3)
{
  return !lhs.empty() && ::tolower(lhs.front()) == ::tolower(c);
}

template <typename _Ty> inline bool starts_with(_Ty&& lhs, int c) // (4)
{
  using char_type = typename char_ranges::value_type<decay_t<_Ty>>::type;
  return starts_with(cxx17::basic_string_view<char_type>{lhs}, c);
}

// ends_with(), since C++20:
template <typename _CharT>
inline bool ends_with(cxx17::basic_string_view<_CharT> lhs,
                      cxx17::basic_string_view<_CharT> v) // (1)
{
  return lhs.size() >= v.size() && iequals(lhs.substr(lhs.size() - v.size(), lhs.npos), v);
}

template <typename _T1, typename _T2> inline bool ends_with(_T1&& lhs, _T2&& v) // (2)
{
  using char_type = typename char_ranges::value_type<decay_t<_T1>>::type;
  return ends_with(cxx17::basic_string_view<char_type>{lhs}, cxx17::basic_string_view<char_type>{v});
}

template <typename _CharT>
inline bool ends_with(cxx17::basic_string_view<_CharT> lhs, int c) // (3)
{
  return !lhs.empty() && ::tolower(lhs.back()) == ::tolower(c);
}

template <typename _Ty> inline bool ends_with(_Ty&& lhs, int c) // (4)
{
  using char_type = typename char_ranges::value_type<decay_t<_Ty>>::type;
  return ends_with(cxx17::basic_string_view<char_type>{lhs}, c);
}
} // namespace ic
} // namespace cxx20

#endif
