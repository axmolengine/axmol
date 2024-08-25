//--------------------------------------------------------------------------------------
// File: MfMediaEngine.h
//
// Modified from https://github.com/microsoft/Xbox-ATG-Samples/tree/main/UWPSamples/Graphics/VideoTextureUWP
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
// Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
//
// https://axmol.dev/
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

namespace ax
{

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

    void setAutoPlay(bool bAutoPlay) { m_autoPlay = bAutoPlay; }

    bool open(std::string_view sourceUri) override;
    bool close() override;

    bool setLoop(bool bLoop) override;
    bool setRate(double fRate) override;

    bool transferVideoFrame() override;

    bool play() override;
    bool pause() override;
    bool stop() override;
    bool isPlaybackEnded() const override { return m_bPlaybackEnded; }

    bool setCurrentTime(double fPosInSeconds) override;
    double getCurrentTime() override;
    double getDuration() override;

    MEMediaState getState() const override { return m_state; }

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
    bool m_bPlaybackEnded = false;

    bool m_autoPlay = false;
    MEMediaState m_state = MEMediaState::Closed;

    MEIntPoint m_videoExtent;

    std::function<void(MEMediaEventType)> _onMediaEvent;
    std::function<void(const MEVideoFrame&)> _onVideoFrame;
};

struct MfMediaEngineFactory : public MediaEngineFactory {
    MediaEngine* createMediaEngine() override
    {
        auto engine = new MfMediaEngine();
        engine->Initialize();
        return engine;
    }
    void destroyMediaEngine(MediaEngine* me) override
    {
        delete static_cast<MfMediaEngine*>(me);
    }
};

}

#endif
#endif
