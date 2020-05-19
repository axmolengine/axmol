/****************************************************************************

 Copyright (c) 2018-2020 HALX99.

 http://www.cocos2d-x.org

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
#include "audio/include/AudioDecoderWav.h"
#include "audio/include/AudioMacros.h"
#include "platform/CCFileUtils.h"

#include <mmreg.h>

namespace cocos2d {
    enum : uint32_t {
        WAV_SIGN_ID = MAKE_FOURCC('W', 'A', 'V', 'E'),
        WAV_FMT_ID = MAKE_FOURCC('f', 'm', 't', ' '),
        WAV_DATA_ID = MAKE_FOURCC('d', 'a', 't', 'a'),
        WAV_HEADER_SIZE = sizeof(struct WAV_CHUNK_HEADER),
        WAV_RIFF_SIZE = sizeof(WAV_RIFF_CHUNK),
    };

    // 00000001-0000-0010-8000-00aa00389b71
    static const GUID WAV_SUBTYPE_PCM = {
        0x00000001, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
    };

    // 00000003-0000-0010-8000-00aa00389b71
    static const GUID WAV_SUBTYPE_IEEE_FLOAT = {
        0x00000003, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
    };

    static bool wav_scan_chunk(WAV_FILE* wavf, uint32_t chunkID, void* header, void* body, uint32_t bodySize) {
        auto& fs = wavf->Stream;
        auto h = (WAV_CHUNK_HEADER*)header;
        for (; fs.read(h, WAV_HEADER_SIZE) == WAV_HEADER_SIZE; ) {
            wavf->PcmDataOffset += WAV_HEADER_SIZE;
            if (h->ChunkID == chunkID)
            { // chunk found
                if (body)
                { // require read body?
                    auto readsize = (std::min)(bodySize, h->ChunkSize);
                    fs.read(body, readsize);
                    if (h->ChunkSize > bodySize)
                        fs.seek(h->ChunkSize - bodySize, SEEK_CUR);
                    wavf->PcmDataOffset += h->ChunkSize;
                }
                return true;
            }
            else {
                // Skip other non specified chunk
                fs.seek(h->ChunkSize, SEEK_CUR);
                wavf->PcmDataOffset += h->ChunkSize;
            }
        }
        return false;
    }
    static bool wav_open(const std::string& fullPath, WAV_FILE* wavf)
    {
        bool succeed = wavf->Stream.open(fullPath);
        if (!succeed)
            return false;

        auto& fileStream = wavf->Stream;
        wavf->PcmDataOffset = 0;

        // Parsing RIFF chunk
        fileStream.read(&wavf->FileHeader, WAV_RIFF_SIZE);
        wavf->PcmDataOffset += WAV_RIFF_SIZE;

        if (wavf->FileHeader.Riff.Format != WAV_SIGN_ID)
            return false; // not .wav file

        // check somthings
        auto h = &wavf->FileHeader;

        // Parsing FMT chunk
        if (!wav_scan_chunk(wavf, WAV_FMT_ID, &wavf->FileHeader.Fmt, &wavf->FileHeader.Fmt.AudioFormat, sizeof(wavf->FileHeader.Fmt) - sizeof(WAV_RIFF_CHUNK)))
            return false;

        auto& fmtInfo = h->Fmt;
        wavf->BytesPerFrame = fmtInfo.BitsPerSample / 8 * fmtInfo.NumChannels;

        int bitDepth = (wavf->BytesPerFrame / fmtInfo.NumChannels) << 3;

        // Read PCM data or extensible data if exists.
        switch (fmtInfo.AudioFormat)
        { // Check supported format
        case WAV_FORMAT::PCM:
        case WAV_FORMAT::IEEE:
            switch (bitDepth)
            {
            case 4: wavf->SourceFormat = AUDIO_SOURCE_FORMAT::PCM_16; break;
            case 8: wavf->SourceFormat = AUDIO_SOURCE_FORMAT::PCM_U8; break;
            case 16: wavf->SourceFormat = AUDIO_SOURCE_FORMAT::PCM_16; break;
            case 24: wavf->SourceFormat = AUDIO_SOURCE_FORMAT::PCM_24; break;
            case 32: wavf->SourceFormat = (fmtInfo.AudioFormat == WAV_FORMAT::IEEE) ? AUDIO_SOURCE_FORMAT::PCM_FLT32 : AUDIO_SOURCE_FORMAT::PCM_32; break;
            case 64: wavf->SourceFormat = (fmtInfo.AudioFormat == WAV_FORMAT::IEEE) ? AUDIO_SOURCE_FORMAT::PCM_FLT64 : AUDIO_SOURCE_FORMAT::PCM_64; break;
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
            if (!IsEqualGUID(fmtInfo.ExtParams.SubFormat, WAV_SUBTYPE_PCM)
                && !IsEqualGUID(fmtInfo.ExtParams.SubFormat, WAV_SUBTYPE_IEEE_FLOAT))
            {
                fileStream.close();
                return false;
            }
            break;
        default:
            ALOGW("The wav format %d doesn't supported currently!", (int)fmtInfo.AudioFormat);
            fileStream.close();
            assert(false);
            return false;;
        }

        return wav_scan_chunk(wavf, WAV_DATA_ID, &h->PcmData, nullptr, 0);
    }

    static int wav_read(WAV_FILE* wavf, char* pcmBuf, uint32_t bytesToRead)
    {
        return wavf->Stream.read(pcmBuf, bytesToRead);
    }

    static int wav_pcm_seek(WAV_FILE* wavf, int frameOffset)
    {
        auto offset = frameOffset * wavf->BytesPerFrame + wavf->PcmDataOffset;
        return wavf->Stream.seek(offset, SEEK_SET) >= 0 ? 0 : -1;
    }

    static int wav_pcm_tell(WAV_FILE* wavf)
    {
        auto offset = wavf->Stream.seek(0, SEEK_CUR);
        return (offset - wavf->PcmDataOffset) / wavf->BytesPerFrame;
    }

    static int wav_close(WAV_FILE* wavf)
    {
        return  wavf->Stream.close();
    }

    AudioDecoderWav::AudioDecoderWav()
    {
        memset(&_wavf, 0, offsetof(WAV_FILE, Stream));
    }

    AudioDecoderWav::~AudioDecoderWav()
    {
        close();
    }

    bool AudioDecoderWav::open(const std::string& fullPath)
    {
        if (wav_open(fullPath, &_wavf))
        {
            _sampleRate = _wavf.FileHeader.Fmt.SampleRate;
            _channelCount = _wavf.FileHeader.Fmt.NumChannels;
            _bytesPerFrame = _wavf.BytesPerFrame;
            _totalFrames = _wavf.FileHeader.PcmData.ChunkSize / _bytesPerFrame;
            _sourceFormat = _wavf.SourceFormat;

            _isOpened = true;
            return true;
        }
        return false;
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
        auto bytesToRead = _bytesPerFrame * framesToRead;
        long bytesRead = wav_read(&_wavf, pcmBuf, bytesToRead);
        return static_cast<uint32_t>(bytesRead / _bytesPerFrame);
    }

    bool AudioDecoderWav::seek(uint32_t frameOffset)
    {
        return wav_pcm_seek(&_wavf, frameOffset) == 0;
    }

    uint32_t AudioDecoderWav::tell() const
    {
        return wav_pcm_tell(&_wavf);
    }

} // namespace cocos2d {
