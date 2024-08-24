/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
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
#include "platform/Device.h"
#include <string.h>
#include <android/log.h>
#include <jni.h>
#include "base/Types.h"
#include "platform/android/jni/JniHelper.h"
#include "platform/FileUtils.h"
#include "yasio/string_view.hpp"

static const char* deviceHelperClassName = "org.axmol.lib.AxmolEngine";

namespace ax
{

int Device::getDPI()
{
    static int dpi = -1;
    if (dpi == -1)
    {
        dpi = JniHelper::callStaticIntMethod(deviceHelperClassName, "getDPI");
    }
    return dpi;
}

float Device::getPixelRatio()
{
    // refer to: https://developer.android.com/training/multiscreen/screendensities?hl=en-us
    return Device::getDPI() / 160.0f;
}

void Device::setAccelerometerEnabled(bool isEnabled)
{
    if (isEnabled)
    {
        JniHelper::callStaticVoidMethod(deviceHelperClassName, "enableAccelerometer");
    }
    else
    {
        JniHelper::callStaticVoidMethod(deviceHelperClassName, "disableAccelerometer");
    }
}

void Device::setAccelerometerInterval(float interval)
{
    JniHelper::callStaticVoidMethod(deviceHelperClassName, "setAccelerometerInterval", interval);
}

class BitmapDC
{
public:
    BitmapDC() : _width(0), _height(0), _data(nullptr) {}

    ~BitmapDC() {}

    bool getBitmapFromJavaShadowStroke(std::string_view text,
                                       int nWidth,
                                       int nHeight,
                                       Device::TextAlign eAlignMask,
                                       const FontDefinition& textDefinition)
    {
        JniMethodInfo methodInfo;
        if (!JniHelper::getStaticMethodInfo(methodInfo, "org.axmol.lib.BitmapHelper",
                                            "createTextBitmapShadowStroke",
                                            "([BLjava/lang/String;IIIIIIIIZFFFFZIIIIFZI)Z"))
        {
            AXLOGE("{} {}: error to get methodInfo", __FILE__, __LINE__);
            return false;
        }

        // Do a full lookup for the font path using FileUtils in case the given font name is a relative path to a font
        // file asset, or the path has been mapped to a different location in the app package:
        std::string fullPathOrFontName = textDefinition._fontName;
        if (FileUtils::getInstance()->isFileExist(fullPathOrFontName))
        {
            fullPathOrFontName = FileUtils::getInstance()->fullPathForFilename(textDefinition._fontName);
            // If the path name returned includes the 'assets' dir then that needs to be removed, because the
            // android.content.Context requires this portion of the path to be omitted for assets inside the app
            // package.
            using namespace cxx17;
            if (cxx20::starts_with(cxx17::string_view{fullPathOrFontName}, "assets/"_sv))
            {
                fullPathOrFontName =
                    fullPathOrFontName.substr(sizeof("assets/") - 1);  // Chop out the 'assets/' portion of the path.
            }
        }

        /**create bitmap
         * this method call Cococs2dx.createBitmap()(java code) to create the bitmap, the java code
         * will call Java_org_axmol_lib_BitmapHelper_nativeInitBitmapDC() to init the width, height
         * and data.
         * use this approach to decrease the jni call number
         */
        int count           = static_cast<int>(text.length());
        jbyteArray strArray = methodInfo.env->NewByteArray(count);
        methodInfo.env->SetByteArrayRegion(strArray, 0, count, reinterpret_cast<const jbyte*>(text.data()));
        jstring jstrFont = methodInfo.env->NewStringUTF(fullPathOrFontName.c_str());

        if (!methodInfo.env->CallStaticBooleanMethod(
                methodInfo.classID, methodInfo.methodID, strArray, jstrFont, textDefinition._fontSize,
                textDefinition._fontFillColor.r, textDefinition._fontFillColor.g, textDefinition._fontFillColor.b,
                textDefinition._fontAlpha, eAlignMask, nWidth, nHeight, textDefinition._shadow._shadowEnabled,
                textDefinition._shadow._shadowOffset.width, -textDefinition._shadow._shadowOffset.height,
                textDefinition._shadow._shadowBlur, textDefinition._shadow._shadowOpacity,
                textDefinition._stroke._strokeEnabled, textDefinition._stroke._strokeColor.r,
                textDefinition._stroke._strokeColor.g, textDefinition._stroke._strokeColor.b,
                textDefinition._stroke._strokeAlpha, textDefinition._stroke._strokeSize, textDefinition._enableWrap,
                textDefinition._overflow))
        {
            return false;
        }

        methodInfo.env->DeleteLocalRef(strArray);
        methodInfo.env->DeleteLocalRef(jstrFont);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);

        return true;
    }

public:
    int _width;
    int _height;
    unsigned char* _data;
};

static BitmapDC& sharedBitmapDC()
{
    static BitmapDC s_BmpDC;
    return s_BmpDC;
}

Data Device::getTextureDataForText(std::string_view text,
                                   const FontDefinition& textDefinition,
                                   TextAlign align,
                                   int& width,
                                   int& height,
                                   bool& hasPremultipliedAlpha)
{
    Data ret;
    do
    {
        BitmapDC& dc = sharedBitmapDC();

        if (!dc.getBitmapFromJavaShadowStroke(text, (int)textDefinition._dimensions.width,
                                              (int)textDefinition._dimensions.height, align, textDefinition))
        {
            break;
        };

        width  = dc._width;
        height = dc._height;
        ret.fastSet(dc._data, width * height * 4);
        hasPremultipliedAlpha = true;
    } while (0);

    return ret;
}

void Device::setKeepScreenOn(bool value)
{
    JniHelper::callStaticVoidMethod(deviceHelperClassName, "setKeepScreenOn", value);
}

void Device::vibrate(float duration)
{
    JniHelper::callStaticVoidMethod(deviceHelperClassName, "vibrate", duration);
}

void Device::prepareImpactFeedbackGenerator(ImpactFeedbackStyle style) {}

void Device::impactOccurred(ImpactFeedbackStyle style)
{
    JniHelper::callStaticVoidMethod(deviceHelperClassName, "impactOccurred", (int)style);
}

void Device::prepareNotificationFeedbackGenerator() {}

void Device::notificationOccurred(NotificationFeedbackType type)
{
    JniHelper::callStaticVoidMethod(deviceHelperClassName, "notificationOccurred", (int)type);
}

void Device::prepareSelectionFeedbackGenerator() {}

void Device::selectionChanged()
{
    JniHelper::callStaticVoidMethod(deviceHelperClassName, "selectionChanged");
}

}

// this method is called by BitmapHelper
extern "C" {
/**
 * this method is called by java code to init width, height and pixels data
 */
JNIEXPORT void JNICALL
Java_org_axmol_lib_BitmapHelper_nativeInitBitmapDC(JNIEnv* env, jclass, jint width, jint height, jbyteArray pixels)
{
    int size                    = width * height * 4;
    ax::BitmapDC& bitmapDC = ax::sharedBitmapDC();
    bitmapDC._width             = width;
    bitmapDC._height            = height;
    bitmapDC._data              = (unsigned char*)malloc(sizeof(unsigned char) * size);
    env->GetByteArrayRegion(pixels, 0, size, (jbyte*)bitmapDC._data);
}
};
