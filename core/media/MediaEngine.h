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

#if !defined(AXME_NO_AXMOL)
#    include "base/Logging.h"
#    include "platform/PlatformMacros.h"
#    define AXME_TRACE AXLOGD
#else
#    define AXME_TRACE printf
#    define NS_AX_BEGIN \
        namespace ax    \
        {
#    define NS_AX_END }
#    define AX_BREAK_IF(cond) \
        if (cond)             \
        break
#    define USING_NS_AX using namespace ax
#endif

// #define AXME_USE_IMFME 1

#if __has_include(<winapifamily.h>)
#    include <winapifamily.h>
#endif

#include <functional>
#include <memory>
#include <chrono>

#include "yasio/string_view.hpp"
#include "yasio/byte_buffer.hpp"

using namespace std::string_view_literals;

NS_AX_BEGIN

enum class MEMediaEventType
{
    Playing = 0,
    Paused,
    Stopped,
    Error,
};

/**
 * Possible states of media playback.
 */
enum class MEMediaState
{
    /** Media has been closed and cannot be played again. */
    Closed,

    /** Media is being prepared for playback. */
    Preparing,

    /** Media is currently playing. */
    Playing,

    /** Playback has been paused, but can be resumed. */
    Paused,

    /** Playback has been stopped, but can be restarted. */
    Stopped,

    /** Unrecoverable error occurred during playback. */
    Error,
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

struct MEIntPoint
{
    MEIntPoint() : x(0), y(0) {}
    MEIntPoint(int x_, int y_) : x(x_), y(y_) {}
    bool equals(const MEIntPoint& rhs) const { return this->x == rhs.x && this->y == rhs.y; }
    void set(int x_, int y_)
    {
        this->x = x_;
        this->y = y_;
    }

    int x;
    int y;
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
    int _rotation{0};
    bool _fullRange = true;
    bool equals(const MEVideoPixelDesc& rhs) const
    {
        return _dim.equals(rhs._dim) && _PF == rhs._PF && _fullRange == rhs._fullRange && _rotation == rhs._rotation;
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
// file uri helper: https://www.ietf.org/rfc/rfc3986.txt
//
static constexpr std::string_view LOCAL_FILE_URI_PREFIX = "file:///"sv;  // The localhost file prefix

inline std::string& path2uri(std::string& path)
{
    // windows: file:///D:/xxx/xxx.mp4
    // unix: file:///home/xxx/xxx.mp4
    // android_asset:
    //   - file:///android_asset/xxx/xxx.mp4
    //   - asset://android_asset/xxx/xxx.mp4
    if (!path.empty())
    {
        if (path[0] == '/')
            path.insert(0, LOCAL_FILE_URI_PREFIX.data(), LOCAL_FILE_URI_PREFIX.length() - 1);
        else
        {
            if (!cxx20::starts_with(path, "assets/"sv))  // not android asset
                path.insert(0, LOCAL_FILE_URI_PREFIX.data(), LOCAL_FILE_URI_PREFIX.length());
            else
                path.replace(0, "assets/"sv.length(), "file:///android_asset/");
        }
    }
    return path;
}

//
// redisigned corss-platform MediaEngine, inspired from microsoft media foundation: IMFMediaEngine
//
class MediaEngine
{
public:
    virtual ~MediaEngine() {}
    virtual void setCallbacks(std::function<void(MEMediaEventType)> onMediaEvent,
                              std::function<void(const MEVideoFrame&)> onVideoFrame) = 0;
    virtual void setAutoPlay(bool bAutoPlay)                                         = 0;
    virtual bool open(std::string_view sourceUri)                                    = 0;
    virtual bool close()                                                             = 0;
    virtual bool setLoop(bool bLooping)                                              = 0;
    virtual bool setRate(double fRate)                                               = 0;
    virtual bool setCurrentTime(double fSeekTimeInSec)                               = 0;
    virtual double getCurrentTime()                                                  = 0;
    virtual double getDuration()                                                     = 0;
    virtual bool play()                                                              = 0;
    virtual bool pause()                                                             = 0;
    virtual bool stop()                                                              = 0;
    virtual bool isPlaybackEnded() const                                             = 0;
    virtual MEMediaState getState() const                                            = 0;
    virtual bool transferVideoFrame()                                                = 0;
};

class MediaEngineFactory
{
public:
    virtual ~MediaEngineFactory() {}
    static std::unique_ptr<MediaEngineFactory> create();

    virtual MediaEngine* createMediaEngine()         = 0;
    virtual void destroyMediaEngine(MediaEngine* me) = 0;
};

NS_AX_END
