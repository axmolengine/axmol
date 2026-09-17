/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#import <UIKit/UIKit.h>

#include "axmol/platform/Device.h"
#include "axmol/base/Types.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/AccelerationEvent.h"
#include "axmol/base/Director.h"

// Accelerometer
#if !defined(AX_TARGET_OS_TVOS)
#    import <CoreMotion/CoreMotion.h>
#endif
#import <CoreFoundation/CoreFoundation.h>
// Vibrate
#import <AudioToolbox/AudioToolbox.h>

#if !defined(AX_TARGET_OS_TVOS)
static UIImpactFeedbackGenerator* impactFeedbackGenerator[3];
static UINotificationFeedbackGenerator* notificationFeedbackGenerator;
static UISelectionFeedbackGenerator* selectionFeedbackGenerator;
#endif

#define SENSOR_DELAY_GAME 0.02

#if !defined(AX_TARGET_OS_TVOS)
@interface CCAccelerometerDispatcher : NSObject {
    ax::Acceleration* _acceleration;
    CMMotionManager* _motionManager;
}

+ (id)sharedAccelerometerDispatcher;
- (id)init;
- (void)setAccelerometerEnabled:(bool)isEnabled;
- (void)setAccelerometerInterval:(float)interval;

@end

@implementation CCAccelerometerDispatcher

static CCAccelerometerDispatcher* s_pAccelerometerDispatcher;

+ (id)sharedAccelerometerDispatcher
{
    if (s_pAccelerometerDispatcher == nil)
    {
        s_pAccelerometerDispatcher = [[self alloc] init];
    }

    return s_pAccelerometerDispatcher;
}

- (id)init
{
    if ((self = [super init]))
    {
        _acceleration                              = new ax::Acceleration();
        _motionManager                             = [[CMMotionManager alloc] init];
        _motionManager.accelerometerUpdateInterval = SENSOR_DELAY_GAME;
    }
    return self;
}

- (void)dealloc
{
    s_pAccelerometerDispatcher = nullptr;
    delete _acceleration;
}

- (void)setAccelerometerEnabled:(bool)isEnabled
{
    if (isEnabled)
    {
        [_motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue]
                                             withHandler:^(CMAccelerometerData* accelerometerData, NSError* error) {
                                               [self accelerometer:accelerometerData];
                                             }];
    }
    else
    {
        [_motionManager stopAccelerometerUpdates];
    }
}

- (void)setAccelerometerInterval:(float)interval
{
    _motionManager.accelerometerUpdateInterval = interval;
}

- (void)accelerometer:(CMAccelerometerData*)accelerometerData
{
    _acceleration->x         = accelerometerData.acceleration.x;
    _acceleration->y         = accelerometerData.acceleration.y;
    _acceleration->z         = accelerometerData.acceleration.z;
    _acceleration->timestamp = accelerometerData.timestamp;

    double tmp = _acceleration->x;
    UIInterfaceOrientation orientation;
    if (@available(iOS 13.0, *))
    {
        orientation = [[[UIApplication sharedApplication].windows[0] windowScene] interfaceOrientation];
    }
    else
    {
        // Fallback on earlier versions
        orientation = [[UIApplication sharedApplication] statusBarOrientation];
    }

    switch (orientation)
    {
    case UIInterfaceOrientationLandscapeRight:
        _acceleration->x = -_acceleration->y;
        _acceleration->y = tmp;
        break;

    case UIInterfaceOrientationLandscapeLeft:
        _acceleration->x = _acceleration->y;
        _acceleration->y = -tmp;
        break;

    case UIInterfaceOrientationPortraitUpsideDown:
        _acceleration->x = -_acceleration->y;
        _acceleration->y = -tmp;
        break;

    case UIInterfaceOrientationPortrait:
        break;
    default:
        NSAssert(false, @"unknown orientation");
    }

    ax::AccelerationEvent event(*_acceleration);
    auto dispatcher = ax::Director::getInstance()->getEventDispatcher();
    dispatcher->dispatchEvent(&event);
}
@end
#endif  // !defined(AX_TARGET_OS_TVOS)

//

namespace ax
{

static float getDevicePixelRatio(int& sdpi)
{
    float scale = 1.0f;

    if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)])
        scale = [[UIScreen mainScreen] scale];

    UIUserInterfaceIdiom userInterfaceIdiom = [UIDevice.currentDevice userInterfaceIdiom];
    if (userInterfaceIdiom == UIUserInterfaceIdiomPad)
        sdpi = 132;
    else if (userInterfaceIdiom == UIUserInterfaceIdiomPhone)
        sdpi = 163;
    else
        sdpi = 160;

    return scale;
}

