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
#import <WebKit/WebKit.h>

#include "axmol/ui/WebView/WebViewImpl-ios.h"
#include "axmol/ui/WebView/WebView.h"
#include "axmol/ui/UIHelper.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/base/Logging.h"
#include "axmol/base/Director.h"
#include "axmol/platform/RenderView.h"
#include "axmol/platform/ios/RenderHostView-ios.h"
#include "axmol/platform/FileUtils.h"

@class UIWebViewWrapper;

static NSString* const AXWebViewConsoleMessageHandlerName = @"axmolWebViewConsole";

@interface AXWebViewScriptMessageHandler : NSObject <WKScriptMessageHandler>
@property(nonatomic, assign) UIWebViewWrapper* webViewWrapper;
@end

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

- (void)handleScriptMessage:(WKScriptMessage*)message;

- (void)evaluateJS:(std::string_view)js;

- (void)goBack;

- (void)goForward;

- (void)setScalesPageToFit:(const bool)scalesPageToFit;
@end

@interface UIWebViewWrapper () <WKUIDelegate, WKNavigationDelegate>
@property(nonatomic) WKWebView* wkWebView;
@property(nonatomic) BOOL suppressNextDidFinishLoading;
@property(nonatomic, retain) AXWebViewScriptMessageHandler* scriptMessageHandler;

@property(nonatomic, copy) NSString* jsScheme;
@end

@implementation AXWebViewScriptMessageHandler

- (void)userContentController:(WKUserContentController*)userContentController
      didReceiveScriptMessage:(WKScriptMessage*)message
{
    AX_UNUSED_PARAM(userContentController);
    [self.webViewWrapper handleScriptMessage:message];
}

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
        self.wkWebView                    = nil;
        self.shouldStartLoading           = nullptr;
        self.didFinishLoading             = nullptr;
        self.didFailLoading               = nullptr;
        self.suppressNextDidFinishLoading = NO;
        self.scriptMessageHandler         = nil;
    }
    return self;
}

- (void)dealloc
{
    self.wkWebView.UIDelegate         = nil;
    self.wkWebView.navigationDelegate = nil;
    [self.wkWebView.configuration.userContentController
        removeScriptMessageHandlerForName:AXWebViewConsoleMessageHandlerName];
    [self.wkWebView removeFromSuperview];
    [self.wkWebView release];
    self.wkWebView                           = nil;
    self.scriptMessageHandler.webViewWrapper = nil;
    self.scriptMessageHandler                = nil;
    self.jsScheme                            = nil;
    [super dealloc];
}

- (void)setupWebView
{
    if (!self.wkWebView)
    {
        static NSString* const consoleBridgeScript =
            @"(function(){"
            @"if(window.__axmolWebViewConsoleBridgeInstalled){return;}"
            @"window.__axmolWebViewConsoleBridgeInstalled=true;"
            @"function stringify(value){"
            @"try{"
            @"if(value instanceof Error){return value.stack||value.message||String(value);}"
            @"if(typeof value==='object'){return JSON.stringify(value);}"
            @"return String(value);"
            @"}catch(e){return String(value);}"
            @"}"
            @"function post(level,args,source,line,column,stack){"
            @"try{"
            @"window.webkit.messageHandlers.axmolWebViewConsole.postMessage({"
            @"level:level,"
            @"message:Array.prototype.map.call(args,stringify).join(' '),"
            @"source:source||'',"
            @"line:line||0,"
            @"column:column||0,"
            @"stack:stack||''"
            @"});"
            @"}catch(e){}"
            @"}"
            @"['log','info','warn','error'].forEach(function(level){"
            @"var original=console[level];"
            @"console[level]=function(){"
            @"post(level,arguments);"
            @"if(original){original.apply(console,arguments);}"
            @"};"
            @"});"
            @"window.addEventListener('error',function(event){"
            @"var error=event.error;"
            @"post('exception',[event.message||'JavaScript "
            @"error'],event.filename,event.lineno,event.colno,error&&error.stack);"
            @"});"
            @"window.addEventListener('unhandledrejection',function(event){"
            @"var reason=event.reason;"
            @"post('unhandledrejection',[reason&&(reason.stack||reason.message)||reason],'',0,0,reason&&reason.stack);"
            @"});"
            @"})();";

        WKWebViewConfiguration* configuration          = [[[WKWebViewConfiguration alloc] init] autorelease];
        WKUserContentController* userContentController = [[[WKUserContentController alloc] init] autorelease];
        WKUserScript* userScript                       = [[[WKUserScript alloc] initWithSource:consoleBridgeScript
                                                           injectionTime:WKUserScriptInjectionTimeAtDocumentStart
                                                        forMainFrameOnly:NO] autorelease];

        [userContentController addUserScript:userScript];
        self.scriptMessageHandler                = [[[AXWebViewScriptMessageHandler alloc] init] autorelease];
        self.scriptMessageHandler.webViewWrapper = self;
        [userContentController addScriptMessageHandler:self.scriptMessageHandler
                                                  name:AXWebViewConsoleMessageHandlerName];
        configuration.userContentController = userContentController;

        self.wkWebView                    = [[WKWebView alloc] initWithFrame:CGRectZero configuration:configuration];
        self.wkWebView.UIDelegate         = self;
        self.wkWebView.navigationDelegate = self;
    }
    if (!self.wkWebView.superview)
    {
        auto view     = ax::Director::getInstance()->getRenderView();
        auto hostView = (__bridge RenderHostView*)view->getNativeDisplay();
        [hostView addSubview:self.wkWebView];
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
    if (!self.wkWebView)
    {
        [self setupWebView];
    }

    NSURL* url = [self.wkWebView.URL retain];
    if (!url)
    {
        [self.wkWebView reload];
        return;
    }

    [self.wkWebView stopLoading];
    self.suppressNextDidFinishLoading = YES;
    [self.wkWebView loadHTMLString:@"" baseURL:nil];

    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, static_cast<int64_t>(0.05 * NSEC_PER_SEC)),
                   dispatch_get_main_queue(), ^{
                     NSURLRequest* request = [NSURLRequest requestWithURL:url
                                                              cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
                                                          timeoutInterval:60];
                     [self.wkWebView loadRequest:request];
                     [url release];
                   });
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

