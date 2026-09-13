/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

/// @cond DO_NOT_SHOW

#include <iosfwd>
#include <stdint.h>
#include <string>
#include <string_view>

#include "axmol/platform/PlatformMacros.h"

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

    void loadURL(std::string_view url);
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

    static bool shouldStartLoading(const int viewTag, std::string_view url);
    static void didFinishLoading(const int viewTag, std::string_view url);
    static void didFailLoading(const int viewTag, std::string_view url);
    static void onJsCallback(const int viewTag, std::string_view message);

private:
    int _viewTag;
    WebView* _webView;
};

}  // namespace ui
}  // namespace ax

/// @endcond
