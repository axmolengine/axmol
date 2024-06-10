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

#define LOG_TAG "AudioDecoderManager"

#include "audio/AudioDecoderManager.h"
#include "audio/AudioDecoderOgg.h"
#include "audio/AudioMacros.h"
#include "platform/FileUtils.h"
#include "base/Logging.h"

#if !defined(__APPLE__)
#    include "audio/AudioDecoderMp3.h"
#    include "audio/AudioDecoderWav.h"
#else
#    include "audio/AudioDecoderEXT.h"
#endif

#include "yasio/string_view.hpp"

NS_AX_BEGIN

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
    if (cxx20::ic::ends_with(path, ".ogg"))
    {
        return new AudioDecoderOgg();
    }
#if !defined(__APPLE__)
    else if (cxx20::ic::ends_with(path, ".mp3"))
    {
        return new AudioDecoderMp3();
    }
    else if (cxx20::ic::ends_with(path, ".wav"))
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

NS_AX_END  // namespace ax

#undef LOG_TAG
