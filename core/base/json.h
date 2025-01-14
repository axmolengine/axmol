#pragma once

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "simdjson/simdjson.h"

namespace simdjson
{

/**
 * @addtogroup base
 * @{
 */
/**
 @brief A dedicated class for easy load padded string for simdjson parser.

 The only legal use case is:

 PaddedString strJson;
 FileUtils::getInstance()->getContents(filePath, &strJson);
 ondemand::parser parser;
 ondemand::document settings = parser.iterate(strJson);
 */
class PaddedString final
{
public:
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
        assert(!data_ptr || size <= viable_size);  // not allow enlarge
        if (!data_ptr)
            data_ptr = internal::allocate_padded_buffer(size);
        viable_size = size;
    }

    char* data() { return data_ptr; }
    const char* data() const { return data_ptr; }

    operator padded_string_view() const noexcept
    {
        return padded_string_view(data(), size(), size() + SIMDJSON_PADDING);
    }

private:
    PaddedString(const PaddedString&) = delete;
    size_t viable_size{0};
    char* data_ptr{nullptr};
};

// end of base group
/** @} */

}  // namespace ax::simdjson
