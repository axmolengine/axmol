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

#include "axmol/audio/AudioDecoderVorbis.h"
#include "axmol/audio/AudioMacros.h"
#include "axmol/platform/FileUtils.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include <unistd.h>
#    include <errno.h>
#endif

namespace ax
{

static size_t ov_fread_r(void* buffer, size_t element_size, size_t element_count, void* handle)
{
    auto* fs = static_cast<IFileStream*>(handle);
    return fs->read(buffer, static_cast<uint32_t>(element_size * element_count));
}

static int ov_fseek_r(void* handle, ogg_int64_t offset, int whence)
{
    auto* fs = static_cast<IFileStream*>(handle);
    return fs->seek(offset, whence) < 0 ? -1 : 0;
}

static long ov_ftell_r(void* handle)
{
    auto* fs = static_cast<IFileStream*>(handle);
    return fs->tell();
}

static int ov_fclose_r(void* handle)
{
    auto* fs = static_cast<IFileStream*>(handle);
    delete fs;
    return 0;
}

AudioDecoderVorbis::AudioDecoderVorbis() {}

AudioDecoderVorbis::~AudioDecoderVorbis()
{
    close();
}

bool AudioDecoderVorbis::open(std::string_view fullPath)
{
    if (_isOpened)
        return true;
    auto stream = FileUtils::getInstance()->openFileStream(fullPath, IFileStream::Mode::READ).release();
    if (!stream)
    {
        AXLOGE("Trouble with ogg(1): {}\n", strerror(errno));
        return false;
    }

    return open(stream);
}

AudioDecoderVorbis::AudioDecoderVorbis(IFileStream* stream)
{
    open(stream);
}

bool AudioDecoderVorbis::open(IFileStream* stream)
{
    static ov_callbacks OV_CALLBACKS_POSIX = {ov_fread_r, ov_fseek_r, ov_fclose_r, ov_ftell_r};

    if (0 == ov_open_callbacks(stream, &_vf, nullptr, 0, OV_CALLBACKS_POSIX))
    {
        // header
        vorbis_info* vi = ov_info(&_vf, -1);
        _sampleRate     = static_cast<uint32_t>(vi->rate);
        _channelCount   = vi->channels;
        _bytesPerBlock  = vi->channels * sizeof(short);
        _totalFrames    = static_cast<uint32_t>(ov_pcm_total(&_vf, -1));
        _isOpened       = true;
        return true;
    }
    return false;
}

void AudioDecoderVorbis::close()
{
    if (isOpened())
    {
        ov_clear(&_vf);
        _isOpened = false;
    }
}

uint32_t AudioDecoderVorbis::read(uint32_t framesToRead, char* pcmBuf)
{
    int currentSection = 0;
    int bytesToRead    = framesToBytes(framesToRead);
    int32_t bytesRead  = static_cast<int32_t>(ov_read(&_vf, pcmBuf, bytesToRead, 0, 2, 1, &currentSection));
    return bytesToFrames(bytesRead);
}

bool AudioDecoderVorbis::seek(uint32_t frameOffset)
{
    return 0 == ov_pcm_seek(&_vf, frameOffset);
}
}  // namespace ax
