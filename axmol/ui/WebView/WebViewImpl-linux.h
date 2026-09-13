/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/platform/PlatformMacros.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX) && defined(AX_HAVE_WEBKIT2GTK)

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

class GTKWebKit;

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
    GTKWebKit* _gtkWebKit;
    WebView* _webView;
};
}  // namespace ui
}  // namespace ax

#endif
