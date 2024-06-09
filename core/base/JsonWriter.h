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

 A simple wrapper dotnet Utf8JsonWriter like API style of rapidjson

 ****************************************************************************/

#pragma once

#include <string_view>

#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

namespace ax
{
struct JsonWriterOptions
{
    char indentChar                              = ' ';
    int indentCharCount                          = 2;
    rapidjson::PrettyFormatOptions formatOptions = rapidjson::kFormatDefault;
};

template <bool _Pretty = true>
struct JsonWriterImpl
{
    using type = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
};

template <>
struct JsonWriterImpl<false>
{
    using type = rapidjson::Writer<rapidjson::StringBuffer>;
};

template <bool _Pretty = true>
class JsonWriter
{
    using writer_type = typename JsonWriterImpl<_Pretty>::type;

public:
    JsonWriter()
    {
        new (static_cast<void*>(_writerHold)) writer_type(_buffer);
        setOptions(JsonWriterOptions{});
    }
    ~JsonWriter() { writer().~writer_type(); }

    explicit JsonWriter(const JsonWriterOptions& options) { setOptions(options); }

    void setOptions(const JsonWriterOptions& options)
    {
        if constexpr (_Pretty)
        {
            prettyWriter().SetIndent(options.indentChar, options.indentCharCount);
            prettyWriter().SetFormatOptions(options.formatOptions);
        }
    }

    void writePropertyName(std::string_view propertyName)
    {
        writer().Key(propertyName.data(), static_cast<rapidjson::SizeType>(propertyName.length()), false);
    }

#pragma region write values
    void writeBoolValue(bool value) { writer().Bool(value); }
    void writeNumberValue(int value) { writer().Int(value); }
    void writeNumberValue(long long value) { writer().Int64(value); }
    void writeNumberValue(double value) { writer().Double(value); }
    void writeStringValue(std::string_view value)
    {
        writer().String(value.data(), static_cast<rapidjson::SizeType>(value.length()));
    }

    void writeNullValue() { writer().Null(); }

    template <typename Intty, size_t _N>
    void writeNumberValues(Intty (&values)[_N])
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
    void writeStartArray() { writer().StartArray(); }
    void writeEndArray() { writer().EndArray(); }
    void writeStartObject() { writer().StartObject(); }
    void writeEndObject() { writer().EndObject(); }
#pragma endregion

    explicit operator std::string_view() { return std::string_view{_buffer.GetString(), _buffer.GetLength()}; }

protected:
    inline writer_type& writer() { return *reinterpret_cast<writer_type*>(&_writerHold[0]); }
    inline JsonWriterImpl<true>::type& prettyWriter()
    {
        return *reinterpret_cast<JsonWriterImpl<true>::type*>(&_writerHold[0]);
    }
    rapidjson::StringBuffer _buffer;
    uint8_t _writerHold[sizeof(writer_type)];
};
}  // namespace ax
