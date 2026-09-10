/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#if defined(__ANDROID__)
#    include "axmol/media/MediaEngine.h"

#    include <jni.h>

namespace ax
{

class AndroidMediaEngine : public MediaEngine
{
public:
    AndroidMediaEngine();
    ~AndroidMediaEngine();
    void setCallbacks(std::function<void(MEMediaEventType)> onMediaEvent,
                      std::function<void(const MEVideoFrame&)> onVideoFrame) override
    {
        _onMediaEvent = std::move(onMediaEvent);
        _onVideoFrame = std::move(onVideoFrame);
    }
    void setAutoPlay(bool bAutoPlay) override;
    bool open(std::string_view sourceUri) override;
    bool close() override;
    bool setLoop(bool bLooping) override;
    bool setRate(double fRate) override;
    bool setCurrentTime(double fSeekTimeInSec) override;
    double getCurrentTime() override { return _currentTime; }
    double getDuration() override { return _duration; }
    bool play() override;
    bool pause() override;
    bool stop() override;
    bool isPlaybackEnded() const override;
    MEMediaState getState() const override;
    bool transferVideoFrame() override;

    // internal stubs
    void _fireMediaEvent(MEMediaEventType event)
    {
        if (_onMediaEvent)
            _onMediaEvent(event);
    }
    void _setDuration(double duration) { _duration = duration; }
    void _setVideoMeta(int outputX, int outputY, int videoX, int videoY, int cbcrOffset, int rotation, int videoPF);

    void _processVideoFrame(const uint8_t* sampleData, size_t sampleLen, int64_t presentationTimeUs);

private:
    jobject _mediaPlayer{nullptr};  // java object strong-refs
    std::function<void(MEMediaEventType)> _onMediaEvent;
    std::function<void(const MEVideoFrame&)> _onVideoFrame;

    MEIntPoint _outputDim;
    MEIntPoint _videoDim;
    int _cbcrOffset{0};
    int _videoRotation{0};
    int _videoPF{-1};

    tlx::byte_buffer _frameBuffer1;  // for write
    tlx::byte_buffer _frameBuffer2;  // for read
    mutable std::mutex _frameBuffer1Mtx;

    double _currentTime{0.0};  // current time in seconds
    double _duration{0.0};     // duration in seconds
};

struct AndroidMediaEngineFactory : public MediaEngineFactory
{
    MediaEngine* createMediaEngine() override { return new AndroidMediaEngine(); }
    void destroyMediaEngine(MediaEngine* me) override { delete static_cast<AndroidMediaEngine*>(me); }
};

}  // namespace ax

#endif
