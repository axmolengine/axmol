/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 Jozef Pridavok
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

#include "axmol/platform/PlatformConfig.h"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)

#    include "axmol/ui/UIEditBox/UIEditBoxImpl-mac.h"
#    include "axmol/base/Director.h"
#    include "axmol/base/text_utils.h"
#    include "axmol/ui/UIEditBox/UIEditBox.h"
#    include "axmol/ui/UIEditBox/Mac/UIEditBoxMac.h"

namespace ax
{

namespace ui
{

EditBoxImpl* __createSystemEditBox(EditBox* pEditBox)
{
    return new EditBoxImplMac(pEditBox);
}

EditBoxImplMac::EditBoxImplMac(EditBox* pEditText) : EditBoxImplCommon(pEditText), _sysEdit(nullptr)
{
    //! TODO: Retina on Mac
    //! _inRetinaMode = [[RenderHostView sharedERenderView] contentScaleFactor] == 2.0f ? true : false;
    _inRetinaMode = false;
}

EditBoxImplMac::~EditBoxImplMac()
{
    [_sysEdit release];
}

void EditBoxImplMac::createNativeControl(const ax::Rect& frame)
{
    auto renderView = ax::Director::getInstance()->getRenderView();
    Size size       = frame.size;
    NSRect rect     = NSMakeRect(0, 0, size.width * renderView->getScaleX(), size.height * renderView->getScaleY());

    float factor = ax::Director::getInstance()->getContentScaleFactor();

    rect.size.width /= factor;
    rect.size.height /= factor;

    _sysEdit = [[UIEditBoxImplMac alloc] initWithFrame:rect editBox:this];
    this->setNativeVisible(false);
}

NSFont* EditBoxImplMac::constructFont(std::string_view fontName, int fontSize)
{
    // [NSString stringWithUTF8String:fontName.data()];
    NSString* fntName  = [[NSString alloc] initWithBytes:fontName.data()
                                                 length:fontName.length()
                                               encoding:NSUTF8StringEncoding];
    fntName            = [[fntName lastPathComponent] stringByDeletingPathExtension];
    float retinaFactor = _inRetinaMode ? 2.0f : 1.0f;
    auto renderView    = ax::Director::getInstance()->getRenderView();
    float scaleFactor  = renderView->getScaleX();

    if (fontSize == -1)
    {
        NSRect frameRect = [_sysEdit.textInput frame];
        fontSize         = frameRect.size.height * 2 / 3;
    }
    else
    {
        fontSize = fontSize * scaleFactor / retinaFactor;
    }

    NSFont* textFont = [NSFont fontWithName:fntName size:fontSize];
    if (textFont == nil)
    {
        textFont = [NSFont systemFontOfSize:fontSize];
    }

    return textFont;
}

void EditBoxImplMac::setNativeFont(std::string_view fontName, int fontSize)
{
    NSFont* textFont = constructFont(fontName, fontSize);
    [_sysEdit setFont:textFont];
}

void EditBoxImplMac::setNativePlaceholderFont(std::string_view fontName, int fontSize)
{
    NSFont* textFont = constructFont(fontName, fontSize);

    if (!textFont)
    {
        AXLOGW("Font not found: {}", fontName);
        return;
    }
    [_sysEdit setPlaceholderFont:textFont];
}

void EditBoxImplMac::setNativeFontColor(const ax::Color32& color)
{
    NSColor* newColor = [NSColor colorWithCalibratedRed:color.r / 255.0f
                                                  green:color.g / 255.0f
                                                   blue:color.b / 255.0f
                                                  alpha:color.a / 255.f];

    [_sysEdit setTextColor:newColor];
}

void EditBoxImplMac::setNativePlaceholderFontColor(const ax::Color32& color)
{
    NSColor* newColor = [NSColor colorWithCalibratedRed:color.r / 255.f
                                                  green:color.g / 255.f
                                                   blue:color.b / 255.f
                                                  alpha:color.a / 255.f];
    [_sysEdit setPlaceholderFontColor:newColor];
}

void EditBoxImplMac::setNativeInputMode(EditBox::InputMode inputMode)
{
    [_sysEdit setInputMode:inputMode];
    auto oldPosition = _editBox->getPosition();
    _editBox->setPosition(_editBox->getPosition() + Vec2(10, 10));
    _editBox->setPosition(oldPosition);
}

void EditBoxImplMac::setNativeMaxLength(int maxLength)
{
    [_sysEdit setMaxLength:maxLength];
}

void EditBoxImplMac::setNativeInputFlag(EditBox::InputFlag inputFlag)
{
    [_sysEdit setInputFlag:inputFlag];
}

void EditBoxImplMac::setNativeReturnType(EditBox::KeyboardReturnType returnType)
{
    [_sysEdit setReturnType:returnType];
}

void EditBoxImplMac::setNativeTextHorizontalAlignment(ax::TextHAlignment alignment)
{
    [_sysEdit setTextHorizontalAlignment:alignment];
}

bool EditBoxImplMac::isEditing()
{
    return [_sysEdit isEditState] ? true : false;
}

void EditBoxImplMac::setNativeText(std::string_view text)
{
    NSString* nsText = [[NSString alloc] initWithBytes:text.data() length:text.length() encoding:NSUTF8StringEncoding];
    [_sysEdit setText:nsText];
}

void EditBoxImplMac::setNativePlaceHolder(std::string_view text)
{
    NSString* nsText = [[NSString alloc] initWithBytes:text.data() length:text.length() encoding:NSUTF8StringEncoding];
    [_sysEdit setPlaceHolder:nsText];
}

void EditBoxImplMac::setNativeVisible(bool visible)
{
    [_sysEdit setVisible:visible];
}

void EditBoxImplMac::updateNativeFrame(const ax::Rect& rect)
{
    RenderView* renderView = Director::getInstance()->getRenderView();
    auto windowSize        = renderView->getWindowSize();
    // Coordinate System on OSX has its origin at the lower left corner.
    //    https://developer.apple.com/library/ios/documentation/General/Conceptual/Devpedia-CocoaApp/CoordinateSystem.html
    auto screenPosY = windowSize.height - rect.origin.y - rect.size.height;
    [_sysEdit updateFrame:CGRectMake(rect.origin.x, screenPosY, rect.size.width, rect.size.height)];
}

std::string_view EditBoxImplMac::getNativeDefaultFontName()
{
    return [[_sysEdit getDefaultFontName] UTF8String];
}

void EditBoxImplMac::nativeOpenKeyboard()
{
    [_sysEdit setVisible:YES];
    [_sysEdit openKeyboard];
}

void EditBoxImplMac::nativeCloseKeyboard()
{
    [_sysEdit closeKeyboard];
}

}  // namespace ui

}  // namespace ax

#endif  // #if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
