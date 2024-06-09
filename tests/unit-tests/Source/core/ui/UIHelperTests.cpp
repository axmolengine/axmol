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
#include "ui/UIHelper.h"

USING_NS_AX;
using ax::ui::Helper;


TEST_SUITE("ui/Helper") {
    TEST_CASE("getSubStringOfUTF8String") {
        SUBCASE("normal_cases") {
            std::string source = "abcdefghij";
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 2) == "ab");
            CHECK(Helper::getSubStringOfUTF8String(source, 2, 2) == "cd");
            CHECK(Helper::getSubStringOfUTF8String(source, 4, 2) == "ef");
        }

        SUBCASE("empty_string") {
            std::string source = "";

            // OK
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 1) == "");

            // Error: These cases cause "out of range" error
            CHECK(Helper::getSubStringOfUTF8String(source, 1, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 1, 1) == "");
        }

        SUBCASE("ascii") {
            std::string source = "abc";

            // OK
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 1, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 2, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 3, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 3) == "abc");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 4) == "abc");
            CHECK(Helper::getSubStringOfUTF8String(source, 1, 2) == "bc");
            CHECK(Helper::getSubStringOfUTF8String(source, 1, 3) == "bc");
            CHECK(Helper::getSubStringOfUTF8String(source, 2, 1) == "c");
            CHECK(Helper::getSubStringOfUTF8String(source, 2, 2) == "c");
            CHECK(Helper::getSubStringOfUTF8String(source, 3, 1) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 3, 2) == "");

            // Error: These cases cause "out of range" error
            CHECK(Helper::getSubStringOfUTF8String(source, 4, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 4, 1) == "");
        }

        SUBCASE("CJK") {
            std::string source = "这里是中文测试例";

            // OK
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 1, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 7, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 8, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 8, 1) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 1) == "\xe8\xbf\x99");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 4) == "\xe8\xbf\x99\xe9\x87\x8c\xe6\x98\xaf\xe4\xb8\xad");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 8) ==
                    "\xe8\xbf\x99\xe9\x87\x8c\xe6\x98\xaf\xe4\xb8\xad\xe6\x96\x87\xe6\xb5\x8b\xe8\xaf\x95\xe4\xbe\x8b");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 100) ==
                    "\xe8\xbf\x99\xe9\x87\x8c\xe6\x98\xaf\xe4\xb8\xad\xe6\x96\x87\xe6\xb5\x8b\xe8\xaf\x95\xe4\xbe\x8b");
            CHECK(Helper::getSubStringOfUTF8String(source, 2, 5) ==
                    "\xe6\x98\xaf\xe4\xb8\xad\xe6\x96\x87\xe6\xb5\x8b\xe8\xaf\x95");
            CHECK(Helper::getSubStringOfUTF8String(source, 6, 2) == "\xe8\xaf\x95\xe4\xbe\x8b");
            CHECK(Helper::getSubStringOfUTF8String(source, 6, 100) == "\xe8\xaf\x95\xe4\xbe\x8b");

            // Error: These cases cause "out of range" error
            CHECK(Helper::getSubStringOfUTF8String(source, 9, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 9, 1) == "");
        }

        SUBCASE("redundant_utf8_sequence_for_directory_traversal_attack") {
            // Redundant UTF-8 sequence for Directory traversal attack (1)
            std::string source = "\xC0\xAF";

            // Error: Can't convert string to correct encoding such as UTF-32
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 1) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 1, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 1, 1) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 2) == "");
        }

        SUBCASE("redundant_utf8_sequence_for_directory_traversal_attack_2") {
            std::string source = "\xE0\x80\xAF";

            // Error: Can't convert string to correct encoding such as UTF-32
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 1) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 1, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 1, 1) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 3) == "");
        }

        SUBCASE("redundant_utf8_sequence_for_directory_traversal_attack_3") {
            std::string source = "\xF0\x80\x80\xAF";

            // Error: Can't convert string to correct encoding such as UTF-32
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 1) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 1, 0) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 1, 1) == "");
            CHECK(Helper::getSubStringOfUTF8String(source, 0, 4) == "");
        }
    }
}
