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

#include "AvfMediaEngine.h"

#if defined(__APPLE__)

#    include <TargetConditionals.h>

#    include <assert.h>
#    include <condition_variable>
#    include <cstdint>
#    include <mutex>
#    include <utility>
#    include "yasio/string_view.hpp"
#    include "yasio/endian_portable.hpp"

#if TARGET_OS_IPHONE
#    import <UIKit/UIKit.h>
#endif

using namespace ax;

#define AX_ALIGN_ANY(x, a) ((((x) + (a) - 1) / (a)) * (a))

namespace ax
{
struct AvfMediaCallbackState
{
    std::mutex mutex;
    std::condition_variable callbackFinished;
    AvfMediaEngine* engine = nullptr;
    void* currentPlayerItem = nullptr;
    uint64_t generation = 0;
    size_t activeCallbacks = 0;
};
}  // namespace ax

namespace
{
class AvfMediaCallbackGuard;
thread_local AvfMediaCallbackGuard* g_currentCallbackGuard = nullptr;

class AvfMediaCallbackGuard
{
public:
    explicit AvfMediaCallbackGuard(const std::shared_ptr<ax::AvfMediaCallbackState>& state,
                                   uint64_t generation = 0)
        : _state(state)
    {
        std::lock_guard<std::mutex> lock(_state->mutex);
        if (_state->engine != nullptr && (generation == 0 || _state->generation == generation))
        {
            _engine = _state->engine;
            ++_state->activeCallbacks;
            _previousGuard = g_currentCallbackGuard;
            g_currentCallbackGuard = this;
        }
    }

    ~AvfMediaCallbackGuard()
    {
        if (_engine == nullptr)
            return;
        {
            std::lock_guard<std::mutex> lock(_state->mutex);
            --_state->activeCallbacks;
            _state->callbackFinished.notify_all();
        }
        g_currentCallbackGuard = _previousGuard;
    }

    AvfMediaCallbackGuard(const AvfMediaCallbackGuard&) = delete;
    AvfMediaCallbackGuard& operator=(const AvfMediaCallbackGuard&) = delete;

    explicit operator bool() const { return _engine != nullptr; }
    ax::AvfMediaEngine* get() const { return _engine; }

    static size_t currentThreadGuardCount(const ax::AvfMediaCallbackState* state)
    {
        size_t count = 0;
        for (auto* guard = g_currentCallbackGuard; guard != nullptr; guard = guard->_previousGuard)
            if (guard->_state.get() == state)
                ++count;
        return count;
    }

private:
    std::shared_ptr<ax::AvfMediaCallbackState> _state;
    ax::AvfMediaEngine* _engine = nullptr;
    AvfMediaCallbackGuard* _previousGuard = nullptr;
};
}  // namespace

@interface AVMediaSessionHandler : NSObject
- (AVMediaSessionHandler*)initWithCallbackState:(std::shared_ptr<ax::AvfMediaCallbackState>)callbackState;
- (void)detachMediaEngine;
- (void)registerUINotifications;
- (void)deregisterUINotifications;
- (void)playerItemDidPlayToEndTime:(NSNotification*)notification;
@end

@implementation AVMediaSessionHandler {
    std::shared_ptr<ax::AvfMediaCallbackState> _callbackState;
}

- (AVMediaSessionHandler*)initWithCallbackState:(std::shared_ptr<ax::AvfMediaCallbackState>)callbackState
{
    self = [super init];
    if (self)
        _callbackState = std::move(callbackState);
    return self;
}

- (void)detachMediaEngine
{
    [self deregisterUINotifications];
}

- (void)registerUINotifications
{
#if TARGET_OS_IPHONE
    auto nc = [NSNotificationCenter defaultCenter];

    [nc addObserver:self
           selector:@selector(handleAudioRouteChange:)
               name:AVAudioSessionRouteChangeNotification
             object:[AVAudioSession sharedInstance]];
    [nc addObserver:self
           selector:@selector(handleActive:)
               name:UIApplicationDidBecomeActiveNotification
             object:nil];
    [nc addObserver:self
           selector:@selector(handleDeactive:)
               name:UIApplicationWillResignActiveNotification
             object:nil];
    [nc addObserver:self
           selector:@selector(handleEnterBackround:)
               name:UIApplicationDidEnterBackgroundNotification
             object:nil];
    [nc addObserver:self
           selector:@selector(handleEnterForground:)
               name:UIApplicationWillEnterForegroundNotification
             object:nil];
#endif
}

