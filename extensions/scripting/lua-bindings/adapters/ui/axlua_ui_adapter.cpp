/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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
#include "lua-bindings/adapters/ui/axlua_ui_adapter.hpp"
#include "lua-bindings/generated/axlua_ui_gen.h"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS)
#    include "lua-bindings/generated/axlua_video_gen.h"
#    include "lua-bindings/generated/axlua_webview_gen.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_MAC
#    include "lua-bindings/generated/axlua_video_gen.h"
#elif defined(_WIN32)
#    if defined(AX_ENABLE_MFMEDIA)
#        include "lua-bindings/generated/axlua_video_gen.h"
#    endif
#    if defined(AX_ENABLE_MSEDGE_WEBVIEW2)
#        include "lua-bindings/generated/axlua_webview_gen.h"
#    endif
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
#    include "lua-bindings/generated/axlua_webview_gen.h"
#    if defined(AX_ENABLE_VLC_MEDIA)
#        include "lua-bindings/generated/axlua_video_gen.h"
#    endif
#endif

#include "lua-bindings/runtime/axlua_adapter.h"
#include "lua-bindings/runtime/axlua_conversions.h"
#include "lua-bindings/runtime/axlua_runtime.h"
#include "lua-bindings/runtime/LuaValue.h"
#include "axmol/ui/CocosGUI.h"
#include "axmol/base/FocusEventListener.h"

using namespace ui;

static void extendWidget(lua_State* L)
{
    (void)L;
}

static int axlua_ListView_addScrollViewEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc       = 0;
    ListView* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "axui.ListView", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<ListView*>(axlua::adapter::to_usertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_ListView_addScrollViewEventListener'\n",
                                    NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(L, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        axlua::Callback<void(ax::Object*, ui::ScrollView::EventType)> callback(L, 2);
        auto scrollViewCallback = [callback = std::move(callback)](ax::Object* ref, ui::ScrollView::EventType eventType) mutable {
            callback(ref, eventType);
        };
        self->addEventListener((ui::ScrollView::ScrollViewCallback)scrollViewCallback);
        return 0;
    }

    luaL_error(
        L, "'addScrollViewEventListener' function of ListView has wrong number of arguments: %d, was expecting %d\n",
        argc, 1);

    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'addScrollViewEventListener'.", &conversionError);
    return 0;
#endif
}

static void extendListView(lua_State* L)
{
    axlua::adapter::push_literal(L, "axui.ListView");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "addScrollViewEventListener", axlua_ListView_addScrollViewEventListener);
    }
    lua_pop(L, 1);
}

static int axlua_LayoutParameter_setMargin(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc              = 0;
    LayoutParameter* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "axui.LayoutParameter", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<LayoutParameter*>(axlua::adapter::to_usertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_LayoutParameter_setMargin'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_table(L, 2, 0, &conversionError))
        {
            goto argumentError;
        }
#endif

        Margin margin;
        axlua::adapter::push_literal(L, "left");
        lua_gettable(L, 2);
        margin.left = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        axlua::adapter::push_literal(L, "top");
        lua_gettable(L, 2);
        margin.top = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        axlua::adapter::push_literal(L, "right");
        lua_gettable(L, 2);
        margin.right = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        axlua::adapter::push_literal(L, "bottom");
        lua_gettable(L, 2);
        margin.bottom = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        self->setMargin(margin);
        return 0;
    }

    luaL_error(L, "'setMargin' function of LayoutParameter has wrong number of arguments: %d, was expecting %d\n", argc,
               1);

    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'setMargin'.", &conversionError);
    return 0;
#endif
}

static int axlua_LayoutParameter_getMargin(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc              = 0;
    LayoutParameter* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "axui.LayoutParameter", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<LayoutParameter*>(axlua::adapter::to_usertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_LayoutParameter_getMargin'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (0 == argc)
    {
        Margin margin = self->getMargin();

        lua_newtable(L);

        axlua::adapter::push_literal(L, "left");
        lua_pushnumber(L, (lua_Number)margin.left);
        lua_rawset(L, -3);

        axlua::adapter::push_literal(L, "top");
        lua_pushnumber(L, (lua_Number)margin.top);
        lua_rawset(L, -3);

        axlua::adapter::push_literal(L, "right");
        lua_pushnumber(L, (lua_Number)margin.right);
        lua_rawset(L, -3);

        axlua::adapter::push_literal(L, "bottom");
        lua_pushnumber(L, (lua_Number)margin.bottom);
        lua_rawset(L, -3);

        return 1;
    }

    luaL_error(L, "'getMargin' function of LayoutParameter has wrong number of arguments: %d, was expecting %d\n", argc,
               0);

    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'getMargin'.", &conversionError);
    return 0;
#endif
}

static void extendLayoutParameter(lua_State* L)
{
    axlua::adapter::push_literal(L, "axui.LayoutParameter");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "setMargin", axlua_LayoutParameter_setMargin);
        axlua::adapter::set_function(L, "getMargin", axlua_LayoutParameter_getMargin);
    }
    lua_pop(L, 1);
}

