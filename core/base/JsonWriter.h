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

 A simple JsonWriter implementation like dotnet Utf8JsonWriter API.

 ****************************************************************************/

#pragma once

#include <string_view>

#include "yasio/byte_buffer.hpp"
#include "base/format.h"

namespace ax
{
struct JsonWriterOptions
{
    char indentChar       = ' ';
    int indentCharCount   = 2;
    int initialBufferSize = 128;
};

template <bool _Pretty = true>
class JsonWriter
{
public:
    JsonWriter()
    {
        setOptions(JsonWriterOptions{});
        _buffer.reserve(_options.initialBufferSize);
    }
    ~JsonWriter() {}

    explicit JsonWriter(const JsonWriterOptions& options) { setOptions(options); }

    void setOptions(const JsonWriterOptions& options) { _options = options; }

    explicit operator std::string_view() const { return std::string_view{_buffer.data(), _buffer.size()}; }

#pragma region write values
    void writeBoolValue(bool value) { writeUnquoteValue(value); }
    void writeNumberValue(int value) { writeUnquoteValue(value); }
    void writeNumberValue(long long value) { writeUnquoteValue(value); }
    void writeNumberValue(double value) { writeUnquoteValue(value); }
    void writeNullValue() { writeUnquoteValue("null"sv); }

    void writeStringValue(std::string_view value) { writeQuoteValue(value); }

    template <typename _Nty, size_t _N>
    void writeNumberValues(_Nty (&values)[_N])
    {
        for (auto v : values)
            writeNumberValue(v);
    }
#pragma endregion

#pragma region write values with propName
    void writeBool(std::string_view propertyName, bool value)
    {
        writePropertyName(propertyName);
        writeNumberValue(value);
    }
    void writeNumber(std::string_view propertyName, int value)
    {
        writePropertyName(propertyName);
        writeNumberValue(value);
    }
    void writeNumber(std::string_view propertyName, long long value)
    {
        writePropertyName(propertyName);
        writeNumberValue(value);
    }
    void writeNumber(std::string_view propertyName, double value)
    {
        writePropertyName(propertyName);
        writeNumberValue(value);
    }
    void writeString(std::string_view propertyName, std::string_view value)
    {
        writePropertyName(propertyName);
        writeStringValue(value);
    }
    void writeNull(std::string_view propertyName)
    {
        writePropertyName(propertyName);
        writeNullValue();
    }
    template <typename Intty, size_t _N>
    void writeNumberArray(std::string_view propertyName, Intty (&values)[_N])
    {
        writeStartArray(propertyName);
        writeNumberValues(values);
        writeEndArray();
    }
#pragma endregion

#pragma region array & object
    void writeStartArray(std::string_view propertyName)
    {
        writePropertyName(propertyName);
        writeStartArray();
    }
    void writeStartObject(std::string_view propertyName)
    {
        writePropertyName(propertyName);
        writeStartObject();
    }
    void writeStartArray() { writeStartCollection('['); }
    void writeEndArray() { writeEndCollection(']'); }

    void writeStartObject() { writeStartCollection('{'); }
    void writeEndObject() { writeEndCollection('}'); }
#pragma endregion

    void writePropertyName(std::string_view propertyName)
    {
        if constexpr (_Pretty)
        {
            fillIndentChars();
            _pendingValue = true;
        }
        _buffer += '"';
        _buffer += propertyName;
        _buffer += "\":"sv;
        if constexpr (_Pretty)
            _buffer += _options.indentChar;
    }

protected:
    void writeQuoteValue(const std::string_view& value)
    {
        if constexpr (_Pretty)
            if (!_pendingValue)
                fillIndentChars();

        _buffer += '"';
        _buffer += value;
        _buffer += "\","sv;
        if constexpr (_Pretty)
            _buffer += '\n';

        _pendingValue = false;
    }
    template <typename _Ty>
    void writeUnquoteValue(const _Ty& value)
    {
        if constexpr (_Pretty)
            if (!_pendingValue)
                fillIndentChars();

        fmt::vformat_to(std::back_inserter(_buffer), "{},", fmt::make_format_args(value));
        if constexpr (_Pretty)
            _buffer += '\n';

        _pendingValue = false;
    }

    void writeStartCollection(const char startChar)
    {
        if constexpr (_Pretty)
            if (!_pendingValue)
                fillIndentChars();

        ++_level;

        _buffer += startChar;
        if constexpr (_Pretty)
            _buffer += '\n';

        _pendingValue = false;
    }

    void writeEndCollection(const char termChar)
    {
        if (_buffer.empty())
            return;

        --_level;

        if constexpr (_Pretty)
        {
            if (_buffer.back() == '\n')
                _buffer.pop_back();  // pop '\n'
        }
        if (_buffer.back() == ',')
            _buffer.pop_back();
        if constexpr (_Pretty)
        {
            _buffer += '\n';
            fillIndentChars();
        }
        _buffer += termChar;

        if (_level != 0)
            _buffer += ',';
        if constexpr (_Pretty)
            _buffer += '\n';
    }

    void fillIndentChars()
    {
        if constexpr (_Pretty)
            if (_level)
                _buffer.expand(_level * _options.indentCharCount, _options.indentChar);
    }

    yasio::sbyte_buffer _buffer;
    uint16_t _level{0};
    bool _pendingValue{false};
    JsonWriterOptions _options;
};
}  // namespace ax
