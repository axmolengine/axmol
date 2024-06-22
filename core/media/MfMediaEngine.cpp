//--------------------------------------------------------------------------------------
// File: MfMediaEngine.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
// Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
// 
// https://axmol.dev/
//-------------------------------------------------------------------------------------

#include "media/MfMediaEngine.h"

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP || defined(AXME_USE_IMFME))

#    include "ntcvt/ntcvt.hpp"

#    include "MFUtils.h"

#    include "yasio/string_view.hpp"

NS_AX_BEGIN

using Microsoft::WRL::ComPtr;

namespace
{
class MediaEngineNotify : public IMFMediaEngineNotify
{
    long m_cRef;
    IMFNotify* m_pCB;

public:
    MediaEngineNotify() noexcept : m_cRef(1), m_pCB(nullptr) {}

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        if (__uuidof(IMFMediaEngineNotify) == riid)
        {
            *ppv = static_cast<IMFMediaEngineNotify*>(this);
        }
        else
        {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();

        return S_OK;
    }

    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_cRef); }

    STDMETHODIMP_(ULONG) Release()
    {
        LONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0)
        {
            delete this;
        }
        return cRef;
    }

    void SetCallback(IMFNotify* pCB) { m_pCB = pCB; }

    // EventNotify is called when the Media Engine sends an event.
    STDMETHODIMP EventNotify(DWORD meEvent, DWORD_PTR param1, DWORD)
    {
        if (meEvent == MF_MEDIA_ENGINE_EVENT_NOTIFYSTABLESTATE)
        {
            SetEvent(reinterpret_cast<HANDLE>(param1));
        }
        else
        {
            m_pCB->OnMediaEngineEvent(meEvent);
        }

        return S_OK;
    }
};
}  // namespace

MfMediaEngine::MfMediaEngine() noexcept : m_bkgColor{} {}

MfMediaEngine::~MfMediaEngine()
{
    if (m_mediaEngine)
        m_mediaEngine->Shutdown();
}

bool MfMediaEngine::Initialize()
{
    // Initialize Media Foundation (see Main.cpp for code to handle Windows N Editions)
    DX::ThrowIfFailed(MFUtils::InitializeMFOnce());

    // Setup Media Engine

    // Create our event callback object.
    ComPtr<MediaEngineNotify> spNotify = new MediaEngineNotify();
    if (spNotify == nullptr)
    {
        DX::ThrowIfFailed(E_OUTOFMEMORY);
    }

    spNotify->SetCallback(this);

    // Set configuration attribiutes.
    ComPtr<IMFAttributes> attributes;
    DX::ThrowIfFailed(MFCreateAttributes(attributes.GetAddressOf(), 1));
    // DX::ThrowIfFailed(attributes->SetUnknown(MF_MEDIA_ENGINE_DXGI_MANAGER,
    // reinterpret_cast<IUnknown*>(dxgiManager.Get())));
    DX::ThrowIfFailed(attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, reinterpret_cast<IUnknown*>(spNotify.Get())));
    DX::ThrowIfFailed(
        attributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM));

    DX::ThrowIfFailed(attributes->SetUINT32(MF_MEDIA_ENGINE_STREAM_CONTAINS_ALPHA_CHANNEL, 1));

    // Create MediaEngine.
    ComPtr<IMFMediaEngineClassFactory> mfFactory;
    DX::ThrowIfFailed(
        CoCreateInstance(CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_ALL, IID_PPV_ARGS(mfFactory.GetAddressOf())));

    DX::ThrowIfFailed(mfFactory->CreateInstance(0, attributes.Get(), m_mediaEngine.ReleaseAndGetAddressOf()));

    MFUtils::CreateInstance(CLSID_WICImagingFactory, m_wicFactory);

    return m_mediaEngine != nullptr;
}

bool MfMediaEngine::setLoop(bool bLoop)
{
    if (m_mediaEngine)
        return SUCCEEDED(m_mediaEngine->SetLoop(bLoop));
    return false;
}

bool MfMediaEngine::setRate(double fRate)
{
    if (m_mediaEngine)
        return SUCCEEDED(m_mediaEngine->SetPlaybackRate(fRate));

    return false;
}

bool MfMediaEngine::play()
{
    if (m_state == MEMediaState::Playing)
        return true;

    m_bPlaybackEnded = false;

    HRESULT hr = S_OK;
    if (m_mediaEngine)
    {
        if (m_state == MEMediaState::Stopped)
            m_mediaEngine->SetCurrentTime(0);
        hr      = (m_mediaEngine->Play());
        m_state = MEMediaState::Playing;
    }
    return SUCCEEDED(hr);
}

bool MfMediaEngine::pause()
{
    if (m_mediaEngine)
    {
        return SUCCEEDED(m_mediaEngine->Pause());
    }
    return false;
}

void MfMediaEngine::SetMuted(bool muted)
{
    if (m_mediaEngine)
    {
        DX::ThrowIfFailed(m_mediaEngine->SetMuted(muted));
    }
}

bool MfMediaEngine::open(std::string_view sourceUri)
{
    auto bstrUrl = ntcvt::from_chars(sourceUri);

    m_readyToPlay = false;
    m_state       = MEMediaState::Preparing;
    if (m_mediaEngine)
        return SUCCEEDED(m_mediaEngine->SetSource(bstrUrl.data()));
    return false;
}

bool MfMediaEngine::close()
{
    if (m_state == MEMediaState::Closed)
        return true;

    if (m_mediaEngine)
    {
        m_state = MEMediaState::Closed;
        return SUCCEEDED(m_mediaEngine->SetSource(NULL));
    }

    return false;
}

