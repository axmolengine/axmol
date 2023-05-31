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

#include <windows.h>
#include "./xaudio2.h"

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

struct XAudio2Playback;

class VoiceCallback : public IXAudio2VoiceCallback
{
public:
    XAudio2Playback* playback;
    VoiceCallback(XAudio2Playback* backend) : playback(backend) {}
    ~VoiceCallback() {}

    STDMETHOD_(void, OnStreamEnd()) {}
    STDMETHOD_(void, OnVoiceProcessingPassEnd(THIS)) {}
    STDMETHOD_(void, OnVoiceProcessingPassStart(THIS_ UINT32 SamplesRequired));
    STDMETHOD_(void, OnBufferEnd(THIS_ void* /*pBufferContext*/)) {}
    STDMETHOD_(void, OnBufferStart(THIS_ void* /*pBufferContext*/)) {}
    STDMETHOD_(void, OnLoopEnd(THIS_ void* /*pBufferContext*/)) {}
    STDMETHOD_(void, OnVoiceError(THIS_ void* /*pBufferContext*/, HRESULT /*Error*/)) {}
};

struct XAudio2Playback final : public BackendBase
{
    XAudio2Playback(DeviceBase* device) noexcept;
    ~XAudio2Playback() override;

    IXAudio2* mAudioEngine{nullptr};
    IXAudio2MasteringVoice* mMasteringVoice{nullptr};
    IXAudio2SourceVoice* mSourceVoice{nullptr};

    std::unique_ptr<VoiceCallback> mVoiceCallback;
    void mixerProc(int len) noexcept;

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

    DEF_NEWDEL(XAudio2Playback)
};

void VoiceCallback::OnVoiceProcessingPassStart(THIS_ UINT32 SamplesRequired)
{
    playback->mixerProc(SamplesRequired);
}

XAudio2Playback::XAudio2Playback(DeviceBase* device) noexcept : BackendBase{device}
{
    HRESULT hr = XAudio2Create(&mAudioEngine, 0u, 0u);
    if (FAILED(hr))
        WARN("Failed to create IXAudio2 instance: 0x%08lx\n", hr);
    /*
     * About AudioCategory
     * The default AudioCategory_GameEffects which doesn't support uap3:Capability: backgroundMediaPlayback
     */
    hr = mAudioEngine->CreateMasteringVoice(&mMasteringVoice, 0u, 0u, 0u, nullptr /* szDeviceId */, nullptr,
                                            AudioCategory_GameMedia);
    if (FAILED(hr))
        WARN("Failed to create IXAudio2MasteringVoice instance: 0x%08lx\n", hr);
    mVoiceCallback.reset(new VoiceCallback(this));
}

XAudio2Playback::~XAudio2Playback()
{
    mVoiceCallback.reset(nullptr);

    if (mMasteringVoice)
        mMasteringVoice->DestroyVoice();

    if (mSourceVoice)
        mSourceVoice->DestroyVoice();

    if (mBuffer != NULL)
        delete[] mBuffer;

    if (mAudioEngine)
        mAudioEngine->Release();
}

void XAudio2Playback::mixerProc(int len) noexcept
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

    mDevice->renderSamples(mBuffer, static_cast<uint>(samples), mDevice->channelsFromFmt());

    XAUDIO2_BUFFER bufferRegionToSubmit;
    memset(&bufferRegionToSubmit, 0, sizeof(bufferRegionToSubmit));
    bufferRegionToSubmit.AudioBytes = static_cast<UINT32>(bufferRegionSize);
    bufferRegionToSubmit.pAudioData = mBuffer;

    hr = mSourceVoice->SubmitSourceBuffer(&bufferRegionToSubmit);
    if (FAILED(hr))
    {
        ERR("SubmitSourceBuffer() failed: 0x%08lx\n", hr);
    }
}

void XAudio2Playback::open(const char* name)
{

    WAVEFORMATEX wf;

    wf.wFormatTag      = WAVE_FORMAT_PCM;
    wf.nChannels       = (mDevice->FmtChans == DevFmtMono) ? 1 : 2;
    wf.wBitsPerSample  = 16;
    wf.nSamplesPerSec  = static_cast<int>(mDevice->Frequency);
    wf.nAvgBytesPerSec = wf.nChannels * wf.nSamplesPerSec * wf.wBitsPerSample / 8;
    wf.nBlockAlign     = wf.wBitsPerSample * wf.nChannels / 8;
    wf.cbSize          = 0;

    mAudioEngine->CreateSourceVoice(&mSourceVoice, &wf, 0, XAUDIO2_DEFAULT_FREQ_RATIO, mVoiceCallback.get(), NULL,
                                    NULL);

    DevFmtType devtype = DevFmtShort;
    mFrameSize         = BytesFromDevFmt(devtype) * wf.nChannels;
    mFrequency         = static_cast<uint>(wf.nSamplesPerSec);
    mFmtChans          = (wf.nChannels == 1) ? DevFmtMono : DevFmtStereo;
    mFmtType           = devtype;
    mUpdateSize        = 8192;

    mDevice->DeviceName = name ? name : defaultDeviceName;
}

bool XAudio2Playback::reset()
{
    mDevice->Frequency  = mFrequency;
    mDevice->FmtChans   = mFmtChans;
    mDevice->FmtType    = mFmtType;
    mDevice->UpdateSize = mUpdateSize;
    mDevice->BufferSize = mUpdateSize * 2; /* SDL always (tries to) use two periods. */
    setDefaultWFXChannelOrder();
    return true;
}

