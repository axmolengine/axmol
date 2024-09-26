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

#define LOG_TAG "AudioDecoderMp3"
#include "audio/AudioDecoderMp3.h"
#include "audio/AudioMacros.h"
#include "platform/FileUtils.h"

#include "base/Logging.h"

#if !AX_USE_MPG123
#    define MINIMP3_IMPLEMENTATION
#    include "minimp3/minimp3_ex.h"
#else
// because the cocos2d-x engine has ssize_t typedef, so disable mpg123 ssize_t
#    define MPG123_DEF_SSIZE_T
#    include "mpg123.h"
#endif

#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#    include <unistd.h>
#    include <errno.h>
#endif

#if !AX_USE_MPG123
struct mp3dec_impl
{
    mp3dec_ex_t _dec;
    mp3dec_io_t _decIO;
};
#endif

namespace ax
{

#if !AX_USE_MPG123
static size_t minimp3_read_r(void* buf, size_t size, void* user_data)
{
    return ((IFileStream*)user_data)->read(buf, size);
}

static int minimp3_seek_r(uint64_t position, void* user_data)
{
    return ((IFileStream*)user_data)->seek(position, SEEK_SET) < 0 ? -1 : 0;
}
#else
static bool __mp3Inited = false;

static ssize_t mpg123_read_r(void* handle, void* buffer, size_t count)
{
    return ((IFileStream*)handle)->read(buffer, count);
}

static off_t mpg123_lseek_r(void* handle, off_t offset, int whence)
{
    return ((IFileStream*)handle)->seek(offset, whence);
}

void mpg123_close_r(void* handle)
{
    ((IFileStream*)handle)->close();
}
#endif
bool AudioDecoderMp3::lazyInit()
{
    bool ret = true;
#if AX_USE_MPG123
    if (!__mp3Inited)
    {
        int error = mpg123_init();
        if (error == MPG123_OK)
        {
            __mp3Inited = true;
        }
        else
        {
            AXLOGE("Basic setup goes wrong: {}", mpg123_plain_strerror(error));
            ret = false;
        }
    }
#endif
    return ret;
}

void AudioDecoderMp3::destroy()
{
#if AX_USE_MPG123
    if (__mp3Inited)
    {
        mpg123_exit();
        __mp3Inited = false;
    }
#endif
}

AudioDecoderMp3::AudioDecoderMp3() : _handle(nullptr)
{
    lazyInit();
}

AudioDecoderMp3::~AudioDecoderMp3()
{
    close();
}

bool AudioDecoderMp3::open(std::string_view fullPath)
{
#if !AX_USE_MPG123
    do
    {
        _fileStream = FileUtils::getInstance()->openFileStream(fullPath, IFileStream::Mode::READ);
        if (!_fileStream)
        {
            AXLOGE("Trouble with minimp3(1): {}\n", strerror(errno));
            break;
        }

        auto handle = new mp3dec_impl();

        handle->_decIO.read      = minimp3_read_r;
        handle->_decIO.seek      = minimp3_seek_r;
        handle->_decIO.read_data = handle->_decIO.seek_data = _fileStream.get();

        if (mp3dec_ex_open_cb(&handle->_dec, &handle->_decIO, MP3D_SEEK_TO_SAMPLE) != 0)
        {
            delete handle;
            break;
        }

        auto& info    = handle->_dec.info;
        _channelCount = info.channels;  // the _channelCount is samplesPerFrame
        _sampleRate   = info.hz;
#    ifndef MINIMP3_FLOAT_OUTPUT
        _sourceFormat  = AUDIO_SOURCE_FORMAT::PCM_16;
        _bytesPerBlock = sizeof(uint16_t) * _channelCount;
#    else
        _sourceFormat  = AUDIO_SOURCE_FORMAT::PCM_FLT32;
        _bytesPerBlock = sizeof(float) * _channelCount;
#    endif
        // samples
        _totalFrames = handle->_dec.samples / _channelCount;

        // store the handle
        _handle = handle;

        _isOpened = true;
        return true;
    } while (false);

    return false;
#else
    int32_t rate    = 0;
    int error       = MPG123_OK;
    int mp3Encoding = 0;
    int channel     = 0;
    do
    {
        _handle = mpg123_new(nullptr, &error);
        if (nullptr == _handle)
        {
            AXLOGE("Basic setup goes wrong: {}", mpg123_plain_strerror(error));
            break;
        }

        if (!_fileStream.open(fullPath))
        {
            AXLOGE("Trouble with mpg123(1): {}\n", strerror(errno));
            break;
        }

        mpg123_replace_reader_handle(_handle, mpg123_read_r, mpg123_lseek_r, mpg123_close_r);

        if (mpg123_open_handle(_handle, _fileStream) != MPG123_OK ||
            mpg123_getformat(_handle, &rate, &channel, &mp3Encoding) != MPG123_OK)
        {
            AXLOGE("Trouble with mpg123(2): {}\n", mpg123_strerror(_handle));
            break;
        }

        _channelCount = channel;
        _sampleRate   = rate;

        if (mp3Encoding == MPG123_ENC_SIGNED_16)
        {
            _bytesPerBlock = 2 * _channelCount;
            _sourceFormat  = AUDIO_SOURCE_FORMAT::PCM_16;
        }
        else if (mp3Encoding == MPG123_ENC_FLOAT_32)
        {
            _bytesPerBlock = 4 * _channelCount;
            _sourceFormat  = AUDIO_SOURCE_FORMAT::PCM_FLT32;
        }
        else
        {
            AXLOGE("Bad encoding: {0:#x}!\n", mp3Encoding);
            break;
        }

        /* Ensure that this output format will not change (it could, when we allow it). */
        mpg123_format_none(_handle);
        mpg123_format(_handle, rate, channel, mp3Encoding);
        /* Ensure that we can get accurate length by call mpg123_length */
        mpg123_scan(_handle);

        _totalFrames = mpg123_length(_handle);

        _isOpened = true;
        return true;
    } while (false);

    if (_handle != nullptr)
    {
        mpg123_close(_handle);
        mpg123_delete(_handle);
        _handle = nullptr;
    }
    return false;
#endif
}

void AudioDecoderMp3::close()
{
    if (isOpened())
    {
#if !AX_USE_MPG123
        if (_handle)
        {
            mp3dec_ex_close(&_handle->_dec);

            delete _handle;
            _handle = nullptr;
            _fileStream.reset();
        }
#else
        if (_handle != nullptr)
        {
            mpg123_close(_handle);
            mpg123_delete(_handle);
            _handle = nullptr;

            _fileStream.reset();
        }
#endif
        _isOpened = false;
    }
}

uint32_t AudioDecoderMp3::read(uint32_t framesToRead, char* pcmBuf)
{
#if !AX_USE_MPG123
    auto sampelsToRead = framesToRead * _channelCount;
    auto samplesRead   = mp3dec_ex_read(&_handle->_dec, (mp3d_sample_t*)pcmBuf, sampelsToRead);
    return static_cast<uint32_t>(samplesRead / _channelCount);
#else
    int bytesToRead  = framesToBytes(framesToRead);
    size_t bytesRead = 0;
    int err          = mpg123_read(_handle, (unsigned char*)pcmBuf, bytesToRead, &bytesRead);
    if (err == MPG123_ERR)
    {
        AXLOGE("Trouble with mpg123: {}\n", mpg123_strerror(_handle));
        return 0;
    }

    return bytesToFrames(bytesRead);
#endif
}

bool AudioDecoderMp3::seek(uint32_t frameOffset)
{
#if !AX_USE_MPG123
    return 0 == mp3dec_ex_seek(&_handle->_dec, frameOffset);
#else
    off_t offset = mpg123_seek(_handle, frameOffset, SEEK_SET);
    // AXLOGD("mpg123_seek return: {}", (int)offset);
    return (offset >= 0 && offset == frameOffset);
#endif
}
}  // namespace ax

#undef LOG_TAG
