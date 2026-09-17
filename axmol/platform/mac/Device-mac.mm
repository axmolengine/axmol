/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "axmol/platform/Device.h"
#include <Foundation/Foundation.h>
#include <Cocoa/Cocoa.h>
#include <string>
#include "axmol/base/Types.h"

namespace ax
{

void Device::getClipboardText(std::function<void(std::string_view)> callback)
{
    if (!callback)
        return;
    @autoreleasepool
    {
        NSPasteboard* pb = [NSPasteboard generalPasteboard];
        NSString* s      = [pb stringForType:NSPasteboardTypeString];
        if (!s)
        {
            callback(std::string_view{});
            return;
        }
        const char* utf8 = [s UTF8String];
        NSUInteger len   = [s lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        callback(utf8 ? std::string_view(utf8, static_cast<size_t>(len)) : std::string_view{});
    }
}

void Device::setClipboardText(std::string_view text)
{
    @autoreleasepool
    {
        NSPasteboard* pb = [NSPasteboard generalPasteboard];
        [pb clearContents];

        NSString* s = [[NSString alloc] initWithBytes:text.data()
                                               length:(NSUInteger)text.size()
                                             encoding:NSUTF8StringEncoding];
        if (s)
            [pb setString:s forType:NSPasteboardTypeString];
    }
}

void Device::clearClipboard()
{
    @autoreleasepool
    {
        NSPasteboard* pb = [NSPasteboard generalPasteboard];
        [pb clearContents];
    }
}

int Device::getDPI()
{
    NSScreen* screen           = [NSScreen mainScreen];
    NSDictionary* description  = [screen deviceDescription];
    NSSize displayPixelSize    = [[description objectForKey:NSDeviceSize] sizeValue];
    CGSize displayPhysicalSize = CGDisplayScreenSize([[description objectForKey:@"NSScreenNumber"] unsignedIntValue]);

    return ((displayPixelSize.width / displayPhysicalSize.width) * 25.4f);
}

float Device::getPixelRatio()
{
    NSScreen* screen  = [NSScreen mainScreen];
    const auto points = [screen frame];
    const auto pixels = [screen convertRectToBacking:points];
    return static_cast<float>(pixels.size.width / points.size.width);
}

void Device::setAccelerometerEnabled(bool isEnabled) {}

void Device::setAccelerometerInterval(float interval) {}

void Device::setKeepScreenOn(bool value) {}

void Device::vibrate(float duration) {}

void Device::prepareImpactFeedbackGenerator(ImpactFeedbackStyle style) {}

void Device::impactOccurred(ImpactFeedbackStyle style) {}

void Device::prepareNotificationFeedbackGenerator() {}

void Device::notificationOccurred(NotificationFeedbackType type) {}

void Device::prepareSelectionFeedbackGenerator() {}

void Device::selectionChanged() {}

}  // namespace ax
