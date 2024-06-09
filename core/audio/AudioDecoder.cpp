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

#include "audio/AudioDecoder.h"
#include "audio/AudioMacros.h"
#include "platform/FileUtils.h"

#define LOG_TAG "AudioDecoder"

NS_AX_BEGIN

AudioDecoder::AudioDecoder()
    : _isOpened(false)
    , _totalFrames(0)
    , _bytesPerBlock(0)
    , _samplesPerBlock(1)
    , _sampleRate(0)
    , _channelCount(0)
    , _sourceFormat(AUDIO_SOURCE_FORMAT::PCM_16)
{}

AudioDecoder::~AudioDecoder() {}

bool AudioDecoder::isOpened() const
{
    return _isOpened;
}

uint32_t AudioDecoder::readFixedFrames(uint32_t framesToRead, char* pcmBuf)
{
    uint32_t framesRead     = 0;
    uint32_t framesReadOnce = 0;
    do
    {
        framesReadOnce = read(framesToRead - framesRead, pcmBuf + framesToBytes((framesRead)));
        framesRead += framesReadOnce;
    } while (framesReadOnce != 0 && framesRead < framesToRead);

    if (framesRead < framesToRead)
    {
        memset(pcmBuf + framesToBytes(framesRead), 0x0, framesToBytes(framesToRead - framesRead));
    }

    return framesRead;
}

uint32_t AudioDecoder::getTotalFrames() const
{
    return _totalFrames;
}

uint32_t AudioDecoder::framesToBytes(uint32_t frames) const
{
    return _bytesPerBlock * frames;
}

uint32_t AudioDecoder::bytesToFrames(uint32_t bytes) const
{
    return bytes / _bytesPerBlock;
}

uint32_t AudioDecoder::getSampleRate() const
{
    return _sampleRate;
}

uint32_t AudioDecoder::getChannelCount() const
{
    return _channelCount;
}

uint32_t AudioDecoder::getSamplesPerBlock() const
{
    return _samplesPerBlock;
}

AUDIO_SOURCE_FORMAT AudioDecoder::getSourceFormat() const
{
    return _sourceFormat;
}
NS_AX_END  // namespace ax

#undef LOG_TAG
