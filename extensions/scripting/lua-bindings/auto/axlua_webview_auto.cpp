#include "lua-bindings/auto/axlua_webview_auto.hpp"
#include "ui/UIWebView/UIWebView.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_webview_WebView_setJavascriptInterfaceScheme(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_setJavascriptInterfaceScheme'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axui.WebView:setJavascriptInterfaceScheme");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_setJavascriptInterfaceScheme'", nullptr);
            return 0;
        }
        cobj->setJavascriptInterfaceScheme(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:setJavascriptInterfaceScheme",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_setJavascriptInterfaceScheme'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_loadHTMLString(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_loadHTMLString'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axui.WebView:loadHTMLString");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_loadHTMLString'", nullptr);
            return 0;
        }
        cobj->loadHTMLString(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axui.WebView:loadHTMLString");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axui.WebView:loadHTMLString");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_loadHTMLString'", nullptr);
            return 0;
        }
        cobj->loadHTMLString(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:loadHTMLString",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_loadHTMLString'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_loadURL(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_loadURL'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 2) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axui.WebView:loadURL");

            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3,&arg1, "axui.WebView:loadURL");

            if (!ok) { break; }
            cobj->loadURL(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axui.WebView:loadURL");

            if (!ok) { break; }
            cobj->loadURL(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axui.WebView:loadURL",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_loadURL'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_loadFile(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_loadFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axui.WebView:loadFile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_loadFile'", nullptr);
            return 0;
        }
        cobj->loadFile(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:loadFile",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_loadFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_stopLoading(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_stopLoading'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_stopLoading'", nullptr);
            return 0;
        }
        cobj->stopLoading();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:stopLoading",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_stopLoading'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_reload(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_reload'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_reload'", nullptr);
            return 0;
        }
        cobj->reload();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:reload",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_reload'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_canGoBack(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_canGoBack'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_canGoBack'", nullptr);
            return 0;
        }
        auto&& ret = cobj->canGoBack();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:canGoBack",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_canGoBack'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_canGoForward(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_canGoForward'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_canGoForward'", nullptr);
            return 0;
        }
        auto&& ret = cobj->canGoForward();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:canGoForward",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_canGoForward'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_goBack(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_goBack'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_goBack'", nullptr);
            return 0;
        }
        cobj->goBack();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:goBack",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_goBack'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_goForward(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_goForward'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_goForward'", nullptr);
            return 0;
        }
        cobj->goForward();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:goForward",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_goForward'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_evaluateJS(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_evaluateJS'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axui.WebView:evaluateJS");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_evaluateJS'", nullptr);
            return 0;
        }
        cobj->evaluateJS(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:evaluateJS",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_evaluateJS'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_setScalesPageToFit(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_setScalesPageToFit'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "axui.WebView:setScalesPageToFit");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_setScalesPageToFit'", nullptr);
            return 0;
        }
        cobj->setScalesPageToFit(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:setScalesPageToFit",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_setScalesPageToFit'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_getOnJSCallback(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_getOnJSCallback'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_getOnJSCallback'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getOnJSCallback();
        #pragma warning NO CONVERSION FROM NATIVE FOR std::function;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:getOnJSCallback",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_getOnJSCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_setBounces(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_setBounces'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "axui.WebView:setBounces");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_setBounces'", nullptr);
            return 0;
        }
        cobj->setBounces(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:setBounces",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_setBounces'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_setOpacityWebView(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_setOpacityWebView'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "axui.WebView:setOpacityWebView");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_setOpacityWebView'", nullptr);
            return 0;
        }
        cobj->setOpacityWebView(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:setOpacityWebView",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_setOpacityWebView'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_getOpacityWebView(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_getOpacityWebView'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_getOpacityWebView'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getOpacityWebView();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:getOpacityWebView",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_getOpacityWebView'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_setBackgroundTransparent(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::ui::WebView*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_webview_WebView_setBackgroundTransparent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_setBackgroundTransparent'", nullptr);
            return 0;
        }
        cobj->setBackgroundTransparent();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:setBackgroundTransparent",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_setBackgroundTransparent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_webview_WebView_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axui.WebView",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_create'", nullptr);
            return 0;
        }
        auto&& ret = ax::ui::WebView::create();
        object_to_luaval<ax::ui::WebView>(tolua_S, "axui.WebView",(ax::ui::WebView*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axui.WebView:create",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_create'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_webview_WebView_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::ui::WebView* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_webview_WebView_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::ui::WebView();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"axui.WebView");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axui.WebView:WebView",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_webview_WebView_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_webview_WebView_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (WebView)");
    return 0;
}

int lua_register_ax_webview_WebView(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axui.WebView");
    tolua_cclass(tolua_S,"WebView","axui.WebView","axui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"WebView");
        tolua_function(tolua_S,"new",lua_ax_webview_WebView_constructor);
        tolua_function(tolua_S,"setJavascriptInterfaceScheme",lua_ax_webview_WebView_setJavascriptInterfaceScheme);
        tolua_function(tolua_S,"loadHTMLString",lua_ax_webview_WebView_loadHTMLString);
        tolua_function(tolua_S,"loadURL",lua_ax_webview_WebView_loadURL);
        tolua_function(tolua_S,"loadFile",lua_ax_webview_WebView_loadFile);
        tolua_function(tolua_S,"stopLoading",lua_ax_webview_WebView_stopLoading);
        tolua_function(tolua_S,"reload",lua_ax_webview_WebView_reload);
        tolua_function(tolua_S,"canGoBack",lua_ax_webview_WebView_canGoBack);
        tolua_function(tolua_S,"canGoForward",lua_ax_webview_WebView_canGoForward);
        tolua_function(tolua_S,"goBack",lua_ax_webview_WebView_goBack);
        tolua_function(tolua_S,"goForward",lua_ax_webview_WebView_goForward);
        tolua_function(tolua_S,"evaluateJS",lua_ax_webview_WebView_evaluateJS);
        tolua_function(tolua_S,"setScalesPageToFit",lua_ax_webview_WebView_setScalesPageToFit);
        tolua_function(tolua_S,"getOnJSCallback",lua_ax_webview_WebView_getOnJSCallback);
        tolua_function(tolua_S,"setBounces",lua_ax_webview_WebView_setBounces);
        tolua_function(tolua_S,"setOpacityWebView",lua_ax_webview_WebView_setOpacityWebView);
        tolua_function(tolua_S,"getOpacityWebView",lua_ax_webview_WebView_getOpacityWebView);
        tolua_function(tolua_S,"setBackgroundTransparent",lua_ax_webview_WebView_setBackgroundTransparent);
        tolua_function(tolua_S,"create", lua_ax_webview_WebView_create);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::ui::WebView).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axui.WebView";
    g_typeCast[typeName] = "axui.WebView";
    return 1;
}
TOLUA_API int register_all_ax_webview(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"axui",0);
	tolua_beginmodule(tolua_S,"axui");

	lua_register_ax_webview_WebView(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

