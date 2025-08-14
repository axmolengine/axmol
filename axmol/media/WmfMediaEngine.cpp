//////////////////////////////////////////////////////////////////////////
//
// WmfMediaEngine.cpp : Playback helper class.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
//
// https://axmol.dev/
//////////////////////////////////////////////////////////////////////////

#include "WmfMediaEngine.h"

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)

#    include <Shlwapi.h>
#    include "MFUtils.h"

#    include "ntcvt/ntcvt.hpp"
#    include "yasio/sz.hpp"

namespace ax
{

// IF_FAILED_GOTO macro.
// Jumps to 'label' on failure.
#    ifndef IF_FAILED_GOTO
#        define IF_FAILED_GOTO(hr, label) \
            if (FAILED(hr))               \
            {                             \
                goto label;               \
            }
#    endif

#    define CHECK_HR(hr) IF_FAILED_GOTO(hr, done)

// const UINT WM_APP_PLAYER_EVENT = ::RegisterWindowMessageW(L"mfmedia-event");

// static MFOffset MakeOffset(float v)
//{
//     // v = offset.value + (offset.fract / denominator), where denominator = 65536.0f.
//     const int denominator = std::numeric_limits<WORD>::max() + 1;
//     MFOffset offset;
//     offset.value = short(v);
//     offset.fract = WORD(denominator * (v - offset.value));
//     return offset;
// }

//-------------------------------------------------------------------
//  Name: CreateSourceStreamNode
//  Description:  Creates a source-stream node for a stream.
//
//  pSource: Pointer to the media source that contains the stream.
//  pSourcePD: Presentation descriptor for the media source.
//  pSourceSD: Stream descriptor for the stream.
//  ppNode: Receives a pointer to the new node.
//-------------------------------------------------------------------
static HRESULT CreateSourceStreamNode(IMFMediaSource* pSource,
                                      IMFPresentationDescriptor* pSourcePD,
                                      IMFStreamDescriptor* pSourceSD,
                                      IMFTopologyNode** ppNode)
{
    if (!pSource || !pSourcePD || !pSourceSD || !ppNode)
    {
        return E_POINTER;
    }

    TComPtr<IMFTopologyNode> SourceNode;
    HRESULT hr = S_OK;

    // Create the source-stream node.
    CHECK_HR(hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &SourceNode));

    // Set attribute: Pointer to the media source.
    CHECK_HR(hr = SourceNode->SetUnknown(MF_TOPONODE_SOURCE, pSource));

    // Set attribute: Pointer to the presentation descriptor.
    CHECK_HR(hr = SourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pSourcePD));

    // Set attribute: Pointer to the stream descriptor.
    CHECK_HR(hr = SourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSourceSD));

    // Return the IMFTopologyNode pointer to the caller.
    *ppNode = SourceNode.Get();
    (*ppNode)->AddRef();

done:
    return hr;
}

/**
 * Implements a callback object for the MF video sample grabber sink.
 */
class MFVideoSampler : public IMFSampleGrabberSinkCallback
{
public:
    /** Default constructor. */
    MFVideoSampler(WmfMediaEngine* owner) : RefCount(1), Owner(owner) {}

    //~ IMFSampleGrabberSinkCallback interface

    STDMETHODIMP OnClockPause(MFTIME SystemTime) { return S_OK; }

    STDMETHODIMP OnClockRestart(MFTIME SystemTime) { return S_OK; }

    STDMETHODIMP OnClockSetRate(MFTIME SystemTime, float flRate) { return S_OK; }

    STDMETHODIMP OnClockStart(MFTIME SystemTime, LONGLONG llClockStartOffset) { return S_OK; }

    STDMETHODIMP OnClockStop(MFTIME SystemTime) { return S_OK; }

    STDMETHODIMP OnProcessSample(REFGUID MajorMediaType,
                                 DWORD SampleFlags,
                                 LONGLONG SampleTime,
                                 LONGLONG SampleDuration,
                                 const BYTE* SampleBuffer,
                                 DWORD SampleSize)
    {
        Owner->HandleVideoSample(SampleBuffer, SampleSize);
        return S_OK;
    }

    STDMETHODIMP OnSetPresentationClock(IMFPresentationClock* Clock) { return S_OK; }

    STDMETHODIMP OnShutdown() { return S_OK; }

public:
    //~ IUnknown interface

    STDMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&RefCount); }

#    if _MSC_VER == 1900
#        pragma warning(push)
#        pragma warning(disable : 4838)
#    endif

    STDMETHODIMP QueryInterface(REFIID RefID, void** Object)
    {
        static const QITAB QITab[] = {QITABENT(MFVideoSampler, IMFSampleGrabberSinkCallback), {0}};

        return QISearch(this, QITab, RefID, Object);
    }

#    if _MSC_VER == 1900
#        pragma warning(pop)
#    endif

    STDMETHODIMP_(ULONG) Release()
    {
        auto CurrentRefCount = InterlockedDecrement(&RefCount);

        if (CurrentRefCount == 0)
        {
            delete this;
        }

        return CurrentRefCount;
    }

private:
    /** Hidden destructor (this class is reference counted). */
    virtual ~MFVideoSampler() { assert(RefCount == 0); }

private:
    /** Holds a reference counter for this instance. */
    long RefCount;

    WmfMediaEngine* Owner;  // weak ref to owner
};

#    if 0
///////////////////////////////////////////////////////////////////////
//  Name: CreateInstance
//  Description:  Static class method to create the WmfMediaEngine object.
//
//  hVideo:   Handle to the video window.
//  hEvent:   Handle to the window to receive notifications.
//  ppPlayer: Receives an AddRef's pointer to the WmfMediaEngine object.
//            The caller must release the pointer.
/////////////////////////////////////////////////////////////////////////

HRESULT WmfMediaEngine::CreateInstance(WmfMediaEngine** ppPlayer)
{
    AXME_TRACE((L"WmfMediaEngine::Create\n"));

    if (ppPlayer == NULL)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;

    auto pPlayer = new WmfMediaEngine();

    if (pPlayer == NULL)
    {
        return E_OUTOFMEMORY;
    }

    hr = pPlayer->Initialize();

    if (SUCCEEDED(hr))
        *ppPlayer = pPlayer;
    else
        pPlayer->Release();

    return hr;
}
#    endif

///////////////////////////////////////////////////////////////////////
//  WmfMediaEngine constructor
/////////////////////////////////////////////////////////////////////////

WmfMediaEngine::WmfMediaEngine() : m_nRefCount(1) {}

///////////////////////////////////////////////////////////////////////
//  WmfMediaEngine destructor
/////////////////////////////////////////////////////////////////////////

