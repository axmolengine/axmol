#include "axmol/platform/ImageLoader.h"
#include "axmol/base/Logging.h"

#include <algorithm>
#include <ctype.h>
#include <mutex>
#include <string>
#include "axmol/tlx/hlookup.hpp"
#include <utility>

namespace ax
{
namespace
{
using Decoder = ImageLoader::Decoder;

class DecoderRegistry
{
public:
    void registerDecoder(std::string_view extension, Decoder decoder)
    {
        if (extension.empty() || !decoder)
            return;

        std::lock_guard lock(_mtx);
        tlx::set_item(_decoders, extension, std::move(decoder));
    }

    Decoder findDecoder(std::string_view extension)
    {
        if (_decoders.empty() || extension.empty())
            return nullptr;

        std::lock_guard lock(_mtx);
        auto it = _decoders.find(extension);
        if (it != _decoders.end())
            return it->second;
        return nullptr;
    }

private:
    std::mutex _mtx;
    tlx::string_map<Decoder> _decoders;
};

DecoderRegistry& registry()
{
    static DecoderRegistry value;
    return value;
}

}  // namespace

void ImageLoader::registerDecoder(std::string_view extension, Decoder decoder)
{
    if (extension.empty() || !decoder)
        return;

    registry().registerDecoder(extension, std::move(decoder));
}

bool ImageLoader::decode(std::string_view extension, const uint8_t* data, ssize_t dataLen, DecodedImage& result)
{
    Decoder decoder = registry().findDecoder(extension);

    if (!decoder)
    {
        AXLOGW("Image decoder not found for extension: '{}'", extension);
        return false;
    }

    return decoder(data, dataLen, result);
}

}  // namespace ax
