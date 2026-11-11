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

#include "UIWebViewTest.h"

#include "axmol/platform/RenderView.h"

#include <algorithm>
#include <iterator>
#include <string>

using namespace ax;
using namespace ax::ui;

namespace
{
// Page loaded by the "Load Data" button: a small page which reflows with the web view instead of using a fixed
// font size, so it stays readable whatever the window size is. It also reports its own viewport, which makes the
// relayout on resize visible.
constexpr std::string_view kInlineHTML =
    "<!DOCTYPE html><html><head><meta charset=\"utf-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<style>"
    "html,body{height:100%;margin:0}"
    "body{display:flex;align-items:center;justify-content:center;padding:4vmin;color:#e8eefc;text-align:center;"
    "font-family:system-ui,-apple-system,'Segoe UI',Roboto,sans-serif;"
    "background:radial-gradient(circle at 30% 20%,#1d3b6e,#0b1220 70%)}"
    ".card{display:flex;flex-direction:column;align-items:center;gap:2.5vmin;max-width:92%;padding:5vmin 6vmin;"
    "border-radius:3vmin;background:rgba(255,255,255,.06)}"
    "h1{margin:0;font-size:7vmin}"
    "p{margin:0;font-size:3.4vmin;line-height:1.5;opacity:.85}"
    "img{width:16vmin;height:16vmin}"
    "#viewport{font-family:ui-monospace,SFMono-Regular,Menlo,monospace;font-size:3vmin;color:#4ac1ff}"
    "@media (max-width:480px){h1{font-size:11vmin}p{font-size:5vmin}#viewport{font-size:4.4vmin}}"
    "</style></head>"
    "<body><main class=\"card\"><h1>Hello World</h1><img src=\"Icon.png\" alt=\"icon\">"
    "<p>loaded with WebView::loadHTMLString()</p><div id=\"viewport\">viewport: <span>?</span></div></main>"
    "<script>"
    "var out=document.querySelector('#viewport span');"
    "function update(){out.textContent=window.innerWidth+' x '+window.innerHeight+' css px, dpr '+"
    "(window.devicePixelRatio||1);}"
    "addEventListener('resize',update);update();"
    "</script></body></html>";
}  // namespace

WebViewTests::WebViewTests()
{
    ADD_TEST_CASE(WebViewTest);
}

bool WebViewTest::init()
{
    if (!UIScene::init())
        return false;

    auto director = Director::getInstance();
    if (director->getRenderView()->getWindowPlatform() == WindowPlatform::Wayland)
    {
        TTFConfig ttfConfig("fonts/arial.ttf", 16);
        auto label = Label::createWithTTF(ttfConfig, "WebView not implemented for Wayland window platform");

        auto size = director->getCanvasSize();
        label->setPosition(Vec2(size.width / 2, size.height / 2));

        addChild(label);
        return true;
    }

    _webView = ui::WebView::create();
    _webView->setScalesPageToFit(true);
    _webView->setOnShouldStartLoading(AX_CALLBACK_2(WebViewTest::onWebViewShouldStartLoading, this));
    _webView->setOnDidFinishLoading(AX_CALLBACK_2(WebViewTest::onWebViewDidFinishLoading, this));
    _webView->setOnDidFailLoading(AX_CALLBACK_2(WebViewTest::onWebViewDidFailLoading, this));
    addChild(_webView);

    // Sits behind the web view, so it shows through when the page background is made transparent.
    _hello = Sprite::create("Hello.png");
    addChild(_hello);

    // URL row: "https://" + input field + "Visit URL", arranged by relayout().
    _httpLabel = Text::create("https://", "Arial", 20);
    _httpLabel->setTextColor(Color32::green);
    _httpLabel->setAnchorPoint(Vec2(1.0f, 0.5f));
    addChild(_httpLabel);

    _urlField = InputField::create("Input a URL here", "Arial", 20);
    _urlField->setPlaceholderColor(Color32::red);
    addChild(_urlField);
    // InputField will enable IME when user interacts with it

    _visitBtn = addButton("Visit URL", [this] {
        const std::string url(_urlField->getString());
        if (!url.empty())
        {
            _webView->loadURL(fmt::format("https://{}", url));
        }
    });

    _rightColumn.push_back(addButton("Reload", [this] { _webView->reload(); }));
    _rightColumn.push_back(addButton("Forward", [this] { _webView->goForward(); }));
    _rightColumn.push_back(addButton("Back", [this] { _webView->goBack(); }));

    _leftColumn.push_back(addButton("Load FILE", [this] { _webView->loadFile("Test.html"); }));
    _leftColumn.push_back(addButton("Load Data", [this] { loadInlineHTML(); }));
    _evalJsBtn = addButton("Evaluate JS", [this] { _webView->evaluateJS("alert(\"hello\")"); }, "evalJs");
    _leftColumn.push_back(_evalJsBtn);
    _opacityBtn = addButton("Opacity 1.0f", [this] { cycleOpacity(); }, "Opacity");
    _leftColumn.push_back(_opacityBtn);
    _leftColumn.push_back(addButton("Transparent BG", [this] { _webView->setBackgroundTransparent(); }, "Transparent"));

    _webView->loadURL("https://axmol.dev");

    relayout();
    return true;
}

