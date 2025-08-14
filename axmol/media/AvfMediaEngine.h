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

#include "MediaEngine.h"

#if defined(__APPLE__)

#    import <AVFoundation/AVFoundation.h>

@class AVMediaSessionHandler;

namespace ax
{

class AvfMediaEngine : public MediaEngine
{
public:
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
    double getCurrentTime() override;
    double getDuration() override;
    bool play() override;
    bool pause() override;
    bool stop() override;
    bool isPlaybackEnded() const override { return _playbackEnded; }
    MEMediaState getState() const override;
    bool transferVideoFrame() override;

    void onStatusNotification(void* context);
    void onPlayerEnd();
    bool isPlaying() const {
        return _state == MEMediaState::Playing;
    }

    void internalPlay(bool replay = false);
    void internalPause();

private:
    std::function<void(MEMediaEventType)> _onMediaEvent;
    std::function<void(const MEVideoFrame&)> _onVideoFrame;
    MEVideoPixelFormat _videoPF = MEVideoPixelFormat::INVALID;
    MEMediaState _state = MEMediaState::Closed;
    int _videoRotation{0};
    MEIntPoint _videoExtent;
    AVPlayer* _player = nil;
    AVPlayerItem* _playerItem = nil;
    AVPlayerItemOutput* _playerOutput = nil;
    AVMediaSessionHandler* _sessionHandler = nil;

    bool _bAutoPlay = false;
    bool _repeatEnabled = false;
    bool _playbackEnded = false;

    /*
    true: luma=[0,255] chroma=[1,255]
    false: luma=[16,235] chroma=[16,240]
    */
    bool _bFullColorRange = false;
};

struct AvfMediaEngineFactory : public MediaEngineFactory
{
    MediaEngine* createMediaEngine() override
    {
        return new AvfMediaEngine();
    }
    void destroyMediaEngine(MediaEngine* me) override { delete static_cast<AvfMediaEngine*>(me); }
};

}

#endif
