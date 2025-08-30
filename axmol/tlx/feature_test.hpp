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
