/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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

#ifndef AXMOL__UTF8_H
#define AXMOL__UTF8_H

#include "platform/PlatformMacros.h"
#include <fmt/format.h>
#include <vector>
#include <string>
#include <sstream>

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#    include <jni.h>
#endif

NS_AX_BEGIN

namespace StringUtils
{

namespace UnicodeCharacters
{
const char32_t NewLine           = 0x000A;  // 10
const char32_t CarriageReturn    = 0x000D;  // 13
const char32_t NextCharNoChangeX = 0x0008;  // 8
const char32_t Space             = 0x0020;  // 32
const char32_t NoBreakSpace      = 0x00A0;  // 160
}  // namespace UnicodeCharacters

namespace AsciiCharacters
{
const char NewLine           = '\n';
const char CarriageReturn    = '\r';
const char NextCharNoChangeX = '\b';
const char Space             = ' ';
}  // namespace AsciiCharacters

template <typename T>
inline std::string toString(T arg)
{
    return fmt::to_string(arg);
}

// DEPRECATED since axmol-2.1.4, use fmt::format instead
std::string AX_DLL format(const char* format, ...) AX_FORMAT_PRINTF(1, 2);
std::string AX_DLL vformat(const char* format, va_list ap);

/**
 *  @brief Converts from UTF8 string to UTF16 string.
 *
 *  This function resizes \p outUtf16 to required size and
 *  fill its contents with result UTF16 string if conversion success.
 *  If conversion fails it guarantees not to change \p outUtf16.
 *
 *  @param inUtf8 The source UTF8 string to be converted from.
 *  @param outUtf16 The output string to hold the result UTF16s.
 *  @return True if succeed, otherwise false.
 *  @note Please check the return value before using \p outUtf16
 *  e.g.
 *  @code
 *    std::u16string utf16;
 *    bool ret = StringUtils::UTF8ToUTF16("你好hello", utf16);
 *    if (ret) {
 *        do_some_thing_with_utf16(utf16);
 *    }
 *  @endcode
 */
AX_DLL bool UTF8ToUTF16(std::string_view inUtf8, std::u16string& outUtf16);

/**
 *  @brief Same as \a UTF8ToUTF16 but converts form UTF8 to UTF32.
 *
 *  @see UTF8ToUTF16
 */
AX_DLL bool UTF8ToUTF32(std::string_view inUtf8, std::u32string& outUtf32);

/**
 *  @brief Same as \a UTF8ToUTF16 but converts form UTF16 to UTF8.
 *
 *  @see UTF8ToUTF16
 */
AX_DLL bool UTF16ToUTF8(std::u16string_view inUtf16, std::string& outUtf8);

/**
 *  @brief Same as \a UTF8ToUTF16 but converts form UTF16 to UTF32.
 *
 *  @see UTF8ToUTF16
 */
AX_DLL bool UTF16ToUTF32(std::u16string_view inUtf16, std::u32string& outUtf32);

/**
 *  @brief Same as \a UTF8ToUTF16 but converts form UTF32 to UTF8.
 *
 *  @see UTF8ToUTF16
 */
AX_DLL bool UTF32ToUTF8(std::u32string_view inUtf32, std::string& outUtf8);

/**
 *  @brief Same as \a UTF8ToUTF16 but converts form UTF32 to UTF16.
 *
 *  @see UTF8ToUTF16
 */
AX_DLL bool UTF32ToUTF16(std::u32string_view inUtf32, std::u16string& outUtf16);

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)

/**
 *  @brief convert jstring to utf8 std::string,  same function with env->getStringUTFChars.
 *         because getStringUTFChars can not pass special emoticon
 *  @param env   The JNI Env
 *  @param srcjStr The jstring which want to convert
 *  @param ret   True if the conversion succeeds and the ret pointer isn't null
 *  @returns the result of utf8 string
 */
AX_DLL std::string getStringUTFCharsJNI(JNIEnv* env, jstring srcjStr, bool* ret = nullptr);

/**
 *  @brief create a jstring with utf8 std::string, same function with env->newStringUTF
 *         because newStringUTF can not convert special emoticon
 *  @param env   The JNI Env
 *  @param srcjStr The std::string which want to convert
 *  @param ret     True if the conversion succeeds and the ret pointer isn't null
 *  @returns the result of jstring,the jstring need to DeleteLocalRef(jstring);
 */
