/**
 * OpenAL cross platform audio library
 * Copyright (C) 2018 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include "config.h"

#ifdef _WIN32
#    define WIN32_LEAN_AND_MEAN 1
#    include <Windows.h>
#else
#    include <unistd.h>
#endif

#include "alc/backends/xaudio2.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>

#include "almalloc.h"
#include "alnumeric.h"
#include "core/device.h"
#include "core/logging.h"

#include <xaudio2.h>

#ifdef _WIN32
#    define DEVNAME_PREFIX "OpenAL Soft on "
#else
#    define DEVNAME_PREFIX ""
#endif

constexpr char defaultDeviceName[] = DEVNAME_PREFIX "Default Device";

class VoiceCallback;

struct XAudio2Backend final : public BackendBase
{
    XAudio2Backend(DeviceBase* device) noexcept;
    ~XAudio2Backend() override;

    IXAudio2* audioengine;
    IXAudio2MasteringVoice* masteringvoice;
    IXAudio2SourceVoice* source;

    VoiceCallback* callback;
    void mix(int len) noexcept;

    void open(const char* name) override;
    bool reset() override;
    void start() override;
    void stop() override;

    uint mDeviceID{0};
    uint mFrameSize{0};

    uint mFrequency{0u};
    DevFmtChannels mFmtChans{};
    DevFmtType mFmtType{};
    uint mUpdateSize{0u};

    BYTE* mBuffer{nullptr};
    size_t mBufferSize{0};

    DEF_NEWDEL(XAudio2Backend)
};

class VoiceCallback : public IXAudio2VoiceCallback
{
public:
    XAudio2Backend* master;
    VoiceCallback(XAudio2Backend* backend) : master(backend) {}
    ~VoiceCallback() {}

    STDMETHOD_(void, OnStreamEnd()) { ; }
    STDMETHOD_(void, OnVoiceProcessingPassEnd(THIS)) {}
    STDMETHOD_(void, OnVoiceProcessingPassStart(THIS_ UINT32 SamplesRequired)) { master->mix(SamplesRequired); }
    STDMETHOD_(void, OnBufferEnd(THIS_ void* pBufferContext)) {}
    STDMETHOD_(void, OnBufferStart(THIS_ void* pBufferContext)) {}
    STDMETHOD_(void, OnLoopEnd(THIS_ void* pBufferContext)) {}
    STDMETHOD_(void, OnVoiceError(THIS_ void* pBufferContext, HRESULT Error)) {}
};

XAudio2Backend::XAudio2Backend(DeviceBase* device) noexcept : BackendBase{device}
{
    XAudio2Create(&audioengine);
    HRESULT hr = audioengine->CreateMasteringVoice(&masteringvoice);
    source     = nullptr;
    callback   = new VoiceCallback(this);
}

XAudio2Backend::~XAudio2Backend()
{
    delete callback;

    if (masteringvoice)
        masteringvoice->DestroyVoice();

    if (source)
        source->DestroyVoice();

    if (mBuffer != NULL)
        delete[] mBuffer;

    audioengine->Release();
}

void XAudio2Backend::mix(int len) noexcept
{
    const auto ulen = static_cast<unsigned int>(len);
    assert((ulen % mFrameSize) == 0);

    HRESULT hr;
    size_t samples          = ulen / mFrameSize;
    size_t bufferRegionSize = samples * mFrameSize;
    if (mBufferSize < ulen)
    {
        if (mBuffer != NULL)
            delete[] mBuffer;
        mBufferSize = bufferRegionSize;
        mBuffer     = new (std::nothrow) BYTE[mBufferSize];
    }

    mDevice->renderSamples(mBuffer, samples, mDevice->channelsFromFmt());

    XAUDIO2_BUFFER bufferRegionToSubmit;
    memset(&bufferRegionToSubmit, 0, sizeof(bufferRegionToSubmit));
    bufferRegionToSubmit.AudioBytes = bufferRegionSize;
    bufferRegionToSubmit.pAudioData = mBuffer;

    hr = source->SubmitSourceBuffer(&bufferRegionToSubmit);
    if (FAILED(hr))
    {
        ERR("SubmitSourceBuffer() failed: 0x%08lx\n", hr);
    }
}

void XAudio2Backend::open(const char* name)
{

    WAVEFORMATEX wf;

    wf.wFormatTag      = WAVE_FORMAT_PCM;
    wf.nChannels       = (mDevice->FmtChans == DevFmtMono) ? 1 : 2;
    wf.wBitsPerSample  = 16;
    wf.nSamplesPerSec  = static_cast<int>(mDevice->Frequency);
    wf.nAvgBytesPerSec = wf.nChannels * wf.nSamplesPerSec * wf.wBitsPerSample / 8;
    wf.nBlockAlign     = wf.wBitsPerSample * wf.nChannels / 8;
    wf.cbSize          = 0;

    audioengine->CreateSourceVoice(&source, &wf, 0, XAUDIO2_DEFAULT_FREQ_RATIO, callback, NULL, NULL);

    DevFmtType devtype = DevFmtShort;
    mFrameSize         = BytesFromDevFmt(devtype) * wf.nChannels;
    mFrequency         = static_cast<uint>(wf.nSamplesPerSec);
    mFmtChans          = (wf.nChannels == 1) ? DevFmtMono : DevFmtStereo;
    mFmtType           = devtype;
    mUpdateSize        = 8192;

    mDevice->DeviceName = name ? name : defaultDeviceName;
}

bool XAudio2Backend::reset()
{
    mDevice->Frequency  = mFrequency;
    mDevice->FmtChans   = mFmtChans;
    mDevice->FmtType    = mFmtType;
    mDevice->UpdateSize = mUpdateSize;
    mDevice->BufferSize = mUpdateSize * 2; /* SDL always (tries to) use two periods. */
    setDefaultWFXChannelOrder();
    return true;
}

