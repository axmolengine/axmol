//////////////////////////////////////////////////////////////////////////
//
// MFMediaPlayer.h : Playback helper class.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// 
// refer to: 
//  a. https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/multimedia/mediafoundation/protectedplayback
//  b. https://docs.microsoft.com/en-us/windows/win32/medfound/seeking--fast-forward--and-reverse-play
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>

// C RunTime Header Files
#include <tchar.h>
#include <commdlg.h>  // OpenFile dialog
#include <assert.h>

// Media Foundation headers
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>
#include <mferror.h>
#include <nserror.h>  // More DRM errors.

// EVR headers
#include <evr.h>

// Safe string functions
#include <strsafe.h>
#include "MFUtils.h"

#include <functional>
#include <string>

using namespace MFUtils;

#define CMD_PENDING 0x01
#define CMD_PENDING_SEEK 0x02
#define CMD_PENDING_RATE 0x04

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

using SampleEventCallback = std::function<void(uint8_t* buffer, size_t size)>;
using SessionEventCallback = std::function<void(int ev)>;

class MFMediaPlayer : public IMFAsyncCallback
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
    SampleEventCallback SampleEvent;
    SessionEventCallback SessionEvent;

    static HRESULT CreateInstance(MFMediaPlayer** ppPlayer, HWND hwndEvent = NULL);

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

    // Playback
    HRESULT OpenURL(const WCHAR* sURL);
    HRESULT Close();
    HRESULT Shutdown();
    HRESULT HandleEvent(UINT_PTR pUnkPtr);
    MFPlayerState GetState() const { return m_state; }

    UINT32 GetVideoWidth() const { return m_uVideoWidth; }
    UINT32 GetVideoHeight() const { return m_uVideoHeight; }

    const GUID& GetVideoOutputFormat() const { return m_VideoOutputFormat; }

    // Video functionality
    void SetLooping(BOOL bLooping) { m_bLooping = bLooping; }

    void SetPlayOnOpen(BOOL bPlayOnOpen) { m_bPlayOnOpen = bPlayOnOpen; }

    BOOL CanSeek() const;
    MFTIME GetDuration() const;
    MFTIME GetCurrentPosition() const;

    // Set position in 100ns units, will reply if play ended
    // see: https://docs.microsoft.com/en-us/windows/win32/medfound/mf-pd-duration-attribute
    HRESULT SetPosition(MFTIME hnsPosition);

    BOOL CanScrub() const;
    HRESULT Scrub(BOOL bScrub);

    BOOL CanFastForward() const;
    BOOL CanRewind() const;
    HRESULT SetRate(float fRate);
    HRESULT FastForward();
    HRESULT Rewind();

    HRESULT Play();
    HRESULT Pause();
    HRESULT Stop();

    BOOL IsH264() const { return m_bIsH264; }

protected:
    HRESULT SetPositionInternal(const MFTIME& hnsPosition);
    HRESULT StartPlayback(const MFTIME* hnsPosition);
    HRESULT CommitRateChange(float fRate, BOOL bThin);
    float GetNominalRate() const;

    HRESULT UpdatePendingCommands(Command cmd);

protected:
    // Constructor is private. Use static CreateInstance method to instantiate.
    MFMediaPlayer(HWND hwndEvent);

    // Destructor is private. Caller should call Release.
    virtual ~MFMediaPlayer();

    HRESULT CreateOutputNode(IMFStreamDescriptor* pSourceSD, IMFTopologyNode** ppNode);

    HRESULT Initialize();
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
    SeekState m_nominal{CmdStop, 1.0, FALSE, 0};    // Current nominal state.
    SeekState m_request{CmdNone, 1.0, FALSE, 0};    // Pending request state.
    BOOL m_bPending = FALSE;      // Is a request pending?

    mutable CritSec m_critsec;  // Protects the seeking and rate-change states.

    HWND m_hwndEvent;      // App window to receive events.
    MFPlayerState m_state;  // Current state of the media session.
    HANDLE m_hCloseEvent;  // Event to wait on while closing

    UINT32 m_uVideoWidth  = 0;
    UINT32 m_uVideoHeight = 0;

    BOOL m_bLooping = FALSE;
    BOOL m_bPlayOnOpen = TRUE;

    BOOL m_bIsH264 = FALSE;
    GUID m_VideoOutputFormat{};
};
