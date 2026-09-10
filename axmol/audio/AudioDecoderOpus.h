/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#if defined(AX_ENABLE_OPUS)
#    include "axmol/audio/AudioDecoder.h"

#    include <memory>

struct OggOpusFile;

namespace ax
{

/**
 * @brief The class for decoding compressed ogg(opus) audio file to PCM buffer.
 */
class AudioDecoderOpus : public AudioDecoder
{
public:
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
     * @brief Sets frame offest to be read.
     * @param frameOffset The frame offest to be set.
     * @return true if succeed, otherwise false
     */
    bool seek(uint32_t frameOffset) override;

protected:
    AudioDecoderOpus();
    ~AudioDecoderOpus();

    AudioDecoderOpus(IFileStream* stream);
    bool open(IFileStream* stream);

    OggOpusFile* _of{nullptr};

    friend class AudioDecoderManager;
};

}  // namespace ax

#endif
