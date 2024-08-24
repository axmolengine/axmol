/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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
#include "ui/UIWebView/UIWebViewImpl-android.h"

#include <unordered_map>
#include <stdlib.h>
#include <string>
#include "platform/android/jni/JniHelper.h"

#include "ui/UIWebView/UIWebView.h"
#include "platform/GLView.h"
#include "base/Director.h"
#include "platform/FileUtils.h"
#include "ui/UIHelper.h"
#include "yasio/string_view.hpp"

static const char* className = "org.axmol.lib.WebViewHelper";

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "", __VA_ARGS__)

static constexpr std::string_view s_assetsBaseUrl = "file:///android_asset/"sv;
static constexpr std::string_view s_sdRootBaseUrl  = "file://"sv;

static std::string getFixedBaseUrl(std::string_view baseUrl)
{
    std::string fixedBaseUrl;
    if (baseUrl.empty())
    {
        fixedBaseUrl = s_assetsBaseUrl;
    }
    else if (baseUrl.find(s_sdRootBaseUrl) != std::string::npos)
    {
        fixedBaseUrl = baseUrl;
    }
    else if (baseUrl[0] != '/')
    {
        using namespace cxx17;  // for cxx17::string_view literal
        if (cxx20::starts_with(baseUrl, "assets/"_sv))
        {
            fixedBaseUrl.assign(s_assetsBaseUrl).push_back(baseUrl[7]);
        }
        else
        {
            fixedBaseUrl.assign(s_assetsBaseUrl).append(baseUrl);
        }
    }
    else
    {
        fixedBaseUrl.assign(s_sdRootBaseUrl).append(baseUrl);
    }

    if (fixedBaseUrl.c_str()[fixedBaseUrl.length() - 1] != '/')
    {
        fixedBaseUrl.push_back('/');
    }

    return fixedBaseUrl;
}

