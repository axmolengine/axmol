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

using namespace ax;
using namespace ax::ui;

WebViewTests::WebViewTests()
{
    ADD_TEST_CASE(WebViewTest);
}

bool WebViewTest::init()
{
    if (UIScene::init())
    {
        Size winSize = Director::getInstance()->getVisibleSize();

        _webView = ax::ui::WebView::create();
        _webView->setPosition(winSize / 2);
        _webView->setContentSize(winSize * 0.5);
        _webView->loadURL("https://www.baidu.com");
        _webView->setScalesPageToFit(true);

        _webView->setOnShouldStartLoading(AX_CALLBACK_2(WebViewTest::onWebViewShouldStartLoading, this));
        _webView->setOnDidFinishLoading(AX_CALLBACK_2(WebViewTest::onWebViewDidFinishLoading, this));
        _webView->setOnDidFailLoading(AX_CALLBACK_2(WebViewTest::onWebViewDidFailLoading, this));

        this->addChild(_webView);

        auto spriteHello = Sprite::create("Hello.png");
        spriteHello->setPosition(winSize / 2);
        this->addChild(spriteHello);

        TextField* urlTextField = TextField::create("Input a URL here", "Arial", 20);
        urlTextField->setPlaceHolderColor(Color3B::RED);
        urlTextField->setPosition(Vec2(winSize / 2) + Vec2(-80, _webView->getContentSize().height / 2 +
                                                                    urlTextField->getContentSize().height / 2 + 10));
        this->addChild(urlTextField);

        Text* httpLabel = Text::create("https:// ", "Arial", 20);
        httpLabel->setTextColor(Color4B::GREEN);
        httpLabel->setAnchorPoint(Vec2(1.0, 0.5));
        httpLabel->setPosition(urlTextField->getPosition() - Vec2(urlTextField->getContentSize().width / 2, 0));
        this->addChild(httpLabel);

        Button* resetBtn = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
        resetBtn->setTitleText("Visit URL");
        resetBtn->setPosition(Vec2(winSize / 2) + Vec2(50, _webView->getContentSize().height / 2 +
                                                               resetBtn->getContentSize().height / 2 + 10));
        resetBtn->addClickEventListener([this, urlTextField](Object*) {
            if (urlTextField->getString().size() != 0)
            {
                _webView->loadURL(std::string("https://").append(urlTextField->getString()));
            }
        });
        this->addChild(resetBtn);

        Button* reloadBtn = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
        reloadBtn->setTitleText("Reload");
        reloadBtn->setPosition(
            Vec2(winSize / 2) +
            Vec2(_webView->getContentSize().width / 2 + reloadBtn->getContentSize().width / 2 + 10, 50));
        reloadBtn->addClickEventListener([this, reloadBtn](Object*) { _webView->reload(); });
        this->addChild(reloadBtn);

        Button* forwardBtn = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
        forwardBtn->setTitleText("Forward");
        forwardBtn->setPosition(
            Vec2(winSize / 2) +
            Vec2(_webView->getContentSize().width / 2 + forwardBtn->getContentSize().width / 2 + 10, 0));
        forwardBtn->addClickEventListener([this](Object*) { _webView->goForward(); });
        this->addChild(forwardBtn);

        Button* backBtn = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
        backBtn->setTitleText("Back");
        backBtn->setPosition(
            Vec2(winSize / 2) +
            Vec2(_webView->getContentSize().width / 2 + backBtn->getContentSize().width / 2 + 10, -50));
        backBtn->addClickEventListener([this](Object*) { _webView->goBack(); });
        this->addChild(backBtn);

        Button* loadFileBtn = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
        loadFileBtn->setTitleText("Load FILE");
        loadFileBtn->setPosition(
            Vec2(winSize / 2) -
            Vec2(_webView->getContentSize().width / 2 + loadFileBtn->getContentSize().width / 2 + 10, 50));
        loadFileBtn->addClickEventListener([this](Object*) { _webView->loadFile("Test.html"); });
        this->addChild(loadFileBtn);

        Button* loadHTMLBtn = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
        loadHTMLBtn->setTitleText("Load Data");
        loadHTMLBtn->setPosition(
            Vec2(winSize / 2) -
            Vec2(_webView->getContentSize().width / 2 + loadHTMLBtn->getContentSize().width / 2 + 10, 0));
        loadHTMLBtn->addClickEventListener([this](Object*) {
            _webView->loadHTMLString("<body style=\"font-size:50px;\">Hello World <img src=\"Icon.png\"/> </body>",
                                     "Images/");
        });
        this->addChild(loadHTMLBtn);

        Button* evalJsBtn = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
        evalJsBtn->setTitleText("Evaluate JS");
        evalJsBtn->setPosition(
            Vec2(winSize / 2) -
            Vec2(_webView->getContentSize().width / 2 + evalJsBtn->getContentSize().width / 2 + 10, -50));
        evalJsBtn->addClickEventListener([this](Object*) { _webView->evaluateJS("alert(\"hello\")"); });
        evalJsBtn->setName("evalJs");
        this->addChild(evalJsBtn);

        Button* opacityBtn = Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
        opacityBtn->setTitleText("Opacity 1.f");
        opacityBtn->setPosition(
            Vec2(winSize / 2) -
            Vec2(_webView->getContentSize().width / 2 + opacityBtn->getContentSize().width / 2 + 10, 100));
        opacityBtn->addClickEventListener([this, opacityBtn](Object*) {
            auto currentOpacity = _webView->getOpacityWebView();
            if (currentOpacity == 1.f)
            {
                _webView->setOpacityWebView(.5f);
                opacityBtn->setTitleText("Opacity .5f");
            }
            else if (currentOpacity == .5f)
            {
                _webView->setOpacityWebView(0);
                opacityBtn->setTitleText("Opacity 0.f");
            }
            else
            {
                _webView->setOpacityWebView(1.f);
                opacityBtn->setTitleText("Opacity 1.f");
            }
        });
        opacityBtn->setName("Opacity");
        this->addChild(opacityBtn);

        Button* transparentBgBtn =
            Button::create("cocosui/animationbuttonnormal.png", "cocosui/animationbuttonpressed.png");
        transparentBgBtn->setTitleText("Transparent BG");
        transparentBgBtn->setPosition(
            Vec2(winSize / 2) +
            Vec2(_webView->getContentSize().width / 2 + transparentBgBtn->getContentSize().width / 2 + 10, -100));
        transparentBgBtn->addClickEventListener([this](Object*) { _webView->setBackgroundTransparent(); });
        transparentBgBtn->setName("Transparent");
        this->addChild(transparentBgBtn);

        return true;
    }
    return false;
}

bool WebViewTest::onWebViewShouldStartLoading(ui::WebView* sender, std::string_view url)
{
    AXLOGD("onWebViewShouldStartLoading, url is {}", url);
    // don't do any OpenGL operation here!! It's forbidden!
    return true;
}

void WebViewTest::onWebViewDidFinishLoading(ui::WebView* sender, std::string_view url)
{
    auto node = (ui::Button*)this->getChildByName("evalJs");
    node->setTitleText("start loading...");
    AXLOGD("onWebViewDidFinishLoading, url is {}", url);
}

void WebViewTest::onWebViewDidFailLoading(ui::WebView* sender, std::string_view url)
{
    AXLOGD("onWebViewDidFailLoading, url is {}", url);
}
