#pragma once

// llvm clang and Apple clang std::from_chars not support floating-point
// use fast_float instead: https://github.com/fastfloat/fast_float
#if !defined(_WIN32) && defined(__clang__)
#include "fast_float/fast_float.h"
namespace axstd {
    using fast_float::from_chars;
    using from_chars_result = fast_float::from_chars_result;
}

#else
#include <charconv>
namespace axstd {
    using std::from_chars;
    using std::from_chars_result;
}
#endif
