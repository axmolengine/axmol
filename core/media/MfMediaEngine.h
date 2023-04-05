//--------------------------------------------------------------------------------------
// File: MfMediaEngine.h
//
// Modified from https://github.com/microsoft/Xbox-ATG-Samples/tree/main/UWPSamples/Graphics/VideoTextureUWP
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//-------------------------------------------------------------------------------------

#pragma once

#if defined(_WIN32)
#include <winapifamily.h>
#include "media/MediaEngine.h"

#    if defined(WINAPI_FAMILY) && (WINAPI_FAMILY != WINAPI_FAMILY_DESKTOP_APP || defined(AXME_USE_IMFME))

#include <stdint.h>
#include <mfapi.h>

#include <mfmediaengine.h>
#include <wincodec.h>
#include <wrl/client.h>

#include "media/MFUtils.h"

NS_AX_BEGIN

//-------------------------------------------------------------------------------------
class IMFNotify
{
public:
    virtual ~IMFNotify() = default;

    IMFNotify(const IMFNotify&)            = delete;
    IMFNotify& operator=(const IMFNotify&) = delete;

    IMFNotify(IMFNotify&&)            = delete;
    IMFNotify& operator=(IMFNotify&&) = delete;

    virtual void OnMediaEngineEvent(uint32_t meEvent) = 0;

protected:
    IMFNotify() = default;
};

//-------------------------------------------------------------------------------------

class MfMediaEngine : public IMFNotify, public MediaEngine
{
public:
    MfMediaEngine() noexcept;
    ~MfMediaEngine();
    
    MfMediaEngine(const MfMediaEngine&)            = delete;
    MfMediaEngine& operator=(const MfMediaEngine&) = delete;

    MfMediaEngine(MfMediaEngine&&)            = default;
    MfMediaEngine& operator=(MfMediaEngine&&) = default;

    bool Initialize();

    void FireMediaEvent(MEMediaEventType event)
    {
        if (m_eventCallback)
            m_eventCallback(event);
    }

    void SetMediaEventCallback(MEMediaEventCallback cb) override { m_eventCallback = cb; }

    void SetAutoPlay(bool bAutoPlay) { m_autoPlay = bAutoPlay; }

    bool Open(std::string_view sourceUri) override;
    bool Close() override;
 
    bool SetLoop(bool bLoop) override;
    bool SetRate(double fRate) override;

    bool TransferVideoFrame(std::function<void(const MEVideoFrame&)> callback) override;

    bool Play() override;
    bool Pause() override;
    bool Stop() override;

    bool SetCurrentTime(double fPosInSeconds) override;

    MEMediaState GetState() const override { return m_state; }

    void SetMuted(bool muted);

    // Callbacks
    void OnMediaEngineEvent(uint32_t meEvent) override;

    // Properties
    void UpdateVideoExtent();

private:
    Microsoft::WRL::ComPtr<IMFMediaEngine> m_mediaEngine;

    Microsoft::WRL::ComPtr<IWICImagingFactory> m_wicFactory;
    Microsoft::WRL::ComPtr<IWICBitmap> m_wicBitmap;

    MFARGB m_bkgColor;

    bool m_readyToPlay = false;
    bool m_stopping = false;

    bool m_autoPlay = false;
    MEMediaState m_state = MEMediaState::Closed;

    MEIntPoint m_videoExtent;

    MEMediaEventCallback m_eventCallback;
};

struct MfMediaEngineFactory : public MediaEngineFactory {
    MediaEngine* CreateMediaEngine() override
    {
        auto engine = new MfMediaEngine();
        engine->Initialize();
        return engine;
    }
    void DestroyMediaEngine(MediaEngine* me) override 
    {
        delete static_cast<MfMediaEngine*>(me);
    }
};

NS_AX_END

#endif
#endif
