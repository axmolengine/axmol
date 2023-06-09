// VlcMediaEngine.h
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
    static void libvlc_video_cleanup_cb(void* opaque);

    static void libvlc_handle_event(const libvlc_event_t* event, void* userData);

private:
    MEMediaEventCallback _eventCallback;

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

    uint64_t _frameIndex = 0;

    MEIntPoint _outputDim;
    MEIntPoint _videoDim;

    yasio::byte_buffer _frameBuffer1;  // for write
    yasio::byte_buffer _frameBuffer2;  // for read
    mutable std::mutex _frameBuffer1Mtx;
};

struct VlcMediaEngineFactory : public MediaEngineFactory
{
    MediaEngine* CreateMediaEngine() override { return new VlcMediaEngine(); }
    void DestroyMediaEngine(MediaEngine* me) override { delete static_cast<VlcMediaEngine*>(me); }
};

NS_AX_END

#endif