#if TARGET_OS_IPHONE
- (void)handleAudioRouteChange:(NSNotification*)notification
{
    AvfMediaCallbackGuard callback(_callbackState);
    auto me = callback.get();
    if (!callback)
        return;
    if (me->isPlaying())
        me->internalPlay(true);
}

- (void)handleActive:(NSNotification*)notification
{
    AvfMediaCallbackGuard callback(_callbackState);
    auto me = callback.get();
    if (!callback)
        return;
    if (me->isPlaying())
        me->internalPlay();
}

- (void)handleDeactive:(NSNotification*)notification
{
    AvfMediaCallbackGuard callback(_callbackState);
    auto me = callback.get();
    if (!callback)
        return;
    if (me->isPlaying())
        me->internalPause();
}

- (void)handleEnterForground:(NSNotification*)notification
{
    AvfMediaCallbackGuard callback(_callbackState);
    auto me = callback.get();
    if (!callback)
        return;
    if (me->isPlaying())
        me->internalPlay();
}

- (void)handleEnterBackround:(NSNotification*)notification
{
    AvfMediaCallbackGuard callback(_callbackState);
    auto me = callback.get();
    if (!callback)
        return;
    if (me->isPlaying())
        me->internalPause();
}
#endif

- (void)deregisterUINotifications
{
#if TARGET_OS_IPHONE
    auto nc = [NSNotificationCenter defaultCenter];
    [nc removeObserver:self
               name:AVAudioSessionRouteChangeNotification
             object:nil];
    [nc removeObserver:self
                  name:UIApplicationDidBecomeActiveNotification
                object:nil];
    [nc removeObserver:self
                  name:UIApplicationWillResignActiveNotification
                object:nil];
    [nc removeObserver:self
                  name:UIApplicationDidEnterBackgroundNotification
                object:nil];
    [nc removeObserver:self
                  name:UIApplicationWillEnterForegroundNotification
                object:nil];
#endif
}

- (void)playerItemDidPlayToEndTime:(NSNotification*)notification
{
    AvfMediaCallbackGuard callback(_callbackState);
    auto me = callback.get();
    if (!callback)
        return;
    {
        std::lock_guard<std::mutex> lock(_callbackState->mutex);
        if (_callbackState->currentPlayerItem != (__bridge void*)notification.object)
            return;
    }
    me->onPlayerEnd();
}

- (void)observeValueForKeyPath:(NSString*)keyPath
                      ofObject:(id)object
                        change:(NSDictionary<NSKeyValueChangeKey, id>*)change
                       context:(void*)context
{
    AvfMediaCallbackGuard callback(_callbackState);
    auto me = callback.get();
    if (!callback)
        return;
    if ((__bridge id)context == object && [keyPath isEqualToString:@"status"])
        me->onStatusNotification(context);
}

@end

