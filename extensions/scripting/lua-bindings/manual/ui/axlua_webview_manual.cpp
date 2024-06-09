/****************************************************************************
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
#include "lua-bindings/manual/ui/axlua_webview_manual.hpp"

#include "ui/UIWebView/UIWebView.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/LuaValue.h"
#include "lua-bindings/manual/LuaEngine.h"

static int axlua_WebView_setOnShouldStartLoading(lua_State* L)
{

    int argc                   = 0;
    ax::ui::WebView* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.WebView", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::ui::WebView*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_WebView_setOnShouldStartLoading'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 1)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));
        std::function<bool(ui::WebView * sender, std::string_view url)> callback =
            [L, handler](ui::WebView* sender, std::string_view url) -> bool {
            toluafix_pushusertype_object(L, sender->_ID, &(sender->_luaID), (void*)sender, "axui.WebView");
            tolua_pushsv(L, url);
            return LuaEngine::getInstance()->getLuaStack()->executeFunctionByHandler(handler, 2);
        };

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        self->setOnShouldStartLoading(callback);

        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "axui.WebView:setOnShouldStartLoading",
               argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_WebView_setOnShouldStartLoading'.", &tolua_err);
#endif
    return 0;
}

static int axlua_WebView_setOnDidFinishLoading(lua_State* L)
{

    int argc                   = 0;
    ax::ui::WebView* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.WebView", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::ui::WebView*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_WebView_setOnDidFinishLoading'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 1)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        LUA_FUNCTION handler                                                     = (toluafix_ref_function(L, 2, 0));
        std::function<void(ui::WebView * sender, std::string_view url)> callback = [L, handler](ui::WebView* sender,
                                                                                                std::string_view url) {
            toluafix_pushusertype_object(L, sender->_ID, &(sender->_luaID), (void*)sender, "axui.WebView");
            tolua_pushsv(L, url);
            LuaEngine::getInstance()->getLuaStack()->executeFunctionByHandler(handler, 2);
        };

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        self->setOnDidFinishLoading(callback);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "axui.WebView:setOnDidFinishLoading",
               argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_WebView_setOnDidFinishLoading'.", &tolua_err);
#endif
    return 0;
}

static int axlua_WebView_setOnDidFailLoading(lua_State* L)
{

    int argc                   = 0;
    ax::ui::WebView* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "axui.WebView", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = static_cast<ax::ui::WebView*>(tolua_tousertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_WebView_setOnDidFailLoading'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 1)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif

        LUA_FUNCTION handler                                                     = (toluafix_ref_function(L, 2, 0));
        std::function<void(ui::WebView * sender, std::string_view url)> callback = [L, handler](ui::WebView* sender,
                                                                                                std::string_view url) {
            toluafix_pushusertype_object(L, sender->_ID, &(sender->_luaID), (void*)sender, "axui.WebView");
            tolua_pushsv(L, url);
            LuaEngine::getInstance()->getLuaStack()->executeFunctionByHandler(handler, 2);
        };

        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        self->setOnDidFailLoading(callback);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "axui.WebView:setOnDidFailLoading", argc,
               1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_WebView_setOnDidFailLoading'.", &tolua_err);
#endif
    return 0;
}

static void extendWebView(lua_State* L)
{
    lua_pushstring(L, "axui.WebView");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "setOnShouldStartLoading", axlua_WebView_setOnShouldStartLoading);
        tolua_function(L, "setOnDidFinishLoading", axlua_WebView_setOnDidFinishLoading);
        tolua_function(L, "setOnDidFailLoading", axlua_WebView_setOnDidFailLoading);
    }
    lua_pop(L, 1);
}

int register_all_ax_webview_manual(lua_State* L)
{
    if (nullptr == L)
        return 0;

    extendWebView(L);

    return 0;
}
