/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "axmol/platform/Common.h"
#include "axmol/platform/apple/FoundationBridge.h"

#include "axmol/base/Director.h"

#include <stdarg.h>
#include <stdio.h>
#include <Cocoa/Cocoa.h>

namespace ax
{

AlertResult showAlert(std::string_view msg, std::string_view title, AlertStyle)
{

    NSString* tmpTitle = svtons(title);
    NSString* tmpMsg   = svtons(msg);

    NSAlert* alert = [[[NSAlert alloc] init] autorelease];
    [alert addButtonWithTitle:@"OK"];
    [alert setMessageText:tmpTitle];
    [alert setInformativeText:tmpMsg];
    [alert setAlertStyle:NSAlertStyleWarning];

    auto renderView = Director::getInstance()->getRenderView();
    id window       = (id)renderView->getNativeWindow();
    [alert beginSheetModalForWindow:window completionHandler:nil];

    return AlertResult::None;
}

}  // namespace ax
