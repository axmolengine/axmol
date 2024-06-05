/****************************************************************************
 Copyright (c) 2017 Serge Zaitsev
 Copyright (c) 2022 Steffen André Langnes
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmolengine.github.io/

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

#include "UIWebViewImpl-linux.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)

#    include <cstdlib>
#    include <gtk/gtk.h>
#    include <webkit2/webkit2.h>
#    include <fcntl.h>
#    include <sys/stat.h>
#    include <iostream>

#    if !defined(AX_PLATFORM_LINUX_WAYLAND)
#        include <X11/Xlib.h>
#        include <gdk/gdkx.h>
#    endif

#    include "UIWebView.h"
#    include "base/Director.h"
#    include "platform/FileUtils.h"
#    include "platform/GLView.h"
#    include "ui/UIHelper.h"

namespace webkit_dmabuf
{

// Get environment variable. Not thread-safe.
static inline std::string get_env(const std::string& name)
{
    auto* value = std::getenv(name.c_str());
    if (value)
    {
        return {value};
    }
    return {};
}

// Set environment variable. Not thread-safe.
static inline void set_env(const std::string& name, const std::string& value)
{
    ::setenv(name.c_str(), value.c_str(), 1);
}

// Checks whether the NVIDIA GPU driver is used based on whether the kernel
// module is loaded.
static inline bool is_using_nvidia_driver()
{
    struct ::stat buffer
    {};
    if (::stat("/sys/module/nvidia", &buffer) != 0)
    {
        return false;
    }
    return S_ISDIR(buffer.st_mode);
}

// Checks whether the windowing system is Wayland.
static inline bool is_wayland_display()
{
    if (!get_env("WAYLAND_DISPLAY").empty())
    {
        return true;
    }
    if (get_env("XDG_SESSION_TYPE") == "wayland")
    {
        return true;
    }
    if (get_env("DESKTOP_SESSION").find("wayland") != std::string::npos)
    {
        return true;
    }
    return false;
}

// Checks whether the GDK X11 backend is used.
// See: https://docs.gdkWindow.org/gdk3/class.DisplayManager.html
static inline bool is_gdk_x11_backend()
{
#    if !defined(AX_PLATFORM_LINUX_WAYLAND)
    auto* manager = gdk_display_manager_get();
    auto* display = gdk_display_manager_get_default_display(manager);
    return GDK_IS_X11_DISPLAY(display);
#    else
    return false;
#    endif
}

// Checks whether WebKit is affected by bug when using DMA-BUF renderer.
// Returns true if all of the following conditions are met:
//  - WebKit version is >= 2.42 (please narrow this down when there's a fix).
//  - Environment variables are empty or not set:
//    - WEBKIT_DISABLE_DMABUF_RENDERER
//  - Windowing system is not Wayland.
//  - GDK backend is X11.
//  - NVIDIA GPU driver is used.
static inline bool is_webkit_dmabuf_bugged()
{
    auto wk_major = webkit_get_major_version();
    auto wk_minor = webkit_get_minor_version();
    // TODO: Narrow down affected WebKit version when there's a fixed version
    auto is_affected_wk_version = wk_major == 2 && wk_minor >= 42;
    if (!is_affected_wk_version)
    {
        return false;
    }
    if (!get_env("WEBKIT_DISABLE_DMABUF_RENDERER").empty())
    {
        return false;
    }
    if (is_wayland_display())
    {
        return false;
    }
    if (!is_gdk_x11_backend())
    {
        return false;
    }
    if (!is_using_nvidia_driver())
    {
        return false;
    }
    return true;
}

// Applies workaround for WebKit DMA-BUF bug if needed.
// See WebKit bug: https://bugs.webkit.org/show_bug.cgi?id=261874
static inline void apply_webkit_dmabuf_workaround()
{
    if (!is_webkit_dmabuf_bugged())
    {
        return;
    }
    set_env("WEBKIT_DISABLE_DMABUF_RENDERER", "1");
}
}  // namespace webkit_dmabuf

static inline std::string create_init_script(const std::string& post_fn)
{
    auto js = std::string{} +
              "(function() {\n\
  'use strict';\n\
  function generateId() {\n\
    var crypto = window.crypto || window.msCrypto;\n\
    var bytes = new Uint8Array(16);\n\
    crypto.getRandomValues(bytes);\n\
    return Array.prototype.slice.call(bytes).map(function(n) {\n\
      return n.toString(16).padStart(2, '0');\n\
    }).join('');\n\
  }\n\
  var Webview = (function() {\n\
    var _promises = {};\n\
    function Webview_() {}\n\
    Webview_.prototype.post = function(message) {\n\
      return (" +
              post_fn +
              ")(message);\n\
    };\n\
    Webview_.prototype.call = function(method) {\n\
      var _id = generateId();\n\
      var _params = Array.prototype.slice.call(arguments, 1);\n\
      var promise = new Promise(function(resolve, reject) {\n\
        _promises[_id] = { resolve, reject };\n\
      });\n\
      this.post(JSON.stringify({\n\
        id: _id,\n\
        method: method,\n\
        params: _params\n\
      }));\n\
      return promise;\n\
    };\n\
    Webview_.prototype.onReply = function(id, status, result) {\n\
      var promise = _promises[id];\n\
      if (result !== undefined) {\n\
        try {\n\
          result = JSON.parse(result);\n\
        } catch {\n\
          promise.reject(new Error(\"Failed to parse binding result as JSON\"));\n\
          return;\n\
        }\n\
      }\n\
      if (status === 0) {\n\
        promise.resolve(result);\n\
      } else {\n\
        promise.reject(result);\n\
      }\n\
    };\n\
    Webview_.prototype.onBind = function(name) {\n\
      if (Object.hasOwn(window, name)) {\n\
        throw new Error('Property \"' + name + '\" already exists');\n\
      }\n\
      window[name] = (function() {\n\
        var params = [name].concat(Array.prototype.slice.call(arguments));\n\
        return Webview_.prototype.call.apply(this, params);\n\
      }).bind(this);\n\
    };\n\
    Webview_.prototype.onUnbind = function(name) {\n\
      if (!Object.hasOwn(window, name)) {\n\
        throw new Error('Property \"' + name + '\" does not exist');\n\
      }\n\
      delete window[name];\n\
    };\n\
    return Webview_;\n\
  })();\n\
  window.__webview__ = new Webview();\n\
})()";
    return js;
}

static void init_gtk_platform(Display* x11Display)
{
    static bool has_initiated = false;
    if (has_initiated)
    {
        return;
    }
#    if !defined(AX_PLATFORM_LINUX_WAYLAND)
    // this will instruct gdk to use XWayland layer on Wayland platforms
    if (webkit_dmabuf::is_wayland_display())
        gdk_set_allowed_backends("x11");
#    endif
    gtk_init(nullptr, nullptr);

    const char* display_string = DisplayString(x11Display);
    GdkDisplay* x11_gdk_display =
        display_string != nullptr ? gdk_display_open(display_string) : gdk_display_get_default();

    auto default_display_manager = gdk_display_manager_get();
    gdk_display_manager_set_default_display(default_display_manager, x11_gdk_display);

    has_initiated = true;
}

class GTKWebKit
{
public:
    GTKWebKit()
    {
        webkit_dmabuf::apply_webkit_dmabuf_workaround();

        m_X11Display      = (Display*)ax::Director::getInstance()->getGLView()->getX11Display();
        m_ParentX11Window = (Window)ax::Director::getInstance()->getGLView()->getX11Window();

        init_gtk_platform(m_X11Display);

        m_Window = gtk_window_new(GTK_WINDOW_POPUP);
        g_signal_connect(G_OBJECT(m_Window), "destroy", G_CALLBACK(+[](GtkWidget*, gpointer arg) {
                             auto* w = static_cast<GTKWebKit*>(arg);
                             // Widget destroyed along with window.
                             w->m_WebView = nullptr;
                             w->m_Window  = nullptr;
                         }),
                         this);
#    if defined(AX_PLATFORM_LINUX_WAYLAND)
        AX_ASSERT(0 && "TODO: Implement for wayland..");
/*
 * Notes:
 *
 * Current Wayland(2024) there is no way to embed a foreign window.
 * Usually we need to create a native window turn it into a standalone
 * browser, place inside of the main window region and interact with it.
 *
 * - Why not just create a window with no title bar and maintain it's
 * position and size manually according to the main window? AFAIK
 * Wayland also doesn't support it, it doesn't even allow the process
 * which created the window to change those properties.
 *
 * - Why not just use XWayland layer do those and never use direct wayland
 * through GLFW or anything?
 * Picture quality and overall performance will not be as good as native
 * wayland usage.
 * Need do more research but it's just makes sense we won't be getting
 * native Wayland performance that's for sure. But the OpenGL drawing
 * part shouldn't be affected.
 *
 * Possible workarounds:
 * 1. Ditch GLFW for Linux platform, use EGL library directly or
 * use GTK+GDK to maintain the all the window handle stuff. Then use
 * Wayland subcompositor concept or https://wayland-book.com/xdg-shell-in-depth/popups.html
 * to draw the WebkitGtk surface. Some usage of subcompositor concept can be found:
 * https://github.com/ehmry/snes9x/
 *
 * - Why this approach is kind of better? Well will be using direct Wayland
 * so some hdpi scaling(if not by fraction) would be kind of better than
 * XWayland and some other stuff probably won't be much noticable, but
 * hey we are removing one more non benificial abstraction layer(XWayland)
 * from the userend.
 *
 * 2. Write custom Wayland compositor(AFAIK it's not a WM but almost
 * acts like one) to allow draw foreign wl_surface...
 * Yes.. No.
 *
 * Bonus: We can go with the direct Wayland window managing + draw the
 * WebView using XWayland abstraction layer and place the window position
 * according to the main window's location. We're not doing exactly the same
 * for x11 case, X11 has something to make a window, a child window of another
 * window so the relative positon gets set automatically by the WM.
 *
 * Why not use CEF or some other WebView implementations? It would be same
 * we would still need to move those around, which Wayland doesn't allow
 * from process context to do it without custom compositor.
 *
 */