void WebViewTest::onEnter()
{
    UIScene::onEnter();

#ifdef AX_PLATFORM_PC
    if (!_windowResizeListener)
    {
        // Keep the layout in sync when the window is resized or toggled to fullscreen.
        _windowResizeListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(
            RenderView::EVENT_WINDOW_RESIZED, [this](CustomEvent*) { relayout(); });
        _windowResizeListener->retain();
    }
#endif

    // The view size may also have changed while this test was not running.
    relayout();
}

void WebViewTest::onExit()
{
    if (_windowResizeListener)
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(_windowResizeListener);
        _windowResizeListener->release();
        _windowResizeListener = nullptr;
    }

    UIScene::onExit();
}

ui::Button* WebViewTest::addButton(std::string_view title, const std::function<void()>& onClick, std::string_view name)
{
    auto button = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
    button->setTitleText(std::string(title));
    if (!name.empty())
    {
        button->setName(name);
    }
    button->addClickEventListener([onClick](Object*) { onClick(); });
    addChild(button);
    return button;
}

void WebViewTest::relayout()
{
    if (!_webView || !_visitBtn || !_urlField)
    {
        return;
    }

    const auto viewSize = Director::getInstance()->getVisibleSize();
    if (viewSize.width <= 0 || viewSize.height <= 0)
    {
        return;
    }

    // The scene lives in design units which are much smaller than the window (cpp-tests uses a 480x320 design),
    // so the insets are proportional with a sane minimum.
    const float margin = std::max(viewSize.width * 0.015f, 4.0f);
    const float gap    = std::max(viewSize.width * 0.012f, 3.0f);
    // cpp-tests draws the test title at the top and the navigation buttons plus the stats at the bottom.
    const float topInset    = std::clamp(viewSize.height * 0.14f, 34.0f, 90.0f);
    const float bottomInset = std::clamp(viewSize.height * 0.17f, 40.0f, 96.0f);

    const auto buttonSize = _visitBtn->getContentSize();
    const float columnW   = buttonSize.width;
    const float rowH      = std::max(_urlField->getContentSize().height, buttonSize.height);

    // The web view fills the middle area, the button columns stay next to it.
    const float urlRowY    = viewSize.height - topInset - rowH * 0.5f;
    const float webTop     = urlRowY - rowH * 0.5f - gap;
    const float webBottom  = bottomInset;
    const float webLeft    = margin + columnW + margin;
    const float webRight   = viewSize.width - margin - columnW - margin;
    const float webWidth   = std::max(webRight - webLeft, 120.0f);
    const float webHeight  = std::max(webTop - webBottom, 80.0f);
    const float webCenterX = (webLeft + webRight) * 0.5f;
    const float webCenterY = webBottom + webHeight * 0.5f;

    _webView->setContentSize(Size(webWidth, webHeight));
    _webView->setPosition(Vec2(webCenterX, webCenterY));

    if (_hello)
    {
        const auto helloSize = _hello->getContentSize();
        const float maxSide  = std::min(webWidth, webHeight) * 0.4f;
        if (helloSize.width > 0 && helloSize.height > 0)
        {
            _hello->setScale(maxSide / std::max(helloSize.width, helloSize.height));
            _hello->setPosition(Vec2(webCenterX, webCenterY));
        }
    }

    // URL row, centered above the web view: [https://][input field][Visit URL]
    const float labelW    = _httpLabel->getContentSize().width;
    const float available = viewSize.width - margin * 2.0f;
    float fieldW          = std::clamp(webWidth * 0.45f, 120.0f, 420.0f);
    fieldW                = std::min(fieldW, std::max(available - labelW - columnW - gap * 2.0f, 80.0f));

    const float rowW = labelW + gap + fieldW + gap + columnW;
    const float rowX = webCenterX - rowW * 0.5f;

    _httpLabel->setPosition(Vec2(rowX, urlRowY));
    _urlField->setContentSize(Size(fieldW, rowH));
    _urlField->setPosition(Vec2(rowX + labelW + gap + fieldW * 0.5f, urlRowY));
    _visitBtn->setPosition(Vec2(rowX + rowW - columnW * 0.5f, urlRowY));

    // Both button columns are stacked from the top edge of the web view. Shrink them when the design area is too
    // short for the natural button size, so that no control ends up outside the visible area.
    const auto maxButtons = static_cast<float>(std::max(_leftColumn.size(), _rightColumn.size()));
    float buttonScale     = 1.0f;
    if (maxButtons > 0.0f)
    {
        const float needed = maxButtons * buttonSize.height + (maxButtons - 1.0f) * gap;
        if (needed > webHeight)
        {
            buttonScale =
                std::clamp((webHeight - (maxButtons - 1.0f) * gap) / (maxButtons * buttonSize.height), 0.4f, 1.0f);
        }
    }

    const float scaledButtonH = buttonSize.height * buttonScale;
    const auto layoutColumn   = [&](const std::vector<ui::Button*>& buttons, float centerX) {
        float y = webTop - scaledButtonH * 0.5f;
        for (auto* button : buttons)
        {
            button->setScale(buttonScale);
            button->setPosition(Vec2(centerX, y));
            y -= scaledButtonH + gap;
        }
    };
    layoutColumn(_leftColumn, margin + columnW * 0.5f);
    layoutColumn(_rightColumn, viewSize.width - margin - columnW * 0.5f);
}

