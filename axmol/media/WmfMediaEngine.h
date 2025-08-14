#pragma once
//////////////////////////////////////////////////////////////////////////
//
// WmfMediaEngine.h : Playback helper class.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// refer to:
//  a.
//  https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/multimedia/mediafoundation/protectedplayback
//  b. https://docs.microsoft.com/en-us/windows/win32/medfound/seeking--fast-forward--and-reverse-play
//
// Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
// https://axmol.dev/
//////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
#    include <winapifamily.h>
#endif

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)

#    include <Windows.h>

// C RunTime Header Files
#    include <tchar.h>
#    include <commdlg.h>  // OpenFile dialog
#    include <assert.h>

// Media Foundation headers
#    include <mfapi.h>
#    include <mfobjects.h>
#    include <mfidl.h>
#    include <mferror.h>
#    include <nserror.h>  // More DRM errors.

// EVR headers
#    include <evr.h>

// Safe string functions
#    include <strsafe.h>
#    include "MFUtils.h"

#    include <functional>
#    include <string>

#    include "MediaEngine.h"

#    include <atomic>
#    include <mutex>
#    include <deque>

#    include "yasio/byte_buffer.hpp"

namespace ax
{

using namespace MFUtils;

#    define CMD_PENDING 0x01
#    define CMD_PENDING_SEEK 0x02
#    define CMD_PENDING_RATE 0x04

class WmfMediaEngine : public IMFAsyncCallback, public MediaEngine
{
    enum Command
    {
        CmdNone = 0,
        CmdStop,
        CmdStart,
        CmdPause,
        CmdSeek,
    };

    class CritSec
    {
    private:
        CRITICAL_SECTION m_criticalSection;

    public:
        CritSec() { InitializeCriticalSection(&m_criticalSection); }
        ~CritSec() { DeleteCriticalSection(&m_criticalSection); }
        void Lock() { EnterCriticalSection(&m_criticalSection); }
        void Unlock() { LeaveCriticalSection(&m_criticalSection); }
    };

    class AutoLock
    {
    private:
        CritSec* m_pCriticalSection;

    public:
        AutoLock(CritSec& crit)
        {
            m_pCriticalSection = &crit;
            m_pCriticalSection->Lock();
        }
        ~AutoLock() { m_pCriticalSection->Unlock(); }
    };

public:
    // Constructor is private. Use static CreateInstance method to instantiate.
    WmfMediaEngine();

    HRESULT Initialize();

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IMFAsyncCallback methods
    STDMETHODIMP GetParameters(DWORD*, DWORD*)
    {
        // Implementation of this method is optional.
        return E_NOTIMPL;
    }

    STDMETHODIMP Invoke(IMFAsyncResult* pAsyncResult);

    void fireMediaEvent(MEMediaEventType event)
    {
        if (_onMediaEvent)
            _onMediaEvent(event);
    }

    void setCallbacks(std::function<void(MEMediaEventType)> onMediaEvent,
                     std::function<void(const MEVideoFrame&)> onVideoFrame) override
    {
        _onMediaEvent = std::move(onMediaEvent);
        _onVideoFrame = std::move(onVideoFrame);
    }

    // Playback
    bool open(std::string_view sourceUri) override;
    bool close() override;
    HRESULT Shutdown();
    HRESULT HandleEvent(IMFMediaEvent* pUnkPtr);
    MEMediaState getState() const override { return m_state; }

    const GUID& GetVideoOutputFormat() const { return m_VideoOutputFormat; }

    // Video functionality
    bool setLoop(bool bLooping) override
    {
        m_bLooping = bLooping;
        return true;
    }

    void setAutoPlay(bool bAutoPlay) override { m_bAutoPlay = bAutoPlay; }

    BOOL CanSeek() const;
    MFTIME GetDuration() const;
    MFTIME GetCurrentPosition() const;

    bool setCurrentTime(double sec) override
    {
        return SUCCEEDED(SetPosition(static_cast<MFTIME>((std::nano::den / 100) * sec)));
    }

    double getCurrentTime() override;
    double getDuration() override;

    // Set position in 100ns units, will reply if play ended
    // see: https://docs.microsoft.com/en-us/windows/win32/medfound/mf-pd-duration-attribute
    HRESULT SetPosition(MFTIME hnsPosition);

    BOOL CanScrub() const;
    HRESULT Scrub(BOOL bScrub);

    BOOL CanFastForward() const;
    BOOL CanRewind() const;
    bool setRate(double fRate) override;
    HRESULT FastForward();
    HRESULT Rewind();

    bool play() override;
    bool pause() override;
    bool stop() override;

    bool isPlaybackEnded() const override { return m_bPlaybackEnded; }

