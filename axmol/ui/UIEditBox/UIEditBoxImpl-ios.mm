/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 James Chen
 Copyright (c) 2013-2015 zilongshanren
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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
#include "axmol/ui/UIEditBox/UIEditBoxImpl-ios.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)

#    define kLabelZOrder 9999

#    include "axmol/ui/UIEditBox/UIEditBox.h"
#    include "axmol/base/Director.h"
#    include "axmol/2d/Label.h"
#    import "axmol/platform/ios/RenderHostView-ios.h"

#    import <Foundation/Foundation.h>
#    import <UIKit/UIKit.h>

#    import "axmol/ui/UIEditBox/iOS/UIEditBoxIOS.h"

#    define getEditBoxImplIOS() ((ax::ui::EditBoxImplIOS*)_editBox)

namespace ax
{

namespace ui
{

EditBoxImpl* __createSystemEditBox(EditBox* pEditBox)
{
    return new EditBoxImplIOS(pEditBox);
}

EditBoxImplIOS::EditBoxImplIOS(EditBox* pEditText) : EditBoxImplCommon(pEditText), _systemControl(nullptr) {}

EditBoxImplIOS::~EditBoxImplIOS()
{
    [_systemControl release];
    _systemControl = nil;
}

void EditBoxImplIOS::createNativeControl(const Rect& frame)
{
    auto renderView = ax::Director::getInstance()->getRenderView();

    Rect rect(0, 0, frame.size.width * renderView->getScaleX(), frame.size.height * renderView->getScaleY());

    float factor = ax::Director::getInstance()->getContentScaleFactor();

    rect.size.width /= factor;
    rect.size.height /= factor;

    _systemControl = [[UIEditBoxImplIOS_objc alloc]
        initWithFrame:CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height)
              editBox:this];
}

bool EditBoxImplIOS::isEditing()
{
    return [_systemControl isEditState] ? true : false;
}

void EditBoxImplIOS::doAnimationWhenKeyboardMove(float duration, float distance)
{
    if ([_systemControl isEditState] || distance < 0.0f)
    {
        [_systemControl doAnimationWhenKeyboardMoveWithDuration:duration distance:distance];
    }
}

void EditBoxImplIOS::setNativeFont(std::string_view fontName, int fontSize)
{
    UIFont* textFont = constructFont(fontName, fontSize);
    if (textFont != nil)
    {
        [_systemControl setFont:textFont];
    }
}

void EditBoxImplIOS::setNativeFontColor(const Color32& color)
{
    _systemControl.textColor = [UIColor colorWithRed:color.r / 255.0f
                                               green:color.g / 255.0f
                                                blue:color.b / 255.0f
                                               alpha:color.a / 255.f];
}

void EditBoxImplIOS::setNativePlaceholderFont(std::string_view fontName, int fontSize)
{
    UIFont* textFont = constructFont(fontName, fontSize);
    if (textFont != nil)
    {
        [_systemControl setPlaceholderFont:textFont];
    }
}

void EditBoxImplIOS::setNativePlaceholderFontColor(const Color32& color)
{
    [_systemControl setPlaceholderTextColor:[UIColor colorWithRed:color.r / 255.0f
                                                            green:color.g / 255.0f
                                                             blue:color.b / 255.0f
                                                            alpha:color.a / 255.f]];
}

void EditBoxImplIOS::setNativeInputMode(EditBox::InputMode inputMode)
{
    [_systemControl setInputMode:inputMode];

    auto oldPos = _editBox->getPosition();
    _editBox->setPosition(oldPos + Vec2(10, 10));
    _editBox->setPosition(oldPos);
}

void EditBoxImplIOS::setNativeInputFlag(EditBox::InputFlag inputFlag)
{
    [_systemControl setInputFlag:inputFlag];
}

NSString* removeSiriString(NSString* str)
{
    NSString* siriString = @"\xef\xbf\xbc";
    return [str stringByReplacingOccurrencesOfString:siriString withString:@""];
}

std::string_view EditBoxImplIOS::getText()
{
    return [removeSiriString(_systemControl.text) UTF8String];
}

void EditBoxImplIOS::setNativeReturnType(EditBox::KeyboardReturnType returnType)
{
    [_systemControl setReturnType:returnType];
}

void EditBoxImplIOS::setNativeTextHorizontalAlignment(ax::TextHAlignment alignment)
{
    [_systemControl setTextHorizontalAlignment:alignment];
}

void EditBoxImplIOS::setNativeText(std::string_view text)
{
    NSString* nsText = [[NSString alloc] initWithBytes:text.data() length:text.length() encoding:NSUTF8StringEncoding];
    if ([nsText compare:_systemControl.text] != NSOrderedSame)
    {
        _systemControl.text = nsText;
    }
}

void EditBoxImplIOS::setNativePlaceHolder(std::string_view text)
{
    NSString* nsText = [[NSString alloc] initWithBytes:text.data() length:text.length() encoding:NSUTF8StringEncoding];
    [_systemControl setPlaceHolder:nsText];
}

void EditBoxImplIOS::setNativeVisible(bool visible)
{
    [_systemControl setVisible:visible];
}

void EditBoxImplIOS::updateNativeFrame(const Rect& rect)
{
    auto renderView = ax::Director::getInstance()->getRenderView();
    auto hostView   = (__bridge RenderHostView*)renderView->getNativeDisplay();

    float factor = hostView.contentScaleFactor;

    [_systemControl updateFrame:CGRectMake(rect.origin.x / factor, rect.origin.y / factor, rect.size.width / factor,
                                           rect.size.height / factor)];
}

std::string_view EditBoxImplIOS::getNativeDefaultFontName()
{
    return [[_systemControl getDefaultFontName] UTF8String];
}

void EditBoxImplIOS::nativeOpenKeyboard()
{
    [_systemControl setVisible:YES];
    [_systemControl openKeyboard];
}

void EditBoxImplIOS::nativeCloseKeyboard()
{
    [_systemControl closeKeyboard];
}

UIFont* EditBoxImplIOS::constructFont(std::string_view fontName, int fontSize)
{
    AXASSERT(!fontName.empty(), "fontName can't be nullptr");
    auto hostView      = static_cast<RenderHostView*>(ax::Director::getInstance()->getRenderView()->getNativeDisplay());
    float retinaFactor = hostView.contentScaleFactor;
    NSString* fntName  = [[NSString alloc] initWithBytes:fontName.data()
                                                 length:fontName.length()
                                               encoding:NSUTF8StringEncoding];
    fntName            = [[fntName lastPathComponent] stringByDeletingPathExtension];

    auto renderView   = ax::Director::getInstance()->getRenderView();
    float scaleFactor = renderView->getScaleX();

    if (fontSize == -1)
    {
        fontSize = _systemControl.frameRect.size.height * 2 / 3;
    }
    else
    {
        fontSize = fontSize * scaleFactor / retinaFactor;
    }

    UIFont* textFont = [UIFont fontWithName:fntName size:fontSize];
    if (textFont == nil)
    {
        textFont = [UIFont systemFontOfSize:fontSize];
    }
    return textFont;
}
}  // namespace ui

}  // namespace ax

#endif /* #if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) */