void XAudio2Backend::start()
{
    if (source)
        source->Start();
}

void XAudio2Backend::stop()
{
    if (source)
        source->Stop();
}

#include <chrono>
#include <ppltasks.h>
#include <mutex>
#include <Windows.Media.Capture.h>
#include <Windows.Media.MediaProperties.h>
#include <Windows.Storage.Streams.h>
using namespace Concurrency;

using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;
using namespace Windows::Storage::Streams;

struct XAudio2Capture;

ref class XAudio2Delegates sealed
{
    XAudio2Capture* capture;
    internal : XAudio2Delegates(XAudio2Capture* cap) { capture = cap; };

public:
    void OnFailed(Windows::Media::Capture::MediaCapture ^ sender,
                  Windows::Media::Capture::MediaCaptureFailedEventArgs ^ errorEventArgs);
};

// This is called XAudio2, because it is supposed to be UWP driver, but XAudio2 doesn't handle capture at all. We'll use
// MediaCapture instead
struct XAudio2Capture final : public BackendBase
{
    XAudio2Capture(DeviceBase* device) noexcept;
    ~XAudio2Capture() override;

    MediaCapture ^ capture { nullptr };
    IRandomAccessStream ^ buffer { nullptr };
    IInputStream ^ instream { nullptr };
    XAudio2Delegates ^ delegates;
    bool record{false};

    void open(const char* name) override;
    void start() override;
    void stop() override;
    void captureSamples(al::byte* buffer, uint samples) override;
    uint availableSamples() override;

    void failed();

    BYTE mBuffer[8192];
    size_t mSkip{0};
    size_t mBufferSize{0};
    size_t mFrameSize{0};
    std::recursive_mutex mutex;

    DEF_NEWDEL(XAudio2Capture)
};

void XAudio2Delegates::OnFailed(Windows::Media::Capture::MediaCapture ^ sender,
                                Windows::Media::Capture::MediaCaptureFailedEventArgs ^ errorEventArgs)
{
    capture->failed();
}

XAudio2Capture::XAudio2Capture(DeviceBase* device) noexcept : BackendBase{device}
{
    delegates = ref new XAudio2Delegates(this);
}

XAudio2Capture::~XAudio2Capture()
{
    stop();
    delegates = nullptr;
}

void XAudio2Capture::captureSamples(al::byte* buffer, uint samples)
{
    int nsmp = availableSamples();
    if (samples > nsmp)
        samples = nsmp;
    if (!samples)
        return;
    size_t tt = samples * mFrameSize;
    mutex.lock();
    memcpy(buffer, mBuffer + mSkip, tt);
    memmove(mBuffer, mBuffer + mSkip + tt, mBufferSize - mSkip - tt);
    mBufferSize -= tt;
    mutex.unlock();
}