WmfMediaEngine::~WmfMediaEngine()
{
    assert(m_pSession == NULL);  // If FALSE, the app did not call Shutdown().

    // Note: The application must call Shutdown() because the media
    // session holds a reference count on the WmfMediaEngine object. (This happens
    // when WmfMediaEngine calls IMediaEventGenerator::BeginGetEvent on the
    // media session.) As a result, there is a circular reference count
    // between the WmfMediaEngine object and the media session. Calling Shutdown()
    // breaks the circular reference count.

    // Note: If CreateInstance failed, the application will not call
    // Shutdown(). To handle that case, we must call Shutdown() in the
    // destructor. The circular ref-count problem does not occcur if
    // CreateInstance has failed. Also, calling Shutdown() twice is
    // harmless.

    ClearPendingBuffers();

    Shutdown();
}

//////////////////////////////////////////////////////////////////////
//  Name: Initialize
//  Initializes the WmfMediaEngine object. This method is called by the
//  CreateInstance method.
/////////////////////////////////////////////////////////////////////////

HRESULT WmfMediaEngine::Initialize()
{
    HRESULT hr = S_OK;

    if (m_hCloseEvent)
    {
        return MF_E_ALREADY_INITIALIZED;
    }

    // Start up Media Foundation platform.
    CHECK_HR(hr = MFUtils::InitializeMFOnce());

    m_hOpenEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_hOpenEvent == NULL)
    {
        CHECK_HR(hr = HRESULT_FROM_WIN32(GetLastError()));
    }

    m_hCloseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_hCloseEvent == NULL)
    {
        CHECK_HR(hr = HRESULT_FROM_WIN32(GetLastError()));
    }

done:
    return hr;
}

///////////////////////////////////////////////////////////////////////
//  AddRef
/////////////////////////////////////////////////////////////////////////

ULONG WmfMediaEngine::AddRef()
{
    return InterlockedIncrement(&m_nRefCount);
}

///////////////////////////////////////////////////////////////////////
//  Release
/////////////////////////////////////////////////////////////////////////

ULONG WmfMediaEngine::Release()
{
    ULONG uCount = InterlockedDecrement(&m_nRefCount);
    if (uCount == 0)
    {
        delete this;
    }
    // For thread safety, return a temporary variable.
    return uCount;
}

///////////////////////////////////////////////////////////////////////
//  QueryInterface
/////////////////////////////////////////////////////////////////////////