extern "C" {
/*
 * Class:     org_axmol_lib_WebViewHelper
 * Method:    shouldStartLoading
 * Signature: (ILjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_org_axmol_lib_WebViewHelper_shouldStartLoading(JNIEnv* env,
                                                                                          jclass,
                                                                                          jint index,
                                                                                          jstring jurl)
{
    auto charUrl    = env->GetStringUTFChars(jurl, NULL);
    std::string url = charUrl;
    env->ReleaseStringUTFChars(jurl, charUrl);
    return ax::ui::WebViewImpl::shouldStartLoading(index, url);
}

/*
 * Class:     org_axmol_lib_WebViewHelper
 * Method:    didFinishLoading
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_axmol_lib_WebViewHelper_didFinishLoading(JNIEnv* env,
                                                                                    jclass,
                                                                                    jint index,
                                                                                    jstring jurl)
{
    // LOGD("didFinishLoading");
    auto charUrl    = env->GetStringUTFChars(jurl, NULL);
    std::string url = charUrl;
    env->ReleaseStringUTFChars(jurl, charUrl);
    ax::ui::WebViewImpl::didFinishLoading(index, url);
}

/*
 * Class:     org_axmol_lib_WebViewHelper
 * Method:    didFailLoading
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_axmol_lib_WebViewHelper_didFailLoading(JNIEnv* env,
                                                                                  jclass,
                                                                                  jint index,
                                                                                  jstring jurl)
{
    // LOGD("didFailLoading");
    auto charUrl    = env->GetStringUTFChars(jurl, NULL);
    std::string url = charUrl;
    env->ReleaseStringUTFChars(jurl, charUrl);
    ax::ui::WebViewImpl::didFailLoading(index, url);
}

/*
 * Class:     org_axmol_lib_WebViewHelper
 * Method:    onJsCallback
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_axmol_lib_WebViewHelper_onJsCallback(JNIEnv* env,
                                                                                jclass,
                                                                                jint index,
                                                                                jstring jmessage)
{
    // LOGD("jsCallback");
    auto charMessage    = env->GetStringUTFChars(jmessage, NULL);
    std::string message = charMessage;
    env->ReleaseStringUTFChars(jmessage, charMessage);
    ax::ui::WebViewImpl::onJsCallback(index, message);
}
}

namespace
{

int createWebViewJNI()
{
    ax::JniMethodInfo t;
    if (ax::JniHelper::getStaticMethodInfo(t, className, "createWebView", "()I"))
    {
        // LOGD("error: %s,%d",__func__,__LINE__);
        jint viewTag = t.env->CallStaticIntMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
        return viewTag;
    }
    return -1;
}

std::string getUrlStringByFileName(std::string_view fileName)
{
    // LOGD("error: %s,%d",__func__,__LINE__);
    std::string urlString = ax::FileUtils::getInstance()->fullPathForFilename(fileName);
    if (urlString.empty())
        return urlString;

    if (urlString[0] == '/')
        urlString.insert(urlString.begin(), s_sdRootBaseUrl.begin(), s_sdRootBaseUrl.end());
    else if (cxx20::starts_with(cxx17::string_view{urlString}, "assets/"sv))
        urlString.replace(0, sizeof("assets/") - 1, s_assetsBaseUrl);
    else
        urlString.insert(urlString.begin(), s_assetsBaseUrl.begin(), s_assetsBaseUrl.end());

    return urlString;
}
}  // namespace

namespace ax
{

namespace ui
{

static std::unordered_map<int, ax::ui::WebViewImpl*> s_WebViewImpls;

WebViewImpl::WebViewImpl(WebView* webView) : _viewTag(-1), _webView(webView)
{
    _viewTag                 = createWebViewJNI();
    s_WebViewImpls[_viewTag] = this;
}

WebViewImpl::~WebViewImpl()
{
    JniHelper::callStaticVoidMethod(className, "removeWebView", _viewTag);
    s_WebViewImpls.erase(_viewTag);
}

void WebViewImpl::loadData(const Data& data,
                           std::string_view MIMEType,
                           std::string_view encoding,
                           std::string_view baseURL)
{
    std::string dataString(reinterpret_cast<char*>(data.getBytes()), static_cast<unsigned int>(data.getSize()));
    JniHelper::callStaticVoidMethod(className, "loadData", _viewTag, dataString, MIMEType, encoding,
                                    baseURL);
}

void WebViewImpl::loadHTMLString(std::string_view string, std::string_view baseURL)
{
    JniHelper::callStaticVoidMethod(className, "loadHTMLString", _viewTag, string, getFixedBaseUrl(baseURL));
}

void WebViewImpl::loadURL(std::string_view url)
{
    this->loadURL(url, false);
}

void WebViewImpl::loadURL(std::string_view url, bool cleanCachedData)
{
    JniHelper::callStaticVoidMethod(className, "loadUrl", _viewTag, url, cleanCachedData);
}

void WebViewImpl::loadFile(std::string_view fileName)
{
    auto fullPath = getUrlStringByFileName(fileName);
    JniHelper::callStaticVoidMethod(className, "loadFile", _viewTag, fullPath);
}

void WebViewImpl::stopLoading()
{
    JniHelper::callStaticVoidMethod(className, "stopLoading", _viewTag);
}

void WebViewImpl::reload()
{
    JniHelper::callStaticVoidMethod(className, "reload", _viewTag);
}

bool WebViewImpl::canGoBack()
{
    return JniHelper::callStaticBooleanMethod(className, "canGoBack", _viewTag);
}

bool WebViewImpl::canGoForward()
{
    return JniHelper::callStaticBooleanMethod(className, "canGoForward", _viewTag);
}

void WebViewImpl::goBack()
{
    JniHelper::callStaticVoidMethod(className, "goBack", _viewTag);
}

void WebViewImpl::goForward()
{
    JniHelper::callStaticVoidMethod(className, "goForward", _viewTag);
}

void WebViewImpl::setJavascriptInterfaceScheme(std::string_view scheme)
{
    JniHelper::callStaticVoidMethod(className, "setJavascriptInterfaceScheme", _viewTag, scheme);
}

void WebViewImpl::evaluateJS(std::string_view js)
{
    JniHelper::callStaticVoidMethod(className, "evaluateJS", _viewTag, js);
}

void WebViewImpl::setScalesPageToFit(const bool scalesPageToFit)
{
    JniHelper::callStaticVoidMethod(className, "setScalesPageToFit", _viewTag, scalesPageToFit);
}

bool WebViewImpl::shouldStartLoading(const int viewTag, std::string_view url)
{
    bool allowLoad = true;
    auto it        = s_WebViewImpls.find(viewTag);
    if (it != s_WebViewImpls.end())
    {
        auto webView = it->second->_webView;
        if (webView->_onShouldStartLoading)
        {
            allowLoad = webView->_onShouldStartLoading(webView, url);
        }
    }
    return allowLoad;
}

void WebViewImpl::didFinishLoading(const int viewTag, std::string_view url)
{
    auto it = s_WebViewImpls.find(viewTag);
    if (it != s_WebViewImpls.end())
    {
        auto webView = it->second->_webView;
        if (webView->_onDidFinishLoading)
        {
            webView->_onDidFinishLoading(webView, url);
        }
    }
}

void WebViewImpl::didFailLoading(const int viewTag, std::string_view url)
{
    auto it = s_WebViewImpls.find(viewTag);
    if (it != s_WebViewImpls.end())
    {
        auto webView = it->second->_webView;
        if (webView->_onDidFailLoading)
        {
            webView->_onDidFailLoading(webView, url);
        }
    }
}

void WebViewImpl::onJsCallback(const int viewTag, std::string_view message)
{
    auto it = s_WebViewImpls.find(viewTag);
    if (it != s_WebViewImpls.end())
    {
        auto webView = it->second->_webView;
        if (webView->_onJSCallback)
        {
            webView->_onJSCallback(webView, message);
        }
    }
}

void WebViewImpl::draw(ax::Renderer* renderer, ax::Mat4 const& transform, uint32_t flags)
{
    if (flags & ax::Node::FLAGS_TRANSFORM_DIRTY)
    {
        auto uiRect = ax::ui::Helper::convertBoundingBoxToScreen(_webView);
        JniHelper::callStaticVoidMethod(className, "setWebViewRect", _viewTag, (int)uiRect.origin.x,
                                        (int)uiRect.origin.y, (int)uiRect.size.width, (int)uiRect.size.height);
    }
}

void WebViewImpl::setVisible(bool visible)
{
    JniHelper::callStaticVoidMethod(className, "setVisible", _viewTag, visible);
}

void WebViewImpl::setOpacityWebView(const float opacity)
{
    JniHelper::callStaticVoidMethod(className, "setOpacityWebView", _viewTag, opacity);
};

float WebViewImpl::getOpacityWebView() const
{
    return JniHelper::callStaticFloatMethod(className, "getOpacityWebView", _viewTag);
};

void WebViewImpl::setBackgroundTransparent()
{
    JniHelper::callStaticVoidMethod(className, "setBackgroundTransparent", _viewTag);
};

void WebViewImpl::setBounces(bool bounces)
{
    // empty function as this was mainly a fix for iOS
}
}  // namespace ui
}  // namespace ax
