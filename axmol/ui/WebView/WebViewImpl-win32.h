/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Author: Jeff Wang <wohaaitinciu@gmail.com>
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/platform/PlatformMacros.h"

#if defined(_WIN32) && defined(AX_ENABLE_MSEDGE_WEBVIEW2)

#    include <string>
#    include "axmol/platform/StdC.h"

namespace ax
{

class Data;
class Renderer;
class Mat4;

namespace ui
{
class WebView;
}

}  // namespace ax

class Win32WebControl;

namespace ax
{

namespace ui
{

class WebViewImpl
{
public:
    WebViewImpl(ax::ui::WebView* webView);
    virtual ~WebViewImpl();

    void setJavascriptInterfaceScheme(std::string_view scheme);
    void loadData(const ax::Data& data, std::string_view MIMEType, std::string_view encoding, std::string_view baseURL);
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
}  // namespace ax

#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
