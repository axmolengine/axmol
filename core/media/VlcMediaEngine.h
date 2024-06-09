// VlcMediaEngine.h
// Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
// https://axmol.dev/

#pragma once

#    include "MediaEngine.h"

#if defined(AX_ENABLE_VLC_MEDIA)

#include "vlc/vlc.h"

NS_AX_BEGIN

class VlcMediaEngine : public MediaEngine
{
public:
    VlcMediaEngine();
    ~VlcMediaEngine();
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

    void handleEvent(MEMediaEventType event);

    bool updatePlaybackProperties();

    static void* libvlc_video_lock(void* data, void** p_pixels);
    static void libvlc_video_unlock(void* data, void* id, void* const* p_pixels);
    static unsigned int libvlc_video_format_setup(void** opaque,
                                                  char* chroma,
                                                  unsigned* width,
                                                  unsigned* height,
                                                  unsigned* pitches,
                                                  unsigned* lines);
    static void libvlc_handle_event(const libvlc_event_t* event, void* userData);

private:
    std::function<void(MEMediaEventType)> _onMediaEvent;
    std::function<void(const MEVideoFrame&)> _onVideoFrame;

    std::string _sourceUri;

    static libvlc_instance_t* s_vlc;
    libvlc_instance_t* _vlc{};
    libvlc_media_player_t* _mp{}; // media player
    libvlc_media_list_player_t* _mlp{}; // media list player
    libvlc_media_list_t* _ml{}; // media list
    libvlc_event_manager_t* _mp_events{};
    libvlc_event_manager_t* _mpl_events{};

    bool _bAutoPlay = false;
    bool _looping   = false;
    bool _playbackEnded = false;

    std::atomic<MEMediaState> _state{MEMediaState::Closed};

    uint64_t _frameIndex = 0;

    MEIntPoint _videoDim;

    /*
    * codec dim, not for render, just record info i.e
    *   H265: ALIGN_XY(_videoDim, 32)
    *   H264: ALIGN_X(_videoDim.x, 16)
    */
    MEIntPoint _codecDim;

    std::string _videoCodecMimeType;

    yasio::byte_buffer _frameBuffer1;  // for write
    yasio::byte_buffer _frameBuffer2;  // for read
    mutable std::mutex _frameBuffer1Mtx;
};

struct VlcMediaEngineFactory : public MediaEngineFactory
{
    MediaEngine* createMediaEngine() override { return new VlcMediaEngine(); }
    void destroyMediaEngine(MediaEngine* me) override { delete static_cast<VlcMediaEngine*>(me); }
};

NS_AX_END

#endif
