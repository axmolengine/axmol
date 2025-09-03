/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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
#include "platform/Common.h"
#include "platform/apple/FoundationBridge.h"

#include "base/Director.h"

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
    [alert setMessageText:tmpMsg];
    [alert setInformativeText:tmpTitle];
    [alert setAlertStyle:NSAlertStyleWarning];

    auto renderView = Director::getInstance()->getRenderView();
    id window       = (id)renderView->getCocoaWindow();
    [alert beginSheetModalForWindow:window completionHandler:nil];

    return AlertResult::None;
}

}  // namespace ax
