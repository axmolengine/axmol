/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/platform/PlatformConfig.h"
#include "axmol/platform/Device.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC

#    include <Foundation/Foundation.h>
#    include <Cocoa/Cocoa.h>
#    include <string>

#elif AX_TARGET_PLATFORM == AX_PLATFORM_IOS

#    import <UIKit/UIKit.h>

#endif

#include "axmol/base/Types.h"
#include "axmol/platform/apple/Device-apple.h"

namespace FontUtils
{
NSMutableParagraphStyle* _calculateParagraphStyle(bool enableWrap, int overflow)
{
    NSMutableParagraphStyle* paragraphStyle = [[[NSMutableParagraphStyle alloc] init] autorelease];
    paragraphStyle.lineBreakMode            = NSLineBreakByWordWrapping;
    return paragraphStyle;
}

NSTextAlignment _calculateTextAlignment(ax::Device::TextAlign align)
{
    unsigned uHoriFlag      = (int)align & 0x0f;
    NSTextAlignment nsAlign = (2 == uHoriFlag)   ? NSTextAlignmentRight
                              : (3 == uHoriFlag) ? NSTextAlignmentCenter
                                                 : NSTextAlignmentLeft;

    return nsAlign;
}

CGFloat _calculateTextDrawStartWidth(ax::Device::TextAlign align, CGSize realDimensions, CGSize dimensions)
{
    CGFloat xPadding   = 0;
    unsigned uHoriFlag = (int)align & 0x0f;
    switch (uHoriFlag)
    {
        // center
    case 3:
        xPadding = (dimensions.width - realDimensions.width) / 2.0f;
        break;
        // right
    case 2:
        xPadding = dimensions.width - realDimensions.width;
        break;
    default:
        break;
    }
    return xPadding;
}

}  // namespace FontUtils