int Device::getDPI()
{
    static int dpi = -1;

    if (dpi == -1)
    {
        int sdpi;  // the standard dpi without scaling
        float scale = getDevicePixelRatio(sdpi);
        return static_cast<int>(scale * sdpi);
    }

    return dpi;
}

float Device::getPixelRatio()
{
    int ignored_sdpi;
    return getDevicePixelRatio(ignored_sdpi);
}

void Device::setAccelerometerEnabled(bool isEnabled)
{
#if !defined(AX_TARGET_OS_TVOS)
    [[CCAccelerometerDispatcher sharedAccelerometerDispatcher] setAccelerometerEnabled:isEnabled];
#endif
}

void Device::setAccelerometerInterval(float interval)
{
#if !defined(AX_TARGET_OS_TVOS)
    [[CCAccelerometerDispatcher sharedAccelerometerDispatcher] setAccelerometerInterval:interval];
#endif
}

void Device::setKeepScreenOn(bool value)
{
    [[UIApplication sharedApplication] setIdleTimerDisabled:(BOOL)value];
}

/*!
 @brief Only works on iOS devices that support vibration (such as iPhone). Should only be used for important alerts. Use
 risks rejection in iTunes Store.
 @param duration ignored for iOS
 */
void Device::vibrate(float duration)
{
    // See http://stackoverflow.com/questions/4724980/making-the-iphone-vibrate
    // should vibrate no matter it is silient or not
    if ([[UIDevice currentDevice].model isEqualToString:@"iPhone"])
    {
        AudioServicesPlaySystemSound(1352);  // works ALWAYS as of this post
    }
    else
    {
        // Not an iPhone, so doesn't have vibrate
        // play the less annoying tick noise or one of your own
        AudioServicesPlayAlertSound(kSystemSoundID_Vibrate);
    }
}

void Device::prepareImpactFeedbackGenerator(ImpactFeedbackStyle style)
{
#if !defined(AX_TARGET_OS_TVOS)
    if (impactFeedbackGenerator[style] == nullptr)
    {
        UIImpactFeedbackStyle impactStyle;
        switch (style)
        {
        case ImpactFeedbackStyleLight:
            impactStyle = UIImpactFeedbackStyleLight;
            break;
        case ImpactFeedbackStyleMedium:
            impactStyle = UIImpactFeedbackStyleMedium;
            break;
        case ImpactFeedbackStyleHeavy:
            impactStyle = UIImpactFeedbackStyleHeavy;
            break;
        }

        impactFeedbackGenerator[style] = [[UIImpactFeedbackGenerator alloc] initWithStyle:impactStyle];
    }
    [impactFeedbackGenerator[style] prepare];
#endif
}

void Device::impactOccurred(ImpactFeedbackStyle style)
{
#if !defined(AX_TARGET_OS_TVOS)
    if (impactFeedbackGenerator[style] == nullptr)
    {
        prepareImpactFeedbackGenerator(style);
    }
    [impactFeedbackGenerator[style] impactOccurred];
#endif
}

void Device::prepareNotificationFeedbackGenerator()
{
#if !defined(AX_TARGET_OS_TVOS)
    if (notificationFeedbackGenerator == nullptr)
    {
        notificationFeedbackGenerator = [[UINotificationFeedbackGenerator alloc] init];
    }
    [notificationFeedbackGenerator prepare];
#endif
}

void Device::notificationOccurred(NotificationFeedbackType type)
{
#if !defined(AX_TARGET_OS_TVOS)
    if (notificationFeedbackGenerator == nullptr)
    {
        prepareNotificationFeedbackGenerator();
    }

    UINotificationFeedbackType notificationType;
    switch (type)
    {
    case NotificationFeedbackTypeError:
        notificationType = UINotificationFeedbackTypeError;
        break;
    case NotificationFeedbackTypeSuccess:
        notificationType = UINotificationFeedbackTypeSuccess;
        break;
    case NotificationFeedbackTypeWarning:
        notificationType = UINotificationFeedbackTypeWarning;
        break;
    }

    [notificationFeedbackGenerator notificationOccurred:notificationType];
#endif
}

void Device::prepareSelectionFeedbackGenerator()
{
#if !defined(AX_TARGET_OS_TVOS)
    if (selectionFeedbackGenerator == nullptr)
    {
        selectionFeedbackGenerator = [[UISelectionFeedbackGenerator alloc] init];
    }
    [selectionFeedbackGenerator prepare];
#endif
}

