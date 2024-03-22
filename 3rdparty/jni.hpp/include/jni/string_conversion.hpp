#pragma once

// If you want to supply your own UTF-8 <-> UTF-16 conversion routines, create a header file 
// that can be found at <jni/string_conversion.hpp> and will be found first in the lookup chain.

#include <string>
#include <locale>
#include <codecvt>

namespace jni
   {
    inline std::u16string convertUTF8ToUTF16(const std::string& string)
       {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().from_bytes(string);
       }

    inline std::string convertUTF16ToUTF8(const std::u16string& string)
       {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().to_bytes(string);
       }
   }
