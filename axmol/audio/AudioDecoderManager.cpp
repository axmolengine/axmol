/****************************************************************************
Copyright (c) 2016 Chukong Technologies Inc.
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

#include "axmol/audio/AudioDecoderManager.h"
#include "axmol/audio/AudioDecoderVorbis.h"
#include "axmol/audio/AudioDecoderOpus.h"
#include "axmol/audio/AudioMacros.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/base/Logging.h"

#if !defined(__APPLE__)
#    include "axmol/audio/AudioDecoderMp3.h"
#    include "axmol/audio/AudioDecoderWav.h"
#else
#    include "axmol/audio/AudioDecoderEXT.h"
#endif

#include "yasio/tlx/string_view.hpp"

namespace ax
{

bool AudioDecoderManager::init()
{
#if !defined(__APPLE__)
    AudioDecoderMp3::lazyInit();
#endif
    return true;
}

void AudioDecoderManager::destroy()
{
#if !defined(__APPLE__)
    AudioDecoderMp3::destroy();
#endif
}

AudioDecoder* AudioDecoderManager::createDecoder(std::string_view path)
{
    if (tlx::ic::ends_with(path, ".ogg") || tlx::ic::ends_with(path, ".opus"))
    {
        constexpr char VORBIS_SIGN[]        = {0x1, 'v', 'o', 'r', 'b', 'i', 's', '\0'};
        constexpr char OPUS_SIGN[]          = {'O', 'p', 'u', 's', 'H', 'e', 'a', 'd'};
        constexpr int OGG_CODEC_SIGN_OFFSET = 28;
        constexpr int OGG_CODEC_SIGN_SIZE   = 8;

        auto stream = FileUtils::getInstance()->openFileStream(path, IFileStream::Mode::READ);
        if (!stream)
            return nullptr;
        if (stream->size() < OGG_CODEC_SIGN_OFFSET + OGG_CODEC_SIGN_SIZE)
        {
            return nullptr;
        }
        stream->seek(OGG_CODEC_SIGN_OFFSET, SEEK_SET);
        char codecSign[OGG_CODEC_SIGN_SIZE];
        stream->read(codecSign, OGG_CODEC_SIGN_SIZE);
        stream->seek(0, SEEK_SET);

        if (memcmp(codecSign, VORBIS_SIGN, OGG_CODEC_SIGN_SIZE) == 0)
        {
            return new AudioDecoderVorbis(stream.release());
        }
#if defined(AX_ENABLE_OPUS)
        else if (memcmp(codecSign, OPUS_SIGN, OGG_CODEC_SIGN_SIZE) == 0)
        {
            return new AudioDecoderOpus(stream.release());
        }
#endif
        else
        {
            AXLOGE("Fail create decoder due to codec error in file: {}", path);
            return nullptr;
        }
    }
#if !defined(__APPLE__)
    else if (tlx::ic::ends_with(path, ".mp3"))
    {
        return new AudioDecoderMp3();
    }
    else if (tlx::ic::ends_with(path, ".wav"))
    {
        return new AudioDecoderWav();
    }
#else
    else
    {
        return new AudioDecoderEXT();
    }
#endif

    return nullptr;
}

void AudioDecoderManager::destroyDecoder(AudioDecoder* decoder)
{
    if (decoder)
        decoder->close();
    delete decoder;
}

}  // namespace ax
