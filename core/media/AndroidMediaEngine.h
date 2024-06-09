/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
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
    double getCurrentTime() override { return _currentTime; }
    double getDuration() override { return _duration; }
    bool play() override;
    bool pause() override;
    bool stop() override;
    bool isPlaybackEnded() const override;
    MEMediaState getState() const override;
    bool transferVideoFrame() override;

    void handleVideoSample(const uint8_t* buf, size_t len, int outputX, int outputY, int videoX, int videoY, int rotation);
    void updateCurrentTime(double currentTime) { _currentTime = currentTime; }
    void updateDuration(double duration) { _duration = duration; }

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

    double _currentTime{};
    double _duration{};
};

struct AndroidMediaEngineFactory : public MediaEngineFactory
{
    MediaEngine* createMediaEngine() override { return new AndroidMediaEngine(); }
    void destroyMediaEngine(MediaEngine* me) override { delete static_cast<AndroidMediaEngine*>(me); }
};

NS_AX_END

#endif
