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
 ****************************************************************************/

#pragma once

#include "audio/AudioDecoder.h"
#include <memory>

#if !defined(MAKE_FOURCC)
#    define MAKE_FOURCC(a, b, c, d) ((uint32_t)((a) | ((b) << 8) | ((c) << 16) | (((uint32_t)(d)) << 24)))
#endif

#if !defined(_WIN32)
typedef struct _GUID
{
    uint32_t Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID;
__inline int IsEqualGUID(const GUID& rguid1, const GUID& rguid2)
{
    return !::memcmp(&rguid1, &rguid2, sizeof(GUID));
}
#else
#include <guiddef.h>
#endif

namespace ax
{

// http://soundfile.sapp.org/doc/WaveFormat/
enum class WAV_FORMAT : uint16_t
{
    UNKNOWN   = 0x0,    // Unknown Wave Format
    PCM       = 0x1,    // PCM Format
    ADPCM     = 0x2,    // Microsoft ADPCM Format
    IEEE      = 0x3,    // IEEE float/double
    ALAW      = 0x6,    // 8-bit ITU-T G.711 A-law
    MULAW     = 0x7,    // 8-bit ITU-T G.711 MUL-law
    IMA_ADPCM = 0x11,   // IMA ADPCM Format
    EXT       = 0xFFFE  // Set via subformat
};

#pragma pack(push, 1)
struct WAV_CHUNK_HEADER
{
    uint32_t ChunkID;
    /*
    ** The ChunkSize gives the size of the valid data in the chunk. It does not include the padding, the size of
    *chunkID, or the size of chunkSize.
    ** see: https://docs.microsoft.com/en-us/windows/win32/xaudio2/resource-interchange-file-format--riff-
    */
    uint32_t ChunkSize;
};
struct WAV_RIFF_CHUNK
{
    WAV_CHUNK_HEADER Header;
    uint32_t Format;
};
struct WAV_FMT_CHUNK
{
    WAV_CHUNK_HEADER Header;
    WAV_FORMAT AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
    struct
    {
        /* The follow fields is optional */
        uint16_t cbSize;
        union
        {
            uint16_t ValidBitsPerSample;
            uint16_t SamplesPerBlock;
            uint16_t Reserved;
        } Samples;
        uint32_t ChannelMask;
        GUID SubFormat;
    } ExtParams;
};
struct WAV_FILE_HEADER
{
    WAV_RIFF_CHUNK Riff;
    WAV_FMT_CHUNK Fmt;
    WAV_CHUNK_HEADER PcmData;
};
#pragma pack(pop)

struct WAV_FILE
{
    WAV_FILE_HEADER FileHeader;
    AUDIO_SOURCE_FORMAT SourceFormat;
    uint32_t PcmDataOffset;
    std::unique_ptr<IFileStream> Stream{};
};

/**
 * @brief The class for decoding compressed audio file to PCM buffer.
 */
class AudioDecoderWav : public AudioDecoder
{
public:
    /**
     * @brief Opens an audio file specified by a file path.
     * @return true if succeed, otherwise false.
     */
    bool open(std::string_view path) override;

    /**
     * @brief The helper function for convert frames to bytes
     */
    uint32_t framesToBytes(uint32_t frames) const override;

    /**
     * @brief The helper function for convert bytes to frames
     */
    uint32_t bytesToFrames(uint32_t bytes) const override;

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
    friend class AudioDecoderManager;
    AudioDecoderWav();
    ~AudioDecoderWav();

    mutable WAV_FILE _wavf;
};

}  // namespace ax