AX_DLL jstring newStringUTFJNI(JNIEnv* env, std::string_view utf8Str, bool* ret = nullptr);
#endif

/**
 *  @brief Trims the unicode spaces at the end of char16_t vector.
 */
AX_DLL void trimUTF16Vector(std::vector<char16_t>& str);

/**
 *  @brief Trims the unicode spaces at the end of char32_t vector.
 */
AX_DLL void trimUTF32Vector(std::vector<char32_t>& str);

/**
 *  @brief Whether the character is a whitespace character.
 *  @param ch    The unicode character.
 *  @returns     Whether the character is a white space character.
 *
 *  @see http://en.wikipedia.org/wiki/Whitespace_character#Unicode
 *
 */
AX_DLL bool isUnicodeSpace(char32_t ch);

/**
 *  @brief Whether the character is a Chinese/Japanese/Korean character.
 *  @param ch    The unicode character.
 *  @returns     Whether the character is a Chinese character.
 *
 *  @see http://www.searchtb.com/2012/04/chinese_encode.html
 *  @see http://tieba.baidu.com/p/748765987
 *
 */
AX_DLL bool isCJKUnicode(char32_t ch);

/**
 *  @brief Whether the character is a non-breaking character.
 *  @param ch    The unicode character.
 *  @returns     Whether the character is a non-breaking character.
 *
 *  @see https://en.wikipedia.org/wiki/Space_(punctuation)#Spaces_in_Unicode
 *  @see https://en.wikipedia.org/wiki/Non-breaking_space
 *  @see https://en.wikipedia.org/wiki/Figure_space
 *  @see https://en.wikipedia.org/wiki/Word_joiner
 *
 */
AX_DLL bool isUnicodeNonBreaking(char32_t ch);

/**
 *  @brief Returns the length of the string in characters.
 *  @param utf8 An UTF-8 encoded string.
 *  @returns The length of the string in characters.
 */
AX_DLL int32_t getCharacterCountInUTF8String(std::string_view utf8);

/**
 *  @brief Gets the index of the last character that is not equal to the character given.
 *  @param str   The string to be searched.
 *  @param c     The character to be searched for.
 *  @returns The index of the last character that is not \p c.
 */
AX_DLL unsigned int getIndexOfLastNotChar16(const std::vector<char16_t>& str, char16_t c);

/**
 *  @brief Gets char16_t vector from a given utf16 string.
 */
AX_DLL std::vector<char16_t> getChar16VectorFromUTF16String(const std::u16string& utf16);

/**
 *  @brief Whether has non-ascii utf-8 characters
 */
AX_DLL bool hasNonAsciiUTF8(const char* str, size_t len);

/**
 *  @brief Whether contains utf-8 or all characters are ascii
 */
AX_DLL bool detectNonAsciiUTF8(const char* str, size_t len, bool restrictUTF8, bool* pAllCharsAreAscii);

/**
 *  @brief isLegalUTF8String, contains ascii characters
 */
AX_DLL bool isLegalUTF8String(const char* str, size_t len);

/**
 * Utf8 sequence
 * Store all utf8 chars as std::string
 * Build from std::string
 */
class AX_DLL StringUTF8
{
public:
    struct CharUTF8
    {
        std::string _char;
        bool isASCII() const { return _char.size() == 1; }
    };
    typedef std::vector<CharUTF8> CharUTF8Store;

    StringUTF8();
    StringUTF8(std::string_view newStr);
    ~StringUTF8();

    std::size_t length() const;
    void replace(std::string_view newStr);

    std::string getAsCharSequence() const;
    std::string getAsCharSequence(std::size_t pos) const;
    std::string getAsCharSequence(std::size_t pos, std::size_t len) const;

    bool deleteChar(std::size_t pos);
    bool insert(std::size_t pos, std::string_view insertStr);
    bool insert(std::size_t pos, const StringUTF8& insertStr);

    CharUTF8Store& getString() { return _str; }
    const CharUTF8Store& getString() const { return _str; }

private:
    CharUTF8Store _str;
};

}  // namespace StringUtils

NS_AX_END

#endif /** defined(__cocos2dx__ccUTF8__) */