- (void)handleScriptMessage:(WKScriptMessage*)message
{
    if (![message.name isEqualToString:AXWebViewConsoleMessageHandlerName] ||
        ![message.body isKindOfClass:[NSDictionary class]])
    {
        return;
    }

    NSDictionary* body = (NSDictionary*)message.body;

    auto stringValue = [](id value) -> std::string {
        if ([value isKindOfClass:[NSString class]])
        {
            return [(NSString*)value UTF8String];
        }
        if ([value respondsToSelector:@selector(stringValue)])
        {
            return [[value stringValue] UTF8String];
        }
        return {};
    };

    auto intValue = [](id value) -> int {
        if ([value respondsToSelector:@selector(intValue)])
        {
            return [value intValue];
        }
        return 0;
    };

    const auto level  = stringValue(body[@"level"]);
    const auto text   = stringValue(body[@"message"]);
    const auto source = stringValue(body[@"source"]);
    const auto stack  = stringValue(body[@"stack"]);
    const auto line   = intValue(body[@"line"]);
    const auto column = intValue(body[@"column"]);
    auto messageText  = text;
    if (!stack.empty())
    {
        messageText += "\n";
        messageText += stack;
    }

    if (level == "error" || level == "exception" || level == "unhandledrejection")
    {
        AXLOGE("WKWebView JavaScript {}: {} ({}:{}:{})", level, messageText, source, line, column);
    }
    else if (level == "warn")
    {
        AXLOGW("WKWebView JavaScript {}: {} ({}:{}:{})", level, messageText, source, line, column);
    }
    else
    {
        AXLOGD("WKWebView JavaScript {}: {} ({}:{}:{})", level, messageText, source, line, column);
    }
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
    if (self.suppressNextDidFinishLoading)
    {
        self.suppressNextDidFinishLoading = NO;
        return;
    }

    if (self.didFinishLoading)
    {
        NSString* url = [webView.URL absoluteString];
        if (url)
        {
            self.didFinishLoading([url UTF8String]);
        }
    }
}

- (void)webView:(WKWebView*)webView didFailProvisionalNavigation:(WKNavigation*)navigation withError:(NSError*)error
{
    AX_UNUSED_PARAM(webView);
    AX_UNUSED_PARAM(navigation);

    NSString* errorInfo = error.userInfo[NSURLErrorFailingURLStringErrorKey];
    AXLOGE("WKWebView provisional navigation failed: {} ({})", errorInfo ? [errorInfo UTF8String] : "",
           error.localizedDescription ? [error.localizedDescription UTF8String] : "");

    if (self.didFailLoading)
    {
        if (errorInfo)
        {
            self.didFailLoading([errorInfo UTF8String]);
        }
    }
}

- (void)webView:(WKWebView*)webView didFailNavigation:(WKNavigation*)navigation withError:(NSError*)error
{
    AX_UNUSED_PARAM(webView);
    AX_UNUSED_PARAM(navigation);

    AXLOGE("WKWebView navigation failed: {}",
           error.localizedDescription ? [error.localizedDescription UTF8String] : "");
}

- (void)webViewWebContentProcessDidTerminate:(WKWebView*)webView
{
    AX_UNUSED_PARAM(webView);

    AXLOGE("WKWebView web content process terminated");
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

namespace ax
{

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
        const auto uiRect = ax::ui::Helper::getNodeNativeWindowRect(_webView);
        [_uiWebViewWrapper setFrameWithX:uiRect.origin.x
                                       y:uiRect.origin.y
                                   width:uiRect.size.width
                                  height:uiRect.size.height];
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
