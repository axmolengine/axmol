
#pragma once

#if !defined(AXME_NO_AXMOL)
#    include "base/CCConsole.h"
#    include "platform/CCPlatformMacros.h"
#    define AXME_TRACE AXLOG
#else
#    define AXME_TRACE printf
#    define NS_AX_BEGIN \
        namespace ax    \
        {
#    define NS_AX_END }
#    define AX_BREAK_IF(cond) \
        if (cond)             \
        break
#define USING_NS_AX using namespace ax
#endif

// #define AXME_USE_IMFME 1

#if __has_include(<winapifamily.h>)
#    include <winapifamily.h>
#endif

#include <functional>
#include <memory>
#include <chrono>
#include <string_view>
#include "yasio/detail/byte_buffer.hpp"

using namespace std::string_view_literals;

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

/**
 * SampleVideo: (1928x1080)
 *   - YUY2,RGB32,BGR32: works well
 *   - NV12: has green border
 *   - Y420V/F: on apple, needs test
 */

enum class MEVideoPixelFormat
{
    INVALID,
    YUY2,
    NV12,  // '420v' '420f'
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

#if defined(_DEBUG) || !defined(_NDEBUG)
struct YCbCrBiPlanarPixelInfo
{
    unsigned int YPitch = 0;
    MEIntPoint YDim;
    unsigned int CbCrPitch = 0;
    MEIntPoint CbCrDim;
};
#endif

/*
 *
 * RGB32/BGR32: _dim==_videoDim
 * H264(YUY2):
 *   LumaTexture(LA8, RG8):
 *     - _dim.x = ALIGN(_videoDim.x, 16),
 *     - _dim.y = _videoDim.y
 *   CHromaTexture(RGBA8)
 *     - chromaDim.x = _dim.x / 2
 *     - chromaDim.y = _dim.y
 * NV12/HEVC:
 *   LumaTexture(
 *     - _dim.x = ALIGN(_videoDim.x, 32)
 *     - _dim.y = ALIGN(_videoDim.y, 32)
 *   ChromaTexture(RG8)
 *     - chromaDim.x = _dim.x / 2
 *     - chromaDim.y = _dim.y / 2
 */
struct MEVideoPixelDesc
{
    MEVideoPixelDesc() : _PF(MEVideoPixelFormat::INVALID), _dim() {}
    MEVideoPixelDesc(MEVideoPixelFormat pixelFormat, const MEIntPoint& dim) : _PF(pixelFormat), _dim(dim) {}
    MEVideoPixelFormat _PF;  // the pixel format
    MEIntPoint _dim;         // the aligned frame size
    bool _fullRange = true;
    bool equals(const MEVideoPixelDesc& rhs) const
    {
        return _dim.equals(rhs._dim) && _PF == rhs._PF && _fullRange == rhs._fullRange;
    }
};

struct MEVideoFrame
{
    MEVideoFrame(const uint8_t* data,
                 const uint8_t* cbcrData,
                 size_t len,
                 const MEVideoPixelDesc& vpd,
                 const MEIntPoint& videoDim)
        : _vpd(vpd), _dataPointer(data), _cbcrDataPointer(cbcrData), _dataLen(len), _videoDim(videoDim){};
    const uint8_t* _dataPointer;  // the video data
    const size_t _dataLen;        // the video data len
    const uint8_t* _cbcrDataPointer;
    MEVideoPixelDesc _vpd;  // the video pixel desc
    MEIntPoint _videoDim;   // the aligned frame size
#if defined(_DEBUG) || !defined(_NDEBUG)
    YCbCrBiPlanarPixelInfo _ycbcrDesc{};
#endif
};

//
// redisigned corss-platform MediaEngine, inspired from microsoft media foundation: IMFMediaEngine
//
class MediaEngine
{
public:
    virtual ~MediaEngine() {}
    virtual void SetMediaEventCallback(MEMediaEventCallback cb)                        = 0;
    virtual void SetAutoPlay(bool bAutoPlay)                                           = 0;
    virtual bool Open(std::string_view sourceUri)                                      = 0;
    virtual bool Close()                                                               = 0;
    virtual bool SetLoop(bool bLooping)                                                = 0;
    virtual bool SetRate(double fRate)                                                 = 0;
    virtual bool SetCurrentTime(double fSeekTimeInSec)                                 = 0;
    virtual bool Play()                                                                = 0;
    virtual bool Pause()                                                               = 0;
    virtual bool Stop()                                                                = 0;
    virtual MEMediaState GetState() const                                              = 0;
    virtual bool TransferVideoFrame(std::function<void(const MEVideoFrame&)> callback) = 0;
};

class MediaEngineFactory
{
public:
    virtual MediaEngine* CreateMediaEngine()         = 0;
    virtual void DestroyMediaEngine(MediaEngine* me) = 0;
};

std::unique_ptr<MediaEngineFactory> CreatePlatformMediaEngineFactory();

NS_AX_END
