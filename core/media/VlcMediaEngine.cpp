/***************************************************
* VlcMediaEngine.cpp

required codec-runtime: ubuntu-restricted-extras (contains intel-media-va-driver)
sudo apt install ubuntu-restricted-extras

*/
#if defined(AX_ENABLE_VLC_MEDIA)
#    include "VlcMediaEngine.h"

#    include "VlcMediaEngine.h"

NS_AX_BEGIN

static constexpr auto VLC_OUTPUT_FORMAT = ax::MEVideoPixelFormat::NV12;

void* VlcMediaEngine::libvlc_video_lock(void* data, void** p_pixels)
{
    VlcMediaEngine* mediaEngine = static_cast<VlcMediaEngine*>(data);

    auto& outputDim    = mediaEngine->_outputDim;
    auto& outputBuffer = mediaEngine->_frameBuffer1;
    mediaEngine->_frameBuffer1Mtx.lock();
    if constexpr (VLC_OUTPUT_FORMAT == ax::MEVideoPixelFormat::NV12)
    {
        outputBuffer.resize_fit(outputDim.x * outputDim.y +
                                (mediaEngine->_outputDim.x * mediaEngine->_outputDim.y >> 1));  // NV12
        p_pixels[0] = outputBuffer.data();
        p_pixels[1] = outputBuffer.data() + (outputDim.x * outputDim.y);
    }
    else if constexpr (VLC_OUTPUT_FORMAT == ax::MEVideoPixelFormat::YUY2)
    {
        outputBuffer.resize_fit(outputDim.x * outputDim.y +
                                ((mediaEngine->_outputDim.x >> 1) * mediaEngine->_outputDim.y * 4));  // YUY2
        p_pixels[0] = outputBuffer.data();
    }
    else
    {

        outputBuffer.resize_fit(outputDim.x * outputDim.y * 4);  // RGBA32
        p_pixels[0] = outputBuffer.data();
    }
    return nullptr;
}

void VlcMediaEngine::libvlc_video_unlock(void* data, void* id, void* const* p_pixels)
{
    VlcMediaEngine* mediaEngine = static_cast<VlcMediaEngine*>(data);

    mediaEngine->_frameBuffer1Mtx.unlock();

    ++mediaEngine->_frameIndex;

    assert(id == nullptr);
}

static void display(void* data, void* id)
{
    (void)data;
    assert(id == nullptr);
}

unsigned int VlcMediaEngine::libvlc_video_format_setup(void** opaque,
                                                       char* chroma,  // forcc, refer to:vlc_fourcc.h
                                                       unsigned* width,
                                                       unsigned* height,
                                                       unsigned* pitches,
                                                       unsigned* lines)
{
    // refer to: vmem.c:Open https://github.com/videolan/vlc/blob/3.0.18/modules/video_output/vmem.c#L150
    // future: 4.0: will be widths, heights:
    // https://github.com/videolan/vlc/blob/master/modules/video_output/vmem.c#L156
    VlcMediaEngine* mediaEngine = static_cast<VlcMediaEngine*>(*opaque);

    // plane1
    width[0]   = mediaEngine->_outputDim.x;
    height[0]  = mediaEngine->_outputDim.y;
    pitches[0] = width[0];   // bytesPerRow
    lines[0]   = height[0];  // rows

    int num_of_plane = 1;

    if constexpr (VLC_OUTPUT_FORMAT == ax::MEVideoPixelFormat::NV12)
    {
        memcpy(chroma, "NV12", 4);

        pitches[1] = mediaEngine->_outputDim.x;       // bytesPerRow
        lines[1]   = mediaEngine->_outputDim.y >> 1;  // rows

        num_of_plane = 2;
    }
    else if constexpr (VLC_OUTPUT_FORMAT == ax::MEVideoPixelFormat::YUY2)
    {
        memcpy(chroma, "YUY2", 4);
        pitches[0] = width[0] * 2;  // bytesPerRow
    }
    else
    {
        memcpy(chroma, "RGBA", 4);
        pitches[0] = width[0] * 4;  // bytesPerRow
    }

    // return the number of picture buffers allocated, 0 indicates failure
    return num_of_plane;
}

void VlcMediaEngine::libvlc_video_cleanup_cb(void* opaque) {}