void Device::selectionChanged()
{
#if !defined(AX_TARGET_OS_TVOS)
    if (selectionFeedbackGenerator == nullptr)
    {
        prepareSelectionFeedbackGenerator();
    }
    [selectionFeedbackGenerator selectionChanged];
    [selectionFeedbackGenerator prepare];
#endif
}

int Device::getDisplayRefreshRate()
{
    UIScreen* mainScreen = [UIScreen mainScreen];
    int hz               = static_cast<int>(mainScreen.maximumFramesPerSecond);
    if (hz < MIN_REFRESH_RATE || hz > MAX_REFRESH_RATE)
        hz = DEFAULT_REFRESH_RATE;
    return hz;
}

static Device::Orientation _preferredOrientation = Device::Orientation::Unknown;

void Device::setPreferredOrientation(Device::Orientation orientation)
{
#if !defined(AX_TARGET_OS_TVOS)
    _preferredOrientation = orientation;

    auto renderView = Director::getInstance()->getRenderView();
    if (!renderView)
        return;  // will take affect when creating renderView

    // Always perform UI work on main thread and obtain window/VC there.
    dispatch_async(dispatch_get_main_queue(), ^{
      auto mainWindow      = (__bridge UIWindow*)renderView->getNativeWindow();
      UIViewController* vc = mainWindow.rootViewController;

      if (@available(iOS 16.0, *))
      {
          // Modern API: mark for update then attempt rotation
          [vc setNeedsUpdateOfSupportedInterfaceOrientations];
      }
      else
      {
          // Fallback: present/dismiss minimal full-screen controller to force re-evaluation.
          // Present from the top-most VC to avoid container interception.
          UIViewController* dummy      = [[UIViewController alloc] init];
          dummy.view.backgroundColor   = [UIColor clearColor];
          dummy.modalPresentationStyle = UIModalPresentationFullScreen;

          [vc presentViewController:dummy
                           animated:NO
                         completion:^{
                           dispatch_async(dispatch_get_main_queue(), ^{
                             [dummy dismissViewControllerAnimated:NO
                                                       completion:^{
                                                         [UIViewController attemptRotationToDeviceOrientation];
                                                       }];
                           });
                         }];
      }
    });
#endif
}

Device::Orientation Device::getPreferredOrientation()
{
    return _preferredOrientation;
}

Device::OrientationMask Device::getSupportedOrientations()
{
    NSArray* plistOrientations = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"UISupportedInterfaceOrientations"];
    OrientationMask mask       = static_cast<OrientationMask>(0);
    for (NSString* entry in plistOrientations)
    {
        if ([entry isEqualToString:@"UIInterfaceOrientationPortrait"])
        {
            mask = static_cast<OrientationMask>(mask | OrientationMask::Portrait);
        }
        else if ([entry isEqualToString:@"UIInterfaceOrientationPortraitUpsideDown"])
        {
            mask = static_cast<OrientationMask>(mask | OrientationMask::ReversePortrait);
        }
        else if ([entry isEqualToString:@"UIInterfaceOrientationLandscapeLeft"])
        {
            mask = static_cast<OrientationMask>(mask | OrientationMask::Landscape);
        }
        else if ([entry isEqualToString:@"UIInterfaceOrientationLandscapeRight"])
        {
            mask = static_cast<OrientationMask>(mask | OrientationMask::ReverseLandscape);
        }
    }

    return mask;
}

Device::Orientation Device::getCurrentOrientation()
{
#if !defined(AX_TARGET_OS_TVOS)
    auto renderView = Director::getInstance()->getRenderView();
    if (!renderView)
        return Orientation::Unknown;
    auto window = (__bridge UIWindow*)renderView->getNativeWindow();
    UIInterfaceOrientation uiOrientation;
    if (@available(iOS 13.0, *))
    {
        uiOrientation = window.windowScene.interfaceOrientation;
    }
    else
    {
        // Fallback on earlier versions
        uiOrientation = UIApplication.sharedApplication.statusBarOrientation;
    }
    switch (uiOrientation)
    {
    case UIInterfaceOrientationPortrait:
        return Orientation::Portrait;
    case UIInterfaceOrientationLandscapeLeft:
        return Orientation::Landscape;
    case UIInterfaceOrientationLandscapeRight:
        return Orientation::ReverseLandscape;
    case UIInterfaceOrientationPortraitUpsideDown:
        return Orientation::ReversePortrait;
    default:
        break;
    }
#endif

    return Orientation::Unknown;
}

