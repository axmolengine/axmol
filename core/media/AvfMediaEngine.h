#include "MediaEngine.h"

#if defined(__APPLE__)

#    import <AVFoundation/AVFoundation.h>

@class AVMediaSessionHandler;

NS_AX_BEGIN

class AvfMediaEngine : public MediaEngine
{
public:
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
    
    void onStatusNotification(void* context);
    void onPlayerEnd();
    void FireEvent(MEMediaEventType event)
    {
        if (_eventCallback)
            _eventCallback(event);
    }
    bool isPlaying() const {
        return _state == MEMediaState::Playing;
    }
    
    void internalPlay(bool replay = false);
    void internalPause();
    
private:
    MEMediaEventCallback _eventCallback;
    MEVideoPixelFormat _videoPF = MEVideoPixelFormat::INVALID;
    MEMediaState _state = MEMediaState::Closed;
    MEIntPoint _videoExtent;
    AVPlayer* _player = nil;
    AVPlayerItem* _playerItem = nil;
    AVPlayerItemOutput* _playerOutput = nil;
    AVMediaSessionHandler* _sessionHandler = nil;

    bool _bAutoPlay = false;
    bool _repeatEnabled = false;

    /* 
    true: luma=[0,255] chroma=[1,255]
    false: luma=[16,235] chroma=[16,240]
    */
    bool _bFullColorRange = false; 
};

struct AvfMediaEngineFactory : public MediaEngineFactory
{
    MediaEngine* CreateMediaEngine() override
    {
        return new AvfMediaEngine();
    }
    void DestroyMediaEngine(MediaEngine* me) override { delete static_cast<AvfMediaEngine*>(me); }
};

NS_AX_END

#endif
