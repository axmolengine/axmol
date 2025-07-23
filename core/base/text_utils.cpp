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

#include "base/text_utils.h"
#include "platform/Common.h"
#include "base/Logging.h"
#include "ConvertUTF.h"
#include <limits>

using namespace llvm;

namespace ax
{

namespace text_utils
{

std::string_view ltrim(std::string_view s)
{
    if (!s.empty())
    {
        auto first = s.data();
        auto last  = first + s.length();
        while (first < last && std::isspace(*first))
        {
            ++first;
        }
        return std::string_view{first, static_cast<size_t>(last - first)};
    }
    return s;
}

std::string_view rtrim(std::string_view s)
{
    if (!s.empty())
    {
        auto first  = s.data();
        auto last   = first + s.length();
        auto rfirst = last - 1;
        while (rfirst > first && std::isspace(*rfirst))
        {
            --rfirst;
        }
        return std::string_view{first, static_cast<size_t>(rfirst + 1 - first)};
    }
    return s;
}

/*
 * @str:    the string to search through.
 * @c:        the character to not look for.
 *
 * Return value: the index of the last character that is not c.
 * */
unsigned int getIndexOfLastNotChar16(const std::vector<char16_t>& str, char16_t c)
{
    int len = static_cast<int>(str.size());

    int i = len - 1;
    for (; i >= 0; --i)
        if (str[i] != c)
            return i;

    return i;
}

/*
 * @str:    the string to trim
 * @index:    the index to start trimming from.
 *
 * Trims str st str=[0, index) after the operation.
 *
 * Return value: the trimmed string.
 * */
static void trimUTF16VectorFromIndex(std::vector<char16_t>& str, int index)
{
    int size = static_cast<int>(str.size());
    if (index >= size || index < 0)
        return;

    str.erase(str.begin() + index, str.begin() + size);
}

/*
 * @str:    the string to trim
 * @index:    the index to start trimming from.
 *
 * Trims str st str=[0, index) after the operation.
 *
 * Return value: the trimmed string.
 * */
static void trimUTF32VectorFromIndex(std::vector<char32_t>& str, int index)
{
    int size = static_cast<int>(str.size());
    if (index >= size || index < 0)
        return;

    str.erase(str.begin() + index, str.begin() + size);
}

/*
 * @ch is the unicode character whitespace?
 *
 * Reference: http://en.wikipedia.org/wiki/Whitespace_character#Unicode
 *
 * Return value: weather the character is a whitespace character.
 * */
bool isUnicodeSpace(char32_t ch)
{
    return (ch >= 0x0009 && ch <= 0x000D) || ch == 0x0020 || ch == 0x0085 || ch == 0x00A0 || ch == 0x1680 ||
           (ch >= 0x2000 && ch <= 0x200A) || ch == 0x2028 || ch == 0x2029 || ch == 0x202F || ch == 0x205F ||
           ch == 0x3000;
}

bool isCJKUnicode(char32_t ch)
{
    return (ch >= 0x4E00 && ch <= 0x9FBF)     // CJK Unified Ideographs
           || (ch >= 0x2E80 && ch <= 0x2FDF)  // CJK Radicals Supplement & Kangxi Radicals
           ||
           (ch >= 0x2FF0 && ch <= 0x30FF)  // Ideographic Description Characters, CJK Symbols and Punctuation & Japanese
           || (ch >= 0x3100 && ch <= 0x31BF)     // Korean
           || (ch >= 0xAC00 && ch <= 0xD7AF)     // Hangul Syllables
           || (ch >= 0xF900 && ch <= 0xFAFF)     // CJK Compatibility Ideographs
           || (ch >= 0xFE30 && ch <= 0xFE4F)     // CJK Compatibility Forms
           || (ch >= 0x31C0 && ch <= 0x4DFF)     // Other extensions
           || (ch >= 0x1f004 && ch <= 0x1f682);  // Emoji
}

bool isUnicodeNonBreaking(char32_t ch)
{
    return ch == 0x00A0      // Non-Breaking Space
           || ch == 0x202F   // Narrow Non-Breaking Space
           || ch == 0x2007   // Figure Space
           || ch == 0x2060;  // Word Joiner
}

void trimUTF16Vector(std::vector<char16_t>& str)
{
    int len = static_cast<int>(str.size());

    if (len <= 0)
        return;

    int last_index = len - 1;

    // Only start trimming if the last character is whitespace..
    if (isUnicodeSpace(str[last_index]))
    {
        for (int i = last_index - 1; i >= 0; --i)
        {
            if (isUnicodeSpace(str[i]))
                last_index = i;
            else
                break;
        }

        trimUTF16VectorFromIndex(str, last_index);
    }
}

void trimUTF32Vector(std::vector<char32_t>& str)
{
    int len = static_cast<int>(str.size());

    if (len <= 0)
        return;

    int last_index = len - 1;

    // Only start trimming if the last character is whitespace..
    if (isUnicodeSpace(str[last_index]))
    {
        for (int i = last_index - 1; i >= 0; --i)
        {
            if (isUnicodeSpace(str[i]))
                last_index = i;
            else
                break;
        }

        trimUTF32VectorFromIndex(str, last_index);
    }
}

template <typename T>
struct ConvertTrait
{
    typedef T ArgType;
};
template <>
struct ConvertTrait<char>
{
    typedef UTF8 ArgType;
};
template <>
struct ConvertTrait<char16_t>
{
    typedef UTF16 ArgType;
};
template <>
struct ConvertTrait<char32_t>
{
    typedef UTF32 ArgType;
};

template <typename From, typename To, typename FromTrait = ConvertTrait<From>, typename ToTrait = ConvertTrait<To>>
bool utfConvert(const std::basic_string_view<From>& from,
                std::basic_string<To>& to,
                ConversionResult (*cvtfunc)(const typename FromTrait::ArgType**,
                                            const typename FromTrait::ArgType*,
                                            typename ToTrait::ArgType**,
                                            typename ToTrait::ArgType*,
                                            ConversionFlags))
{
    static_assert(sizeof(From) == sizeof(typename FromTrait::ArgType), "Error size mismatched");
    static_assert(sizeof(To) == sizeof(typename ToTrait::ArgType), "Error size mismatched");

    if (from.empty())
    {
        to.clear();
        return true;
    }

    // See: http://unicode.org/faq/utf_bom.html#gen6
    static const int most_bytes_per_character = 4;

    const size_t maxNumberOfChars = from.length();  // all UTFs at most one element represents one character.
    const size_t numberOfOut      = maxNumberOfChars * most_bytes_per_character / sizeof(To);

    std::basic_string<To> working(numberOfOut, 0);

    auto inbeg = reinterpret_cast<const typename FromTrait::ArgType*>(&from[0]);
    auto inend = inbeg + from.length();

    auto outbeg = reinterpret_cast<typename ToTrait::ArgType*>(&working[0]);
    auto outend = outbeg + working.length();
    auto r      = cvtfunc(&inbeg, inend, &outbeg, outend, strictConversion);
    if (r != conversionOK)
        return false;

    working.resize(reinterpret_cast<To*>(outbeg) - &working[0]);
    to = std::move(working);

    return true;
};

bool UTF8ToUTF16(std::string_view utf8, std::u16string& outUtf16)
{
    return utfConvert(utf8, outUtf16, ConvertUTF8toUTF16);
}

bool UTF8ToUTF32(std::string_view utf8, std::u32string& outUtf32)
{
    return utfConvert(utf8, outUtf32, ConvertUTF8toUTF32);
}

bool UTF16ToUTF8(std::u16string_view utf16, std::string& outUtf8)
{
    return utfConvert(utf16, outUtf8, ConvertUTF16toUTF8);
}

bool UTF16ToUTF32(std::u16string_view utf16, std::u32string& outUtf32)
{
    return utfConvert(utf16, outUtf32, ConvertUTF16toUTF32);
}

bool UTF32ToUTF8(std::u32string_view utf32, std::string& outUtf8)
{
    return utfConvert(utf32, outUtf8, ConvertUTF32toUTF8);
}

bool UTF32ToUTF16(std::u32string_view utf32, std::u16string& outUtf16)
{
    return utfConvert(utf32, outUtf16, ConvertUTF32toUTF16);
}

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
std::string getStringUTFCharsJNI(JNIEnv* env, jstring srcjStr, bool* ret)
{
    std::string utf8Str;
    if (srcjStr != nullptr && env != nullptr)
    {
        const unsigned short* unicodeChar = (const unsigned short*)env->GetStringChars(srcjStr, nullptr);
        size_t unicodeCharLength          = env->GetStringLength(srcjStr);
        const std::u16string unicodeStr((const char16_t*)unicodeChar, unicodeCharLength);
        bool flag = UTF16ToUTF8(unicodeStr, utf8Str);
        if (ret)
        {
            *ret = flag;
        }
        if (!flag)
        {
            utf8Str = "";
        }
        env->ReleaseStringChars(srcjStr, unicodeChar);
    }
    else
    {
        if (ret)
        {
            *ret = false;
        }
        utf8Str = "";
    }
    return utf8Str;
}

jstring newStringUTFJNI(JNIEnv* env, std::string_view utf8Str, bool* ret)
{
    std::u16string utf16Str;
    bool flag = ax::text_utils::UTF8ToUTF16(utf8Str, utf16Str);

    if (ret)
    {
        *ret = flag;
    }

    if (!flag)
    {
        utf16Str.clear();
    }
    jstring stringText = env->NewString((const jchar*)utf16Str.data(), utf16Str.length());
    return stringText;
}
#endif

std::vector<char16_t> getChar16VectorFromUTF16String(const std::u16string& utf16)
{
    return std::vector<char16_t>(utf16.begin(), utf16.end());
}

size_t getCharacterCountInUTF8String(std::string_view utf8)
{
    return countUTF8Chars(utf8);
}

size_t countUTF8Chars(std::string_view utf8)
{
    int count = 0;

    if (!utf8.empty())
    {
        const UTF8* source = (const UTF8*)utf8.data();
        const UTF8* sourceEnd   = (const UTF8*)utf8.data() + utf8.length();
        while (source != sourceEnd)
        {
            auto size = getUTF8SequenceSize(source, sourceEnd);
            if (size == 0)
            {
                // Invalid UTF-8 sequence found
                return 0;
            }
            source += size;
            ++count;
        }
    }
  
    return count;
}

size_t getUTF8ByteOffset(std::string_view utf8, size_t utf8CharOffset)
{
    if (utf8CharOffset >= utf8.length())
        return std::string_view::npos;

    const UTF8* source = (const UTF8*)utf8.data();
    const UTF8* sourceEnd = source + utf8.length();
    for (size_t i = 0; i < utf8CharOffset; ++i)
    {
        auto size = getUTF8SequenceSize(source, sourceEnd);
        if (size == 0)
        {
            // Invalid UTF-8 sequence found
            return std::string_view::npos;
        }
        source += size;
    }
    return static_cast<unsigned int>(source - (const UTF8*)utf8.data());
}

size_t eraseUTF8CharAt(std::string& str, size_t utf8CharOffset)
{ 
    if (utf8CharOffset >= str.length())
        return 0;
    const UTF8* source = (const UTF8*)str.data();
    const UTF8* sourceEnd = source + str.length();
    size_t byteOffset = getUTF8ByteOffset(str, utf8CharOffset);
    if (byteOffset == std::string_view::npos)
        return 0;
    auto size = getUTF8SequenceSize(source + byteOffset, sourceEnd);
    if (size == 0)
        return 0;
    str.erase(byteOffset, size);
    return size;
}

bool hasNonAsciiUTF8(const char* str, size_t len)
{
    return detectNonAsciiUTF8(str, len, true, nullptr);
}

bool detectNonAsciiUTF8(const char* str, size_t len, bool restrictUTF8, bool* pAllCharsAreAscii)
{
    bool allCharsAreAscii  = true;
    bool nonAsciiUTF8Found = false;
    bool invalidUTF8Found  = false;
    for (size_t i = 0; i < len;)
    {
        auto& current = str[i];
        int numByte   = getNumBytesForUTF8(current);
        if (numByte > 1)
        {
            allCharsAreAscii = false;
            if (isLegalUTF8Sequence((const UTF8*)&current, (const UTF8*)&current + numByte))
            {
                nonAsciiUTF8Found = true;
                if (!restrictUTF8)
                    break;
            }
            else
            {  // invalid utf-8 sequence found
                invalidUTF8Found = true;
                if (restrictUTF8)
                    break;
            }
        }
        else
        {  // not a valid utf-8 chars
            if ((current & 0x80) != 0 || current == 0)
                allCharsAreAscii = false;
        }
        i += numByte;
    }

    if (pAllCharsAreAscii)
        *pAllCharsAreAscii = allCharsAreAscii;

    return nonAsciiUTF8Found && (!invalidUTF8Found || !restrictUTF8);
}

bool isLegalUTF8String(const char* str, size_t len)
{
    return ::isLegalUTF8String((const UTF8**)&str, (const UTF8*)str + len);
}

StringUTF8::StringUTF8() {}

StringUTF8::StringUTF8(std::string_view newStr)
{
    replace(newStr);
}

StringUTF8::~StringUTF8() {}

std::size_t StringUTF8::length() const
{
    return _str.size();
}

void StringUTF8::replace(std::string_view newStr)
{
    _str.clear();
    if (!newStr.empty())
    {
        UTF8* sequenceUtf8 = (UTF8*)newStr.data();

        int lengthString = countUTF8Chars(newStr);

        if (lengthString == 0)
        {
            AXLOGD("Bad utf-8 set string: {}", newStr);
            return;
        }

        while (*sequenceUtf8)
        {
            std::size_t lengthChar = getNumBytesForUTF8(*sequenceUtf8);

            CharUTF8 charUTF8;
            charUTF8._char.append((char*)sequenceUtf8, lengthChar);
            sequenceUtf8 += lengthChar;

            _str.emplace_back(charUTF8);
        }
    }
}

std::string StringUTF8::getAsCharSequence() const
{
    return getAsCharSequence(0, std::numeric_limits<std::size_t>::max());
}

std::string StringUTF8::getAsCharSequence(std::size_t pos) const
{
    return getAsCharSequence(pos, std::numeric_limits<std::size_t>::max());
}

std::string StringUTF8::getAsCharSequence(std::size_t pos, std::size_t len) const
{
    std::string charSequence;
    std::size_t maxLen = _str.size() - pos;
    if (len > maxLen)
    {
        len = maxLen;
    }

    std::size_t endPos = len + pos;
    while (pos < endPos)
    {
        charSequence.append(_str[pos++]._char);
    }

    return charSequence;
}

bool StringUTF8::deleteChar(std::size_t pos)
{
    if (pos < _str.size())
    {
        _str.erase(_str.begin() + pos);
        return true;
    }
    else
    {
        return false;
    }
}

bool StringUTF8::insert(std::size_t pos, std::string_view insertStr)
{
    StringUTF8 utf8(insertStr);

    return insert(pos, utf8);
}

bool StringUTF8::insert(std::size_t pos, const StringUTF8& insertStr)
{
    if (pos <= _str.size())
    {
        _str.insert(_str.begin() + pos, insertStr._str.begin(), insertStr._str.end());

        return true;
    }
    else
    {
        return false;
    }
}

}  // namespace text_utils

}  // namespace ax
