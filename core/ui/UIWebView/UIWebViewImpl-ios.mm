/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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
#import <WebKit/WKWebView.h>
#import <WebKit/WKUIDelegate.h>
#import <WebKit/WKNavigationDelegate.h>

#include "ui/UIWebView/UIWebViewImpl-ios.h"
#include "ui/UIWebView/UIWebView.h"
#include "renderer/Renderer.h"
#include "base/Director.h"
#include "platform/GLView.h"
#include "platform/ios/EAGLView-ios.h"
#include "platform/FileUtils.h"

@interface UIWebViewWrapper : NSObject
@property(nonatomic) std::function<bool(std::string url)> shouldStartLoading;
@property(nonatomic) std::function<void(std::string url)> didFinishLoading;
@property(nonatomic) std::function<void(std::string url)> didFailLoading;
@property(nonatomic) std::function<void(std::string url)> onJsCallback;

@property(nonatomic, readonly, getter=canGoBack) BOOL canGoBack;
@property(nonatomic, readonly, getter=canGoForward) BOOL canGoForward;

+ (instancetype)newWebViewWrapper;

- (void)setVisible:(bool)visible;

- (void)setBounces:(bool)bounces;

- (void)setOpacityWebView:(float)opacity;

- (float)getOpacityWebView;

- (void)setBackgroundTransparent;

- (void)setFrameWithX:(float)x y:(float)y width:(float)width height:(float)height;

- (void)setJavascriptInterfaceScheme:(std::string_view)scheme;

- (void)loadData:(std::string_view)data
            MIMEType:(std::string_view)MIMEType
    textEncodingName:(std::string_view)encodingName
             baseURL:(std::string_view)baseURL;

- (void)loadHTMLString:(std::string_view)string baseURL:(std::string_view)baseURL;

- (void)loadUrl:(std::string_view)urlString cleanCachedData:(BOOL)needCleanCachedData;

- (void)loadFile:(std::string_view)filePath;

- (void)stopLoading;

- (void)reload;

- (void)evaluateJS:(std::string_view)js;

- (void)goBack;

- (void)goForward;

- (void)setScalesPageToFit:(const bool)scalesPageToFit;
@end

@interface UIWebViewWrapper () <WKUIDelegate, WKNavigationDelegate>
@property(nonatomic) WKWebView* wkWebView;

@property(nonatomic, copy) NSString* jsScheme;
@end

@implementation UIWebViewWrapper {
}

+ (instancetype)newWebViewWrapper
{
    return [[self alloc] init];
}

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        self.wkWebView          = nil;
        self.shouldStartLoading = nullptr;
        self.didFinishLoading   = nullptr;
        self.didFailLoading     = nullptr;
    }
    return self;
}

- (void)dealloc
{
    self.wkWebView.UIDelegate         = nil;
    self.wkWebView.navigationDelegate = nil;
    [self.wkWebView removeFromSuperview];
    [self.wkWebView release];
    self.wkWebView = nil;
    self.jsScheme  = nil;
    [super dealloc];
}

- (void)setupWebView
{
    if (!self.wkWebView)
    {
        self.wkWebView                    = [[WKWebView alloc] init];
        self.wkWebView.UIDelegate         = self;
        self.wkWebView.navigationDelegate = self;
    }
    if (!self.wkWebView.superview)
    {
        auto view     = ax::Director::getInstance()->getGLView();
        auto eaglView = (EAGLView*)view->getEAGLView();
        [eaglView addSubview:self.wkWebView];
    }
}

- (void)setVisible:(bool)visible
{
    if (!self.wkWebView)
    {
        [self setupWebView];
    }
    self.wkWebView.hidden = !visible;
}

- (void)setBounces:(bool)bounces
{
    self.wkWebView.scrollView.bounces = bounces;
}

- (void)setOpacityWebView:(float)opacity
{
    if (!self.wkWebView)
    {
        [self setupWebView];
    }
    self.wkWebView.alpha = opacity;
    [self.wkWebView setOpaque:YES];
}

