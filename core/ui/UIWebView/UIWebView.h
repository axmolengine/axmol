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
#pragma once

#include "ui/UIWidget.h"
#include "ui/GUIExport.h"
#include "base/Data.h"

#if (defined(_WIN32) && defined(AX_ENABLE_MSEDGE_WEBVIEW2)) ||                             \
    (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS || \
     AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
/**
 * @addtogroup ui
 * @{
 */

namespace ax
{
namespace ui
{

class WebViewImpl;

/**
 * @brief A View that displays web pages.
 *
 * @note WebView displays web pages base on system widget.
 * It's mean WebView displays web pages above all graphical elements of cocos2d-x.
 * @js NA
 */
class AX_GUI_DLL WebView : public ax::ui::Widget
{
public:
    /**
     * Allocates and initializes a WebView.
     */
    static WebView* create();

    /**
     * Set javascript interface scheme.
     *
     * @see WebView::setOnJSCallback()
     */
    void setJavascriptInterfaceScheme(std::string_view scheme);

    /**
     * Sets the main page contents, MIME type, content encoding, and base URL.
     *
     * @param data The content for the main page.
     * @param MIMEType The MIME type of the data.
     * @param encoding The encoding of the data.
     * @param baseURL The base URL for the content.
     */
    void loadData(const ax::Data& data,
                  std::string_view MIMEType,
                  std::string_view encoding,
                  std::string_view baseURL);

    /**
     * Sets the main page content and base URL.
     *
     * @param string The content for the main page.
     * @param baseURL The base URL for the content.
     */
    void loadHTMLString(std::string_view string, std::string_view baseURL = "");

    /**
     * Loads the given URL. It doesn't clean cached data.
     *
     * @param url Content URL.
     */
    void loadURL(std::string_view url);

    /**
     * Loads the given URL with cleaning cached data or not.
     * @param url Content URL.
     * @cleanCachedData Whether to clean cached data.
     */
    void loadURL(std::string_view url, bool cleanCachedData);

    /**
     * Loads the given fileName.
     *
     * @param fileName Content fileName.
     */
    void loadFile(std::string_view fileName);

    /**
     * Stops the current load.
     */
    void stopLoading();

    /**
     * Reloads the current URL.
     */
    void reload();

    /**
     * Gets whether this WebView has a back history item.
     *
     * @return WebView has a back history item.
     */
    bool canGoBack();

    /**
     * Gets whether this WebView has a forward history item.
     *
     * @return WebView has a forward history item.
     */
    bool canGoForward();

    /**
     * Goes back in the history.
     */
    void goBack();

    /**
     * Goes forward in the history.
     */
    void goForward();

    /**
     * Evaluates JavaScript in the context of the currently displayed page.
     */
    void evaluateJS(std::string_view js);

    /**
     * Set WebView should support zooming. The default value is false.
     */
    void setScalesPageToFit(const bool scalesPageToFit);

    /**
     * Call before a web view begins loading.
     *
     * @param callback The web view that is about to load new content.
     * @return YES if the web view should begin loading content; otherwise, NO.
     */
    void setOnShouldStartLoading(const std::function<bool(WebView* sender, std::string_view url)>& callback);

    /**
     * A callback which will be called when a WebView event happens.
     */
    typedef std::function<void(WebView* sender, std::string_view url)> ccWebViewCallback;

    /**
     * Call after a web view finishes loading.
     *
     * @param callback The web view that has finished loading.
     */
    void setOnDidFinishLoading(const ccWebViewCallback& callback);

    /**
     * Call if a web view failed to load content.
     *
     * @param callback The web view that has failed loading.
     */
    void setOnDidFailLoading(const ccWebViewCallback& callback);

    /**
     * This callback called when load URL that start with javascript interface scheme.
     */
    void setOnJSCallback(const ccWebViewCallback& callback);

    /**
     * Get the callback when WebView is about to start.
     */
    std::function<bool(WebView* sender, std::string_view url)> getOnShouldStartLoading() const;

    /**
     * Get the callback when WebView has finished loading.
     */
    ccWebViewCallback getOnDidFinishLoading() const;

    /**
     * Get the callback when WebView has failed loading.
     */
    ccWebViewCallback getOnDidFailLoading() const;

    /**
     *Get the Javascript callback.
     */
    ccWebViewCallback getOnJSCallback() const;

    /**
     * Set whether the webview bounces at end of scroll of WebView.
     */
    void setBounces(bool bounce);

    virtual void draw(ax::Renderer* renderer, ax::Mat4 const& transform, uint32_t flags) override;

    /**
     * Toggle visibility of WebView.
     */
    virtual void setVisible(bool visible) override;
    /**
     * SetOpacity of webview.
     */
    virtual void setOpacityWebView(float opacity);

    /**
     * getOpacity of webview.
     */
    virtual float getOpacityWebView() const;

    /**
     * set the background transparent
     */
    virtual void setBackgroundTransparent();
    virtual void onEnter() override;
    virtual void onExit() override;

    /**
     * Default constructor.
     */
    WebView();
    virtual ~WebView();

protected:
    virtual ax::ui::Widget* createCloneInstance() override;
    virtual void copySpecialProperties(Widget* model) override;

    std::function<bool(WebView* sender, std::string_view url)> _onShouldStartLoading = nullptr;
    ccWebViewCallback _onDidFinishLoading                                            = nullptr;
    ccWebViewCallback _onDidFailLoading                                              = nullptr;
    ccWebViewCallback _onJSCallback                                                  = nullptr;

private:
    WebViewImpl* _impl = nullptr;
    friend class WebViewImpl;
};

}  // namespace ui
}  // namespace ax

#endif

// end group
/// @}