void XAudio2Playback::start()
{
    if (mSourceVoice)
        mSourceVoice->Start();
}

void XAudio2Playback::stop()
{
    if (mSourceVoice)
        mSourceVoice->Stop();
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

    Platform::Agile<MediaCapture> mCapture{nullptr};
    IRandomAccessStream ^ mStream { nullptr };
    IInputStream ^ mInputStream { nullptr };
    XAudio2Delegates ^ mDelegates;
    bool mRecord{false};

    void open(const char* name) override;
    void start() override;
    void stop() override;
    void captureSamples(std::byte* buffer, uint samples) override;
    uint availableSamples() override;

    void failed();

    BYTE mBuffer[8192];
    uint mSkip{0};
    uint mBufferSize{0};
    uint mFrameSize{0};
    std::recursive_mutex mMutex;

    DEF_NEWDEL(XAudio2Capture)
};

void XAudio2Delegates::OnFailed(Windows::Media::Capture::MediaCapture ^ sender,
                                Windows::Media::Capture::MediaCaptureFailedEventArgs ^ errorEventArgs)
{
    capture->failed();
}

XAudio2Capture::XAudio2Capture(DeviceBase* device) noexcept : BackendBase{device}
{
    mDelegates = ref new XAudio2Delegates(this);
}

XAudio2Capture::~XAudio2Capture()
{
    stop();
    mDelegates = nullptr;
}

void XAudio2Capture::captureSamples(std::byte* buffer, uint samples)
{
    uint nsmp = availableSamples();
    if (samples > nsmp)
        samples = nsmp;
    if (!samples)
        return;
    uint tt = samples * mFrameSize;
    std::unique_lock<std::recursive_mutex> lck(mMutex);
    memcpy(buffer, mBuffer + mSkip, tt);
    memmove(mBuffer, mBuffer + mSkip + tt, mBufferSize - mSkip - tt);
    mBufferSize -= tt;
}

uint XAudio2Capture::availableSamples()
{
    if (!mRecord)
        return 0;
    mMutex.lock();
    uint cur  = ((mBufferSize > mSkip) ? (mBufferSize - mSkip) : 0u) / mFrameSize;
    uint room = static_cast<uint>(sizeof(mBuffer) - mBufferSize);
    mMutex.unlock();
    if (room > 1024u)
    {
        IBuffer ^ buf = ref new Buffer(room);
        create_task(mInputStream->ReadAsync(buf, room, InputStreamOptions::Partial)).then([=](IBuffer ^ ibuf) {
            uint blen   = ibuf->Length;
            auto reader = Windows::Storage::Streams::DataReader::FromBuffer(ibuf);
            std::unique_lock<std::recursive_mutex> lck(mMutex);
            reader->ReadBytes(::Platform::ArrayReference<BYTE>(mBuffer + mBufferSize, blen));
            mBufferSize += blen;
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
    mFrameSize          = BytesFromDevFmt(devtype);  // Mono ?
    mDevice->DeviceName = name ? name : defaultDeviceName;
}

void XAudio2Capture::start()
{
    if (!mRecord)
    {
        mStream = ref new InMemoryRandomAccessStream();
        if (mCapture != nullptr)
        {
            mCapture->StopRecordAsync();
        }
        try
        {
            MediaCaptureInitializationSettings ^ settings = ref new MediaCaptureInitializationSettings();
            settings->StreamingCaptureMode                = StreamingCaptureMode::Audio;
            settings->AudioDeviceId = Windows::Media::Devices::MediaDevice::GetDefaultAudioCaptureId(
                Windows::Media::Devices::AudioDeviceRole::Default);

            mCapture = ref new MediaCapture();
            create_task(mCapture->InitializeAsync(settings))
                .then([this](void) {
                    mCapture->Failed += ref new Windows::Media::Capture::MediaCaptureFailedEventHandler(
                        mDelegates, &XAudio2Delegates::OnFailed);
                    mBufferSize = 0;
                    mSkip       = 82;
                    return mCapture->StartRecordToStreamAsync(
                        MediaEncodingProfile::CreateWav(AudioEncodingQuality::High), mStream);
                })
                .then([this](void) {
                    mInputStream = mStream->GetInputStreamAt(0);
                    mRecord      = true;
                })
                .then([this](task<void> t) {
                    try
                    {
                        t.get();
                    }
                    catch (Platform::Exception ^ e)
                    {
                        mInputStream = nullptr;
                        mStream      = nullptr;
                        mCapture     = nullptr;
                        mRecord      = false;
                    }
                });
            while (mStream && !mRecord)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        catch (Platform::Exception ^ e)
        {
            mInputStream = nullptr;
            mStream      = nullptr;
            mCapture     = nullptr;
        }
    }
}

void XAudio2Capture::stop()
{
    if (mRecord)
    {
        if (mCapture != nullptr)
        {
            create_task(mCapture->StopRecordAsync()).then([=]() {
                mRecord      = false;
                mStream      = nullptr;
                mCapture     = nullptr;
                mInputStream = nullptr;
            });
            while (mStream)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        else
            mRecord = false;
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

std::string XAudio2BackendFactory::probe(BackendType /*type*/)
{
    std::string outnames;

    /* Includes null char. */
    outnames.append(defaultDeviceName, sizeof(defaultDeviceName));

    return outnames;
}

BackendPtr XAudio2BackendFactory::createBackend(DeviceBase* device, BackendType type)
{
    if (type == BackendType::Playback)
        return BackendPtr{new XAudio2Playback{device}};
    if (type == BackendType::Capture)
        return BackendPtr{new XAudio2Capture{device}};
    return nullptr;
}