- (float)getOpacityWebView
{
    return self.wkWebView.alpha;
}

- (void)setBackgroundTransparent
{
    if (!self.wkWebView)
    {
        [self setupWebView];
    }
    [self.wkWebView setOpaque:NO];
    [self.wkWebView setBackgroundColor:[UIColor clearColor]];
}

- (void)setFrameWithX:(float)x y:(float)y width:(float)width height:(float)height
{
    if (!self.wkWebView)
    {
        [self setupWebView];
    }
    CGRect newFrame = CGRectMake(x, y, width, height);
    if (!CGRectEqualToRect(self.wkWebView.frame, newFrame))
    {
        self.wkWebView.frame = CGRectMake(x, y, width, height);
    }
}

- (void)setJavascriptInterfaceScheme:(std::string_view)scheme
{
    self.jsScheme = @(scheme.data());
}

- (void)loadData:(std::string_view)data
            MIMEType:(std::string_view)MIMEType
    textEncodingName:(std::string_view)encodingName
             baseURL:(std::string_view)baseURL
{
    auto path = [[NSBundle mainBundle] resourcePath];
    path      = [path stringByAppendingPathComponent:@(baseURL.data())];
    auto url  = [NSURL fileURLWithPath:path];

    [self.wkWebView loadData:[NSData dataWithBytes:data.data() length:data.length()]
                     MIMEType:@(MIMEType.data())
        characterEncodingName:@(encodingName.data())
                      baseURL:url];
}

- (void)loadHTMLString:(std::string_view)string baseURL:(std::string_view)baseURL
{
    if (!self.wkWebView)
    {
        [self setupWebView];
    }
    auto path = [[NSBundle mainBundle] resourcePath];
    path      = [path stringByAppendingPathComponent:@(baseURL.data())];
    auto url  = [NSURL fileURLWithPath:path];
    [self.wkWebView loadHTMLString:@(string.data()) baseURL:url];
}

- (void)loadUrl:(std::string_view)urlString cleanCachedData:(BOOL)needCleanCachedData
{
    if (!self.wkWebView)
    {
        [self setupWebView];
    }
    NSURL* url = [NSURL URLWithString:@(urlString.data())];

    NSURLRequest* request = nil;
    if (needCleanCachedData)
        request = [NSURLRequest requestWithURL:url
                                   cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
                               timeoutInterval:60];
    else
        request = [NSURLRequest requestWithURL:url];

    [self.wkWebView loadRequest:request];
}

- (void)loadFile:(std::string_view)filePath
{
    if (!self.wkWebView)
    {
        [self setupWebView];
    }
    NSURL* url            = [NSURL fileURLWithPath:@(filePath.data())];
    NSURLRequest* request = [NSURLRequest requestWithURL:url];
    [self.wkWebView loadRequest:request];
}

- (void)stopLoading
{
    [self.wkWebView stopLoading];
}

- (void)reload
{
    [self.wkWebView reload];
}

- (BOOL)canGoForward
{
    return self.wkWebView.canGoForward;
}

- (BOOL)canGoBack
{
    return self.wkWebView.canGoBack;
}

- (void)goBack
{
    [self.wkWebView goBack];
}

- (void)goForward
{
    [self.wkWebView goForward];
}

- (void)evaluateJS:(std::string_view)js
{
    if (!self.wkWebView)
    {
        [self setupWebView];
    }
    [self.wkWebView evaluateJavaScript:@(js.data()) completionHandler:nil];
}

- (void)setScalesPageToFit:(const bool)scalesPageToFit
{
    // TODO: there is not corresponding API in WK.
    // https://stackoverflow.com/questions/26295277/wkwebview-equivalent-for-uiwebviews-scalespagetofit/43048514 seems
    // has a solution, but it doesn't support setting it dynamically. If we want to set this feature dynamically, then
    // it will be too complex.
}