#    endif
    }

    ~GTKWebKit()
    {
        if (m_WebView)
        {
            gtk_widget_destroy(GTK_WIDGET(m_WebView));
            m_WebView = nullptr;
        }

        if (m_Window)
        {
            g_signal_handlers_disconnect_by_data(GTK_WINDOW(m_Window), this);
            gtk_window_close(GTK_WINDOW(m_Window));
            m_Window = nullptr;
        }

        if (m_InitScript)
        {
            webkit_user_script_unref(m_InitScript);
            m_InitScript = nullptr;
        }

        deplete_run_loop_event_queue();
    }

    bool createWebView()
    {
        if (m_Window == nullptr)
            return false;

        gtk_widget_realize(m_Window);
        m_WebView = webkit_web_view_new();
        if (m_WebView == nullptr)
            return false;

        WebKitUserContentManager* manager = m_UserContentManager =
            webkit_web_view_get_user_content_manager(WEBKIT_WEB_VIEW(m_WebView));
        if (manager == nullptr)
            return false;

        g_signal_connect(manager, "script-message-received::__webview__",
                         G_CALLBACK(+[](WebKitUserContentManager*, WebKitJavascriptResult* r, gpointer arg) {
                             auto* w = static_cast<GTKWebKit*>(arg);
                             /*char *s = get_string_from_js_result(r);
                             w->on_message(s);
                             g_free(s);*/
                         }),
                         this);
        webkit_user_content_manager_register_script_message_handler(manager, "__webview__");

        initScript("function(message){return window.webkit.messageHandlers.__webview__.postMessage(message);}");

        gtk_container_add(GTK_CONTAINER(m_Window), GTK_WIDGET(m_WebView));
        gtk_widget_show(GTK_WIDGET(m_WebView));

        WebKitSettings* settings = webkit_web_view_get_settings(WEBKIT_WEB_VIEW(m_WebView));
        if (settings == nullptr)
            return false;

        webkit_settings_set_javascript_can_access_clipboard(settings, true);

        gtk_window_set_resizable(GTK_WINDOW(m_Window), true);
        gtk_widget_grab_focus(GTK_WIDGET(m_WebView));
        gtk_widget_show_all(m_Window);
        return embed();
    }

    void setWebViewRect(int x, int y, int width, int height)
    {
        AXLOGI("Pos: {}, {}, Size: w:{}, h:{}", x, y, width, height);

        gtk_window_set_resizable(GTK_WINDOW(m_Window), true);

        GdkGeometry geometry{};
        geometry.min_width = geometry.max_width = width;
        geometry.min_height = geometry.max_height = height;
        GdkWindowHints hints                      = GDK_HINT_MIN_SIZE;
        gtk_window_set_geometry_hints(GTK_WINDOW(m_Window), nullptr, &geometry, hints);

        XUnmapWindow(m_X11Display, m_ChildX11Window);
        XSync(m_X11Display, false);

        XReparentWindow(m_X11Display, m_ChildX11Window, m_ParentX11Window, x, y);

        XMapWindow(m_X11Display, m_ChildX11Window);

        gtk_widget_realize(m_Window);

        usleep(1e3);  // 1ms

        XSync(m_X11Display, false);
        XFlush(m_X11Display);
    }

    void loadURL(std::string_view url, bool cleanCachedData)
    {
        if (cleanCachedData)
        {
            webkit_web_context_clear_cache(webkit_web_view_get_context(WEBKIT_WEB_VIEW(m_WebView)));
        }
        webkit_web_view_load_uri(WEBKIT_WEB_VIEW(m_WebView), url.data());
    }

