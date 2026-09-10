/****************************************************************************
 Copyright (c) 2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/audio/AudioDecoder.h"
#include "axmol/audio/AudioMacros.h"
#include "axmol/platform/FileUtils.h"

namespace ax
{

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
}  // namespace ax
