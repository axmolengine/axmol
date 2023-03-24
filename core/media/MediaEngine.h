
#pragma once

#include "platform/CCPlatformMacros.h"
#include <functional>
#include <memory>
#include "yasio/detail/byte_buffer.hpp"

NS_AX_BEGIN

static constexpr std::string_view FILE_URL_SCHEME = "file://"sv;

enum class MEMediaEventType
{
    Playing = 0,
    Paused,
    Stopped,
    Completed,
    Error,
};

/**
 * Possible states of media playback.
 */
enum class MEMediaState
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

enum class MEVideoSampleFormat
{
    NONE,
    YUY2,
    NV12,
    RGB32,
    BGR32,
};

using MEMediaEventCallback = std::function<void(MEMediaEventType)>;

struct MEIntPoint
{
    MEIntPoint() : x(0), y(0) {}
    MEIntPoint(int x_, int y_) : x(x_), y(y_) {}
    int x;
    int y;

    bool equals(const MEIntPoint& rhs) const { return this->x == rhs.x && this->y == rhs.y; }
};

struct MEVideoTextueSample
{
    yasio::byte_buffer _buffer;
    MEIntPoint _bufferDim;
    MEIntPoint _videoDim;
    MEVideoSampleFormat _format = MEVideoSampleFormat::NONE;
    int _stride                 = 0;  // bytesPerRow
    int _mods                   = 0;  // whether format, videoDim changed
};

class MediaEngine
{
public:
    virtual ~MediaEngine() {}
    virtual void SetMediaEventCallback(MEMediaEventCallback cb)        = 0;
    virtual void SetAutoPlay(bool bAutoPlay)                           = 0;
    virtual bool Open(std::string_view sourceUri)                      = 0;
    virtual bool Close()                                               = 0;
    virtual bool SetLoop(bool bLooping)                                = 0;
    virtual bool SetRate(double fRate)                                 = 0;
    virtual bool SetCurrentTime(double fSeekTimeInSec)                 = 0;
    virtual bool Play()                                                = 0;
    virtual bool Pause()                                               = 0;
    virtual bool Stop()                                                = 0;
    virtual MEMediaState GetState() const                              = 0;
    virtual bool GetLastVideoSample(MEVideoTextueSample& sample) const = 0;
};

class MediaEngineFactory
{
public:
    virtual MediaEngine* CreateMediaEngine()         = 0;
    virtual void DestroyMediaEngine(MediaEngine* me) = 0;
};

std::unique_ptr<MediaEngineFactory> CreatePlatformMediaEngineFactory();

NS_AX_END
