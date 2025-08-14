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

#pragma once

#include <stdint.h>
#include <string>
#include "platform/IFileStream.h"

namespace ax
{

enum class AUDIO_SOURCE_FORMAT : uint16_t
{
    PCM_UNK,  // Unknown
    PCM_U8,
    PCM_16,
    PCM_24,
    PCM_32,
    PCM_64,
    PCM_FLT32,
    PCM_FLT64,
    MULAW,
    ALAW,
    ADPCM,      // Microsoft ADPCM
    IMA_ADPCM,  // IMA ADPCM
};

/**
 * @brief The class for decoding compressed audio file to PCM buffer.
 */
class AudioDecoder
{
public:
    static const uint32_t INVALID_FRAME_INDEX = UINT32_MAX;

    /**
     * @brief Opens an audio file specified by a file path.
     * @return true if succeed, otherwise false.
     */
    virtual bool open(std::string_view path) = 0;

    /**
     * @brief Checks whether decoder has opened file successfully.
     * @return true if succeed, otherwise false.
     */
    virtual bool isOpened() const;

    /**
     * @brief Closes opened audio file.
     * @note The method will also be automatically invoked in the destructor.
     */
    virtual void close() = 0;

    /**
     * @brief Reads audio frames of PCM format.
     * @param framesToRead The number of frames excepted to be read.
     * @param pcmBuf The buffer to hold the frames to be read, its size should be >= |framesToRead| / samplesPerBlock *
     * _bytesPerBlock.
     * @return The number of frames actually read, it's probably less than 'framesToRead'. Returns 0 means reach the end
     * of file.
     */
    virtual uint32_t read(uint32_t framesToRead, char* pcmBuf) = 0;

    /**
     * @brief Reads fixed audio frames of PCM format.
     * @param framesToRead The number of frames excepted to be read.
     * @param pcmBuf The buffer to hold the frames to be read, its size should be >= |framesToRead| / samplesPerBlock *
     * _bytesPerBlock.
     * @return The number of frames actually read, it's probably less than |framesToRead|. Returns 0 means reach the end
     * of file.
     * @note The different between |read| and |readFixedFrames| is |readFixedFrames| will do multiple reading operations
     * if |framesToRead| frames isn't filled entirely, while |read| just does reading operation once whatever
     * |framesToRead| is or isn't filled entirely. If current position reaches the end of frames, the return value may
     * smaller than |framesToRead| and the remaining buffer in |pcmBuf| will be set with silence data (0x00).
     */
    virtual uint32_t readFixedFrames(uint32_t framesToRead, char* pcmBuf);

    /**
     * @brief Sets frame offest to be read.
     * @param frameOffset The frame offest to be set.
     * @return true if succeed, otherwise false
     */
    virtual bool seek(uint32_t frameOffset) = 0;

    /** Gets total frames of current audio.*/
    virtual uint32_t getTotalFrames() const;

    /**
     * @brief The helper function for convert frames to bytes
     */
    virtual uint32_t framesToBytes(uint32_t frames) const;

    /**
     * @brief The helper function for convert bytes to frames
     */
    virtual uint32_t bytesToFrames(uint32_t bytes) const;

    /** Gets sample rate of current audio.*/
    virtual uint32_t getSampleRate() const;

    /** Gets the channel count of current audio.
     * @note Currently we only support 1 or 2 channels.
     */
    virtual uint32_t getChannelCount() const;

    /*
     * @brief Gets samples per block, usually is 1 for .mp3 or .ogg, .wav may > 1
     */
    uint32_t getSamplesPerBlock() const;

    virtual AUDIO_SOURCE_FORMAT getSourceFormat() const;

protected:
    AudioDecoder();
    virtual ~AudioDecoder();

    bool _isOpened;
    uint32_t _totalFrames;
    uint32_t _bytesPerBlock;  // Same as bytesPerFrame when _samplesPerBlock is 1
    uint32_t _samplesPerBlock;
    uint32_t _sampleRate;
    uint32_t _channelCount;
    AUDIO_SOURCE_FORMAT _sourceFormat;

    friend class AudioDecoderManager;
};

}  // namespace ax