uint XAudio2Capture::availableSamples()
{
    if (!record)
        return 0;
    mutex.lock();
    size_t cur  = std::max(mBufferSize - mSkip, (size_t)0) / mFrameSize;
    size_t room = sizeof(mBuffer) - mBufferSize;
    mutex.unlock();
    if (room > 1024)
    {
        IBuffer ^ buf = ref new Buffer(room);
        create_task(instream->ReadAsync(buf, room, InputStreamOptions::Partial)).then([=](IBuffer ^ ibuf) {
            size_t blen = ibuf->Length;
            auto reader = Windows::Storage::Streams::DataReader::FromBuffer(ibuf);
            mutex.lock();
            reader->ReadBytes(::Platform::ArrayReference<BYTE>(mBuffer + mBufferSize, blen));
            mBufferSize += blen;
            mutex.unlock();
        });
    }
    return cur;
}

void XAudio2Capture::failed()
{
    stop();
}

void XAudio2Capture::open(const char* name)
{
    DevFmtType devtype  = DevFmtShort;
    mFrameSize          = BytesFromDevFmt(devtype) * 2;
    mDevice->DeviceName = name ? name : defaultDeviceName;
}

void XAudio2Capture::start()
{
    if (!record)
    {
        buffer = ref new InMemoryRandomAccessStream();
        if (capture != nullptr)
        {
            capture->StopRecordAsync();
        }
        try
        {
            MediaCaptureInitializationSettings ^ settings = ref new MediaCaptureInitializationSettings();
            settings->StreamingCaptureMode                = StreamingCaptureMode::Audio;
            settings->AudioDeviceId = Windows::Media::Devices::MediaDevice::GetDefaultAudioCaptureId(
                Windows::Media::Devices::AudioDeviceRole::Default);

            capture = ref new MediaCapture();
            create_task(capture->InitializeAsync(settings))
                .then([this](void) {
                    capture->Failed += ref new Windows::Media::Capture::MediaCaptureFailedEventHandler(
                        delegates, &XAudio2Delegates::OnFailed);
                    mBufferSize = 0;
                    mSkip       = 82;
                    return capture->StartRecordToStreamAsync(
                        MediaEncodingProfile::CreateWav(AudioEncodingQuality::High), buffer);
                })
                .then([this](void) {
                    instream = buffer->GetInputStreamAt(0);
                    record   = true;
                })
                .then([this](task<void> t) {
                    try
                    {
                        t.get();
                    }
                    catch (Platform::Exception ^ e)
                    {
                        instream = nullptr;
                        buffer   = nullptr;
                        capture  = nullptr;
                        record   = false;
                    }
                });
            while (buffer && (!record))
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        catch (Platform::Exception ^ e)
        {
            instream = nullptr;
            buffer   = nullptr;
            capture  = nullptr;
        }
    }
}

void XAudio2Capture::stop()
{
    if (record)
    {
        if (capture)
        {
            create_task(capture->StopRecordAsync()).then([=]() {
                record   = false;
                buffer   = nullptr;
                capture  = nullptr;
                instream = nullptr;
            });
            while (buffer)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        else
            record = false;
    }
}

BackendFactory& XAudio2BackendFactory::getFactory()
{
    static XAudio2BackendFactory factory{};
    return factory;
}

bool XAudio2BackendFactory::init()
{
    return true;
}

bool XAudio2BackendFactory::querySupport(BackendType type)
{
    return type == BackendType::Playback || type == BackendType::Capture;
}

std::string XAudio2BackendFactory::probe(BackendType type)
{
    std::string outnames;

    /* Includes null char. */
    outnames.append(defaultDeviceName, sizeof(defaultDeviceName));

    return outnames;
}

BackendPtr XAudio2BackendFactory::createBackend(DeviceBase* device, BackendType type)
{
    if (type == BackendType::Playback)
        return BackendPtr{new XAudio2Backend{device}};

    if (type == BackendType::Capture)
        return BackendPtr{new XAudio2Capture{device}};

    return nullptr;
}