namespace ax
{

AvfMediaEngine::AvfMediaEngine() : _callbackState(std::make_shared<AvfMediaCallbackState>())
{
    _callbackState->engine = this;
}

AvfMediaEngine::~AvfMediaEngine()
{
    close();
    std::lock_guard<std::mutex> lock(_callbackState->mutex);
    _callbackState->engine = nullptr;
}

void AvfMediaEngine::onPlayerEnd()
{
    auto callbackState = _callbackState;
    _playbackEnded = true;
    _state = MEMediaState::Stopped;
    fireMediaEvent(MEMediaEventType::Stopped);

    {
        std::lock_guard<std::mutex> lock(callbackState->mutex);
        if (callbackState->engine != this)
            return;
    }

    if (_state != MEMediaState::Closed && _repeatEnabled)
    {
        this->setCurrentTime(0);
        this->play();
    }
}

void AvfMediaEngine::setAutoPlay(bool bAutoPlay)
{
    _bAutoPlay = bAutoPlay;
}

bool AvfMediaEngine::open(std::string_view sourceUri)
{
    close();

    uint64_t generation;
    {
        std::lock_guard<std::mutex> lock(_callbackState->mutex);
        generation = ++_callbackState->generation;
        _callbackState->engine = this;
    }

    NSURL* nsMediaUrl = nil;
    std::string_view Path;

    if (cxx20::starts_with(sourceUri, "file://"sv))
    {
        // Media Framework doesn't percent encode the URL, so the path portion is just a native file path.
        // Extract it and then use it create a proper URL.
        Path             = sourceUri.substr(7);
        NSString* nsPath = [[NSString alloc] initWithBytes:Path.data() length:Path.size() encoding:NSUTF8StringEncoding];
        nsMediaUrl       = [NSURL fileURLWithPath:nsPath isDirectory:NO];
    }
    else
    {
        // Assume that this has been percent encoded for now - when we support HTTP Live Streaming we will need to check
        // for that.
        NSString* nsUri = [[NSString alloc] initWithBytes:sourceUri.data() length:sourceUri.size() encoding:NSUTF8StringEncoding];
        nsMediaUrl      = [NSURL URLWithString:nsUri];
    }

    // open media file
    if (nsMediaUrl == nil)
    {
        AXME_TRACE("Failed to open Media file: {}", sourceUri);
        return false;
    }

    // create player instance
    _player = [[AVPlayer alloc] init];

    if (!_player)
    {
        AXME_TRACE("Failed to create instance of an AVPlayer: {}", sourceUri);
        return false;
    }

    _player.actionAtItemEnd = AVPlayerActionAtItemEndPause;

    // create player item
    _sessionHandler = [[AVMediaSessionHandler alloc] initWithCallbackState:_callbackState];
    assert(_sessionHandler != nil);

    // Use URL asset which gives us resource loading ability if system can't handle the scheme
    AVURLAsset* urlAsset = [[AVURLAsset alloc] initWithURL:nsMediaUrl options:nil];

    _playerItem = [AVPlayerItem playerItemWithAsset:urlAsset];

    if (_playerItem == nil)
    {
        AXME_TRACE("Failed to open player item with Url: {}", sourceUri);
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(_callbackState->mutex);
        _callbackState->currentPlayerItem = (__bridge void*)_playerItem;
    }

    _state = MEMediaState::Preparing;

    auto callbackState = _callbackState;
    AVPlayerItem* playerItem = _playerItem;

    // load tracks
    [[_playerItem asset] loadValuesAsynchronouslyForKeys:@[ @"tracks" ]
                                       completionHandler:^{
                                         AvfMediaCallbackGuard callback(callbackState, generation);
                                         if (!callback)
                                             return;

                                         NSError* nsError = nil;

                                         if ([[playerItem asset] statusOfValueForKey:@"tracks" error:&nsError] ==
                                             AVKeyValueStatusLoaded)
                                         {
                                             // File movies will be ready now
                                             if (playerItem.status == AVPlayerItemStatusReadyToPlay)
                                             {
                                                 callback.get()->onStatusNotification((__bridge void*)playerItem);
                                             }
                                         }
                                         else if (nsError != nullptr)
                                         {
                                             NSDictionary* errDetail = [nsError userInfo];
                                             NSString* errStr =
                                                 [[errDetail objectForKey:NSUnderlyingErrorKey] localizedDescription];
                                             NSString* errorReason = [errDetail objectForKey:NSLocalizedFailureReasonErrorKey];
                                             AXME_TRACE("Load media asset failed, {}, {}", errStr.UTF8String, errorReason.UTF8String);
                                         }
                                       }];

    [[NSNotificationCenter defaultCenter] addObserver:_sessionHandler
                                             selector:@selector(playerItemDidPlayToEndTime:)
                                                 name:AVPlayerItemDidPlayToEndTimeNotification
                                               object:_playerItem];
    [_playerItem addObserver:_sessionHandler forKeyPath:@"status" options:0 context:(__bridge void*)_playerItem];

    _player.rate = 0.0;
    [_player replaceCurrentItemWithPlayerItem:_playerItem];

    // TODO: handle EnterForground, EnterBackground, Active, Deactive, AudioRouteChanged
#    if TARGET_OS_IPHONE
    [_sessionHandler registerUINotifications];
#    endif
    return true;
}

void AvfMediaEngine::onStatusNotification(void* context)
{
    if (!_playerItem || context != (__bridge void*)_playerItem)
        return;
    if (_playerItem.status == AVPlayerItemStatusFailed)
    {
        fireMediaEvent(MEMediaEventType::Error);
        return;
    }
    if (_playerItem.status != AVPlayerItemStatusReadyToPlay)
        return;

    for (AVPlayerItemTrack* playerTrack in _playerItem.tracks)
    {
        AVAssetTrack* assetTrack = playerTrack.assetTrack;
        NSString* mediaType      = assetTrack.mediaType;
        if ([mediaType isEqualToString:AVMediaTypeVideo])
        {  // we only care about video

            auto naturalSize = [assetTrack naturalSize];
            _videoExtent.x = naturalSize.width;
            _videoExtent.y = naturalSize.height;

            NSMutableDictionary* outputAttrs = [NSMutableDictionary dictionary];
            if (assetTrack.formatDescriptions.count == 0)
                continue;

            CMFormatDescriptionRef DescRef =
                (__bridge CMFormatDescriptionRef)[assetTrack.formatDescriptions objectAtIndex:0];
            CMVideoCodecType codecType       = CMFormatDescriptionGetMediaSubType(DescRef);

            int videoOutputPF = kCVPixelFormatType_32BGRA;
            if (kCMVideoCodecType_H264 == codecType || kCMVideoCodecType_HEVC == codecType)
            {
                videoOutputPF = kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;

                CFDictionaryRef formatExtensions = CMFormatDescriptionGetExtensions(DescRef);
                if (formatExtensions)
                {
                    CFBooleanRef bFullRange = (CFBooleanRef)CFDictionaryGetValue(
                        formatExtensions, kCMFormatDescriptionExtension_FullRangeVideo);
                    if (bFullRange && (bool)CFBooleanGetValue(bFullRange))
                    {
                        videoOutputPF = kCVPixelFormatType_420YpCbCr8BiPlanarFullRange;
                    }
                }
            }

            CGAffineTransform transform = [assetTrack preferredTransform];
            double radians = atan2(transform.b, transform.a);
            int degrees = static_cast<int>(radians * 180.0 / M_PI);
            _videoRotation = AX_ALIGN_ANY(degrees, 90);

            _bFullColorRange = false;
            switch (videoOutputPF)
            {
            case kCVPixelFormatType_420YpCbCr8BiPlanarFullRange:
                _bFullColorRange = true;
            case kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange:
                _videoPF = MEVideoPixelFormat::NV12;
                break;
            default:  // kCVPixelFormatType_32BGRA
                _videoPF = MEVideoPixelFormat::BGR32;
            }

            [outputAttrs setObject:[NSNumber numberWithInt:videoOutputPF]
                            forKey:(__bridge NSString*)kCVPixelBufferPixelFormatTypeKey];
            [outputAttrs setObject:[NSNumber numberWithInteger:1]
                            forKey:(__bridge NSString*)kCVPixelBufferBytesPerRowAlignmentKey];
            [outputAttrs setObject:[NSNumber numberWithBool:YES]
                            forKey:(__bridge NSString*)kCVPixelBufferMetalCompatibilityKey];

            AVPlayerItemVideoOutput* videoOutput =
                [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:outputAttrs];

            // Only decode for us
            videoOutput.suppressesPlayerRendering = YES;

            [_playerItem addOutput:videoOutput];

            _playerOutput = videoOutput;

            break;
        }
    }

    if (_bAutoPlay) {
        /* Fix issue: #2371 for tvOS
        delay one frame to invoke [player play] to fix player.timeControlStatus
        maybe AVPlayerTimeControlStatusPaused at first app startup
        */
        __weak AVPlayer* weakPlayer = _player;
        auto callbackState = _callbackState;
        uint64_t generation;
        {
            std::lock_guard<std::mutex> lock(callbackState->mutex);
            generation = callbackState->generation;
        }
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            AvfMediaCallbackGuard callback(callbackState, generation);
            AVPlayer* player = weakPlayer;
            if (callback && player != nil && player == callback.get()->_player)
                [player play];
        });
        