    void HandleVideoSample(const uint8_t* buf, size_t len);
    //bool GetLastVideoSample(MEVideoTextueSample& sample) const override;

    bool transferVideoFrame() override;

protected:
    HRESULT SetPositionInternal(const MFTIME& hnsPosition);
    HRESULT StartPlayback(const MFTIME* hnsPosition);
    HRESULT CommitRateChange(float fRate, BOOL bThin);
    float GetNominalRate() const;

    HRESULT UpdatePendingCommands(Command cmd);

    HRESULT GetNativeVideoSize(DWORD* cx, DWORD* cy);

     void ClearPendingBuffers();

protected:
    // Destructor is private. Caller should call Release.
    virtual ~WmfMediaEngine();

    HRESULT CreateOutputNode(IMFStreamDescriptor* pSourceSD, IMFTopologyNode** ppNode);

    HRESULT CreateSession();
    HRESULT CloseSession();

    HRESULT CreateMediaSource(const WCHAR* sURL);
    HRESULT CreateTopologyFromSource(IMFTopology** ppTopology);

    HRESULT AddBranchToPartialTopology(IMFTopology* pTopology, IMFPresentationDescriptor* pSourcePD, DWORD iStream);

    // Media event handlers
    HRESULT OnTopologyReady(IMFMediaEvent* pEvent);
    HRESULT OnPlayEnded(IMFMediaEvent* pEvent);

    HRESULT OnSessionStart(HRESULT hr);
    HRESULT OnSessionStop(HRESULT hr);
    HRESULT OnSessionPause(HRESULT hr);
    HRESULT OnSessionEnded(HRESULT hr);

    long m_nRefCount;  // Reference count.

    TComPtr<IMFMediaSession> m_pSession;
    TComPtr<IMFMediaSource> m_pSource;
    TComPtr<IMFPresentationDescriptor> m_PresentDescriptor;

    TComPtr<IMFRateControl> m_RateControl;
    TComPtr<IMFRateSupport> m_RateSupport;
    TComPtr<IMFPresentationClock> m_pClock;

    TComPtr<IMFMediaType> m_videoInputType;

    DWORD m_caps         = 0;      // Session caps.
    MFTIME m_hnsDuration = 0;      // Duration of the current presentation.
    BOOL m_bCanScrub     = FALSE;  // Does the current session support rate = 0.
    float m_fPrevRate    = 1.0f;

    // Describes the current or requested state, with respect to seeking and
    // playback rate.
    struct SeekState
    {
        Command command;
        float fRate;      // Playback rate
        BOOL bThin;       // Thinned playback?
        MFTIME hnsStart;  // Start position
    };
    SeekState m_nominal{CmdStop, 1.0, FALSE, 0};  // Current nominal state.
    SeekState m_request{CmdNone, 1.0, FALSE, 0};  // Pending request state.
    BOOL m_bPending = FALSE;                      // Is a request pending?

    std::atomic<bool> m_bOpenPending = false;
    std::atomic<bool> m_bClosePending = false;
    bool m_bPlaybackEnded = false;

    mutable CritSec m_critsec;  // Protects the seeking and rate-change states.

    std::atomic<MEMediaState> m_state = MEMediaState::Closed;  // Current state of the media session.

    HANDLE m_hOpenEvent  = nullptr;  // App window to receive events.
    HANDLE m_hCloseEvent = nullptr;  // Event to wait on while closing

    MEIntPoint m_videoExtent;
    MEIntPoint m_frameExtent;  // may same with m_videoExtent

    BOOL m_bLooping  = FALSE;
    BOOL m_bAutoPlay = TRUE;

    BOOL m_bIsH264 = FALSE;
    BOOL m_bIsHEVC = FALSE;  // hvc1,hev1
    GUID m_VideoOutputFormat{};
    MFVideoRotationFormat m_VideoRotation{MFVideoRotationFormat_0};

    std::function<void(MEMediaEventType)> _onMediaEvent;
    std::function<void(const MEVideoFrame&)> _onVideoFrame;

    MEVideoPixelFormat m_videoPF = MEVideoPixelFormat::INVALID;

    mutable std::mutex m_frameBuffer1Mtx;
    yasio::byte_buffer m_frameBuffer1; // for write
    yasio::byte_buffer m_frameBuffer2; // for read
};

struct WmfMediaEngineFactory : public MediaEngineFactory
{
    MediaEngine* createMediaEngine() override
    {
        auto engine = new WmfMediaEngine();
        auto hr     = engine->Initialize();
        if (SUCCEEDED(hr))
            return engine;

        engine->Release();
        return nullptr;
    }
    void destroyMediaEngine(MediaEngine* me) override
    {
        auto wmfme = static_cast<WmfMediaEngine*>(me);
        wmfme->Shutdown();
        wmfme->Release();
    }
};

}

#endif
