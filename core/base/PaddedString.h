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

#include "simdjson/simdjson.h"
#include "platform/FileUtils.h"

NS_AX_BEGIN

/**
 * @addtogroup base
 * @{
 */

/**
 @brief A dedicated class for easy load padded string for simdjson parser.

 The only legal use case is:

 auto strJson = PaddedString::load(fontatlasFile);
 ondemand::parser parser;
 ondemand::document settings = parser.iterate(strJson);
 */
class PaddedString final
{
public:
    static PaddedString load(std::string_view filename)
    {
        PaddedString ret;
        FileUtils::getInstance()->getContents(filename, &ret);
        return ret;
    }

    PaddedString() = default;
    ~PaddedString() { delete[] data_ptr; }

    PaddedString(PaddedString&& o) noexcept : viable_size(o.viable_size), data_ptr(o.data_ptr)
    {
        o.data_ptr = nullptr;  // we take ownership
    }
    PaddedString& operator=(PaddedString&& o) noexcept
    {
        swap(o);
        return *this;
    }
    void swap(PaddedString& o) noexcept
    {
        std::swap(viable_size, o.viable_size);
        std::swap(data_ptr, o.data_ptr);
    }

    using value_type = char;
    size_t size() const { return viable_size; }
    void resize(size_t size)
    {
        assert(!data_ptr || size <= viable_size); // not allow enlarge
        if (!data_ptr)
            data_ptr = simdjson::internal::allocate_padded_buffer(size);
        viable_size = size;
    }

    char* data() { return data_ptr; }
    const char* data() const { return data_ptr; }

    operator simdjson::padded_string_view() const noexcept
    {
        return simdjson::padded_string_view(data(), size(), size() + simdjson::SIMDJSON_PADDING);
    }

private:
    PaddedString(const PaddedString&) = delete;
    size_t viable_size{0};
    char* data_ptr{nullptr};
};

// end of base group
/** @} */

NS_AX_END
