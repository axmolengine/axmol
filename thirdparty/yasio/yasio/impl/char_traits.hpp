#pragma once
#include <stddef.h>

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
inline size_t __xxtraits_find_last_not_of(const typename _Traits::char_type* _Haystack,
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