bool MfMediaEngine::stop()
{
    if (m_mediaEngine)
    {
        HRESULT hr = S_OK;
        if (m_state == MEMediaState::Playing)
        {
            m_stopping = true;
            hr         = m_mediaEngine->Pause();
        }
        else
        {
            if (m_state == MEMediaState::Paused)
            {
                m_state = MEMediaState::Stopped;
                fireMediaEvent(MEMediaEventType::Stopped);
            }
        }
        return SUCCEEDED(hr);
    }
    return false;
}

bool MfMediaEngine::setCurrentTime(double fPosInSeconds)
{
    if (m_mediaEngine)
        return SUCCEEDED(m_mediaEngine->SetCurrentTime(fPosInSeconds));
    return false;
}

double MfMediaEngine::getCurrentTime()
{
    if (m_mediaEngine)
        return m_mediaEngine->GetCurrentTime();

    return 0.0;
}

double MfMediaEngine::getDuration()
{
    if (m_mediaEngine)
        return m_mediaEngine->GetDuration();

    return 0.0;
}


bool MfMediaEngine::transferVideoFrame()
{
    if (m_mediaEngine != nullptr && m_state == MEMediaState::Playing)
    {
        do
        {
            LONGLONG pts;
            AX_BREAK_IF(FAILED(m_mediaEngine->OnVideoStreamTick(&pts)));

            const MFVideoNormalizedRect rect{0, 0, 1.0, 1.0};
            const RECT rcTarget{0, 0, m_videoExtent.x, m_videoExtent.y};
            HRESULT hr = m_mediaEngine->TransferVideoFrame(m_wicBitmap.Get(), &rect, &rcTarget, &m_bkgColor);
            AX_BREAK_IF(FAILED(hr));

            ComPtr<IWICBitmapLock> lockedData;
            DWORD flags = WICBitmapLockRead;
            WICRect srcRect{0, 0, m_videoExtent.x, m_videoExtent.y};

            AX_BREAK_IF(FAILED(m_wicBitmap->Lock(&srcRect, flags, lockedData.GetAddressOf())));

            UINT stride{0};
            AX_BREAK_IF(FAILED(lockedData->GetStride(&stride)));

            UINT bufferSize{0};
            BYTE* data{nullptr};
            AX_BREAK_IF(FAILED(lockedData->GetDataPointer(&bufferSize, &data)));

            _onVideoFrame(MEVideoFrame{data, nullptr, bufferSize, MEVideoPixelDesc{MEVideoPixelFormat::BGR32, m_videoExtent},
                                  m_videoExtent});
            return true;
        } while (false);
    }
    return false;
}

void MfMediaEngine::OnMediaEngineEvent(uint32_t meEvent)
{
    switch (meEvent)
    {
    case MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA:
        m_readyToPlay = true;
        UpdateVideoExtent();
        break;

    case MF_MEDIA_ENGINE_EVENT_CANPLAY:

        // Here we auto-play when ready...
        if (m_readyToPlay)
        {
            fireMediaEvent(MEMediaEventType::Playing);
            play();
        }
        break;

    case MF_MEDIA_ENGINE_EVENT_PLAY:
        m_state = MEMediaState::Playing;
        fireMediaEvent(MEMediaEventType::Playing);
        break;

    case MF_MEDIA_ENGINE_EVENT_PAUSE:
        if (!m_stopping)
        {
            m_state = MEMediaState::Paused;
            fireMediaEvent(MEMediaEventType::Paused);
        }
        else
        {
            m_state = MEMediaState::Stopped;
            fireMediaEvent(MEMediaEventType::Stopped);

            m_stopping = false;
        }
        break;

    case MF_MEDIA_ENGINE_EVENT_ENDED:
        m_bPlaybackEnded = true;
        fireMediaEvent(MEMediaEventType::Stopped);
        break;

    case MF_MEDIA_ENGINE_EVENT_TIMEUPDATE:
        break;

    case MF_MEDIA_ENGINE_EVENT_ERROR:
        m_state = MEMediaState::Error;
        fireMediaEvent(MEMediaEventType::Error);
#    ifdef _DEBUG
        if (m_mediaEngine)
        {
            ComPtr<IMFMediaError> error;
            if (SUCCEEDED(m_mediaEngine->GetError(&error)))
            {
                USHORT errorCode = error->GetErrorCode();
                HRESULT hr       = error->GetExtendedErrorCode();
                char buff[128]   = {};
                sprintf_s(buff, "ERROR: Media Foundation Event Error %u (%08X)\n", errorCode,
                          static_cast<unsigned int>(hr));
                OutputDebugStringA(buff);
            }
            else
            {
                OutputDebugStringA("ERROR: Media Foundation Event Error *FAILED GetError*\n");
            }
        }
#    endif
        break;
    }
}

void MfMediaEngine::UpdateVideoExtent()
{
    if (m_mediaEngine && m_readyToPlay)
    {
        DWORD x = 0, y = 0;
        DX::ThrowIfFailed(m_mediaEngine->GetNativeVideoSize(&x, &y));

        int mods = 0;
        if (m_videoExtent.x != x)
        {
            m_videoExtent.x = x;
            ++mods;
        }
        if (m_videoExtent.x != y)
        {
            m_videoExtent.y = y;
            ++mods;
        }

        if (mods)
            DX::ThrowIfFailed(m_wicFactory->CreateBitmap(x, y, GUID_WICPixelFormat32bppBGRA, WICBitmapCacheOnDemand,
                                                         m_wicBitmap.ReleaseAndGetAddressOf()));
    }
}

NS_AX_END

#endif
