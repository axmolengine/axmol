
#pragma once

#if defined(__ANDROID__)
#    include "MediaEngine.h"

NS_AX_BEGIN

class AndroidMediaEngine : public MediaEngine
{
public:
    AndroidMediaEngine();
    ~AndroidMediaEngine();
    void SetMediaEventCallback(MEMediaEventCallback cb) override;
    void SetAutoPlay(bool bAutoPlay) override;
    bool Open(std::string_view sourceUri) override;
    bool Close() override;
    bool SetLoop(bool bLooping) override;
    bool SetRate(double fRate) override;
    bool SetCurrentTime(double fSeekTimeInSec) override;
    bool Play() override;
    bool Pause() override;
    bool Stop() override;
    MEMediaState GetState() const override;
    bool TransferVideoFrame(std::function<void(const MEVideoFrame&)> callback) override;

    void handleEvent(int event);
    void handleVideoSample(const uint8_t* buf, size_t len, int outputX, int outputY, int videoX, int videoY);

private:
    void* context{};  // java object strong-refs
    MEMediaEventCallback _eventCallback;

    MEIntPoint _outputDim;
    MEIntPoint _videoDim;

    yasio::byte_buffer _frameBuffer1;  // for write
    yasio::byte_buffer _frameBuffer2;  // for read
    mutable std::mutex _frameBuffer1Mtx;
};

struct AndroidMediaEngineFactory : public MediaEngineFactory
{
    MediaEngine* CreateMediaEngine() override { return new AndroidMediaEngine(); }
    void DestroyMediaEngine(MediaEngine* me) override { delete static_cast<AndroidMediaEngine*>(me); }
};

NS_AX_END

#endif
