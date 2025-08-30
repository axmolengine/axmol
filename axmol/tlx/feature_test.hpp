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

// Tests whether compiler has c++23 support
#if (defined(__cplusplus) && __cplusplus > 202002L) || \
    (defined(_MSC_VER) && _MSC_VER > 1934 &&           \
     ((defined(_HAS_CXX23) && _HAS_CXX23 == 1) || (defined(_MSVC_LANG) && (_MSVC_LANG > 202002L))))
#    ifndef _AX_HAS_CXX23
#        define _AX_HAS_CXX23 1
#    endif  // C++23 features macro
#endif      // C++23 features check
#if !defined(_AX_HAS_CXX23)
#    define _AX_HAS_CXX23 0
#endif
