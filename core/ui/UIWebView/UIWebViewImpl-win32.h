/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Author: Jeff Wang <wohaaitinciu@gmail.com>

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

#ifndef __COCOS2D__UI__WEBVIEWIMPL_WIN32_H_
#define __COCOS2D__UI__WEBVIEWIMPL_WIN32_H_

#include "platform/PlatformMacros.h"

#if defined(_WIN32) && defined(AX_ENABLE_MSEDGE_WEBVIEW2)

#    include <string>
#    include "StdC.h"

NS_AX_BEGIN

class Data;
class Renderer;
class Mat4;

namespace ui
{
class WebView;
}

NS_AX_END  // namespace ax

class Win32WebControl;

NS_AX_BEGIN

namespace ui
{

class WebViewImpl
{
public:
    WebViewImpl(ax::ui::WebView* webView);
    virtual ~WebViewImpl();

    void setJavascriptInterfaceScheme(std::string_view scheme);
    void loadData(const ax::Data& data,
                  std::string_view MIMEType,
                  std::string_view encoding,
                  std::string_view baseURL);
    void loadHTMLString(std::string_view string, std::string_view baseURL);
    void loadURL(std::string_view url, bool cleanCachedData);
    void loadFile(std::string_view fileName);
    void stopLoading();
    void reload();
    bool canGoBack();
    bool canGoForward();
    void goBack();
    void goForward();
    void evaluateJS(std::string_view js);
    void setScalesPageToFit(const bool scalesPageToFit);

    virtual void draw(ax::Renderer* renderer, ax::Mat4 const& transform, uint32_t flags);
    virtual void setVisible(bool visible);

    void setBounces(bool bounces);
    void setOpacityWebView(float opacity);
    float getOpacityWebView() const;
    void setBackgroundTransparent();

private:
    bool _createSucceeded;
    Win32WebControl* _systemWebControl;
    WebView* _webView;
};
}  // namespace ui
NS_AX_END  // namespace ax

#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_WIN32

#endif  // __COCOS2D__UI__WEBVIEWIMPL_WIN32_H_
