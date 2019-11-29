/****************************************************************************
 Copyright (c) 2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2018 HALX99.

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
#include "audio/include/AudioDecoderWav.h"
#include "audio/include/AudioMacros.h"
#include "platform/CCFileUtils.h"

#if !defined(_WIN32)
typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;
__inline int IsEqualGUID(const GUID& rguid1, const GUID& rguid2)
{
    return !::memcmp(&rguid1, &rguid2, sizeof(GUID));
}
#endif

static const size_t WAV_FILE_HEADER_SIZE = sizeof(WAV_FILE_HEADER);

// 00000001-0000-0010-8000-00aa00389b71
static const GUID WavSubTypePCM = {
    0x00000001, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
};

// 00000003-0000-0010-8000-00aa00389b71
static const GUID WavSubTypeIEEE_FLOAT = {
    0x00000003, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
};

bool wav_open(const std::string& fullPath, WAV_FILE* wavf)
{
    bool succeed = wavf->FileStream.open(fullPath);
    if (!succeed)
        return false;
    
    auto& fileStream = wavf->FileStream;
    wavf->PcmDataOffset = 0;

    // Read to BitsPerSample
    int bytesToRead = offsetof(struct WAV_FILE_HEADER, Subchunk2ID) - offsetof(struct WAV_FILE_HEADER, ChunkID);
    fileStream.read(&wavf->FileHeader, bytesToRead);
    wavf->PcmDataOffset += bytesToRead;

    // check somthings
    auto h = &wavf->FileHeader;
     // Read PCM data or extensible data if exists.
    if (h->Subchunk1Size == 16 && h->AudioFormat == 1)
    {
        // PCM
        do {
            // Note: 8-bit samples are stored as unsigned bytes, ranging from 0 to 255. 16-bit samples are stored as 2's-complement signed integers, ranging from -32768 to 32767.
            // data
            fileStream.read(&h->Subchunk2ID, sizeof(uint32_t));    // 0x61746164, "data"
            fileStream.read(&h->Subchunk2Size, sizeof(uint32_t));
            wavf->PcmDataOffset += 8;

            if (h->Subchunk2ID == 0x61746164) { // Skip other non "data" chunk
                break;
            }
            else {
                fileStream.seek(h->Subchunk2Size, SEEK_CUR);
                wavf->PcmDataOffset += h->Subchunk2Size;
            }
        } while (true);
    }
    else if (h->Subchunk1Size > 16 && h->AudioFormat == 0xFFFE)
    {
        fileStream.read(&wavf->ExtraParamSize, sizeof(uint16_t));
        wavf->PcmDataOffset += 2;

        if (wavf->ExtraParamSize == 22)
        {
            // if cbSize is set to 22 => WAVEFORMATEXTENSIBLE
            fileStream.read(&wavf->Samples, sizeof(uint16_t));

            // DWORD dwChannelMask; which channels are present in stream
            fileStream.read(&wavf->ChannelMask, sizeof(uint32_t));

            GUID GuidSubFormat = { 0 };
            fileStream.read(&GuidSubFormat, sizeof(GUID));

            wavf->PcmDataOffset += 22;

            // Check sub-format.
            if (!IsEqualGUID(GuidSubFormat, WavSubTypePCM)
                && !IsEqualGUID(GuidSubFormat, WavSubTypeIEEE_FLOAT))
            {
                fileStream.close();
                return false;
            }

            uint32_t chunk;
            // Find "data" chunk.
            while (fileStream.read(&chunk, sizeof(uint32_t)) == sizeof(uint32_t))
            {
                wavf->PcmDataOffset += 4;

                if (chunk == 0x61746164)
                {
                    // "data" chunk
                    h->Subchunk2ID = chunk;  // 0x61746164, "data"
                    fileStream.read(&h->Subchunk2Size, sizeof(uint32_t));
                    wavf->PcmDataOffset += 4;
                    break;
                }
                else
                {
                    // Read other non "data" chunks.
                    uint32_t chunkSize;
                    fileStream.read(&chunkSize, sizeof(uint32_t));

                    wavf->PcmDataOffset += 4;

                    fileStream.seek(chunkSize, SEEK_CUR); // skip only
                    wavf->PcmDataOffset += (int)chunkSize;
                }
            }
        }
        else
        {
            fileStream.close();
            return false;
        }
    }
    else
    {
        fileStream.close();
        return false;
    }

    wavf->BytesPerFrame = h->BitsPerSample / 8 * h->NumChannels;

    return true;
}

int wav_read(WAV_FILE* wavf, char* pcmBuf, size_t bytesToRead)
{
    return wavf->FileStream.read(pcmBuf, bytesToRead);
}

int wav_pcm_seek(WAV_FILE* wavf, int frameOffset)
{
    auto offset = frameOffset * wavf->BytesPerFrame + wavf->PcmDataOffset;
    return wavf->FileStream.seek(offset, SEEK_SET) >= 0 ? 0 : -1;
}

int wav_pcm_tell(WAV_FILE* wavf)
{
    auto offset = wavf->FileStream.seek(0, SEEK_CUR);
    return (offset - wavf->PcmDataOffset) / wavf->BytesPerFrame;
}

int wav_close(WAV_FILE* wavf)
{
    return  wavf->FileStream.close();
}

namespace cocos2d {

    AudioDecoderWav::AudioDecoderWav()
    {
        memset(&_wavf, 0, offsetof(WAV_FILE, FileStream) );
    }

    AudioDecoderWav::~AudioDecoderWav()
    {
        close();
    }

    bool AudioDecoderWav::open(const std::string& fullPath)
    {
        if (wav_open(fullPath, &_wavf))
        {
            _sampleRate = _wavf.FileHeader.SampleRate;
            _channelCount = _wavf.FileHeader.NumChannels;
            _bytesPerFrame = _wavf.BytesPerFrame;
            _totalFrames = _wavf.FileHeader.Subchunk2Size / _bytesPerFrame;

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
