/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
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

#include "sceneext/SceneExtMacros.h"
#include "axmol/ui/Widget.h"

namespace ax::ext
{

class SCNEXT_API IWidgetEventHandlerResolver
{
public:
    virtual ~IWidgetEventHandlerResolver()                                                         = 0;
    virtual ax::ui::Widget::PointerEventHandler resolvePointerEvent(std::string_view callBackName) = 0;
    virtual ax::ui::Widget::ClickEventHandler resolveClickEvent(std::string_view callBackName)     = 0;
    virtual ax::ui::Widget::WidgetEventCallback resolveEvent(std::string_view callBackName)        = 0;
};

}  // namespace ax::ext
