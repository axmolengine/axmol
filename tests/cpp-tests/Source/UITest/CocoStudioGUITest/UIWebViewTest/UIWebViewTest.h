/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "../UIScene.h"
#include "axmol/ui/WebView/WebView.h"

#include <functional>
#include <string_view>
#include <vector>

DEFINE_TEST_SUITE(WebViewTests);

class WebViewTest : public UIScene
{
public:
    CREATE_FUNC(WebViewTest);

    bool init() override;
    void onEnter() override;
    void onExit() override;

    bool onWebViewShouldStartLoading(ax::ui::WebView* sender, std::string_view url);
    void onWebViewDidFinishLoading(ax::ui::WebView* sender, std::string_view url);
    void onWebViewDidFailLoading(ax::ui::WebView* sender, std::string_view url);

private:
    /**
     * @brief Create a button with a click handler.
     *
     * The button is added to the scene and afterwards only moved by relayout(), so the layout can follow the
     * view size without rebuilding the controls.
     */
    ax::ui::Button* addButton(std::string_view title, const std::function<void()>& onClick, std::string_view name = {});

    /**
     * @brief Arrange the web view, the URL row and the button columns for the current view size.
     *
     * The web view takes the free space in the middle, the buttons are stacked in the columns left and right of
     * it, and the URL row is centered above it. Called on init, on re-enter and whenever the window is resized
     * or switched between windowed and fullscreen.
     */
    void relayout();
    void loadInlineHTML();
    void cycleOpacity();

    ax::ui::WebView* _webView{nullptr};
    ax::Sprite* _hello{nullptr};
    ax::ui::Text* _httpLabel{nullptr};
    ax::ui::InputField* _urlField{nullptr};
    ax::ui::Button* _visitBtn{nullptr};
    ax::ui::Button* _evalJsBtn{nullptr};
    ax::ui::Button* _opacityBtn{nullptr};

    // Buttons of the left and the right column, in top to bottom order.
    std::vector<ax::ui::Button*> _leftColumn;
    std::vector<ax::ui::Button*> _rightColumn;

    int _opacityIndex{0};
    ax::EventListenerCustom* _windowResizeListener{nullptr};
};
