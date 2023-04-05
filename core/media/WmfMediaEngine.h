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

#    include "yasio/detail/byte_buffer.hpp"

NS_AX_BEGIN

using namespace MFUtils;

#    define CMD_PENDING 0x01
#    define CMD_PENDING_SEEK 0x02
#    define CMD_PENDING_RATE 0x04

enum class MFPlayerState
{
    Closed = 0,   // No session.
    Ready,        // Session was created, ready to open a file.
    OpenPending,  // Session is opening a file.
    Started,      // Session is playing a file.
    Paused,       // Session is paused.
    Stopped,      // Session is stopped (ready to play).
    Closing       // Application has closed the session, but is waiting for MESessionClosed.
};

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

    void SetMediaEventCallback(MEMediaEventCallback cb) override { m_eventCallback = cb; }

    // Playback
    bool Open(std::string_view sourceUri) override;
    bool Close() override;
    HRESULT Shutdown();
    HRESULT HandleEvent(IMFMediaEvent* pUnkPtr);
    MEMediaState GetState() const override { return m_state; }

    const GUID& GetVideoOutputFormat() const { return m_VideoOutputFormat; }

    // Video functionality
    bool SetLoop(bool bLooping) override
    {
        m_bLooping = bLooping;
        return true;
    }

    void SetAutoPlay(bool bAutoPlay) override { m_bAutoPlay = bAutoPlay; }

    BOOL CanSeek() const;
    MFTIME GetDuration() const;
    MFTIME GetCurrentPosition() const;

    bool SetCurrentTime(double sec) override
    {
        return SUCCEEDED(SetPosition(static_cast<MFTIME>((std::nano::den / 100) * sec)));
    }

    // Set position in 100ns units, will reply if play ended
    // see: https://docs.microsoft.com/en-us/windows/win32/medfound/mf-pd-duration-attribute
    HRESULT SetPosition(MFTIME hnsPosition);

    BOOL CanScrub() const;
    HRESULT Scrub(BOOL bScrub);

    BOOL CanFastForward() const;
    BOOL CanRewind() const;
    bool SetRate(double fRate) override;
    HRESULT FastForward();
    HRESULT Rewind();

    bool Play() override;
    bool Pause() override;
    bool Stop() override;

    void HandleVideoSample(const uint8_t* buf, size_t len);
    //bool GetLastVideoSample(MEVideoTextueSample& sample) const override;

    bool TransferVideoFrame(std::function<void(const MEVideoFrame&)> callback) override;

    void FireMediaEvent(MEMediaEventType event)
    {
        if (m_eventCallback)
            m_eventCallback(event);
    }

protected:
    HRESULT SetPositionInternal(const MFTIME& hnsPosition);
    HRESULT StartPlayback(const MFTIME* hnsPosition);
    HRESULT CommitRateChange(float fRate, BOOL bThin);
    float GetNominalRate() const;

    HRESULT UpdatePendingCommands(Command cmd);

    HRESULT GetNativeVideoSize(DWORD* cx, DWORD* cy);

     void ClearPendingFrames();

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

    MEMediaEventCallback m_eventCallback;

    MEVideoPixelFormat m_videoPF = MEVideoPixelFormat::INVALID;

    mutable std::deque<yasio::byte_buffer> m_framesQueue;
    mutable std::mutex m_framesQueueMtx;
};

struct WmfMediaEngineFactory : public MediaEngineFactory
{
    MediaEngine* CreateMediaEngine() override
    {
        auto engine = new WmfMediaEngine();
        auto hr     = engine->Initialize();
        if (SUCCEEDED(hr))
            return engine;

        engine->Release();
        return nullptr;
    }
    void DestroyMediaEngine(MediaEngine* me) override
    {
        auto wmfme = static_cast<WmfMediaEngine*>(me);
        wmfme->Shutdown();
        wmfme->Release();
    }
};

NS_AX_END

#endif
