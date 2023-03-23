
#pragma once

#include "platform/CCPlatformMacros.h"
#include <functional>
#include <memory>
#include "yasio/detail/byte_buffer.hpp"

NS_AX_BEGIN

enum class MediaEventType
{
    PLAYING = 0,
    PAUSED,
    STOPPED,
    COMPLETED,
};

/**
 * Possible states of media playback.
 */
enum class MediaState
{
    /** Media has been closed and cannot be played again. */
    Closed,

    Closing,

    /** Unrecoverable error occurred during playback. */
    Error,

    /** Playback has been paused, but can be resumed. */
    Paused,

    /** Media is currently playing. */
    Playing,

    /** Media is being prepared for playback. */
    Preparing,

    /** Playback has been stopped, but can be restarted. */
    Stopped,

    /** Playback has been completed, but can be restarted. */
    Completed,
};

enum class VideoSampleFormat
{
    NONE,
    YUY2,
    NV12,
    RGB32,
    BGR32,
};

using MediaEventCallback = std::function<void(MediaEventType)>;

struct VideoExtent
{
    unsigned int cx = 0;
    unsigned int cy = 0;

    bool equals(const VideoExtent& rhs) const { return this->cx == rhs.cx && this->cy == rhs.cy; }
};

class MediaEngine
{
public:
    virtual void SetMediaEventCallback(MediaEventCallback cb)           = 0;
    virtual void SetAutoPlay(bool bAutoPlay)                            = 0;
    virtual int Open(std::string_view sourceUri)                        = 0;
    virtual int Close()                                                 = 0;
    virtual int SetLoop(bool bLooping)                                  = 0;
    virtual int SetRate(double fRate)                                   = 0;
    virtual int SetCurrentTime(double fSeekTimeInSec)                   = 0;
    virtual int Play()                                                  = 0;
    virtual int Pause()                                                 = 0;
    virtual int Stop()                                                  = 0;
    virtual VideoExtent GetVideoExtent() const                          = 0;
    virtual MediaState GetState() const                                 = 0;
    virtual VideoSampleFormat GetVideoSampleFormat() const              = 0;
    virtual bool GetLastVideoFrame(yasio::byte_buffer& frameData) const = 0;
    virtual bool IsH264() const                                         = 0;
};

class MediaEngineFactory
{
public:
    virtual MediaEngine* CreateMediaEngine()         = 0;
    virtual void DestroyMediaEngine(MediaEngine* me) = 0;
};

std::unique_ptr<MediaEngineFactory> CreatePlatformMediaEngineFactory();

NS_AX_END