        _playbackEnded = false;
        _state = MEMediaState::Playing;
        fireMediaEvent(MEMediaEventType::Playing);
    }
}

bool AvfMediaEngine::transferVideoFrame()
{
    AVPlayerItemVideoOutput* videoOutput = (AVPlayerItemVideoOutput*)this->_playerOutput;
    if (!videoOutput)
        return false;

    CMTime currentTime = [videoOutput itemTimeForHostTime:CACurrentMediaTime()];
    if (![videoOutput hasNewPixelBufferForItemTime:currentTime])
        return false;

    CVPixelBufferRef videoFrame = [videoOutput copyPixelBufferForItemTime:currentTime itemTimeForDisplay:nullptr];
    if (!videoFrame)
        return false;

    auto& videoDim = _videoExtent;
    MEIntPoint bufferDim;

    CVPixelBufferLockBaseAddress(videoFrame, kCVPixelBufferLock_ReadOnly);

    if (CVPixelBufferIsPlanar(videoFrame))
    {  // NV12('420v' or '420f' expected
        assert(CVPixelBufferGetPlaneCount(videoFrame) == 2);

        auto YWidth  = static_cast<int>(CVPixelBufferGetWidthOfPlane(videoFrame, 0));   // 1920
        auto YHeight = static_cast<int>(CVPixelBufferGetHeightOfPlane(videoFrame, 0));  // 1080

        auto UVWidth  = static_cast<int>(CVPixelBufferGetWidthOfPlane(videoFrame, 1));   // 960
        auto UVHeight = static_cast<int>(CVPixelBufferGetHeightOfPlane(videoFrame, 1));  // 540

        auto YPitch  = static_cast<int>(CVPixelBufferGetBytesPerRowOfPlane(videoFrame, 0));
        auto UVPitch = static_cast<int>(CVPixelBufferGetBytesPerRowOfPlane(videoFrame, 1));

        auto YDataLen      = YPitch * YHeight;    // 1920x1080: YDataLen=2073600
        auto UVDataLen     = UVPitch * UVHeight;  // 1920x1080: UVDataLen=1036800
        auto frameYData    = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(videoFrame, 0);
        auto frameCbCrData = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(videoFrame, 1);
        // Apple: both H264, HEVC(H265) bufferDimX=ALIGN(videoDim.x, 32), bufferDimY=videoDim.y
        // Windows:
        //    - H264: BufferDimX align videoDim.x with 16, BufferDimY as-is
        //    - HEVC(H265): BufferDim(X,Y) align videoDim(X,Y) with 32
        MEVideoFrame frame{frameYData, frameCbCrData, static_cast<size_t>(YDataLen + UVDataLen), MEVideoPixelDesc{_videoPF, MEIntPoint{YPitch, YHeight}}, videoDim};
        frame._vpd._rotation = _videoRotation;
#if defined(_DEBUG) || !defined(_NDEBUG)
        auto& ycbcrDesc = frame._ycbcrDesc;
        ycbcrDesc.YDim.x = YWidth;
        ycbcrDesc.YDim.y = YHeight;
        ycbcrDesc.CbCrDim.x = UVWidth;
        ycbcrDesc.CbCrDim.y = UVHeight;
        ycbcrDesc.YPitch = YPitch;
        ycbcrDesc.CbCrPitch = UVPitch;
#endif
        _onVideoFrame(frame);
    }
    else
    {  // BGRA
        auto frameData       = (uint8_t*)CVPixelBufferGetBaseAddress(videoFrame);
        size_t frameDataSize = CVPixelBufferGetDataSize(videoFrame);
        _onVideoFrame(MEVideoFrame{frameData, nullptr, frameDataSize, MEVideoPixelDesc{_videoPF, videoDim}, videoDim});
    }
    CVPixelBufferUnlockBaseAddress(videoFrame, kCVPixelBufferLock_ReadOnly);

    CVPixelBufferRelease(videoFrame);
    return true;
}