private:
    void initScript(const std::string& js)
    {
        // Note: Should be called once by createWebView.
        m_InitScript = webkit_user_script_new(js.c_str(), WEBKIT_USER_CONTENT_INJECT_TOP_FRAME,
                                              WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START, nullptr, nullptr);

        webkit_user_content_manager_add_script(m_UserContentManager, m_InitScript);
    }

    bool embed()
    {
        m_ChildX11Window = gdk_x11_window_get_xid(gtk_widget_get_window(m_Window));
        if (m_ChildX11Window == None)
            return false;
        XReparentWindow(m_X11Display, m_ChildX11Window, m_ParentX11Window, 0, 0);

        gtk_widget_realize(m_Window);
        XSync(m_X11Display, false);
        XFlush(m_X11Display);

        while (gtk_events_pending())
        {
            gtk_main_iteration();
        }
        return true;
    }

    using dispatch_fn_t = std::function<void()>;
    void dispatch_in_gtk(std::function<void()> f)
    {
        g_idle_add_full(G_PRIORITY_HIGH_IDLE, (GSourceFunc)([](void* f) -> int {
                            (*static_cast<dispatch_fn_t*>(f))();
                            return G_SOURCE_REMOVE;
                        }),
                        new std::function<void()>(f), [](void* f) { delete static_cast<dispatch_fn_t*>(f); });
    }

    void deplete_run_loop_event_queue()
    {
        bool done = false;
        dispatch_in_gtk([&] { done = true; });
        while (!done)
        {
            gtk_main_iteration();
        }
    }

