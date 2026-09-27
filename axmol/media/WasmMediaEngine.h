/****************************************************************************
 Copyright (c) 2026 Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#if defined(__EMSCRIPTEN__)

#    include "axmol/media/MediaEngine.h"

namespace ax
{

class WasmMediaEngine final : public MediaEngine
{
public:
    WasmMediaEngine();
    ~WasmMediaEngine() override;

    void setCallbacks(std::function<void(MEMediaEventType)> onMediaEvent,
                      std::function<void(const MEVideoFrame&)> onVideoFrame) override;
    void setAutoPlay(bool autoPlay) override;
    bool open(std::string_view sourceUri) override;
    bool close() override;
    bool setLoop(bool looping) override;
    bool setRate(double rate) override;
    bool setVolume(double volume) override;
    double getVolume() const override;
    bool setCurrentTime(double time) override;
    double getCurrentTime() override;
    double getDuration() override;
    bool play() override;
    bool pause() override;
    bool stop() override;
    bool isPlaybackEnded() const override;
    MEMediaState getState() const override;
    bool transferVideoFrame() override;
    void dispatchEvent(MEMediaEventType event);

private:
    int _id = 0;
    std::function<void(MEMediaEventType)> _onMediaEvent;
    std::function<void(const MEVideoFrame&)> _onVideoFrame;
    tlx::byte_buffer _frameBuffer;
    bool _autoPlay = false;
    bool _loop     = false;
    double _rate   = 1.0;
    double _volume = 1.0;
};

struct WasmMediaEngineFactory final : MediaEngineFactory
{
    MediaEngine* createMediaEngine() override { return new WasmMediaEngine(); }
    void destroyMediaEngine(MediaEngine* engine) override { delete static_cast<WasmMediaEngine*>(engine); }
};

}  // namespace ax

#endif