bool AvfMediaEngine::close()
{
    AXLOGD("AvfMediaEngine::close(): this:{}", fmt::ptr(this));
    std::unique_lock<std::mutex> callbackLock(_callbackState->mutex);
    ++_callbackState->generation;
    _callbackState->engine = nullptr;
    _callbackState->currentPlayerItem = nullptr;
    const auto callbacksOwnedByThisThread = AvfMediaCallbackGuard::currentThreadGuardCount(_callbackState.get());
    _callbackState->callbackFinished.wait(callbackLock, [state = _callbackState.get(), callbacksOwnedByThisThread] {
        return state->activeCallbacks <= callbacksOwnedByThisThread;
    });
    callbackLock.unlock();

    if (_playerItem)
    {
        [_playerItem removeObserver:_sessionHandler forKeyPath:@"status"];

        [[NSNotificationCenter defaultCenter] removeObserver:_sessionHandler
                                                        name:AVPlayerItemDidPlayToEndTimeNotification
                                                      object:_playerItem];

        _playerItem = nil;
    }

    _playerOutput = nil;

    if (_player)
    {
        [_player pause];
        [_player replaceCurrentItemWithPlayerItem:nil];
        _player = nil;
    }

    if (_sessionHandler)
    {
        [_sessionHandler detachMediaEngine];
        _sessionHandler = nil;
    }

    _state = MEMediaState::Closed;
    _playbackEnded = false;
    return true;
}

