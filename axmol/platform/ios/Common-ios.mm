/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "axmol/platform/Common.h"

#include <stdarg.h>
#include <stdio.h>

#import <UIKit/UIAlertController.h>
#import <UIKit/UIWindow.h>
#include "axmol/base/Director.h"
#include "axmol/base/Logging.h"
#include "axmol/platform/apple/FoundationBridge.h"

namespace ax
{

AlertResult showAlert(std::string_view msg, std::string_view title, AlertStyle)
{
    // only enable it on iOS.
    // FIXME: Implement it for tvOS
#if !defined(AX_TARGET_OS_TVOS)
    NSString* tmpTitle = svtons(title);
    NSString* tmpMsg   = svtons(msg);

    UIAlertController* alertController = [UIAlertController alertControllerWithTitle:tmpTitle
                                                                             message:tmpMsg
                                                                      preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:@"OK"
                                                            style:UIAlertActionStyleDefault
                                                          handler:^(UIAlertAction* action){
                                                          }];

    [alertController addAction:defaultAction];
    auto rootViewController = [UIApplication sharedApplication].windows[0].rootViewController;
    [rootViewController presentViewController:alertController animated:YES completion:nil];
#else
    AXLOGI("{}: {}", title, msg);
#endif
    return AlertResult::None;
}

}  // namespace ax
