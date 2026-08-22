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