bool AvfMediaEngine::setLoop(bool bLooping)
{
    _repeatEnabled = bLooping;
    if (bLooping)
        _player.actionAtItemEnd = AVPlayerActionAtItemEndNone;
    else
        _player.actionAtItemEnd = AVPlayerActionAtItemEndPause;
    return true;
}
bool AvfMediaEngine::setRate(double fRate)
{
    if (_player)
    {
        [_player setRate:fRate];
        // TODO:

        _player.muted = fRate < 0 ? YES : NO;
    }
    return true;
}
bool AvfMediaEngine::setCurrentTime(double fSeekTimeInSec)
{
    if (_player != nil)
        [_player seekToTime:CMTimeMake(fSeekTimeInSec, 1)];
    return true;
}

double AvfMediaEngine::getCurrentTime()
{
    if (_player != nil) {
        CMTime currTime = [_player currentTime];
        if (CMTIME_IS_VALID(currTime))
            return CMTimeGetSeconds(currTime);
    }

    return 0.0;
}

double AvfMediaEngine::getDuration()
{
    if (_player != nil) {
        if (_player.currentItem != nil) {
            CMTime duration = _player.currentItem.asset.duration;
            return CMTimeGetSeconds(duration);
        }
    }
    return 0.0;
}

bool AvfMediaEngine::play()
{
    if (_state != MEMediaState::Playing)
    {
        [_player play];
        _playbackEnded = false;
        _state = MEMediaState::Playing;
        fireMediaEvent(MEMediaEventType::Playing);
    }
    return true;
}
void AvfMediaEngine::internalPlay(bool replay)
{
    if (_player != nil) {
        if (replay)
            [_player pause];
        [_player play];
    }
}
void AvfMediaEngine::internalPause()
{
    if (_player != nil)
        [_player pause];
}
bool AvfMediaEngine::pause()
{
    if (_state == MEMediaState::Playing)
    {
        [_player pause];
        _state = MEMediaState::Paused;
        fireMediaEvent(MEMediaEventType::Paused);
    }
    return true;
}
bool AvfMediaEngine::stop()
{
    if (_state != MEMediaState::Stopped)
    {
        setCurrentTime(0);
        [_player pause];
        _state = MEMediaState::Stopped;

        // stop() will be invoked in dealloc, which is invoked by _videoPlayer's destructor,
        // so do't send the message when _videoPlayer is being deleted.
    }
    return true;
}
MEMediaState AvfMediaEngine::getState() const
{
    return _state;
}

}

#endif