HRESULT WmfMediaEngine::QueryInterface(REFIID iid, void** ppv)
{
    if (!ppv)
    {
        return E_POINTER;
    }
    if (iid == IID_IUnknown)
    {
        *ppv = static_cast<IUnknown*>(this);
    }
    else if (iid == IID_IMFAsyncCallback)
    {
        *ppv = static_cast<IMFAsyncCallback*>(this);
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}

///////////////////////////////////////////////////////////////////////
//  Name: OpenURL
//  Description:  Opens a URL for playback.
/////////////////////////////////////////////////////////////////////////

bool WmfMediaEngine::open(std::string_view sourceUri)
{
    close();

    if (sourceUri.empty())
        return false;

    AXME_TRACE("WmfMediaEngine::OpenURL\n");
    AXME_TRACE("URL = {}\n", sourceUri);

    // 1. Create a new media session.
    // 2. Create the media source.
    // 3. Create the topology.
    // 4. Queue the topology [asynchronous]
    // 5. Start playback [asynchronous - does not happen in this method.]

    // Create the media session.
    if (FAILED(CreateSession()))
        return false;

    // Set our state to "open pending"
    m_state = MEMediaState::Preparing;

    TComPtr<IUnknown> sharedFromThis;
    this->QueryInterface(IID_IUnknown, &sharedFromThis);

    m_bOpenPending = true;

    Director::getInstance()->getJobSystem()->enqueue([this, sharedFromThis, wsourceUri = ntcvt::from_chars(sourceUri)] {
        TComPtr<IMFTopology> pTopology;
        TComPtr<IMFClock> pClock;

        try
        {
            // Create the media source.
            DX::ThrowIfFailed(CreateMediaSource(wsourceUri.c_str()));
            if (!m_pSession)
                DX::ThrowIfFailed(E_POINTER);

            // Create a partial topology.
            DX::ThrowIfFailed(CreateTopologyFromSource(&pTopology));

            // Set the topology on the media session.
            DX::ThrowIfFailed(m_pSession->SetTopology(0, pTopology.Get()));

            // If SetTopology succeeded, the media session will queue an
            // MESessionTopologySet event.

            // ======> Read media properties
            // Get the session capabilities.
            DX::ThrowIfFailed(m_pSession->GetSessionCapabilities(&m_caps));

            // Get the duration from the presentation descriptor (optional)
            (void)m_PresentDescriptor->GetUINT64(MF_PD_DURATION, (UINT64*)&m_hnsDuration);

            // Get the presentation clock (optional)
            auto hr = m_pSession->GetClock(&pClock);
            if (SUCCEEDED(hr))
                DX::ThrowIfFailed(hr = pClock->QueryInterface(IID_PPV_ARGS(&m_pClock)));

            // Get the rate control interface (optional)
            DX::ThrowIfFailed(MFGetService(m_pSession.Get(), MF_RATE_CONTROL_SERVICE, IID_PPV_ARGS(&m_RateControl)));

            DX::ThrowIfFailed(MFGetService(m_pSession.Get(), MF_RATE_CONTROL_SERVICE, IID_PPV_ARGS(&m_RateSupport)));

            // Check if rate 0 (scrubbing) is supported.
            if (SUCCEEDED(m_RateSupport->IsRateSupported(TRUE, 0, NULL)))
                m_bCanScrub = TRUE;

            // if m_pRate is NULL, m_bCanScrub must be FALSE.
            assert(m_RateControl || !m_bCanScrub);
        }
        catch (const std::exception& ex)
        {
            AXME_TRACE("Exception occurred when Open Media: {}", ex.what());
            (void)ex;
            m_state = MEMediaState::Error;
        }

        m_bOpenPending = false;
        SetEvent(m_hOpenEvent);
    });

    return true;
}

bool WmfMediaEngine::close()
{
    if (m_bOpenPending)
        WaitForSingleObject(m_hOpenEvent, INFINITE);

    ClearPendingBuffers();

    HRESULT hr = S_OK;
    auto state = getState();
    if (state != MEMediaState::Closed)
    {
        this->stop();
        hr = CloseSession();
    }
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////
//  Name: Invoke
//  Description:  Callback for asynchronous BeginGetEvent method.
//
//  pAsyncResult: Pointer to the result.
/////////////////////////////////////////////////////////////////////////

HRESULT WmfMediaEngine::Invoke(IMFAsyncResult* pResult)
{
    HRESULT hr              = S_OK;
    ::MediaEventType meType = MEUnknown;  // Event type

    TComPtr<IMFMediaEvent> pEvent;

    // Get the event from the event queue.
    CHECK_HR(hr = m_pSession->EndGetEvent(pResult, &pEvent));

    // Get the event type.
    CHECK_HR(hr = pEvent->GetType(&meType));

    // If the session is closed, the application is waiting on the
    // m_hCloseEvent event handle. Also, do not get any more
    // events from the session.

    if (meType == MESessionClosed)
    {
        SetEvent(m_hCloseEvent);
    }
    else
    {
        // For all other events, ask the media session for the
        // next event in the queue.
        CHECK_HR(hr = m_pSession->BeginGetEvent(this, NULL));
    }

    // For most events, we post the event as a private window message to the
    // application. This lets the application process the event on it's
    // main thread.

    // However, if call to IMFMediaSession::Close is pending, it means the
    // application is waiting on the m_hCloseEvent event handle. (Blocking
    // call.) In that case, we simply discard the event.

    // NOTE: When IMFMediaSession::Close is called, MESessionClosed is NOT
    // necessarily the next event that we will receive. We may receive
    // any number of other events before receiving MESessionClosed.
    if (!m_bClosePending)
    {
        HandleEvent(pEvent.Get());

        // if (!m_hwndEvent)
        //    HandleEvent((WPARAM)pEvent.Get());
        // else
        //    PostMessage(m_hwndEvent, WM_APP_PLAYER_EVENT, (WPARAM)pEvent.Get(), (LPARAM)0);
    }

done:
    // SAFE_RELEASE(pEvent);
    return S_OK;
}

void WmfMediaEngine::HandleVideoSample(const uint8_t* buf, size_t len)
{
    std::unique_lock<std::mutex> lck(m_frameBuffer1Mtx);
    m_frameBuffer1.assign(buf, buf + len);
}

void WmfMediaEngine::ClearPendingBuffers()
{
    std::unique_lock<std::mutex> lck(m_frameBuffer1Mtx);
    m_frameBuffer1.clear();
    m_frameBuffer2.clear();
}

bool WmfMediaEngine::transferVideoFrame()
{
    if (m_state != MEMediaState::Playing || m_frameBuffer1.empty())
        return false;

    std::unique_lock<std::mutex> lck(m_frameBuffer1Mtx);
    if (!m_frameBuffer1.empty())
    {
        m_frameBuffer2.swap(m_frameBuffer1);
        lck.unlock();  // unlock immidiately before invoke user callback (maybe upload buffer to GPU)

        auto& buffer = m_frameBuffer2;

        auto cbcrData =
            (m_videoPF == MEVideoPixelFormat::NV12) ? buffer.data() + m_frameExtent.x * m_frameExtent.y : nullptr;
        MEVideoFrame frame{buffer.data(), cbcrData, buffer.size(),
                           MEVideoPixelDesc{m_videoPF, MEIntPoint{m_frameExtent.x, m_frameExtent.y}}, m_videoExtent};
        frame._vpd._rotation = static_cast<int>(m_VideoRotation);
#    if defined(_DEBUG)
        switch (m_videoPF)
        {
        case MEVideoPixelFormat::YUY2:
            assert(m_frameExtent.x == (m_bIsH264 ? YASIO_SZ_ALIGN(m_frameExtent.x, 16) : m_frameExtent.x));
            break;
        case MEVideoPixelFormat::NV12:
        {
            // HEVC(H265) on Windows, both height width align 32
            // refer to: https://community.intel.com/t5/Media-Intel-oneAPI-Video/32-byte-alignment-for-HEVC/m-p/1048275
            auto& desc     = frame._ycbcrDesc;
            desc.YDim.x    = YASIO_SZ_ALIGN(m_videoExtent.x, 32);
            desc.YDim.y    = m_bIsHEVC ? YASIO_SZ_ALIGN(m_videoExtent.y, 32) : m_videoExtent.y;
            desc.CbCrDim.x = desc.YDim.x / 2;
            desc.CbCrDim.y = desc.YDim.y / 2;
            desc.YPitch    = desc.YDim.x;
            desc.CbCrPitch = desc.YPitch;

            assert(frame._vpd._dim.x * frame._vpd._dim.y * 3 / 2 == static_cast<int>(frame._dataLen));
            assert((desc.YPitch * desc.YDim.y + desc.CbCrPitch * desc.CbCrDim.y) == static_cast<int>(frame._dataLen));
            break;
        }
        default:
            assert(m_videoPF == MEVideoPixelFormat::RGB32 || m_videoPF == MEVideoPixelFormat::BGR32);
        }
#    endif
        // check data
        _onVideoFrame(frame);

        m_frameBuffer2.clear();

        return true;
    }
    return false;
}

//-------------------------------------------------------------------
//  HandleEvent
//
//  Called by the application when it receives a WM_APP_PLAYER_EVENT
//  message.
//
//  This method is used to process media session events on the
//  application's main thread.
//
//  pUnkPtr: Pointer to the IUnknown interface of a media session
//  event (IMFMediaEvent).
//-------------------------------------------------------------------

HRESULT WmfMediaEngine::HandleEvent(IMFMediaEvent* pEvent)
{
    HRESULT hr               = S_OK;
    HRESULT hrStatus         = S_OK;                   // Event status
    ::MediaEventType meType  = MEUnknown;              // Event type
    MF_TOPOSTATUS TopoStatus = MF_TOPOSTATUS_INVALID;  // Used with MESessionTopologyStatus event.

    // auto pUnk = MFUtils::ReferencedPtrToComPtr((IUnknown*)pUnkPtr);
    // TComPtr<IMFMediaEvent> pEvent;

    PROPVARIANT var;

    if (!pEvent)
    {
        return E_POINTER;
    }

    // CHECK_HR(hr = pUnk->QueryInterface(__uuidof(IMFMediaEvent), (void**)&pEvent));

    // Get the event type.
    CHECK_HR(hr = pEvent->GetType(&meType));

    // Get the event status. If the operation that triggered the event did
    // not succeed, the status is a failure code.
    CHECK_HR(hr = pEvent->GetStatus(&hrStatus));

    // AXME_TRACE("Media event: %s\n", EventName(meType));

    // Check if the async operation succeeded.
    if (SUCCEEDED(hrStatus))
    {
        // Switch on the event type. Update the internal state of the WmfMediaEngine as needed.
        switch (meType)
        {
        case MESessionTopologyStatus:
            // Get the status code.
            CHECK_HR(hr = pEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, (UINT32*)&TopoStatus));
            switch (TopoStatus)
            {
            case MF_TOPOSTATUS_READY:
                hr = OnTopologyReady(pEvent);
                break;
            default:
                // Nothing to do.
                break;
            }
            break;
        case MEEndOfPresentation:
            OnPlayEnded(pEvent);
            break;
        case MESessionStarted:
            OnSessionStart(hrStatus);
            fireMediaEvent(MEMediaEventType::Playing);
            break;

        case MESessionStopped:
            OnSessionStop(hrStatus);
            fireMediaEvent(MEMediaEventType::Stopped);
            break;

        case MESessionPaused:
            OnSessionPause(hrStatus);
            fireMediaEvent(MEMediaEventType::Paused);
            break;

        case MESessionRateChanged:
            // If the rate change succeeded, we've already got the rate
            // cached. If it failed, try to get the actual rate.
            if (FAILED(hrStatus))
            {
                PropVariantInit(&var);

                hr = pEvent->GetValue(&var);

                if (SUCCEEDED(hr) && (var.vt == VT_R4))
                {
                    m_nominal.fRate = var.fltVal;
                }
            }
            break;

        case MESessionEnded:
            OnSessionEnded(hrStatus);
            m_bPlaybackEnded = true;
            fireMediaEvent(MEMediaEventType::Stopped);
            break;

        case MESessionCapabilitiesChanged:
            // The session capabilities changed. Get the updated capabilities.
            m_caps = MFGetAttributeUINT32(pEvent, MF_EVENT_SESSIONCAPS, m_caps);
            break;
        }
    }
    else
    {
        hr = hrStatus;
    }

done:
    return hr;
}

///////////////////////////////////////////////////////////////////////
//  Name: ShutDown
//  Description:  Releases all resources held by this object.
/////////////////////////////////////////////////////////////////////////

HRESULT WmfMediaEngine::Shutdown()
{
    AXME_TRACE("WmfMediaEngine::ShutDown\n");

    HRESULT hr = S_OK;

    // Close the session
    hr = CloseSession();

    if (m_hCloseEvent)
    {
        CloseHandle(m_hCloseEvent);
        m_hCloseEvent = nullptr;
    }

    if (m_hOpenEvent)
    {
        CloseHandle(m_hOpenEvent);
        m_hOpenEvent = nullptr;
    }

    return hr;
}

///
///
/// Protected methods
///
/// All methods that follow are private to the WmfMediaEngine class.
///
///

///////////////////////////////////////////////////////////////////////
//  Name: OnTopologyReady
//  Description:  Handler for MESessionTopologyReady event.
//
//  Note:
//  - The MESessionTopologySet event means the session queued the
//    topology, but the topology is not ready yet. Generally, the
//    applicationno need to respond to this event unless there is an
//    error.
//  - The MESessionTopologyReady event means the new topology is
//    ready for playback. After this event is received, any calls to
//    IMFGetService will get service interfaces from the new topology.
/////////////////////////////////////////////////////////////////////////

HRESULT WmfMediaEngine::OnTopologyReady(IMFMediaEvent* pEvent)
{
    AXME_TRACE("WmfMediaEngine::OnTopologyReady\n");

    UINT32 w = 0, h = 0;
    MFGetAttributeSize(m_videoInputType.Get(), MF_MT_FRAME_SIZE, &w, &h);
    m_frameExtent.x = m_bIsH264 ? YASIO_SZ_ALIGN(w, 16) : w;
    m_frameExtent.y = h;

    DWORD cx = 0, cy = 0;
    GetNativeVideoSize(&cx, &cy);
    m_videoExtent.x = cx;
    m_videoExtent.y = cy;

    // MFVideoRotationFormat
    m_VideoRotation = static_cast<MFVideoRotationFormat>(MFGetAttributeUINT32(m_videoInputType.Get(), MF_MT_VIDEO_ROTATION, FALSE));
    if (m_bAutoPlay)
        StartPlayback(nullptr);

    return S_OK;
}

// Starts playback.

bool WmfMediaEngine::play()
{
    HRESULT hr = S_OK;

    AXME_TRACE("WmfMediaEngine::Play\n");

    if (m_state != MEMediaState::Paused && m_state != MEMediaState::Stopped)
        MF_E_INVALIDREQUEST;

    if (m_pSession == NULL || m_pSource == NULL)
        E_UNEXPECTED;

    AutoLock lock(m_critsec);

    // If another operation is pending, cache the request.
    // Otherwise, start the media session.
    if (m_bPending)
    {
        m_request.command = CmdStart;
    }
    else
    {
        hr = StartPlayback(nullptr);

        m_nominal.command = CmdStart;
        m_bPending        = CMD_PENDING;
    }

    return SUCCEEDED(hr);
}

// Pauses playback.

bool WmfMediaEngine::pause()
{
    HRESULT hr = S_OK;

    if (m_pSession == NULL || m_pSource == NULL)
        E_UNEXPECTED;

    AutoLock lock(m_critsec);

    // If another operation is pending, cache the request.
    // Otherwise, pause the media session.
    if (m_bPending)
    {
        m_request.command = CmdPause;
    }
    else
    {
        hr = m_pSession->Pause();

        m_nominal.command = CmdPause;
        m_bPending        = CMD_PENDING;
    }

    if (SUCCEEDED(hr))
        m_state = MEMediaState::Paused;

    return SUCCEEDED(hr);
}

// Stops playback.

bool WmfMediaEngine::stop()
{
    HRESULT hr = S_OK;

    if (m_pSession == NULL || m_pSource == NULL)
        return false;  // E_UNEXPECTED;

    AutoLock lock(m_critsec);

    if (m_state == MEMediaState::Stopped)
        return false;  // S_OK;

    // If another operation is pending, cache the request.
    // Otherwise, stop the media session.
    if (m_bPending)
    {
        m_request.command = CmdStop;
    }
    else
    {
        hr = m_pSession->Stop();

        m_nominal.command = CmdStop;
        m_bPending        = CMD_PENDING;
    }

    if (SUCCEEDED(hr))
        m_state = MEMediaState::Stopped;

    return SUCCEEDED(hr);
}

BOOL WmfMediaEngine::CanSeek() const
{
    bool bCanSeek = (((m_caps & MFSESSIONCAP_SEEK) == MFSESSIONCAP_SEEK) && (m_hnsDuration > 0) && (m_pClock != NULL));
    return bCanSeek;
}

MFTIME WmfMediaEngine::GetDuration() const
{
    return m_hnsDuration;
}

MFTIME WmfMediaEngine::GetCurrentPosition() const
{
    HRESULT hr = S_OK;

    // TODO:
    AutoLock lock(m_critsec);

    MFTIME hnsPosition = 0;

    if (m_pClock == NULL)
    {
        return MF_E_NO_CLOCK;
    }

    // Return, in order:
    // 1. Cached seek request (nominal position).
    // 2. Pending seek operation (nominal position).
    // 3. Presentation time (actual position).

    if (m_request.command == CmdSeek)
    {
        hnsPosition = m_request.hnsStart;
    }
    else if (m_bPending & CMD_PENDING_SEEK)
    {
        hnsPosition = m_nominal.hnsStart;
    }
    else
    {
        hr = m_pClock->GetTime(&hnsPosition);
    }

    return hnsPosition;
}

double WmfMediaEngine::getCurrentTime()
{
    return GetCurrentPosition() / (std::nano::den / 100.0);
}

double WmfMediaEngine::getDuration()
{
    return GetDuration() / (std::nano::den / 100.0);
}

HRESULT WmfMediaEngine::SetPosition(MFTIME hnsPosition)
{
    AutoLock lock(m_critsec);

    HRESULT hr = S_OK;

    if (m_bPending)
    {
        // Currently seeking or changing rates, so cache this request.
        m_request.command  = CmdSeek;
        m_request.hnsStart = hnsPosition;
    }
    else
    {
        hr = SetPositionInternal(hnsPosition);
    }

    return hr;
}

BOOL WmfMediaEngine::CanScrub() const
{
    return m_bCanScrub;
}

HRESULT WmfMediaEngine::Scrub(BOOL bScrub)
{
    // Scrubbing is implemented as rate = 0.

    AutoLock lock(m_critsec);

    if (!m_RateControl)
    {
        return MF_E_INVALIDREQUEST;
    }
    if (!m_bCanScrub)
    {
        return MF_E_INVALIDREQUEST;
    }

    HRESULT hr = S_OK;

    if (bScrub)
    {
        // Enter scrubbing mode. Cache the current rate.

        if (GetNominalRate() != 0)
        {
            m_fPrevRate = m_nominal.fRate;
        }

        hr = setRate(0.0f) ? S_OK : E_FAIL;
    }
    else
    {
        // Leaving scrubbing mode. Restore the old rate.

        if (GetNominalRate() == 0)
        {
            hr = setRate(m_fPrevRate) ? S_OK : E_FAIL;
        }
    }

    return hr;
}

BOOL WmfMediaEngine::CanFastForward() const
{
    BOOL bCanFF = ((m_caps & MFSESSIONCAP_RATE_FORWARD) == MFSESSIONCAP_RATE_FORWARD);
    return bCanFF;
}

BOOL WmfMediaEngine::CanRewind() const
{
    BOOL bCanRewind = ((m_caps & MFSESSIONCAP_RATE_REVERSE) == MFSESSIONCAP_RATE_REVERSE);
    return bCanRewind;
}

HRESULT WmfMediaEngine::FastForward()
{
    if (!m_RateControl)
    {
        return MF_E_INVALIDREQUEST;
    }

    HRESULT hr    = S_OK;
    float fTarget = GetNominalRate() * 2;

    if (fTarget <= 0.0f)
    {
        fTarget = 1.0f;
    }

    hr = setRate(fTarget) ? S_OK : E_FAIL;

    return hr;
}

HRESULT WmfMediaEngine::Rewind()
{
    if (!m_RateControl)
    {
        return MF_E_INVALIDREQUEST;
    }

    HRESULT hr    = S_OK;
    float fTarget = GetNominalRate() * 2;

    if (fTarget >= 0.0f)
    {
        fTarget = -1.0f;
    }

    setRate(fTarget);

    return hr;
}

bool WmfMediaEngine::setRate(double lfRate)
{
    HRESULT hr = S_OK;
    BOOL bThin = FALSE;

    AutoLock lock(m_critsec);

    const float fRate = static_cast<float>(lfRate);

    if (fRate == GetNominalRate())
    {
        return true;  // S_OK;  // no-op
    }

    if (m_RateSupport == NULL)
    {
        return false;  // MF_E_INVALIDREQUEST;
    }

    // Check if this rate is supported. Try non-thinned playback first,
    // then fall back to thinned playback.

    hr = m_RateSupport->IsRateSupported(FALSE, fRate, NULL);
    if (FAILED(hr) && hr != MF_E_INVALIDREQUEST)
    {
        bThin = TRUE;
        hr    = m_RateSupport->IsRateSupported(TRUE, fRate, NULL);
    }

    if (FAILED(hr) && hr != MF_E_INVALIDREQUEST)
    {
        // Unsupported rate.
        return SUCCEEDED(hr);
    }

    // If there is an operation pending, cache the request.
    if (m_bPending || hr == MF_E_INVALIDREQUEST)
    {
        m_bPending = CMD_PENDING_RATE;

        m_request.fRate = fRate;
        m_request.bThin = bThin;

        // Remember the current transport state (play, paused, etc), so that we
        // can restore it after the rate change, if necessary. However, if
        // anothercommand is already pending, that one takes precedent.

        if (m_request.command == CmdNone)
        {
            m_request.command = m_nominal.command;
        }
    }
    else
    {
        // No pending operation. Commit the new rate.
        hr = CommitRateChange(fRate, bThin);
    }

    return SUCCEEDED(hr);
}

HRESULT WmfMediaEngine::SetPositionInternal(const MFTIME& hnsPosition)
{
    assert(!m_bPending);

    if (m_pSession == NULL)
    {
        return MF_E_INVALIDREQUEST;
    }

    HRESULT hr = StartPlayback(&hnsPosition);
    if (SUCCEEDED(hr))
    {
        // Store the pending state.
        m_nominal.command  = CmdStart;
        m_nominal.hnsStart = hnsPosition;
        m_bPending         = CMD_PENDING_SEEK;

        m_state = MEMediaState::Playing;
    }
    return hr;
}

HRESULT WmfMediaEngine::StartPlayback(const MFTIME* hnsPosition)
{
    HRESULT hr = S_OK;

    PROPVARIANT varStart;
    if (!hnsPosition)
        varStart.vt = VT_EMPTY;
    else
    {
        varStart.vt            = VT_I8;
        varStart.hVal.QuadPart = *hnsPosition;
    }

    // Try apply pending rate before start playback
    if (m_bPending & CMD_PENDING_RATE && m_request.fRate != m_nominal.fRate)
    {
        if (SUCCEEDED(m_RateControl->SetRate(m_request.bThin, m_request.fRate)))
        {
            m_bPending        = FALSE;
            m_request.command = CmdNone;
            m_nominal.fRate   = m_request.fRate;
        }
    }

    m_bPlaybackEnded = false;
    hr = m_pSession->Start(NULL, &varStart);

    // Note: Start is an asynchronous operation. However, we
    // can treat our state as being already started. If Start
    // fails later, we'll get an MESessionStarted event with
    // an error code, and we will update our state then.
    if (SUCCEEDED(hr))
        m_state = MEMediaState::Playing;
    return hr;
}

HRESULT WmfMediaEngine::CommitRateChange(float fRate, BOOL bThin)
{
    assert(!m_bPending);

    // Caller holds the lock.

    HRESULT hr           = S_OK;
    MFTIME hnsSystemTime = 0;
    MFTIME hnsClockTime  = 0;

    Command cmdNow = m_nominal.command;

    TComPtr<IMFClock> pClock;

    // Allowed rate transitions:

    // Positive <-> negative:   Stopped
    // Negative <-> zero:       Stopped
    // Postive <-> zero:        Paused or stopped

    if ((fRate > 0 && m_nominal.fRate <= 0) || (fRate < 0 && m_nominal.fRate >= 0))
    {
        // Transition to stopped.
        if (cmdNow == CmdStart)
        {
            // Get the current clock position. This will be the restart time.
            hr = m_pSession->GetClock(&pClock);
            if (FAILED(hr))
            {
                goto done;
            }

            (void)pClock->GetCorrelatedTime(0, &hnsClockTime, &hnsSystemTime);

            assert(hnsSystemTime != 0);

            // Stop and set the rate
            if (!stop())
            {
                hr = E_FAIL;
                goto done;
            }

            // Cache Request: Restart from stop.
            m_request.command  = CmdSeek;
            m_request.hnsStart = hnsClockTime;
        }
        else if (cmdNow == CmdPause)
        {
            // The current state is paused.

            // For this rate change, the session must be stopped. However, the
            // session cannot transition back from stopped to paused.
            // Therefore, this rate transition is not supported while paused.

            hr = MF_E_UNSUPPORTED_STATE_TRANSITION;
            goto done;
        }
    }
    else if (fRate == 0 && m_nominal.fRate != 0)
    {
        if (cmdNow != CmdPause)
        {
            // Transition to paused.

            // This transisition requires the paused state.

            // Pause and set the rate.
            if (!pause())
            {
                hr = E_FAIL;
                goto done;
            }

            // Request: Switch back to current state.
            m_request.command = cmdNow;
        }
    }

    // Set the rate.
    hr = m_RateControl->SetRate(bThin, fRate);
    if (FAILED(hr))
    {
        goto done;
    }

    // Adjust our current rate and requested rate.
    m_request.fRate = m_nominal.fRate = fRate;

done:
    return hr;
}
float WmfMediaEngine::GetNominalRate() const
{
    return m_request.fRate;
}

HRESULT WmfMediaEngine::UpdatePendingCommands(Command cmd)
{
    HRESULT hr = S_OK;

    PROPVARIANT varStart;
    PropVariantInit(&varStart);

    AutoLock lock(m_critsec);

    if (m_bPending && m_nominal.command == cmd)
    {
        m_bPending = FALSE;

        // The current pending command has completed.

        // First look for rate changes.
        if (m_request.fRate != m_nominal.fRate)
        {
            hr = CommitRateChange(m_request.fRate, m_request.bThin);
            if (FAILED(hr))
            {
                goto done;
            }
        }

        // Now look for seek requests.
        if (!m_bPending)
        {
            switch (m_request.command)
            {
            case CmdNone:
                // Nothing to do.
                break;

            case CmdStart:
                play();
                break;

            case CmdPause:
                pause();
                break;

            case CmdStop:
                stop();
                break;

            case CmdSeek:
                SetPositionInternal(m_request.hnsStart);
                break;
            }
            m_request.command = CmdNone;
        }
    }

done:
    return hr;
}

HRESULT WmfMediaEngine::OnPlayEnded(IMFMediaEvent* pEvent)
{
    AXME_TRACE("WmfMediaEngine::OnPlayEnded\n");

    // The session puts itself into the stopped state autmoatically.

    m_state = MEMediaState::Stopped;

    if (m_bLooping)
    {
        SetPosition(0);
    }

    return S_OK;
}

HRESULT WmfMediaEngine::OnSessionStart(HRESULT hrStatus)
{
    HRESULT hr = S_OK;

    if (FAILED(hrStatus))
    {
        return hrStatus;
    }

    // The Media Session completed a start/seek operation. Check if there
    // is another seek request pending.
    UpdatePendingCommands(CmdStart);

    return hr;
}

HRESULT WmfMediaEngine::OnSessionStop(HRESULT hrStatus)
{
    HRESULT hr = S_OK;

    if (FAILED(hrStatus))
    {
        return hrStatus;
    }

    // The Media Session completed a transition to stopped. This might occur
    // because we are changing playback direction (forward/rewind). Check if
    // there is a pending rate-change request.

    UpdatePendingCommands(CmdStop);

    return hr;
}

HRESULT WmfMediaEngine::OnSessionPause(HRESULT hrStatus)
{
    HRESULT hr = S_OK;

    if (FAILED(hrStatus))
    {
        return hrStatus;
    }

    hr = UpdatePendingCommands(CmdPause);

    return hr;
}

HRESULT WmfMediaEngine::OnSessionEnded(HRESULT hrStatus)
{  // After the session ends, playback starts from position zero. But if the
    // current playback rate is reversed, playback would end immediately
    // (reversing from position 0). Therefore, reset the rate to 1x.

    if (GetNominalRate() < 0.0f)
    {
        m_nominal.command = CmdStop;

        hrStatus = CommitRateChange(1.0f, FALSE);
    }

    return hrStatus;
}

///////////////////////////////////////////////////////////////////////
//  Name: CreateSession
//  Description:  Creates a new instance of the media session.
/////////////////////////////////////////////////////////////////////////

HRESULT WmfMediaEngine::CreateSession()
{
    AXME_TRACE("WmfMediaEngine::CreateSession\n");

    HRESULT hr = S_OK;

    TComPtr<IMFAttributes> pAttributes;
    // IMFActivate   *pEnablerActivate = NULL;

    // Close the old session, if any.
    CHECK_HR(hr = CloseSession());

    assert(m_state == MEMediaState::Closed);

    // Create a new attribute store.
    CHECK_HR(hr = MFCreateAttributes(&pAttributes, 2));

    pAttributes->SetUINT32(MF_LOW_LATENCY, TRUE);
    CHECK_HR(hr = MFCreateMediaSession(pAttributes.Get(), &m_pSession));

    // TODO:

    // If MFCreatePMPMediaSession fails it might return an IMFActivate pointer.
    // This indicates that a trusted binary failed to load in the protected process.
    // An application can use the IMFActivate pointer to create an enabler object, which
    // provides revocation and renewal information for the component that failed to
    // load.

    // This sample does not demonstrate that feature. Instead, we simply treat this
    // case as a playback failure.

    // Start pulling events from the media session
    CHECK_HR(hr = m_pSession->BeginGetEvent((IMFAsyncCallback*)this, NULL));

done:
    // SAFE_RELEASE(pAttributes);
    // SAFE_RELEASE(pEnablerActivate);
    return hr;
}

///////////////////////////////////////////////////////////////////////
//  Name: CloseSession
//  Description:  Closes the media session.
//
//  Note: The IMFMediaSession::Close method is asynchronous, but the
//        WmfMediaEngine::CloseSession method waits on the MESessionClosed event.
//        The MESessionClosed event is guaranteed to be the last event
//        that the media session fires.
/////////////////////////////////////////////////////////////////////////

HRESULT WmfMediaEngine::CloseSession()
{
    HRESULT hr = S_OK;

    // SAFE_RELEASE(m_pVideoDisplay);

    if (m_pSession)
    {
        DWORD dwWaitResult = 0;

        m_bClosePending = true;

        CHECK_HR(hr = m_pSession->Close());

        // Wait for the close operation to complete

        dwWaitResult = WaitForSingleObject(m_hCloseEvent, 5000);

        if (dwWaitResult == WAIT_TIMEOUT)
        {
            AXME_TRACE("CloseSession timed out!\n");
        }

        // Now there will be no more events from this session.
    }

    // Complete shutdown operations.

    // Shut down the media source. (Synchronous operation, no events.)
    if (m_pSource)
    {
        m_pSource->Shutdown();
    }

    // Shut down the media session. (Synchronous operation, no events.)
    if (m_pSession)
    {
        m_pSession->Shutdown();
    }

    m_PresentDescriptor.Reset();
    m_RateControl.Reset();
    m_RateSupport.Reset();
    m_pClock.Reset();

    m_pSource.Reset();
    m_pSession.Reset();

    m_state    = MEMediaState::Closed;
    m_bPending = false;
    m_bClosePending = false;

done:
    return hr;
}

///////////////////////////////////////////////////////////////////////
//  Name: CreateMediaSource
//  Description:  Create a media source from a URL.
//
//  sURL: The URL to open.
/////////////////////////////////////////////////////////////////////////

HRESULT WmfMediaEngine::CreateMediaSource(const WCHAR* sURL)
{
    AXME_TRACE("WmfMediaEngine::CreateMediaSource\n");

    HRESULT hr                = S_OK;
    MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;

    TComPtr<IMFSourceResolver> pSourceResolver;
    TComPtr<IUnknown> pSource;

    m_pSource.Reset();  // SAFE_RELEASE(m_pSource);

    // Create the source resolver.
    CHECK_HR(hr = MFCreateSourceResolver(&pSourceResolver));

    // Use the source resolver to create the media source.

    // Note: For simplicity this sample uses the synchronous method on
    // IMFSourceResolver to create the media source. However, creating a
    // media source can take a noticeable amount of time, especially for
    // a network source. For a more responsive UI, use the asynchronous
    // BeginCreateObjectFromURL method.

    CHECK_HR(hr = pSourceResolver->CreateObjectFromURL(sURL,                       // URL of the source.
                                                       MF_RESOLUTION_MEDIASOURCE,  // Create a source object.
                                                       NULL,                       // Optional property store.
                                                       &ObjectType,                // Receives the created object type.
                                                       &pSource  // Receives a pointer to the media source.
                                                       ));

    // Get the IMFMediaSource interface from the media source.
    CHECK_HR(hr = pSource->QueryInterface(__uuidof(IMFMediaSource), (void**)&m_pSource));

done:
    // SAFE_RELEASE(pSourceResolver);
    // SAFE_RELEASE(pSource);
    return hr;
}

///////////////////////////////////////////////////////////////////////
//  CreateTopologyFromSource
//  Description:  Create a playback topology from the media source.
//
//  Pre-condition: The media source must be created already.
//                 Call CreateMediaSource() before calling this method.
/////////////////////////////////////////////////////////////////////////

HRESULT WmfMediaEngine::CreateTopologyFromSource(IMFTopology** ppTopology)
{
    AXME_TRACE("WmfMediaEngine::CreateTopologyFromSource\n");

    assert(m_pSession != NULL);
    assert(m_pSource != NULL);

    HRESULT hr = S_OK;

    TComPtr<IMFTopology> pTopology;
    DWORD cSourceStreams = 0;

    // Create a new topology.
    CHECK_HR(hr = MFCreateTopology(&pTopology));

    // Create the presentation descriptor for the media source.
    CHECK_HR(hr = m_pSource->CreatePresentationDescriptor(&m_PresentDescriptor));

    // Get the number of streams in the media source.
    CHECK_HR(hr = m_PresentDescriptor->GetStreamDescriptorCount(&cSourceStreams));

    AXME_TRACE("Stream count: {}\n", cSourceStreams);

    // For each stream, create the topology nodes and add them to the topology.
    for (DWORD i = 0; i < cSourceStreams; ++i)
    {
        CHECK_HR(hr = AddBranchToPartialTopology(pTopology.Get(), m_PresentDescriptor.Get(), i));
    }

    // Return the IMFTopology pointer to the caller.
    if (SUCCEEDED(hr))
    {
        *ppTopology = pTopology.Get();
        (*ppTopology)->AddRef();
    }

done:
    return hr;
}

///////////////////////////////////////////////////////////////////////
//  Name:  AddBranchToPartialTopology
//  Description:  Adds a topology branch for one stream.
//
//  pTopology: Pointer to the topology object.
//  pSourcePD: The source's presentation descriptor.
//  iStream: Index of the stream to render.
//
//  Pre-conditions: The topology must be created already.
//
//  Notes: For each stream, we must do the following:
//    1. Create a source node associated with the stream.
//    2. Create an output node for the renderer.
//    3. Connect the two nodes.
//  The media session will resolve the topology, so we do not have
//  to worry about decoders or other transforms.
/////////////////////////////////////////////////////////////////////////

HRESULT WmfMediaEngine::AddBranchToPartialTopology(IMFTopology* pTopology,
                                                   IMFPresentationDescriptor* pSourcePD,
                                                   DWORD iStream)
{
    AXME_TRACE("WmfMediaEngine::AddBranchToPartialTopology\n");

    assert(pTopology != NULL);

    TComPtr<IMFStreamDescriptor> pSourceSD;
    TComPtr<IMFTopologyNode> pSourceNode;
    TComPtr<IMFTopologyNode> pOutputNode;
    BOOL fSelected = FALSE;

    HRESULT hr = S_OK;

    // Get the stream descriptor for this stream.
    CHECK_HR(hr = pSourcePD->GetStreamDescriptorByIndex(iStream, &fSelected, &pSourceSD));

    // Create the topology branch only if the stream is selected.
    // Otherwise, do nothing.
    if (fSelected)
    {
        // Create a source node for this stream.
        CHECK_HR(hr = CreateSourceStreamNode(m_pSource.Get(), pSourcePD, pSourceSD.Get(), &pSourceNode));

        // Create the output node for the renderer.
        CHECK_HR(hr = CreateOutputNode(pSourceSD.Get(), &pOutputNode));

        // Add both nodes to the topology.
        CHECK_HR(hr = pTopology->AddNode(pSourceNode.Get()));

        CHECK_HR(hr = pTopology->AddNode(pOutputNode.Get()));

        // Connect the source node to the output node.
        CHECK_HR(hr = pSourceNode->ConnectOutput(0, pOutputNode.Get(), 0));
    }

done:
    // Clean up.
    return hr;
}

//-------------------------------------------------------------------
//  Name: CreateOutputNode
//  Description:  Create an output node for a stream.
//
//  pSourceSD: Stream descriptor for the stream.
//  ppNode: Receives a pointer to the new node.
//
//  Notes:
//  This function does the following:
//  1. Chooses a renderer based on the media type of the stream.
//  2. Creates an IActivate object for the renderer.
//  3. Creates an output topology node.
//  4. Sets the IActivate pointer on the node.
//-------------------------------------------------------------------

HRESULT WmfMediaEngine::CreateOutputNode(IMFStreamDescriptor* pSourceSD, IMFTopologyNode** ppNode)
{

    TComPtr<IMFTopologyNode> OutputNode;
    TComPtr<IMFMediaTypeHandler> pHandler;
    TComPtr<IMFActivate> pRendererActivate;

    GUID guidMajorType = GUID_NULL;
    HRESULT hr         = S_OK;

    // Get the stream ID.
    DWORD streamID = 0;
    pSourceSD->GetStreamIdentifier(&streamID);  // Just for debugging, ignore any failures.

    // Get the media type handler for the stream.
    CHECK_HR(hr = pSourceSD->GetMediaTypeHandler(&pHandler));

    // Get the major media type.
    CHECK_HR(hr = pHandler->GetMajorType(&guidMajorType));

    // Create a downstream node.
    CHECK_HR(hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &OutputNode));

    // Create an IMFActivate object for the renderer, based on the media type.
    if (MFMediaType_Video == guidMajorType)
    {
        // Create the video renderer.
        AXME_TRACE("Stream {}: video stream\n", streamID);
        // CHECK_HR(hr = MFCreateVideoRendererActivate(hwndVideo, &pRendererActivate));
        auto Sampler                     = MFUtils::MakeComPtr<MFVideoSampler>(this);
        TComPtr<IMFMediaType>& InputType = m_videoInputType;
        CHECK_HR(hr = pHandler->GetCurrentMediaType(InputType.ReleaseAndGetAddressOf()));

        // Create output type
        GUID SubType;
        CHECK_HR(hr = InputType->GetGUID(MF_MT_SUBTYPE, &SubType));

        auto strType = MFUtils::GetVideoTypeName(SubType);
        AXME_TRACE("WmfMediaEngine: Input video type: {}", strType);

        m_bIsH264 = SubType == MFVideoFormat_H264 || SubType == MFVideoFormat_H264_ES;
        m_bIsHEVC = SubType == MFVideoFormat_HEVC || SubType == MFVideoFormat_HEVC_ES;

        GUID VideoOutputFormat;
        if ((SubType == MFVideoFormat_HEVC) || (SubType == MFVideoFormat_HEVC_ES) || (SubType == MFVideoFormat_NV12) ||
            (SubType == MFVideoFormat_IYUV))
        {
            VideoOutputFormat = MFVideoFormat_NV12;
        }
        else
        {
            const bool Uncompressed = (SubType == MFVideoFormat_RGB555) || (SubType == MFVideoFormat_RGB565) ||
                                      (SubType == MFVideoFormat_RGB24) || (SubType == MFVideoFormat_RGB32) ||
                                      (SubType == MFVideoFormat_ARGB32);

            VideoOutputFormat = Uncompressed ? MFVideoFormat_RGB32 : MFVideoFormat_YUY2;
        }

        TComPtr<IMFMediaType> OutputType;
        CHECK_HR(hr = ::MFCreateMediaType(&OutputType));
        CHECK_HR(hr = OutputType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE));
        CHECK_HR(hr = OutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video));
        CHECK_HR(hr = OutputType->SetGUID(MF_MT_SUBTYPE, VideoOutputFormat));

        CHECK_HR(hr = ::MFCreateSampleGrabberSinkActivate(OutputType.Get(), Sampler.Get(), &pRendererActivate));

        m_VideoOutputFormat = VideoOutputFormat;

        if (m_VideoOutputFormat == MFVideoFormat_YUY2)
            m_videoPF = MEVideoPixelFormat::YUY2;
        else if (m_VideoOutputFormat == MFVideoFormat_NV12)
            m_videoPF = MEVideoPixelFormat::NV12;
        else if (m_VideoOutputFormat == MFVideoFormat_RGB32)
            m_videoPF = MEVideoPixelFormat::RGB32;
        // To run as fast as possible, set this attribute (requires Windows 7):
        // CHECK_HR(hr = pRendererActivate->SetUINT32(MF_SAMPLEGRABBERSINK_IGNORE_CLOCK, TRUE));
    }
    else if (MFMediaType_Audio == guidMajorType)
    {
        // Create the audio renderer.
        AXME_TRACE("Stream {}: audio stream\n", streamID);
        CHECK_HR(hr = MFCreateAudioRendererActivate(&pRendererActivate));
    }
    else
    {
        AXME_TRACE("Stream {}: Unknown format\n", streamID);
        CHECK_HR(hr = E_FAIL);
    }

    // Set the IActivate object on the output node.
    CHECK_HR(hr = OutputNode->SetObject(pRendererActivate.Get()));
    CHECK_HR(hr = OutputNode->SetUINT32(MF_TOPONODE_STREAMID, 0));

    // Return the IMFTopologyNode pointer to the caller.
    *ppNode = OutputNode.Get();
    (*ppNode)->AddRef();