private:
    Display* m_X11Display;
    Window m_ParentX11Window;
    Window m_ChildX11Window;
    GtkWidget* m_Window;
    GtkWidget* m_WebView;
    WebKitUserScript* m_InitScript;
    WebKitUserContentManager* m_UserContentManager;
};

NS_AX_BEGIN

namespace ui
{

WebViewImpl::WebViewImpl(WebView* webView) : _createSucceeded(false), _gtkWebKit(nullptr), _webView(webView)
{
    _gtkWebKit       = new GTKWebKit();
    _createSucceeded = _gtkWebKit->createWebView();
}

WebViewImpl::~WebViewImpl()
{
    if (_gtkWebKit != nullptr)
    {
        delete _gtkWebKit;
        _gtkWebKit = nullptr;
    }
}

void WebViewImpl::loadData(const Data& data,
                           std::string_view MIMEType,
                           std::string_view encoding,
                           std::string_view baseURL)
{
    if (_createSucceeded)
    {
        // const auto url = getDataURI(data, MIMEType);
        //_systemWebControl->loadURL(url, false);
    }
}

void WebViewImpl::loadHTMLString(std::string_view string, std::string_view baseURL)
{
    if (_createSucceeded)
    {
        if (string.empty())
        {
            //_systemWebControl->loadHTMLString("data:text/html," + utils::urlEncode("<html></html>"), baseURL);
            return;
        }

        /*const auto html = htmlFromUri(string);
        if (!html.empty())
        {
            //_systemWebControl->loadHTMLString("data:text/html," + utils::urlEncode(html), baseURL);
        }
        else
        {
            //_systemWebControl->loadHTMLString(string, baseURL);
        }*/
    }
}

void WebViewImpl::loadURL(std::string_view url, bool cleanCachedData)
{
    if (_createSucceeded)
    {
        _gtkWebKit->loadURL(url, cleanCachedData);
    }
}

void WebViewImpl::loadFile(std::string_view fileName)
{
    if (_createSucceeded)
    {
        const auto fullPath = FileUtils::getInstance()->fullPathForFilename(fileName);
        //_systemWebControl->loadFile(fullPath);
    }
}

void WebViewImpl::stopLoading()
{
    if (_createSucceeded)
    {
        //_systemWebControl->stopLoading();
    }
}

void WebViewImpl::reload()
{
    if (_createSucceeded)
    {
        //_systemWebControl->reload();
    }
}

bool WebViewImpl::canGoBack()
{
    if (_createSucceeded)
    {
        // return _systemWebControl->canGoBack();
    }
    return false;
}

bool WebViewImpl::canGoForward()
{
    if (_createSucceeded)
    {
        // return _systemWebControl->canGoForward();
    }
    return false;
}

void WebViewImpl::goBack()
{
    if (_createSucceeded)
    {
        //_systemWebControl->goBack();
    }
}

void WebViewImpl::goForward()
{
    if (_createSucceeded)
    {
        //_systemWebControl->goForward();
    }
}

void WebViewImpl::setJavascriptInterfaceScheme(std::string_view scheme)
{
    if (_createSucceeded)
    {
        //_systemWebControl->setJavascriptInterfaceScheme(scheme);
    }
}

void WebViewImpl::evaluateJS(std::string_view js)
{
    if (_createSucceeded)
    {
        //_systemWebControl->evaluateJS(js);
    }
}

void WebViewImpl::setScalesPageToFit(const bool scalesPageToFit)
{
    if (_createSucceeded)
    {
        //_systemWebControl->setScalesPageToFit(scalesPageToFit);
    }
}

void WebViewImpl::draw(Renderer* renderer, Mat4 const& transform, uint32_t flags)
{
    if (!_createSucceeded)
    {
        return;
    }
    if (flags & Node::FLAGS_TRANSFORM_DIRTY)
    {
        const auto uiRect = ax::ui::Helper::convertBoundingBoxToScreen(_webView);
        _gtkWebKit->setWebViewRect(static_cast<int>(uiRect.origin.x), static_cast<int>(uiRect.origin.y),
                                   static_cast<int>(uiRect.size.width), static_cast<int>(uiRect.size.height));
    }

    gtk_main_iteration_do(false);
}

void WebViewImpl::setVisible(bool visible)
{
    if (_createSucceeded)
    {
        //_systemWebControl->setWebViewVisible(visible);
    }
}

void WebViewImpl::setBounces(bool bounces)
{
    //_systemWebControl->setBounces(bounces);
}

void WebViewImpl::setOpacityWebView(float opacity)
{
    //_systemWebControl->setOpacityWebView(opacity);
}

float WebViewImpl::getOpacityWebView() const
{
    // return _systemWebControl->getOpacityWebView();
    return false;
}

void WebViewImpl::setBackgroundTransparent()
{
    //_systemWebControl->setBackgroundTransparent();
}

}  // namespace ui

NS_AX_END

#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