void WebViewTest::loadInlineHTML()
{
    _webView->loadHTMLString(kInlineHTML, "Images/");
}

void WebViewTest::cycleOpacity()
{
    static constexpr float kOpacities[] = {1.0f, 0.5f, 0.0f};

    _opacityIndex++;
    if (_opacityIndex >= static_cast<int>(std::size(kOpacities)))
    {
        _opacityIndex = 0;
    }

    const float opacity = kOpacities[_opacityIndex];
    _webView->setOpacityWebView(opacity);
    if (_opacityBtn)
    {
        _opacityBtn->setTitleText(fmt::format("Opacity {:.1f}f", opacity));
    }
}

bool WebViewTest::onWebViewShouldStartLoading(ui::WebView* sender, std::string_view url)
{
    AXLOGD("onWebViewShouldStartLoading, url is {}", url);
    if (_evalJsBtn)
    {
        _evalJsBtn->setTitleText("Loading...");
    }
    // don't do any OpenGL operation here!! It's forbidden!
    return true;
}

void WebViewTest::onWebViewDidFinishLoading(ui::WebView* sender, std::string_view url)
{
    AXLOGD("onWebViewDidFinishLoading, url is {}", url);
    if (_evalJsBtn)
    {
        _evalJsBtn->setTitleText("Evaluate JS");
    }
}

void WebViewTest::onWebViewDidFailLoading(ui::WebView* sender, std::string_view url)
{
    AXLOGD("onWebViewDidFailLoading, url is {}", url);
    if (_evalJsBtn)
    {
        _evalJsBtn->setTitleText("Evaluate JS");
    }
}
