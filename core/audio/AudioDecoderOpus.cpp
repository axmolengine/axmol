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

 How to convert wav to .opus ?

 - download official opus-tools: https://opus-codec.org/downloads/
 - directly download link: https://archive.mozilla.org/pub/opus/win32/opus-tools-0.2-opus-1.3.zip

 ****************************************************************************/

#if defined(AX_ENABLE_OPUS)

#    include "audio/AudioDecoderOpus.h"
#    include "audio/AudioMacros.h"
#    include "platform/FileUtils.h"

#    include "opus/opusfile.h"

#    if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
#        include <unistd.h>
#        include <errno.h>
#    endif

namespace ax
{

static int op_fseek_r(void* handle, ogg_int64_t offset, int whence)
{
    auto* fs = static_cast<IFileStream*>(handle);
    return fs->seek(offset, whence) < 0 ? -1 : 0;
}

static int op_fclose_r(void* handle)
{
    auto* fs = static_cast<IFileStream*>(handle);
    delete fs;
    return 0;
}

static int op_fread_r(void* _stream, unsigned char* _ptr, int _nbytes)
{
    auto* fs = static_cast<IFileStream*>(_stream);
    return fs->read(_ptr, _nbytes);
}

static opus_int64 op_ftell_r(void* handle)
{
    auto* fs = static_cast<IFileStream*>(handle);
    return fs->tell();
}

AudioDecoderOpus::AudioDecoderOpus() {}

AudioDecoderOpus::AudioDecoderOpus(IFileStream* stream)
{
    open(stream);
}

AudioDecoderOpus::~AudioDecoderOpus()
{
    close();
}

bool AudioDecoderOpus::open(IFileStream* stream)
{
    static OpusFileCallbacks OP_CALLBACKS_POSIX = {op_fread_r, op_fseek_r, op_ftell_r, op_fclose_r};

    if (_of = op_open_callbacks(stream, &OP_CALLBACKS_POSIX, 0, 0, nullptr))
    {
        auto vi        = op_head(_of, -1);
        _sampleRate    = 48000;  // opus always 48K hz
        _channelCount  = vi->channel_count;
        _bytesPerBlock = vi->channel_count * sizeof(short);
        _totalFrames   = static_cast<uint32_t>(op_pcm_total(_of, -1));
        _isOpened      = true;
    }

    return _isOpened;
}

bool AudioDecoderOpus::open(std::string_view fullPath)
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

void AudioDecoderOpus::close()
{
    if (isOpened())
    {
        if (_of)
            op_free(_of);
        _isOpened = false;
    }
}

uint32_t AudioDecoderOpus::read(uint32_t framesToRead, char* pcmBuf)
{
    return op_read(_of, (opus_int16*)pcmBuf, framesToRead, nullptr);
}

bool AudioDecoderOpus::seek(uint32_t frameOffset)
{
    return op_pcm_seek(_of, frameOffset);
}
}  // namespace ax

#endif