void VlcMediaEngine::libvlc_handle_event(const libvlc_event_t* event, void* userData)
{
    if (!event)
        return;
    VlcMediaEngine* mediaEngine = static_cast<VlcMediaEngine*>(userData);
    switch (event->type)
    {
    case libvlc_MediaPlayerPlaying:
        mediaEngine->handleEvent(MEMediaEventType::Playing);
        break;
    case libvlc_MediaPlayerPaused:
        mediaEngine->handleEvent(MEMediaEventType::Paused);
        break;
    // case libvlc_MediaPlayerStopped:
    case libvlc_MediaListPlayerStopped:
        mediaEngine->handleEvent(MEMediaEventType::Stopped);
        break;
    case libvlc_MediaPlayerEndReached:
        // case libvlc_MediaListEndReached:
        mediaEngine->handleEvent(MEMediaEventType::Completed);
        break;
    case libvlc_MediaPlayerEncounteredError:
        mediaEngine->handleEvent(MEMediaEventType::Error);
        break;
    default:;
    }
}

VlcMediaEngine::VlcMediaEngine()
{
    _vlc = libvlc_new(0, nullptr);
    if (!_vlc)
    {
        ax::print(
            "VlcMediaEngine: libvlc_new fail, ensure the plugins folder in directory of libvlc.so,libvlccore.so!");
        return;
    }

    // media players
    _mp = libvlc_media_player_new(_vlc);
    libvlc_video_set_callbacks(_mp, libvlc_video_lock, libvlc_video_unlock, display, this);
    libvlc_video_set_format_callbacks(_mp, libvlc_video_format_setup, libvlc_video_cleanup_cb);

    // media list
    _ml = libvlc_media_list_new(_vlc);

    // media list player
    _mlp = libvlc_media_list_player_new(_vlc);
    libvlc_media_list_player_set_media_player(_mlp, _mp);
    libvlc_media_list_player_set_media_list(_mlp, _ml);

    // register media player events
    _mp_events = libvlc_media_player_event_manager(_mp);
    libvlc_event_attach(_mp_events, libvlc_MediaPlayerPlaying, libvlc_handle_event, this);
    libvlc_event_attach(_mp_events, libvlc_MediaPlayerPaused, libvlc_handle_event, this);
    libvlc_event_attach(_mp_events, libvlc_MediaPlayerStopped, libvlc_handle_event, this);
    libvlc_event_attach(_mp_events, libvlc_MediaPlayerEndReached, libvlc_handle_event, this);
    libvlc_event_attach(_mp_events, libvlc_MediaPlayerEncounteredError, libvlc_handle_event, this);

    // register media list player events
    _mpl_events = libvlc_media_list_player_event_manager(_mlp);
    libvlc_event_attach(_mpl_events, libvlc_MediaListEndReached, libvlc_handle_event, this);
    libvlc_event_attach(_mpl_events, libvlc_MediaListPlayerStopped, libvlc_handle_event, this);
}
VlcMediaEngine::~VlcMediaEngine()
{
    if (_mpl_events)
    {
        libvlc_event_detach(_mpl_events, libvlc_MediaListEndReached, libvlc_handle_event, this);
        libvlc_event_detach(_mpl_events, libvlc_MediaListPlayerStopped, libvlc_handle_event, this);
    }

    if (_mp_events)
    {
        libvlc_event_detach(_mp_events, libvlc_MediaPlayerPlaying, libvlc_handle_event, this);
        libvlc_event_detach(_mp_events, libvlc_MediaPlayerPaused, libvlc_handle_event, this);
        libvlc_event_detach(_mp_events, libvlc_MediaPlayerStopped, libvlc_handle_event, this);
        libvlc_event_detach(_mp_events, libvlc_MediaPlayerEndReached, libvlc_handle_event, this);
        libvlc_event_detach(_mp_events, libvlc_MediaPlayerEncounteredError, libvlc_handle_event, this);
    }

    if (_mlp)
    {
        libvlc_media_list_player_stop(_mlp);
        libvlc_media_list_player_release(_mlp);
    }

    if (_ml)
        libvlc_media_list_release(_ml);
    if (_mp)
        libvlc_media_player_release(_mp);
    if (_vlc)
        libvlc_release(_vlc);
}
void VlcMediaEngine::handleEvent(MEMediaEventType event)
{
    if(event == MEMediaEventType::Playing)
        updatePlaybackProperties();
    
    if (_eventCallback)
        _eventCallback(event);
}
void VlcMediaEngine::SetMediaEventCallback(MEMediaEventCallback cb1)
{
    _eventCallback = std::move(cb1);
}