Device::Orientation Device::getPhysicalOrientation()
{
#if !defined(AX_TARGET_OS_TVOS)
    UIDeviceOrientation deviceOrientation = [[UIDevice currentDevice] orientation];

    switch (deviceOrientation)
    {
    case UIDeviceOrientationPortrait:
        return Orientation::Portrait;
    case UIDeviceOrientationPortraitUpsideDown:
        return Orientation::ReversePortrait;
    case UIDeviceOrientationLandscapeLeft:
        return Orientation::Landscape;
    case UIDeviceOrientationLandscapeRight:
        return Orientation::ReverseLandscape;
    default:
        break;
    }
#endif

    return Orientation::Unknown;
}

// Convert Orientation to OrientationMask
static Device::OrientationMask toMask(Device::Orientation o)
{
    switch (o)
    {
    case Device::Orientation::Portrait:
        return Device::OrientationMask::Portrait;
    case Device::Orientation::ReversePortrait:
        return Device::OrientationMask::ReversePortrait;
    case Device::Orientation::Landscape:
        return Device::OrientationMask::Landscape;
    case Device::Orientation::ReverseLandscape:
        return Device::OrientationMask::ReverseLandscape;
    default:
        return Device::OrientationMask::All;
    }
}

// Pick the first supported orientation from OrientationMask
static Device::Orientation pickFirstSupported(Device::OrientationMask mask)
{
    if ((mask & Device::OrientationMask::Portrait) == Device::OrientationMask::Portrait)
        return Device::Orientation::Portrait;
    if ((mask & Device::OrientationMask::Landscape) == Device::OrientationMask::Landscape)
        return Device::Orientation::Landscape;
    if ((mask & Device::OrientationMask::ReverseLandscape) == Device::OrientationMask::ReverseLandscape)
        return Device::Orientation::ReverseLandscape;
    if ((mask & Device::OrientationMask::ReversePortrait) == Device::OrientationMask::ReversePortrait)
        return Device::Orientation::ReversePortrait;

    return Device::Orientation::Portrait;  // fallback
}

Device::Orientation Device::resolveOrientation()
{
    auto supported = getSupportedOrientations();
    auto preferred = getPreferredOrientation();
    auto physical  = getPhysicalOrientation();

    auto tryUse = [&](Orientation o) -> Orientation {
        return ((supported & toMask(o)) == toMask(o)) ? o : Orientation::Unknown;
    };

    Orientation resolvedOrientation = Orientation::Unknown;

    switch (preferred)
    {
    // Case 1: Preferred is a concrete orientation
    case Orientation::Portrait:
    case Orientation::ReversePortrait:
    case Orientation::Landscape:
    case Orientation::ReverseLandscape:
        resolvedOrientation = tryUse(preferred);
        break;

    // Case 2: SensorPortrait
    case Orientation::SensorPortrait:
        resolvedOrientation = tryUse(physical);
        if (resolvedOrientation == Orientation::Unknown)
            resolvedOrientation =
                bool(supported & OrientationMask::Portrait) ? Orientation::Portrait : Orientation::ReversePortrait;
        break;

    // Case 3: SensorLandscape
    case Orientation::SensorLandscape:
        resolvedOrientation = tryUse(physical);
        if (resolvedOrientation == Orientation::Unknown)
            resolvedOrientation =
                bool(supported & OrientationMask::Landscape) ? Orientation::Landscape : Orientation::ReverseLandscape;
        break;

    // Case 4: Sensor / FullSensor
    case Orientation::Sensor:
    case Orientation::FullSensor:
        resolvedOrientation = tryUse(physical);
        if (resolvedOrientation == Orientation::Unknown)
            resolvedOrientation = pickFirstSupported(supported);
        break;

    // Default / Unknown
    default:
        break;
    }

    // Final fallback
    if (resolvedOrientation == Orientation::Unknown)
        resolvedOrientation = pickFirstSupported(supported);

    return resolvedOrientation;
}

void Device::getClipboardText(std::function<void(std::string_view)> callback)
{
    if (!callback)
        return;
#if TARGET_OS_IOS
    @autoreleasepool
    {
        NSString* text = [UIPasteboard generalPasteboard].string;
        if (text)
            callback(std::string_view([text UTF8String]));
        else
            callback(std::string_view{});
    }
#endif
}

void Device::setClipboardText(std::string_view text)
{
#if TARGET_OS_IOS
    @autoreleasepool
    {
        NSString* s = [[NSString alloc] initWithBytes:text.data() length:text.length() encoding:NSUTF8StringEncoding];
        if (!s)
            s = @"";
        [UIPasteboard generalPasteboard].string = s;
    }
#endif
}

void Device::clearClipboard()
{
#if TARGET_OS_IOS
    [UIPasteboard generalPasteboard].string = nil;
#endif
}

}  // namespace ax