#pragma mark - WKNavigationDelegate
- (void)webView:(WKWebView*)webView
    decidePolicyForNavigationAction:(WKNavigationAction*)navigationAction
                    decisionHandler:(void (^)(WKNavigationActionPolicy))decisionHandler
{
    NSString* url = [[[navigationAction request] URL] absoluteString];
    if ([[webView.URL scheme] isEqualToString:self.jsScheme])
    {
        self.onJsCallback([url UTF8String]);
        decisionHandler(WKNavigationActionPolicyCancel);
        return;
    }
    if (self.shouldStartLoading && url)
    {
        if (self.shouldStartLoading([url UTF8String]))
            decisionHandler(WKNavigationActionPolicyAllow);
        else
            decisionHandler(WKNavigationActionPolicyCancel);

        return;
    }

    decisionHandler(WKNavigationActionPolicyAllow);
}

- (void)webView:(WKWebView*)webView didFinishNavigation:(WKNavigation*)navigation
{
    if (self.didFinishLoading)
    {
        NSString* url = [webView.URL absoluteString];
        self.didFinishLoading([url UTF8String]);
    }
}

- (void)webView:(WKWebView*)webView didFailProvisionalNavigation:(WKNavigation*)navigation withError:(NSError*)error
{
    if (self.didFailLoading)
    {
        NSString* errorInfo = error.userInfo[NSURLErrorFailingURLStringErrorKey];
        if (errorInfo)
        {
            self.didFailLoading([errorInfo UTF8String]);
        }
    }
}

#pragma WKUIDelegate

// Implement js alert function.
- (void)webView:(WKWebView*)webView
    runJavaScriptAlertPanelWithMessage:(NSString*)message
                      initiatedByFrame:(WKFrameInfo*)frame
                     completionHandler:(void (^)())completionHandler
{
    UIAlertController* alertController = [UIAlertController alertControllerWithTitle:message
                                                                             message:nil
                                                                      preferredStyle:UIAlertControllerStyleAlert];
    [alertController addAction:[UIAlertAction actionWithTitle:@"Ok"
                                                        style:UIAlertActionStyleCancel
                                                      handler:^(UIAlertAction* action) {
                                                        completionHandler();
                                                      }]];

    auto rootViewController = [UIApplication sharedApplication].windows[0].rootViewController;
    [rootViewController presentViewController:alertController
                                     animated:YES
                                   completion:^{
                                   }];
}

@end

NS_AX_BEGIN

