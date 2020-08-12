/****************************************************************************
Copyright (c) 2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2018-2020 HALX99.
 
http://www.cocos2d-x.org

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

#include "audio/include/AudioDecoderManager.h"
#include "audio/include/AudioDecoderOgg.h"
#include "audio/include/AudioMacros.h"
#include "platform/CCFileUtils.h"
#include "base/CCConsole.h"

#if CC_TARGET_PLATFORM != CC_PLATFORM_IOS
#include "audio/include/AudioDecoderMp3.h"
#include "audio/include/AudioDecoderWav.h"
#else
#include "audio/include/AudioDecoderEXT.h"
#endif

#include "yasio/cxx17/string_view.hpp"

namespace cocos2d {

bool AudioDecoderManager::init()
{
#if CC_TARGET_PLATFORM != CC_PLATFORM_IOS
    AudioDecoderMp3::lazyInit();
#endif
    return true;
}

void AudioDecoderManager::destroy()
{
#if CC_TARGET_PLATFORM != CC_PLATFORM_IOS
    AudioDecoderMp3::destroy();
#endif
}

AudioDecoder* AudioDecoderManager::createDecoder(const std::string& path)
{
    cxx17::string_view svPath(path);
    if (cxx20::ic::ends_with(svPath, ".ogg"))
    {
        return new (std::nothrow) AudioDecoderOgg();
    }
#if CC_TARGET_PLATFORM != CC_PLATFORM_IOS
    else if (cxx20::ic::ends_with(svPath, ".mp3"))
    {
        return new (std::nothrow) AudioDecoderMp3();
    }
    else if (cxx20::ic::ends_with(svPath, ".wav")) {
        return new (std::nothrow) AudioDecoderWav();
    }
#else
    else {
        return new (std::nothrow) AudioDecoderEXT();
    }
#endif

    return nullptr;
}

void AudioDecoderManager::destroyDecoder(AudioDecoder* decoder)
{
    if (decoder) decoder->close();
    delete decoder;
}

} // namespace cocos2d {

