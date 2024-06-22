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
#    include "yasio/string_view.hpp"
#    include "yasio/endian_portable.hpp"
#    include "yasio/sz.hpp"

#if TARGET_OS_IPHONE
#    import <UIKit/UIKit.h>
#endif

USING_NS_AX;

#define AX_ALIGN_ANY(x, a) ((((x) + (a) - 1) / (a)) * (a))

@interface AVMediaSessionHandler : NSObject
- (AVMediaSessionHandler*)initWithMediaEngine:(AvfMediaEngine*)me;
- (void)dealloc;
- (void)playerItemDidPlayToEndTime:(NSNotification*)notification;
@property AvfMediaEngine* _me;
@end

@implementation AVMediaSessionHandler
@synthesize _me;

- (AVMediaSessionHandler*)initWithMediaEngine:(AvfMediaEngine*)me
{
    self = [super init];
    if (self)
        _me = me;
    return self;
}

- registerUINotifications
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
    if (_me->isPlaying())
        _me->internalPlay(true);
}

- (void)handleActive:(NSNotification*)notification
{
    if (_me->isPlaying())
        _me->internalPlay();
}

- (void)handleDeactive:(NSNotification*)notification
{
    if (_me->isPlaying())
        _me->internalPause();
}


- (void)handleEnterForground:(NSNotification*)notification
{
    if (_me->isPlaying())
        _me->internalPlay();
}

- (void)handleEnterBackround:(NSNotification*)notification
{
    if (_me->isPlaying())
        _me->internalPause();
}
#endif

- deregisterUINotifications
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

- (void)dealloc
{
    [super dealloc];
}

- (void)playerItemDidPlayToEndTime:(NSNotification*)notification
{
    _me->onPlayerEnd();
}

- (void)observeValueForKeyPath:(NSString*)keyPath
                      ofObject:(id)object
                        change:(NSDictionary<NSKeyValueChangeKey, id>*)change
                       context:(void*)context
{
    if ((id)context == object && [keyPath isEqualToString:@"status"])
        _me->onStatusNotification(context);
}

@end

NS_AX_BEGIN

void AvfMediaEngine::onPlayerEnd()
{
    _playbackEnded = true;
    _state = MEMediaState::Stopped;
    fireMediaEvent(MEMediaEventType::Stopped);
    
    if (_repeatEnabled) {
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
        AXME_TRACE("Failed to open Media file: %s", sourceUri.data());
        return false;
    }

    // create player instance
    _player = [[AVPlayer alloc] init];

    if (!_player)
    {
        AXME_TRACE("Failed to create instance of an AVPlayer: %s", sourceUri.data());
        return false;
    }

    _player.actionAtItemEnd = AVPlayerActionAtItemEndPause;

    // create player item
    _sessionHandler = [[AVMediaSessionHandler alloc] initWithMediaEngine:this];
    assert(_sessionHandler != nil);

    // Use URL asset which gives us resource loading ability if system can't handle the scheme
    AVURLAsset* urlAsset = [[AVURLAsset alloc] initWithURL:nsMediaUrl options:nil];

    _playerItem = [[AVPlayerItem playerItemWithAsset:urlAsset] retain];
    [urlAsset release];

    if (_playerItem == nil)
    {
        AXME_TRACE("Failed to open player item with Url: %s", sourceUri.data());
        return false;
    }

    _state = MEMediaState::Preparing;

    // load tracks
    [[_playerItem asset] loadValuesAsynchronouslyForKeys:@[ @"tracks" ]
                                       completionHandler:^{
                                         NSError* nsError = nil;

                                         if ([[_playerItem asset] statusOfValueForKey:@"tracks" error:&nsError] ==
                                             AVKeyValueStatusLoaded)
                                         {
                                             // File movies will be ready now
                                             if (_playerItem.status == AVPlayerItemStatusReadyToPlay)
                                             {
                                                 onStatusNotification(_playerItem);
                                             }
                                         }
                                         else if (nsError != nullptr)
                                         {
                                             NSDictionary* errDetail = [nsError userInfo];
                                             NSString* errStr =
                                                 [[errDetail objectForKey:NSUnderlyingErrorKey] localizedDescription];
                                             NSString* errorReason = [errDetail objectForKey:NSLocalizedFailureReasonErrorKey];
                                             AXME_TRACE("Load media asset failed, %s, %s", errStr.UTF8String, errorReason.UTF8String);
                                         }
                                       }];

    [[NSNotificationCenter defaultCenter] addObserver:_sessionHandler
                                             selector:@selector(playerItemDidPlayToEndTime:)
                                                 name:AVPlayerItemDidPlayToEndTimeNotification
                                               object:_playerItem];
    [_playerItem addObserver:_sessionHandler forKeyPath:@"status" options:0 context:_playerItem];

    _player.rate = 0.0;
    [_player replaceCurrentItemWithPlayerItem:_playerItem];

    // TODO: handle EnterForground, EnterBackground, Active, Deactive, AudioRouteChanged
    [_sessionHandler registerUINotifications];
    return true;
}

void AvfMediaEngine::onStatusNotification(void* context)
{
    if (!_playerItem || context != _playerItem)
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
            CMFormatDescriptionRef DescRef   = (CMFormatDescriptionRef)[assetTrack.formatDescriptions objectAtIndex:0];
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
                            forKey:(NSString*)kCVPixelBufferPixelFormatTypeKey];
            [outputAttrs setObject:[NSNumber numberWithInteger:1]
                            forKey:(NSString*)kCVPixelBufferBytesPerRowAlignmentKey];
            [outputAttrs setObject:[NSNumber numberWithBool:YES] forKey:(NSString*)kCVPixelBufferMetalCompatibilityKey];

            AVPlayerItemVideoOutput* videoOutput =
                [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:outputAttrs];

            // Only decode for us
            videoOutput.suppressesPlayerRendering = YES;

            [_playerItem addOutput:videoOutput];

            _playerOutput = videoOutput;

            break;
        }
    }

    if (_bAutoPlay)
        this->play();
}

bool AvfMediaEngine::transferVideoFrame()
{
    auto videoOutput = static_cast<AVPlayerItemVideoOutput*>(this->_playerOutput);
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
}

bool AvfMediaEngine::close()
{
    if (_state == MEMediaState::Closed)
        return true;
    if (_playerItem != nil)
    {
        if (_player != nil)
        {
            [_sessionHandler deregisterUINotifications];
            [[NSNotificationCenter defaultCenter] removeObserver:_sessionHandler
                                                            name:AVPlayerItemDidPlayToEndTimeNotification
                                                          object:_playerItem];
            [_playerItem removeObserver:_sessionHandler forKeyPath:@"status"];
        }
        [_playerItem release];
        _playerItem = nil;
    }
    if (_player != nil)
    {
        [_player release];
        _player = nil;
    }
    _state = MEMediaState::Closed;
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

NS_AX_END

#endif