namespace ui
{

WebViewImpl::WebViewImpl(WebView* webView) : _uiWebViewWrapper([UIWebViewWrapper newWebViewWrapper]), _webView(webView)
{

    _uiWebViewWrapper.shouldStartLoading = [this](std::string url) {
        if (this->_webView->_onShouldStartLoading)
        {
            return this->_webView->_onShouldStartLoading(this->_webView, url);
        }
        return true;
    };
    _uiWebViewWrapper.didFinishLoading = [this](std::string url) {
        if (this->_webView->_onDidFinishLoading)
        {
            this->_webView->_onDidFinishLoading(this->_webView, url);
        }
    };
    _uiWebViewWrapper.didFailLoading = [this](std::string url) {
        if (this->_webView->_onDidFailLoading)
        {
            this->_webView->_onDidFailLoading(this->_webView, url);
        }
    };
    _uiWebViewWrapper.onJsCallback = [this](std::string url) {
        if (this->_webView->_onJSCallback)
        {
            this->_webView->_onJSCallback(this->_webView, url);
        }
    };
}

WebViewImpl::~WebViewImpl()
{
    [_uiWebViewWrapper release];
    _uiWebViewWrapper = nullptr;
}

void WebViewImpl::setJavascriptInterfaceScheme(std::string_view scheme)
{
    [_uiWebViewWrapper setJavascriptInterfaceScheme:scheme];
}

void WebViewImpl::loadData(const Data& data,
                           std::string_view MIMEType,
                           std::string_view encoding,
                           std::string_view baseURL)
{

    std::string dataString(reinterpret_cast<char*>(data.getBytes()), static_cast<unsigned int>(data.getSize()));
    [_uiWebViewWrapper loadData:dataString MIMEType:MIMEType textEncodingName:encoding baseURL:baseURL];
}

void WebViewImpl::loadHTMLString(std::string_view string, std::string_view baseURL)
{
    [_uiWebViewWrapper loadHTMLString:string baseURL:baseURL];
}

void WebViewImpl::loadURL(std::string_view url)
{
    this->loadURL(url, false);
}

void WebViewImpl::loadURL(std::string_view url, bool cleanCachedData)
{
    [_uiWebViewWrapper loadUrl:url cleanCachedData:cleanCachedData];
}

void WebViewImpl::loadFile(std::string_view fileName)
{
    auto fullPath = ax::FileUtils::getInstance()->fullPathForFilename(fileName);
    [_uiWebViewWrapper loadFile:fullPath];
}

void WebViewImpl::stopLoading()
{
    [_uiWebViewWrapper stopLoading];
}

void WebViewImpl::reload()
{
    [_uiWebViewWrapper reload];
}

bool WebViewImpl::canGoBack()
{
    return _uiWebViewWrapper.canGoBack;
}

bool WebViewImpl::canGoForward()
{
    return _uiWebViewWrapper.canGoForward;
}

void WebViewImpl::goBack()
{
    [_uiWebViewWrapper goBack];
}

void WebViewImpl::goForward()
{
    [_uiWebViewWrapper goForward];
}

void WebViewImpl::evaluateJS(std::string_view js)
{
    [_uiWebViewWrapper evaluateJS:js];
}

void WebViewImpl::setBounces(bool bounces)
{
    [_uiWebViewWrapper setBounces:bounces];
}

void WebViewImpl::setScalesPageToFit(const bool scalesPageToFit)
{
    [_uiWebViewWrapper setScalesPageToFit:scalesPageToFit];
}

void WebViewImpl::draw(ax::Renderer* renderer, ax::Mat4 const& transform, uint32_t flags)
{
    if (flags & ax::Node::FLAGS_TRANSFORM_DIRTY)
    {

        auto director  = ax::Director::getInstance();
        auto glView    = director->getGLView();
        auto frameSize = glView->getFrameSize();

        auto scaleFactor = [static_cast<EAGLView*>(glView->getEAGLView()) contentScaleFactor];

        auto winSize = director->getWinSize();

        auto leftBottom = this->_webView->convertToWorldSpace(ax::Vec2::ZERO);
        auto rightTop   = this->_webView->convertToWorldSpace(
              ax::Vec2(this->_webView->getContentSize().width, this->_webView->getContentSize().height));

        auto x      = (frameSize.width / 2 + (leftBottom.x - winSize.width / 2) * glView->getScaleX()) / scaleFactor;
        auto y      = (frameSize.height / 2 - (rightTop.y - winSize.height / 2) * glView->getScaleY()) / scaleFactor;
        auto width  = (rightTop.x - leftBottom.x) * glView->getScaleX() / scaleFactor;
        auto height = (rightTop.y - leftBottom.y) * glView->getScaleY() / scaleFactor;

        [_uiWebViewWrapper setFrameWithX:x y:y width:width height:height];
    }
}

void WebViewImpl::setVisible(bool visible)
{
    [_uiWebViewWrapper setVisible:visible];
}

void WebViewImpl::setOpacityWebView(float opacity)
{
    [_uiWebViewWrapper setOpacityWebView:opacity];
}

float WebViewImpl::getOpacityWebView() const
{
    return [_uiWebViewWrapper getOpacityWebView];
}

void WebViewImpl::setBackgroundTransparent()
{
    [_uiWebViewWrapper setBackgroundTransparent];
}

}  // namespace ui
}  // namespace ax