done:
    return hr;
}

HRESULT WmfMediaEngine::GetNativeVideoSize(DWORD* cx, DWORD* cy)
{
    if (!m_videoInputType || !cx || !cy)
        return E_POINTER;

    HRESULT hr   = S_OK;
    UINT32 width = 0, height = 0;

    MFVideoArea mfArea = {0};

    do
    {
        BOOL bPanScan = MFGetAttributeUINT32(m_videoInputType.Get(), MF_MT_PAN_SCAN_ENABLED, FALSE);
        if (bPanScan)
        {
            hr = m_videoInputType->GetBlob(MF_MT_PAN_SCAN_APERTURE, (UINT8*)&mfArea, sizeof(MFVideoArea), nullptr);
            AX_BREAK_IF(SUCCEEDED(hr));
        }

        hr = m_videoInputType->GetBlob(MF_MT_MINIMUM_DISPLAY_APERTURE, (UINT8*)&mfArea, sizeof(MFVideoArea), nullptr);
        AX_BREAK_IF(SUCCEEDED(hr));

        hr = m_videoInputType->GetBlob(MF_MT_GEOMETRIC_APERTURE, (UINT8*)&mfArea, sizeof(MFVideoArea), nullptr);

    } while (false);

    if (SUCCEEDED(hr))
    {
        *cx = mfArea.Area.cx;
        *cy = mfArea.Area.cy;
    }
    else  // fallback to frame extent
    {
        *cx = m_frameExtent.x;
        *cy = m_frameExtent.y;
    }
    return hr;
}

}

#endif
