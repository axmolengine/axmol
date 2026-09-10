/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once
/// @cond DO_NOT_SHOW

#include <stdint.h>
#include <iosfwd>
#include <string>
#include <string_view>

#include "axmol/platform/PlatformMacros.h"

@class UIWebViewWrapper;

namespace ax
{

class Data;
class Renderer;
class Mat4;

namespace ui
{

class WebView;

class WebViewImpl
{
public:
    WebViewImpl(WebView* webView);

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

    virtual void setOpacityWebView(float opacity);

    virtual float getOpacityWebView() const;

    virtual void setBackgroundTransparent();

private:
    UIWebViewWrapper* _uiWebViewWrapper;
    WebView* _webView;
};

}  // namespace ui
}  // namespace ax

/// @endcond
