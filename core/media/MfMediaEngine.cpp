//--------------------------------------------------------------------------------------
// File: MfMediaEngine.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#if defined(_WIN32)

#include "media/MfMediaEngine.h"

#include "ntcvt/ntcvt.hpp"

#include "MFUtils.h"

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
        attributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, DXGI_FORMAT::DXGI_FORMAT_B8G8R8X8_UNORM));

    // Create MediaEngine.
    ComPtr<IMFMediaEngineClassFactory> mfFactory;
    DX::ThrowIfFailed(
        CoCreateInstance(CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_ALL, IID_PPV_ARGS(mfFactory.GetAddressOf())));

    DX::ThrowIfFailed(mfFactory->CreateInstance(0, attributes.Get(), m_mediaEngine.ReleaseAndGetAddressOf()));

    CreateInstance(CLSID_WICImagingFactory, m_wicFactory);

    return m_mediaEngine != nullptr;
}

bool MfMediaEngine::SetLoop(bool bLoop)
{
    if (m_mediaEngine)
        return SUCCEEDED(m_mediaEngine->SetLoop(bLoop));
    return false;
}

bool MfMediaEngine::SetRate(double fRate)
{
    if (m_mediaEngine)
        return SUCCEEDED(m_mediaEngine->SetPlaybackRate(fRate));

    return false;
}

bool MfMediaEngine::Play()
{
    if (m_state == MediaState::Playing)
        return true;

    HRESULT hr = S_OK;
    if (m_mediaEngine)
    {
        if (m_state == MediaState::Stopped)
            m_mediaEngine->SetCurrentTime(0);
        hr = (m_mediaEngine->Play());
        m_state = MediaState::Playing;
    }
    return SUCCEEDED(hr);
}

bool MfMediaEngine::Pause()
{
    if (m_mediaEngine)
        return SUCCEEDED(m_mediaEngine->Pause());
    return false;
}

void MfMediaEngine::SetMuted(bool muted)
{
    if (m_mediaEngine)
    {
        DX::ThrowIfFailed(m_mediaEngine->SetMuted(muted));
    }
}

bool MfMediaEngine::Open(std::string_view sourceUri)
{
    auto bstrUrl = ntcvt::from_chars(sourceUri);

    
    m_readyToPlay = false;
    m_state       = MediaState::Preparing;
    if (m_mediaEngine)
        return SUCCEEDED(m_mediaEngine->SetSource(bstrUrl.data()));
    return false;
}


bool MfMediaEngine::Close()
{
    if (m_state == MediaState::Closed)
        return true;
    
    if (m_mediaEngine)
    {
        m_state = MediaState::Closed;
        return SUCCEEDED(m_mediaEngine->SetSource(NULL));
    }
    
    return false;
}

bool MfMediaEngine::Stop()
{
    if (m_mediaEngine)
    {
        HRESULT hr = S_OK;
        if (m_state == MediaState::Playing)
        {
            m_stopping = true;
            hr = m_mediaEngine->Pause();
        }
        else
        {
            if (m_state == MediaState::Paused)
            {
                m_state = MediaState::Stopped;
                if (m_eventCallback)
                    m_eventCallback(MediaEventType::STOPPED);
            }
        }
        return SUCCEEDED(hr);
    }
    return false;
}

bool MfMediaEngine::SetCurrentTime(double fPosInSeconds)
{
    if (m_mediaEngine)
        return SUCCEEDED(m_mediaEngine->SetCurrentTime(fPosInSeconds));
    return E_POINTER;
}

bool MfMediaEngine::GetLastVideoFrame(yasio::byte_buffer& frameData) const
{
    if (m_mediaEngine != nullptr && m_state == MediaState::Playing)
    {
        LONGLONG pts;
        if (m_mediaEngine->OnVideoStreamTick(&pts) == S_OK)
        {
            const MFVideoNormalizedRect rect{0, 0, 1.0, 1.0};
            const RECT rcTarget{0, 0, m_videoExtent.cx, m_videoExtent.cy};
            HRESULT hr = m_mediaEngine->TransferVideoFrame(m_wicBitmap.Get(), &rect, &rcTarget, &m_bkgColor);
            if (hr == S_OK)
            {
                ComPtr<IWICBitmapLock> lockedData;
                DWORD flags = WICBitmapLockRead;
                WICRect srcRect{0, 0, m_videoExtent.cx, m_videoExtent.cy};

                if (SUCCEEDED(m_wicBitmap->Lock(&srcRect, flags, lockedData.GetAddressOf())))
                {
                    UINT stride{0};

                    if (SUCCEEDED(lockedData->GetStride(&stride)))
                    {
                        UINT bufferSize{0};
                        BYTE* data{nullptr};

                        if (SUCCEEDED(lockedData->GetDataPointer(&bufferSize, &data)))
                        {
                            frameData.assign(data, data + bufferSize, std::true_type{});
                        }
                    }
                }

                return true;
            }
        }
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
            if (m_eventCallback)
                m_eventCallback(MediaEventType::PLAYING);
            Play();
        }
        break;

    case MF_MEDIA_ENGINE_EVENT_PLAY:
        m_state = MediaState::Playing;
        if (m_eventCallback)
            m_eventCallback(MediaEventType::PLAYING);
        break;

    case MF_MEDIA_ENGINE_EVENT_PAUSE:
        if (!m_stopping)
        {
            m_state = MediaState::Paused;
            if (m_eventCallback)
                m_eventCallback(MediaEventType::PAUSED);
        }
        else
        {
            m_state = MediaState::Stopped;
            if (m_eventCallback)
                m_eventCallback(MediaEventType::STOPPED);

            m_stopping = false;
        }
        break;

    case MF_MEDIA_ENGINE_EVENT_ENDED:
        m_state      = MediaState::Completed;
        if (m_eventCallback)
            m_eventCallback(MediaEventType::COMPLETED);
        break;

    case MF_MEDIA_ENGINE_EVENT_TIMEUPDATE:
        break;

    case MF_MEDIA_ENGINE_EVENT_ERROR:
        m_state = MediaState::Error;
#ifdef _DEBUG
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
#endif
        break;
    }
}

void MfMediaEngine::UpdateVideoExtent()
{
    if (m_mediaEngine && m_readyToPlay)
    {
        DWORD x, y;
        DX::ThrowIfFailed(m_mediaEngine->GetNativeVideoSize(&x, &y));

        int mods = 0;
        if (m_videoExtent.cx != x)
        {
            m_videoExtent.cx = x;
            ++mods;
        }
        if (m_videoExtent.cy != y)
        {
            m_videoExtent.cy = y;
            ++mods;
        }

        if (mods)
            DX::ThrowIfFailed(m_wicFactory->CreateBitmap(x, y, GUID_WICPixelFormat32bppBGRA, WICBitmapCacheOnDemand,
                                                         m_wicBitmap.ReleaseAndGetAddressOf()));
    }
}

NS_AX_END

#endif
