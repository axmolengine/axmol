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
#import <AudioToolbox/ExtendedAudioFile.h>
#include "audio/AudioDecoder.h"
#include "platform/IFileStream.h"

namespace ax
{

/**
 * @brief The class for decoding compressed audio file to PCM buffer.
 */
class AudioDecoderEXT : public AudioDecoder
{
public:
    static const uint32_t INVALID_FRAME_INDEX = UINT32_MAX;

    AudioDecoderEXT();
    ~AudioDecoderEXT();

    /**
     * @brief Opens an audio file specified by a file path.
     * @return true if succeed, otherwise false.
     */
    bool open(std::string_view path) override;

    /**
     * @brief Closes opened audio file.
     * @note The method will also be automatically invoked in the destructor.
     */
    void close() override;

    /**
     * @brief Reads audio frames of PCM format.
     * @param framesToRead The number of frames excepted to be read.
     * @param pcmBuf The buffer to hold the frames to be read, its size should be >= |framesToRead| / samplesPerBlock *
     * _bytesPerBlock.
     * @return The number of frames actually read, it's probably less than 'framesToRead'. Returns 0 means reach the end
     * of file.
     */
    uint32_t read(uint32_t framesToRead, char* pcmBuf) override;

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
    // uint32_t readFixedFrames(uint32_t framesToRead, char* pcmBuf) override;

    /**
     * @brief Sets frame offest to be read.
     * @param frameOffset The frame offest to be set.
     * @return true if succeed, otherwise false
     */
    bool seek(uint32_t frameOffset) override;

private:
    void closeInternal();

    static OSStatus readCallback(void* inClientData,
                                 SInt64 inPosition,
                                 UInt32 requestCount,
                                 void* buffer,
                                 UInt32* actualCount);
    static SInt64 getSizeCallback(void* inClientData);

    ExtAudioFileRef _extRef;
    std::unique_ptr<IFileStream> _fileStream;
    SInt64 _streamSize;
    AudioFileID _audioFileId;

    AudioStreamBasicDescription _outputFormat;
};

}  // namespace ax