void VlcMediaEngine::SetAutoPlay(bool bAutoPlay)
{
    _bAutoPlay = bAutoPlay;
}
bool VlcMediaEngine::Open(std::string_view sourceUri)
{
    libvlc_media_t* media{};

    media = libvlc_media_new_location(_vlc, sourceUri.data());
    if (!media)
        return false;

    _sourceUri = sourceUri;

    libvlc_media_list_add_media(_ml, media);  // always one media
    libvlc_media_release(media);

    if (_bAutoPlay)
        libvlc_media_list_player_play(_mlp);

    return true;
}
bool VlcMediaEngine::updatePlaybackProperties()
{
    if (_frameIndex == 0)
    {
        auto media = libvlc_media_player_get_media(_mp);
        /* local file, we Get the size of the video with the tracks information of the media. */
        libvlc_media_track_t** tracks{};
        unsigned int nbTracks = libvlc_media_tracks_get(media, &tracks);
        if (!nbTracks)
            return false;

        bool hasVideoTrack = false;
        for (unsigned int i = 0; i < nbTracks; ++i)
        {
            auto track = tracks[i];
            if (track->i_type == libvlc_track_video)
            {
                char mimeType[16] = {0};
                memcpy(mimeType, &track->i_codec, 4);
                mimeType[4] = '/';
                memcpy(mimeType + 5, &track->i_original_fourcc, 4);
                mimeType[9] = '\0';

                ax::print("VlcMediaEngine: sourceUri: %s, codec: %s", _sourceUri.c_str(), mimeType);

                auto vdi = track->video;
                _videoDim.set(vdi->i_width, vdi->i_height);
                hasVideoTrack = true;
                break;
            }
        }
        libvlc_media_tracks_release(tracks, nbTracks);
        if (hasVideoTrack)
            _outputDim.set(_videoDim.x, _videoDim.y);


       // set playback mode
       libvlc_media_list_player_set_playback_mode(_mlp,
                                               _looping ? libvlc_playback_mode_repeat : libvlc_playback_mode_default);
    }
    return true;
}
bool VlcMediaEngine::Close()
{
    if (libvlc_media_list_count(_ml) > 0)
    {
        libvlc_media_list_player_stop(_mlp);
        libvlc_media_list_remove_index(_ml, 0);
    }
    return true;
}
bool VlcMediaEngine::SetLoop(bool bLooping)
{
    _looping = bLooping;
    if (GetState() == MEMediaState::Playing)
        libvlc_media_list_player_set_playback_mode(
            _mlp, _looping ? libvlc_playback_mode_repeat : libvlc_playback_mode_default);
    return true;
}
bool VlcMediaEngine::SetRate(double fRate)
{
    return _mp && libvlc_media_player_set_rate(_mp, static_cast<float>(fRate)) == 0;
}
bool VlcMediaEngine::SetCurrentTime(double fSeekTimeInSec)
{
    if (_mp)
        libvlc_media_player_set_time(_mp, static_cast<libvlc_time_t>(fSeekTimeInSec * 1000));
    return true;
}
bool VlcMediaEngine::Play()
{
    if (_mlp)
        libvlc_media_list_player_play(_mlp);
    return true;
}
bool VlcMediaEngine::Pause()
{
    if (_mlp)
        libvlc_media_list_player_pause(_mlp);
    return true;
}
bool VlcMediaEngine::Stop()
{
    if (_mlp)
        libvlc_media_list_player_stop(_mlp);
    return true;
}
MEMediaState VlcMediaEngine::GetState() const
{
    if (_mlp)
    {
        auto underlying_state = libvlc_media_list_player_get_state(_mlp);
        switch (underlying_state)
        {
        case libvlc_NothingSpecial:
            return MEMediaState::Closed;
        case libvlc_Opening:
            return MEMediaState::Preparing;
        case libvlc_Buffering:
        case libvlc_Playing:
            return MEMediaState::Playing;
        case libvlc_Paused:
            return MEMediaState::Paused;
        case libvlc_Stopped:
            return MEMediaState::Stopped;
        case libvlc_Ended:
            return MEMediaState::Completed;
        case libvlc_Error:
            return MEMediaState::Error;
        }
    }
    return MEMediaState::Closed;
}

bool VlcMediaEngine::TransferVideoFrame(std::function<void(const MEVideoFrame&)> callback)
{
    if (_frameBuffer1.empty())
        return false;

    std::unique_lock<std::mutex> lck(_frameBuffer1Mtx);
    if (UTILS_LIKELY(!_frameBuffer1.empty()))
    {
        _frameBuffer2.swap(_frameBuffer1);
        lck.unlock();  // unlock immidiately before invoke user callback (maybe upload buffer to GPU)

        auto& buffer = _frameBuffer2;

        ax::MEVideoFrame frame{buffer.data(), buffer.data() + _outputDim.x * _outputDim.y, buffer.size(),
                               ax::MEVideoPixelDesc{VLC_OUTPUT_FORMAT, _outputDim}, _videoDim};
        // assert(static_cast<int>(frame._dataLen) >= frame._vpd._dim.x * frame._vpd._dim.y * 3 / 2);
        callback(frame);
        _frameBuffer2.clear();
        return true;
    }

    return false;
}

NS_AX_END

#endif
