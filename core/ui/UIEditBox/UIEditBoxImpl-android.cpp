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

#include "ui/UIEditBox/UIEditBoxImpl-android.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)

#    include "ui/UIEditBox/UIEditBox.h"
#    include <jni.h>
#    include "platform/android/jni/JniHelper.h"
#    include "2d/Label.h"
#    include "base/UTF8.h"
#    include "math/Vec2.h"
#    include "ui/UIHelper.h"
#    include "base/Director.h"
#    include "platform/FileUtils.h"
#    include "yasio/string_view.hpp"

namespace ax
{

static const char* editBoxClassName = "org.axmol.lib.EditBoxHelper";

namespace ui
{

#    define LOGD(...) __android_log_print(ANDROID_LOG_ERROR, "", __VA_ARGS__)
static void editBoxEditingDidBegin(int index);
static void editBoxEditingDidChanged(int index, std::string_view text);
static void editBoxEditingDidEnd(int index, std::string_view text, int action);
extern "C" {
JNIEXPORT void JNICALL Java_org_axmol_lib_EditBoxHelper_editBoxEditingDidBegin(JNIEnv*, jclass, jint index)
{
    editBoxEditingDidBegin(index);
}

JNIEXPORT void JNICALL Java_org_axmol_lib_EditBoxHelper_editBoxEditingChanged(JNIEnv* env,
                                                                                         jclass,
                                                                                         jint index,
                                                                                         jstring text)
{
    std::string textString = StringUtils::getStringUTFCharsJNI(env, text);
    editBoxEditingDidChanged(index, textString);
}

JNIEXPORT void JNICALL Java_org_axmol_lib_EditBoxHelper_editBoxEditingDidEnd(JNIEnv* env,
                                                                                        jclass,
                                                                                        jint index,
                                                                                        jstring text,
                                                                                        jint action)
{
    std::string textString = StringUtils::getStringUTFCharsJNI(env, text);
    editBoxEditingDidEnd(index, textString, action);
}
}

static std::unordered_map<int, EditBoxImplAndroid*> s_allEditBoxes;

EditBoxImpl* __createSystemEditBox(EditBox* editBox)
{
    return new EditBoxImplAndroid(editBox);
}

EditBoxImplAndroid::EditBoxImplAndroid(EditBox* pEditText) : EditBoxImplCommon(pEditText), _editBoxIndex(-1) {}

EditBoxImplAndroid::~EditBoxImplAndroid()
{
    s_allEditBoxes.erase(_editBoxIndex);
    JniHelper::callStaticVoidMethod(editBoxClassName, "removeEditBox", _editBoxIndex);
}

void EditBoxImplAndroid::createNativeControl(const Rect& frame)
{
    auto director  = ax::Director::getInstance();
    auto glView    = director->getGLView();
    auto frameSize = glView->getFrameSize();

    auto winSize    = director->getWinSize();
    auto leftBottom = _editBox->convertToWorldSpace(Point::ZERO);

    auto contentSize = frame.size;
    auto rightTop    = _editBox->convertToWorldSpace(Point(contentSize.width, contentSize.height));

    auto uiLeft   = frameSize.width / 2 + (leftBottom.x - winSize.width / 2) * glView->getScaleX();
    auto uiTop    = frameSize.height / 2 - (rightTop.y - winSize.height / 2) * glView->getScaleY();
    auto uiWidth  = (rightTop.x - leftBottom.x) * glView->getScaleX();
    auto uiHeight = (rightTop.y - leftBottom.y) * glView->getScaleY();
    LOGD("scaleX = %f", glView->getScaleX());
    _editBoxIndex = JniHelper::callStaticIntMethod(editBoxClassName, "createEditBox", (int)uiLeft, (int)uiTop,
                                                   (int)uiWidth, (int)uiHeight, (float)glView->getScaleX());
    s_allEditBoxes[_editBoxIndex] = this;
}

void EditBoxImplAndroid::setNativeFont(const char* pFontName, int fontSize)
{
    auto director            = ax::Director::getInstance();
    auto glView              = director->getGLView();
    auto isFontFileExists    = ax::FileUtils::getInstance()->isFileExist(pFontName);
    std::string realFontPath = pFontName;
    if (isFontFileExists)
    {
        realFontPath = ax::FileUtils::getInstance()->fullPathForFilename(pFontName);
        using namespace cxx17;  // for cxx17::string_view literal
        if (cxx20::starts_with(cxx17::string_view{realFontPath}, "assets/"_sv))
        {
            realFontPath = realFontPath.substr(sizeof("assets/") - 1);  // Chop out the 'assets/' portion of the path.
        }
    }
    JniHelper::callStaticVoidMethod(editBoxClassName, "setFont", _editBoxIndex, realFontPath,
                                    (float)fontSize * glView->getScaleX());
}

void EditBoxImplAndroid::setNativeFontColor(const Color4B& color)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setFontColor", _editBoxIndex, (int)color.r, (int)color.g,
                                    (int)color.b, (int)color.a);
}

