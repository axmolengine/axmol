/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include <doctest.h>
#include "base/UTF8.h"

using namespace ax;


constexpr int kUtf16TestStrLength = 11;


TEST_SUITE("base/UTF8") {
    TEST_CASE("old_tests") {
        static const char16_t __utf16Code[] = {
            0x3042, 0x3044, 0x3046, 0x3048, 0x304A, 0x3042, 0x3044, 0x3046, 0x3048, 0x304A, 0x0041, 0x0000,
        };

        // to avoid Xcode error, char => unsigned char
        // If you use this table, please cast manually as (const char *).
        static const unsigned char __utf8Code[] = {
            0xE3, 0x81, 0x82, 0xE3, 0x81, 0x84, 0xE3, 0x81, 0x86, 0xE3, 0x81, 0x88, 0xE3, 0x81, 0x8A, 0xE3,
            0x81, 0x82, 0xE3, 0x81, 0x84, 0xE3, 0x81, 0x86, 0xE3, 0x81, 0x88, 0xE3, 0x81, 0x8A, 0x41, 0x00,
        };

        static const char16_t WHITE_SPACE_CODE[] = {0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x0020, 0x0085, 0x00A0, 0x1680,
                                                    0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005, 0x2006, 0x2007, 0x2008,
                                                    0x2009, 0x200A, 0x2028, 0x2029, 0x202F, 0x205F, 0x3000};

        std::string originalUTF8     = (const char*)__utf8Code;
        std::u16string originalUTF16 = __utf16Code;

        //---------------------------
        std::string utf8Str;
        CHECK(StringUtils::UTF16ToUTF8(originalUTF16, utf8Str));
        CHECK(memcmp(utf8Str.data(), originalUTF8.data(), originalUTF8.length() + 1) == 0);

        //---------------------------
        std::u16string utf16Str;
        CHECK(StringUtils::UTF8ToUTF16(originalUTF8, utf16Str));
        CHECK(memcmp(utf16Str.data(), originalUTF16.data(), originalUTF16.length() + 1) == 0);
        CHECK(utf16Str.length() == kUtf16TestStrLength);

        //---------------------------
        auto vec1 = StringUtils::getChar16VectorFromUTF16String(originalUTF16);

        CHECK(vec1.size() == originalUTF16.length());

        //---------------------------
        std::vector<char16_t> vec2(vec1);
        vec2.emplace_back(0x2009);
        vec2.emplace_back(0x2009);
        vec2.emplace_back(0x2009);
        vec2.emplace_back(0x2009);

        std::vector<char16_t> vec3(vec2);
        StringUtils::trimUTF16Vector(vec2);

        CHECK(vec1.size() == vec2.size());

        for (size_t i = 0; i < vec2.size(); i++)
        {
            CHECK(vec1.at(i) == vec2.at(i));
        }

        CHECK(StringUtils::getCharacterCountInUTF8String(originalUTF8) == kUtf16TestStrLength);

        CHECK(StringUtils::getIndexOfLastNotChar16(vec3, 0x2009) == (vec1.size() - 1));

        CHECK(originalUTF16.length() == kUtf16TestStrLength);

        size_t whiteCodeNum = sizeof(WHITE_SPACE_CODE) / sizeof(WHITE_SPACE_CODE[0]);
        for (size_t i = 0; i < whiteCodeNum; i++)
        {
            CHECK(StringUtils::isUnicodeSpace(WHITE_SPACE_CODE[i]));
        }

        CHECK(!StringUtils::isUnicodeSpace(0xFFFF));

        CHECK(!StringUtils::isCJKUnicode(0xFFFF));
        CHECK(StringUtils::isCJKUnicode(0x3100));
    }
}
