#include "axmol/platform/ImageLoader.h"
#include "axmol/base/Logging.h"

#include <algorithm>
#include <ctype.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

namespace ax
{
namespace
{
using Decoder = ImageLoader::Decoder;

struct DecoderRegistry
{
    std::unordered_map<std::string, Decoder> byExtension;
};

DecoderRegistry& registry()
{
    static DecoderRegistry value;
    return value;
}

std::mutex& registryMutex()
{
    static std::mutex value;
    return value;
}

std::string normalizeExtension(std::string_view extension)
{
    if (!extension.empty() && extension.front() == '.')
        extension.remove_prefix(1);

    std::string normalized{extension};
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char ch) { return static_cast<char>(tolower(ch)); });
    return normalized;
}
}  // namespace

void ImageLoader::registerDecoder(std::string_view extension, Decoder decoder)
{
    auto normalized = normalizeExtension(extension);
    if (normalized.empty() || !decoder)
        return;

    std::lock_guard lock(registryMutex());
    registry().byExtension[normalized] = std::move(decoder);
}

bool ImageLoader::decode(std::string_view extension,
                         const uint8_t* data,
                         ssize_t dataLen,
                         DecodedImage& result)
{
    auto normalized = normalizeExtension(extension);
    Decoder decoder;

    {
        std::lock_guard lock(registryMutex());
        auto it = registry().byExtension.find(normalized);
        if (it != registry().byExtension.end())
            decoder = it->second;
    }

    if (!decoder)
    {
        AXLOGW("Image decoder not found for extension: '{}'", normalized);
        return false;
    }

    return decoder(data, dataLen, result);
}

}  // namespace ax