void EditBoxImplAndroid::setNativePlaceholderFont(const char* pFontName, int fontSize)
{
    AXLOGD("Warning! You can't change Android Hint fontName and fontSize");
}

void EditBoxImplAndroid::setNativePlaceholderFontColor(const Color4B& color)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setPlaceHolderTextColor", _editBoxIndex, (int)color.r,
                                    (int)color.g, (int)color.b, (int)color.a);
}

void EditBoxImplAndroid::setNativeInputMode(EditBox::InputMode inputMode)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setInputMode", _editBoxIndex, static_cast<int>(inputMode));
}

void EditBoxImplAndroid::setNativeMaxLength(int maxLength)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setMaxLength", _editBoxIndex, maxLength);
}

void EditBoxImplAndroid::setNativeInputFlag(EditBox::InputFlag inputFlag)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setInputFlag", _editBoxIndex, static_cast<int>(inputFlag));
}

void EditBoxImplAndroid::setNativeReturnType(EditBox::KeyboardReturnType returnType)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setReturnType", _editBoxIndex, static_cast<int>(returnType));
}

void EditBoxImplAndroid::setNativeTextHorizontalAlignment(ax::TextHAlignment alignment)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setTextHorizontalAlignment", _editBoxIndex,
                                    static_cast<int>(alignment));
}

bool EditBoxImplAndroid::isEditing()
{
    return false;
}

void EditBoxImplAndroid::setNativeText(const char* pText)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setText", _editBoxIndex, pText);
}

void EditBoxImplAndroid::setNativePlaceHolder(const char* pText)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setPlaceHolderText", _editBoxIndex, pText);
}

void EditBoxImplAndroid::setNativeVisible(bool visible)
{  // don't need to be implemented on android platform.
    JniHelper::callStaticVoidMethod(editBoxClassName, "setVisible", _editBoxIndex, visible);
}

void EditBoxImplAndroid::updateNativeFrame(const Rect& rect)
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "setEditBoxViewRect", _editBoxIndex, (int)rect.origin.x,
                                    (int)rect.origin.y, (int)rect.size.width, (int)rect.size.height);
}

void EditBoxImplAndroid::nativeOpenKeyboard()
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "openKeyboard", _editBoxIndex);
}

void EditBoxImplAndroid::nativeCloseKeyboard()
{
    JniHelper::callStaticVoidMethod(editBoxClassName, "closeKeyboard", _editBoxIndex);
}

void editBoxEditingDidBegin(int index)
{
    auto it = s_allEditBoxes.find(index);
    if (it != s_allEditBoxes.end())
    {
        s_allEditBoxes[index]->editBoxEditingDidBegin();
    }
}
void editBoxEditingDidChanged(int index, std::string_view text)
{
    auto it = s_allEditBoxes.find(index);
    if (it != s_allEditBoxes.end())
    {
        s_allEditBoxes[index]->editBoxEditingChanged(text);
    }
}

void editBoxEditingDidEnd(int index, std::string_view text, int action)
{
    auto it = s_allEditBoxes.find(index);
    if (it != s_allEditBoxes.end())
    {
        s_allEditBoxes[index]->editBoxEditingDidEnd(
            text, static_cast<ax::ui::EditBoxDelegate::EditBoxEndAction>(action));
    }
}

const char* EditBoxImplAndroid::getNativeDefaultFontName()
{
    return "sans-serif";
}

}  // namespace ui

}

#undef LOGD

#endif /* #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) */
