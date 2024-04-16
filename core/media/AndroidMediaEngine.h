
#pragma once

#if defined(__ANDROID__)
#    include "MediaEngine.h"

NS_AX_BEGIN

class AndroidMediaEngine : public MediaEngine
{
public:
    AndroidMediaEngine();
    ~AndroidMediaEngine();
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
    void setAutoPlay(bool bAutoPlay) override;
    bool open(std::string_view sourceUri) override;
    bool close() override;
    bool setLoop(bool bLooping) override;
    bool setRate(double fRate) override;
    bool setCurrentTime(double fSeekTimeInSec) override;
    bool play() override;
    bool pause() override;
    bool stop() override;
    bool isPlaybackEnded() const override;
    MEMediaState getState() const override;
    bool transferVideoFrame() override;

    void handleVideoSample(const uint8_t* buf, size_t len, int outputX, int outputY, int videoX, int videoY, int rotation);

private:
    void* context{};  // java object strong-refs
    std::function<void(MEMediaEventType)> _onMediaEvent;
    std::function<void(const MEVideoFrame&)> _onVideoFrame;

    MEIntPoint _outputDim;
    MEIntPoint _videoDim;
    int _videoRotation{0};

    yasio::byte_buffer _frameBuffer1;  // for write
    yasio::byte_buffer _frameBuffer2;  // for read
    mutable std::mutex _frameBuffer1Mtx;
};

struct AndroidMediaEngineFactory : public MediaEngineFactory
{
    MediaEngine* createMediaEngine() override { return new AndroidMediaEngine(); }
    void destroyMediaEngine(MediaEngine* me) override { delete static_cast<AndroidMediaEngine*>(me); }
};

NS_AX_END

#endif
