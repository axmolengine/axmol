/****************************************************************************
 Copyright (c) 2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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
 
#define LOG_TAG "AudioDecoderOgg"

#include "audio/include/AudioDecoderOgg.h"
#include "audio/include/AudioMacros.h"
#include "platform/CCFileUtils.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <unistd.h>
#include <errno.h>
#endif


namespace cocos2d {


    static size_t ov_fread_r(void* buffer, size_t element_size, size_t element_count, void* handle)
    {
        return ((PXFileStream*)handle)->read(buffer, static_cast<uint32_t>(element_size * element_count));
    }

    static int ov_fseek_r(void * handle, ogg_int64_t offset, int whence)
    {
        auto n = ((PXFileStream*)handle)->seek(offset, whence);
        return n >= 0 ? 0 : -1;
    }
    
    static long ov_ftell_r(void * handle)
    {
        return ((PXFileStream*)handle)->seek(0, SEEK_CUR);
    }

    static int ov_fclose_r(void* handle) {
        ((PXFileStream*)handle)->close();
        return 0;
    }
    
    AudioDecoderOgg::AudioDecoderOgg()
    {
    }

    AudioDecoderOgg::~AudioDecoderOgg()
    {
        close();
    }

    bool AudioDecoderOgg::open(const std::string& fullPath)
    {
        if (!_fileStream.open(fullPath))
        {
            ALOGE("Trouble with ogg(1): %s\n", strerror(errno));
            return false;
        }

        static ov_callbacks OV_CALLBACKS_POSIX = {
               ov_fread_r,
               ov_fseek_r,
               ov_fclose_r,
               ov_ftell_r
        };
              
        if (0 == ov_open_callbacks(&_fileStream, &_vf, nullptr, 0, OV_CALLBACKS_POSIX))
        {
            // header
            vorbis_info* vi = ov_info(&_vf, -1);
            _sampleRate = static_cast<uint32_t>(vi->rate);
            _channelCount = vi->channels;
            _bytesPerFrame = vi->channels * sizeof(short);
            _totalFrames = static_cast<uint32_t>(ov_pcm_total(&_vf, -1));
            _isOpened = true;
            return true;
        }
        return false;
    }

    void AudioDecoderOgg::close()
    {
        if (isOpened())
        {
            ov_clear(&_vf);
            _isOpened = false;
        }
    }

    uint32_t AudioDecoderOgg::read(uint32_t framesToRead, char* pcmBuf)
    {
        int currentSection = 0;
        int bytesToRead = framesToRead * _bytesPerFrame;
        long bytesRead = ov_read(&_vf, pcmBuf, bytesToRead, 0, 2, 1, &currentSection);
        return static_cast<uint32_t>(bytesRead / _bytesPerFrame);
    }

    bool AudioDecoderOgg::seek(uint32_t frameOffset)
    {
        return 0 == ov_pcm_seek(&_vf, frameOffset);
    }

    uint32_t AudioDecoderOgg::tell() const
    {
        return static_cast<uint32_t>(ov_pcm_tell(const_cast<OggVorbis_File*>(&_vf)));
    }

} // namespace cocos2d {
