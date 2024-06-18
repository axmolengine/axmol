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
#define LOG_TAG "AudioDecoderWav"

#include <stddef.h>
#include <assert.h>
#include "audio/AudioDecoderWav.h"
#include "audio/AudioMacros.h"
#include "platform/FileUtils.h"

NS_AX_BEGIN

enum : uint32_t
{
    WAV_SIGN_ID     = MAKE_FOURCC('W', 'A', 'V', 'E'),
    WAV_FMT_ID      = MAKE_FOURCC('f', 'm', 't', ' '),
    WAV_DATA_ID     = MAKE_FOURCC('d', 'a', 't', 'a'),
    WAV_HEADER_SIZE = sizeof(struct WAV_CHUNK_HEADER),
    WAV_RIFF_SIZE   = sizeof(WAV_RIFF_CHUNK),
};

// 00000001-0000-0010-8000-00aa00389b71
static const GUID WAV_SUBTYPE_PCM = {0x00000001, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};

// 00000003-0000-0010-8000-00aa00389b71
static const GUID WAV_SUBTYPE_IEEE_FLOAT = {0x00000003,
                                            0x0000,
                                            0x0010,
                                            {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};

static bool wav_scan_chunk(WAV_FILE* wavf, uint32_t chunkID, void* header, void* body, uint32_t bodySize)
{
    auto& fs = wavf->Stream;
    auto h   = (WAV_CHUNK_HEADER*)header;
    for (; fs->read(h, WAV_HEADER_SIZE) == WAV_HEADER_SIZE;)
    {
        wavf->PcmDataOffset += WAV_HEADER_SIZE;
        if (h->ChunkID == chunkID)
        {  // chunk found
            if (body)
            {  // require read body?
                auto readsize = (std::min)(bodySize, h->ChunkSize);
                fs->read(body, readsize);
                if (h->ChunkSize > bodySize)
                    fs->seek(h->ChunkSize - bodySize, SEEK_CUR);
                wavf->PcmDataOffset += h->ChunkSize;
            }
            return true;
        }
        else
        {
            // Skip other non specified chunk
            fs->seek(h->ChunkSize, SEEK_CUR);
            wavf->PcmDataOffset += h->ChunkSize;
        }
    }
    return false;
}
static bool wav_open(std::string_view fullPath, WAV_FILE* wavf)
{
    wavf->Stream = FileUtils::getInstance()->openFileStream(fullPath, IFileStream::Mode::READ);
    if (!wavf->Stream)
        return false;

    auto& fileStream    = wavf->Stream;
    wavf->PcmDataOffset = 0;

    // Parsing RIFF chunk
    fileStream->read(&wavf->FileHeader, WAV_RIFF_SIZE);
    wavf->PcmDataOffset += WAV_RIFF_SIZE;

    if (wavf->FileHeader.Riff.Format != WAV_SIGN_ID)
        return false;  // not .wav file

    // check somthings
    auto h = &wavf->FileHeader;

    // Parsing FMT chunk
    if (!wav_scan_chunk(wavf, WAV_FMT_ID, &wavf->FileHeader.Fmt, &wavf->FileHeader.Fmt.AudioFormat,
                        sizeof(wavf->FileHeader.Fmt) - sizeof(WAV_RIFF_CHUNK)))
        return false;

    auto& fmtInfo = h->Fmt;

    int bitDepth = (fmtInfo.BitsPerSample);

    // Read PCM data or extensible data if exists.
    switch (fmtInfo.AudioFormat)
    {  // Check supported format
    case WAV_FORMAT::PCM:
    case WAV_FORMAT::IEEE:
        switch (bitDepth)
        {
        case 8:
            wavf->SourceFormat = AUDIO_SOURCE_FORMAT::PCM_U8;
            break;
        case 16:
            wavf->SourceFormat = AUDIO_SOURCE_FORMAT::PCM_16;
            break;
        case 24:
            wavf->SourceFormat = AUDIO_SOURCE_FORMAT::PCM_24;
            break;
        case 32:
            wavf->SourceFormat = (fmtInfo.AudioFormat == WAV_FORMAT::IEEE) ? AUDIO_SOURCE_FORMAT::PCM_FLT32
                                                                           : AUDIO_SOURCE_FORMAT::PCM_32;
            break;
        case 64:
            wavf->SourceFormat = (fmtInfo.AudioFormat == WAV_FORMAT::IEEE) ? AUDIO_SOURCE_FORMAT::PCM_FLT64
                                                                           : AUDIO_SOURCE_FORMAT::PCM_64;
            break;
        }
        break;
    case WAV_FORMAT::MULAW:
        wavf->SourceFormat = AUDIO_SOURCE_FORMAT::MULAW;
        break;
    case WAV_FORMAT::ALAW:
        wavf->SourceFormat = AUDIO_SOURCE_FORMAT::MULAW;
        break;
    case WAV_FORMAT::ADPCM:
        wavf->SourceFormat = AUDIO_SOURCE_FORMAT::ADPCM;
        break;
    case WAV_FORMAT::IMA_ADPCM:
        wavf->SourceFormat = AUDIO_SOURCE_FORMAT::IMA_ADPCM;
        break;
    case WAV_FORMAT::EXT:
        // Check sub-format.
        if (!IsEqualGUID(fmtInfo.ExtParams.SubFormat, WAV_SUBTYPE_PCM) &&
            !IsEqualGUID(fmtInfo.ExtParams.SubFormat, WAV_SUBTYPE_IEEE_FLOAT))
        {
            fileStream.reset();
            return false;
        }
        break;
    default:
        AXLOGW("The wav format {} doesn't supported currently!", (int)fmtInfo.AudioFormat);
        fileStream.reset();
        assert(false);
        return false;
    }

    return wav_scan_chunk(wavf, WAV_DATA_ID, &h->PcmData, nullptr, 0);
}

static int wav_read(WAV_FILE* wavf, char* pcmBuf, uint32_t bytesToRead)
{
    return wavf->Stream->read(pcmBuf, bytesToRead);
}

static int wav_seek(WAV_FILE* wavf, int offset)
{
    wavf->Stream->seek(wavf->PcmDataOffset + offset, SEEK_SET);
    const auto newOffset = wavf->Stream->tell();
    return newOffset >= wavf->PcmDataOffset ? newOffset - wavf->PcmDataOffset : -1;
}

static int wav_close(WAV_FILE* wavf)
{
    const auto result = wavf->Stream->close();
    wavf->Stream.reset();
    return result;
}

AudioDecoderWav::AudioDecoderWav()
{
    memset(&_wavf, 0, offsetof(WAV_FILE, Stream));
}

AudioDecoderWav::~AudioDecoderWav()
{
    close();
}

bool AudioDecoderWav::open(std::string_view fullPath)
{
    if (wav_open(fullPath, &_wavf))
    {
        auto& fmtInfo  = _wavf.FileHeader.Fmt;
        _sampleRate    = fmtInfo.SampleRate;
        _channelCount  = fmtInfo.NumChannels;
        _bytesPerBlock = fmtInfo.BlockAlign;  // == fmtInfo.BitsPerSample * _channelCount / 8;
        _sourceFormat  = _wavf.SourceFormat;

        // See: https://github.com/openalext/openalext/wiki/AL_SOFT_block_alignment
        switch (_sourceFormat)
        {
        case AUDIO_SOURCE_FORMAT::ADPCM:
            _samplesPerBlock = (_bytesPerBlock / _channelCount - 7) * 2 + 2;
            break;
        case AUDIO_SOURCE_FORMAT::IMA_ADPCM:
            _samplesPerBlock = (_bytesPerBlock / _channelCount - 4) / 4 * 8 + 1;
            break;
        default:;
        }

        _totalFrames = bytesToFrames(_wavf.FileHeader.PcmData.ChunkSize);

        _isOpened = true;
        return true;
    }
    return false;
}

uint32_t AudioDecoderWav::framesToBytes(uint32_t frames) const
{
    if (_samplesPerBlock == 1)
        return _bytesPerBlock * frames;

    return frames / _samplesPerBlock * _bytesPerBlock;
}

uint32_t AudioDecoderWav::bytesToFrames(uint32_t bytes) const
{
    if (_samplesPerBlock == 1)
        return bytes / _bytesPerBlock;
    return bytes / _bytesPerBlock * _samplesPerBlock;
}

void AudioDecoderWav::close()
{
    if (isOpened())
    {
        wav_close(&_wavf);
        _isOpened = false;
    }
}

uint32_t AudioDecoderWav::read(uint32_t framesToRead, char* pcmBuf)
{
    auto bytesToRead  = framesToBytes(framesToRead);
    int32_t bytesRead = wav_read(&_wavf, pcmBuf, bytesToRead);
    return bytesToFrames(bytesRead);
}

bool AudioDecoderWav::seek(uint32_t frameOffset)
{
    auto offset = framesToBytes(frameOffset);
    return wav_seek(&_wavf, offset) == offset;
}
NS_AX_END  // namespace ax

#undef LOG_TAG