static int axlua_EditBox_registerScriptEditBoxHandler(lua_State* L)
{
    if (NULL == L)
        return 0;

    int argc      = 0;
    EditBox* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "axui.EditBox", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<EditBox*>(axlua::adapter::to_usertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_EditBox_registerScriptEditBoxHandler'\n",
                                    NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(L, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        LUA_FUNCTION handler = (axlua::adapter::ref_function(L, 2, 0));
        self->registerScriptEditBoxHandler(handler);
        return 0;
    }

    luaL_error(L, "%s function of EditBox  has wrong number of arguments: %d, was expecting %d\n",
               "axui.EditBox:registerScriptEditBoxHandler", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_EditBox_registerScriptEditBoxHandler'.",
                                &conversionError);
    return 0;
#endif
}

static int axlua_EditBox_unregisterScriptEditBoxHandler(lua_State* L)
{

    if (NULL == L)
        return 0;

    int argc      = 0;
    EditBox* self = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "axui.EditBox", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<EditBox*>(axlua::adapter::to_usertype(L, 1, 0));

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_EditBox_unregisterScriptEditBoxHandler'\n",
                                    NULL);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (0 == argc)
    {
        self->unregisterScriptEditBoxHandler();
        return 0;
    }

    luaL_error(L, "%s function of EditBox  has wrong number of arguments: %d, was expecting %d\n",
               "axui.EditBox:unregisterScriptEditBoxHandler", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_EditBox_unregisterScriptEditBoxHandler'.",
                                &conversionError);
    return 0;
#endif
}

static void extendEditBox(lua_State* L)
{
    axlua::adapter::push_literal(L, "axui.EditBox");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::push_literal(L, "registerScriptEditBoxHandler");
        lua_pushcfunction(L, axlua_EditBox_registerScriptEditBoxHandler);
        lua_rawset(L, -3);
        axlua::adapter::push_literal(L, "unregisterScriptEditBoxHandler");
        lua_pushcfunction(L, axlua_EditBox_unregisterScriptEditBoxHandler);
        lua_rawset(L, -3);
    }
    lua_pop(L, 1);
}

int register_all_ax_ui_adapter(lua_State* L)
{
    if (nullptr == L)
        return 0;
    extendWidget(L);
    extendListView(L);
    extendLayoutParameter(L);
    extendEditBox(L);

    return 0;
}

static int toaxlua_FocusEventListener_create(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = 0;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertable(L, 1, "ax.FocusEventListener", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 0)
    {
        ax::FocusEventListener* returnValue = ax::FocusEventListener::create();
        if (nullptr == returnValue)
            return 0;

        axlua::adapter::push_object(L, (void*)returnValue, "ax.FocusEventListener");

        return 1;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.FocusEventListener:create", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'toaxlua_FocusEventListener_create'.", &conversionError);
    return 0;
#endif
}

static void cloneFocusHandler(const FocusEventListener* src, FocusEventListener* dst)
{
    if (nullptr == src || nullptr == dst)
        return;

    if (src != nullptr && dst != nullptr)
        dst->onFocusChanged = src->onFocusChanged;
}

static int toaxlua_FocusEventListener_clone(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc                 = 0;
    FocusEventListener* self = nullptr;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "ax.FocusEventListener", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<FocusEventListener*>(axlua::adapter::to_usertype(L, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'toaxlua_FocusEventListener_clone'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 0)
    {
        ax::FocusEventListener* returnValue = ax::FocusEventListener::create();
        if (nullptr == returnValue)
            return 0;

        cloneFocusHandler(self, returnValue);

        axlua::adapter::push_object(L, (void*)returnValue, "ax.FocusEventListener");

        return 1;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.FocusEventListener:clone", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'toaxlua_FocusEventListener_clone'.", &conversionError);
    return 0;
#endif
}

static int toaxlua_FocusEventListener_registerScriptHandler(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc                 = 0;
    FocusEventListener* self = nullptr;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "ax.FocusEventListener", 0, &conversionError))
        goto argumentError;
#endif

    self = static_cast<FocusEventListener*>(axlua::adapter::to_usertype(L, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(
            L, "invalid 'self' in function 'toaxlua_FocusEventListener_registerScriptHandler'\n", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;

    if (argc == 1)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(L, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#endif
        axlua::Callback<void(ui::Widget*, ui::Widget*)> callback(L, 2);
        self->onFocusChanged = [callback = std::move(callback)](ui::Widget* lost, ui::Widget* gained) mutable {
            callback(lost, gained);
        };
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n",
               "ax.FocusEventListener:registerScriptHandler", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'toaxlua_FocusEventListener_registerScriptHandler'.",
                                &conversionError);
    return 0;
#endif
}

static void extendFocusEventListenerEvent(lua_State* L)
{
    axlua::adapter::push_literal(L, "ax.FocusEventListener");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "create", toaxlua_FocusEventListener_create);
        axlua::adapter::set_function(L, "registerScriptHandler", toaxlua_FocusEventListener_registerScriptHandler);
        axlua::adapter::set_function(L, "clone", toaxlua_FocusEventListener_clone);
    }
    lua_pop(L, 1);
}

int register_ui_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_ui(L);
        register_all_ax_ui_adapter(L);
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS)
        register_all_ax_video(L);
        register_all_ax_webview(L);
#elif AX_TARGET_PLATFORM == AX_PLATFORM_MAC
        register_all_ax_video(L);
#elif defined(_WIN32)
#    if defined(AX_ENABLE_MFMEDIA)
        register_all_ax_video(L);
#    endif
#    if defined(AX_ENABLE_MSEDGE_WEBVIEW2)
        register_all_ax_webview(L);
#    endif
#elif AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
        register_all_ax_webview(L);
#    if defined(AX_ENABLE_VLC_MEDIA)
        register_all_ax_video(L);
#    endif
#endif
        extendFocusEventListenerEvent(L);
    }
    lua_pop(L, 1);

    return 1;
}
