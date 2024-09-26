/***************************************************
* VlcMediaEngine.cpp

required codec-runtime: ubuntu-restricted-extras (contains intel-media-va-driver)
sudo apt install ubuntu-restricted-extras

*/
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

#if defined(AX_ENABLE_VLC_MEDIA)
#    include "VlcMediaEngine.h"

#    include "VlcMediaEngine.h"

namespace ax
{

static constexpr auto VLC_OUTPUT_FORMAT = ax::MEVideoPixelFormat::NV12;

void* VlcMediaEngine::libvlc_video_lock(void* data, void** p_pixels)
{
    VlcMediaEngine* mediaEngine = static_cast<VlcMediaEngine*>(data);

    auto& bufferDim    = mediaEngine->_videoDim;
    auto& outputBuffer = mediaEngine->_frameBuffer1;
    mediaEngine->_frameBuffer1Mtx.lock();
    if constexpr (VLC_OUTPUT_FORMAT == ax::MEVideoPixelFormat::NV12)
    {
        outputBuffer.resize(bufferDim.x * bufferDim.y + (bufferDim.x * bufferDim.y >> 1));  // NV12
        p_pixels[0] = outputBuffer.data();
        p_pixels[1] = outputBuffer.data() + (bufferDim.x * bufferDim.y);
    }
    else if constexpr (VLC_OUTPUT_FORMAT == ax::MEVideoPixelFormat::YUY2)
    {
        outputBuffer.resize(bufferDim.x * bufferDim.y + ((bufferDim.x >> 1) * bufferDim.y * 4));  // YUY2
        p_pixels[0] = outputBuffer.data();
    }
    else
    {
        outputBuffer.resize(bufferDim.x * bufferDim.y * 4);  // RGBA32
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

    // vlc tell us the original codecDim(ALIGNED)
    mediaEngine->_codecDim.set(width[0], height[0]);

    // tell vlc we want render as video size
    width[0]  = mediaEngine->_videoDim.x;
    height[0] = mediaEngine->_videoDim.y;

    // plane0
    pitches[0] = width[0];   // bytesPerRow
    lines[0]   = height[0];  // rows

#    if LIBVLC_VERSION_MAJOR >= 4
    mediaEngine->_videoDim.set(width[1], height[1]);
#    endif

    int num_of_plane = 1;

    if constexpr (VLC_OUTPUT_FORMAT == ax::MEVideoPixelFormat::NV12)
    {
        memcpy(chroma, "NV12", 4);

        // plane1
        pitches[1] = mediaEngine->_videoDim.x;       // bytesPerRow
        lines[1]   = mediaEngine->_videoDim.y >> 1;  // rows

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

void VlcMediaEngine::libvlc_handle_event(const libvlc_event_t* event, void* userData)
{
    if (!event)
        return;
    VlcMediaEngine* mediaEngine = static_cast<VlcMediaEngine*>(userData);
    switch (event->type)
    {
    case libvlc_MediaPlayerPlaying:
        mediaEngine->_playbackEnded = false;
        mediaEngine->_state         = MEMediaState::Playing;
        mediaEngine->handleEvent(MEMediaEventType::Playing);
        break;
    case libvlc_MediaPlayerPaused:
        mediaEngine->_state = MEMediaState::Paused;
        mediaEngine->handleEvent(MEMediaEventType::Paused);
        break;
#    if LIBVLC_VERSION_MAJOR < 4
    case libvlc_MediaPlayerEndReached:
        mediaEngine->_playbackEnded = true;
        mediaEngine->_state         = MEMediaState::Stopped;
        mediaEngine->handleEvent(MEMediaEventType::Stopped);
        break;
    case libvlc_MediaPlayerStopped:
        if (!mediaEngine->_playbackEnded)
        {
            mediaEngine->_state = MEMediaState::Stopped;
            mediaEngine->handleEvent(MEMediaEventType::Stopped);
        }
        break;
#    else
    case libvlc_MediaPlayerStopped:
        mediaEngine->_playbackEnded = true;
        mediaEngine->_state         = MEMediaState::Stopped;
        mediaEngine->handleEvent(MEMediaEventType::Stopped);
        break;
    case libvlc_MediaListPlayerStopped:
        if (!mediaEngine->_playbackEnded)
        {
            mediaEngine->_state = MEMediaState::Stopped;
            mediaEngine->handleEvent(MEMediaEventType::Stopped);
        }
        break;
#    endif
    case libvlc_MediaPlayerEncounteredError:
        mediaEngine->_state = MEMediaState::Error;
        mediaEngine->handleEvent(MEMediaEventType::Error);
        break;
    case libvlc_MediaPlayerMediaChanged:
        mediaEngine->_frameIndex = 0;
        break;
    default:;
    }
}

VlcMediaEngine::VlcMediaEngine()
{
    AXLOGD("libvlc-version: {}", libvlc_get_version());

    // too late set vlc plugins path at hete, vlc maybe read it at DllMain
    //_putenv_s("VLC_PLUGIN_PATH", R"(D:\dev\axmol\3rdparty\vlc\win\lib)");
    _vlc = libvlc_new(0, nullptr);
    if (!_vlc)
    {
        AXLOGI("VlcMediaEngine: libvlc_new fail, ensure install vlc and ubuntu-restricted-extras");
        return;
    }

    // media player
    _mp = libvlc_media_player_new(_vlc);
    libvlc_video_set_callbacks(_mp, libvlc_video_lock, libvlc_video_unlock, nullptr, this);
    libvlc_video_set_format_callbacks(_mp, libvlc_video_format_setup, nullptr);

    // media list
#    if LIBVLC_VERSION_MAJOR < 4
    _ml = libvlc_media_list_new(_vlc);
#    else
    _ml = libvlc_media_list_new();
#    endif

    // media list player
    _mlp = libvlc_media_list_player_new(_vlc);
    libvlc_media_list_player_set_media_player(_mlp, _mp);
    libvlc_media_list_player_set_media_list(_mlp, _ml);

    // register media player events
    _mp_events = libvlc_media_player_event_manager(_mp);
    libvlc_event_attach(_mp_events, libvlc_MediaPlayerPlaying, libvlc_handle_event, this);
    libvlc_event_attach(_mp_events, libvlc_MediaPlayerPaused, libvlc_handle_event, this);
    libvlc_event_attach(_mp_events, libvlc_MediaPlayerStopped, libvlc_handle_event, this);
    libvlc_event_attach(_mp_events, libvlc_MediaPlayerMediaChanged, libvlc_handle_event, this);
#    if LIBVLC_VERSION_MAJOR < 4
    libvlc_event_attach(_mp_events, libvlc_MediaPlayerEndReached, libvlc_handle_event, this);
#    endif
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
        libvlc_event_detach(_mp_events, libvlc_MediaPlayerMediaChanged, libvlc_handle_event, this);
#    if LIBVLC_VERSION_MAJOR < 4
        libvlc_event_detach(_mp_events, libvlc_MediaPlayerEndReached, libvlc_handle_event, this);
#    endif
        libvlc_event_detach(_mp_events, libvlc_MediaPlayerEncounteredError, libvlc_handle_event, this);
    }

    if (_mlp)
    {
#    if LIBVLC_VERSION_MAJOR < 4
        libvlc_media_list_player_stop(_mlp);
#    else
        libvlc_media_list_player_stop_async(_mlp);
#    endif
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
    if (event == MEMediaEventType::Playing)
        updatePlaybackProperties();

    fireMediaEvent(event);
}

void VlcMediaEngine::setAutoPlay(bool bAutoPlay)
{
    _bAutoPlay = bAutoPlay;
}
bool VlcMediaEngine::open(std::string_view sourceUri)
{
    if (_state != MEMediaState::Closed)
        return false;

#    if LIBVLC_VERSION_MAJOR < 4
    libvlc_media_t* media = libvlc_media_new_location(_vlc, sourceUri.data());
#    else
    libvlc_media_t* media = libvlc_media_new_location(sourceUri.data());
#    endif
    if (!media)
        return false;

    _frameIndex = 0;
    _sourceUri  = sourceUri;

    libvlc_media_list_add_media(_ml, media);  // always one media
    libvlc_media_release(media);

    if (_bAutoPlay)
    {
        _state = MEMediaState::Preparing;
        libvlc_media_list_player_play(_mlp);
    }
    else
        _state = MEMediaState::Stopped;

    return true;
}

static void track_codec_to_mime_type(libvlc_media_track_t* track, std::string& out)

{
    out.clear();
    out.reserve(9);  // 4 * 2 + 1
    out.append(reinterpret_cast<const char*>(&track->i_codec), 4);
    out.push_back('/');
    out.append(reinterpret_cast<const char*>(&track->i_original_fourcc), 4);
}

bool VlcMediaEngine::updatePlaybackProperties()
{
    if (_frameIndex == 0)
    {
        auto media = libvlc_media_player_get_media(_mp);
#    if LIBVLC_VERSION_MAJOR < 4
        /* local file, we Get the size of the video with the tracks information of the media. */
        libvlc_media_track_t** tracks{};
        unsigned int nbTracks = libvlc_media_tracks_get(media, &tracks);
        if (!nbTracks)
            return false;

        for (unsigned int i = 0; i < nbTracks; ++i)
        {
            auto track = tracks[i];
            if (track->i_type == libvlc_track_video)
            {
                track_codec_to_mime_type(track, _videoCodecMimeType);
                AXLOGD("VlcMediaEngine: sourceUri: {}, codec: {}", _sourceUri, _videoCodecMimeType);

                auto vdi = track->video;
                auto vdw = vdi->i_width;
                auto vdh = vdi->i_height;
                switch (vdi->i_orientation)
                {
                case libvlc_video_orient_left_bottom: /**< Rotated 90 degrees clockwise (or 270 anti-clockwise) */
                case libvlc_video_orient_right_top:   /**< Rotated 90 degrees anti-clockwise */
                    std::swap(vdw, vdh);
                default:;
                }
                _videoDim.set(vdw, vdh);
                break;
            }
        }
        libvlc_media_tracks_release(tracks, nbTracks);
#    else
        auto tracks = libvlc_media_get_tracklist(media, libvlc_track_type_t::libvlc_track_video);
        if (tracks)
        {
            auto nTrack = libvlc_media_tracklist_count(tracks);
            if (nTrack > 0)
            {
                auto track = libvlc_media_tracklist_at(tracks, 0);
                track_codec_to_mime_type(track, _videoCodecMimeType);
                AXLOGI("VlcMediaEngine: sourceUri: {}, codec: {}", _sourceUri, _videoCodecMimeType);

                auto vdi = track->video;
                _videoDim.set(vdi->i_width, vdi->i_height);
            }
            libvlc_media_tracklist_delete(tracks);
        }
#    endif

        // set playback mode
        libvlc_media_list_player_set_playback_mode(
            _mlp, _looping ? libvlc_playback_mode_repeat : libvlc_playback_mode_default);
    }
    return true;
}
bool VlcMediaEngine::close()
{
    if (libvlc_media_list_count(_ml) > 0)
    {
#    if LIBVLC_VERSION_MAJOR < 4
        libvlc_media_list_player_stop(_mlp);
#    else
        libvlc_media_list_player_stop_async(_mlp);
#    endif
        libvlc_media_list_remove_index(_ml, 0);
    }
    _state = MEMediaState::Closed;
    return true;
}
bool VlcMediaEngine::setLoop(bool bLooping)
{
    _looping = bLooping;
    if (getState() == MEMediaState::Playing)
        libvlc_media_list_player_set_playback_mode(
            _mlp, _looping ? libvlc_playback_mode_repeat : libvlc_playback_mode_default);
    return true;
}
bool VlcMediaEngine::setRate(double fRate)
{
    return _mp && libvlc_media_player_set_rate(_mp, static_cast<float>(fRate)) == 0;
}
bool VlcMediaEngine::setCurrentTime(double fSeekTimeInSec)
{
    if (_mp)
#    if LIBVLC_VERSION_MAJOR < 4
        libvlc_media_player_set_time(_mp, static_cast<libvlc_time_t>(fSeekTimeInSec * 1000));
#    else
        libvlc_media_player_set_time(_mp, static_cast<libvlc_time_t>(fSeekTimeInSec * 1000), true);
#    endif
    return true;
}

double VlcMediaEngine::getCurrentTime()
{
    return libvlc_media_player_get_time(_mp) / 1000.0;
}

double VlcMediaEngine::getDuration()
{
    return libvlc_media_player_get_length(_mp) / 1000.0;
}

bool VlcMediaEngine::play()
{
    if (_mlp && _state != MEMediaState::Closed)
        libvlc_media_list_player_play(_mlp);
    return true;
}
bool VlcMediaEngine::pause()
{
    if (_mlp && _state != MEMediaState::Closed)
        libvlc_media_list_player_pause(_mlp);
    return true;
}
bool VlcMediaEngine::stop()
{
    if (_mlp && _state != MEMediaState::Closed)
#    if LIBVLC_VERSION_MAJOR < 4
        libvlc_media_list_player_stop(_mlp);
#    else
        libvlc_media_list_player_stop_async(_mlp);
#    endif
    return true;
}
MEMediaState VlcMediaEngine::getState() const
{
    return _state;
}

bool VlcMediaEngine::transferVideoFrame()
{
    if (_frameBuffer1.empty())
        return false;

    std::unique_lock<std::mutex> lck(_frameBuffer1Mtx);
    if (AX_LIKELY(!_frameBuffer1.empty()))
    {
        _frameBuffer2.swap(_frameBuffer1);
        lck.unlock();  // unlock immidiately before invoke user callback (maybe upload buffer to GPU)

        auto& buffer = _frameBuffer2;

        ax::MEVideoFrame frame{buffer.data(), buffer.data() + _videoDim.x * _videoDim.y, buffer.size(),
                               ax::MEVideoPixelDesc{VLC_OUTPUT_FORMAT, _videoDim}, _videoDim};
        // assert(static_cast<int>(frame._dataLen) >= frame._vpd._dim.x * frame._vpd._dim.y * 3 / 2);
        _onVideoFrame(frame);
        _frameBuffer2.clear();
        return true;
    }

    return false;
}

}

#endif
