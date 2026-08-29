#if defined(AX_ENABLE_EXT_FAIRYGUI)
#    include "axlua_fairygui_adapter.hpp"
#    include "lua-bindings/generated/axlua_fairygui_gen.h"
#    include "lua-bindings/runtime/axlua_runtime.h"
#    include "lua-bindings/runtime/axlua_adapter.h"
#    include "lua-bindings/runtime/axlua_conversions.h"
#    include "lua-bindings/runtime/LuaEngine.h"

#    include "fairygui/FairyGUI.h"
#    include "fairygui/FairyGUIMacros.h"
#    include "fairygui/GLabel.h"

static int handleFairyguiEvent(int handler, fairygui::EventContext* sender)
{
    LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
    object_to_luaval<fairygui::EventContext>(stack->getLuaState(), "fgui.EventContext",
                                             (fairygui::EventContext*)sender);
    stack->executeFunctionByHandler(handler, 1);
    stack->clean();
    return 0;
}

static int handleFairyguiEventNoParams(int handler)
{
    LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
    stack->executeFunctionByHandler(handler, 0);
    stack->clean();
    return 0;
}

// GObject's inline convenience overload is not exposed as a standalone AST
// method by libclang. Keep the old Lua spelling, but reuse the same callback
// factory as generated bindings instead of maintaining another handler bridge.
static int lua_ax_fairygui_addClickListener(lua_State* L)
{
    auto* self = static_cast<fairygui::GObject*>(axlua::adapter::to_usertype(L, 1, 0));
    if (self == nullptr || !lua_isfunction(L, 2))
        return luaL_error(L, "fgui.GObject:addClickListener expects a function");

    const int argc = lua_gettop(L) - 1;
    if (argc != 1 && argc != 2)
        return luaL_error(L, "fgui.GObject:addClickListener expects a function and optional tag");
    auto callback = axlua::make_lua_callback<void(fairygui::EventContext*)>(L, 2);
    if (argc == 1)
        self->addClickListener(callback);
    else
        self->addClickListener(callback, fairygui::EventTag(static_cast<int>(luaL_checkinteger(L, 3))));
    return 0;
}

static int lua_ax_fairygui_removeClickListener(lua_State* L)
{
    if (nullptr == L)
        return 0;
    int argc                = 0;
    fairygui::GObject* self = nullptr;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "fgui.GObject", 0, &conversionError))
        goto argumentError;
#    endif
    self = static_cast<fairygui::GObject*>(axlua::adapter::to_usertype(L, 1, 0));
#    if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'lua_ax_fairygui_removeClickListener'\n", NULL);
        return 0;
    }
#    endif
    argc = lua_gettop(L) - 1;
    if (0 == argc)
    {
        self->removeClickListener(fairygui::EventTag::None);

        return 0;
    }
    if (1 == argc)
    {
        bool ok = true;
        int tag;
        ok &= luaval_to_int(L, 2, (int*)&tag, "lua_ax_fairygui_removeClickListener");
        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'lua_ax_fairygui_removeClickListener'",
                                        nullptr);
            return 0;
        }
        self->removeClickListener(fairygui::EventTag(tag));
        return 0;
    }
    luaL_error(L, "'removeClickListener' function of GObject has wrong number of arguments: %d, was expecting %d\n",
               argc, 1);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'removeClickListener'.", &conversionError);
    return 0;
#    endif
}

int lua_ax_fairygui_GObject_getData(lua_State* luaState)
{
    int argc               = 0;
    fairygui::GObject* obj = nullptr;
    bool ok                = true;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif
#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "fgui.GObject", 0, &conversionError))
        goto argumentError;
#    endif
    obj = (fairygui::GObject*)axlua::adapter::to_usertype(luaState, 1, 0);
#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'lua_ax_fairygui_GObject_getData'", nullptr);
        return 0;
    }
#    endif
    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            axlua::adapter::raise_error(luaState, "invalid arguments in function 'lua_ax_fairygui_GObject_getData'",
                                        nullptr);
            return 0;
        }
        void* ret = obj->getData();
        object_to_luaval<ax::Object>(luaState, /*"cc."*/ "ax.Object", (ax::Object*)ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getData", argc, 0);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'lua_ax_fairygui_GObject_getData'.", &conversionError);
    return 0;
#    endif
}

static int lua_ax_fairygui_removeEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;
    int argc                          = 0;
    fairygui::UIEventDispatcher* self = nullptr;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "fgui.UIEventDispatcher", 0, &conversionError))
        goto argumentError;
#    endif
    self = static_cast<fairygui::UIEventDispatcher*>(axlua::adapter::to_usertype(L, 1, 0));
#    if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'lua_ax_fairygui_removeEventListener'\n", NULL);
        return 0;
    }
#    endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
        bool ok = true;
        int eventType;
        ok &= luaval_to_int(L, 2, (int*)&eventType, "lua_ax_fairygui_removeEventListener");
        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'lua_ax_fairygui_removeEventListener'",
                                        nullptr);
            return 0;
        }
        self->removeEventListener(eventType);
        return 0;
    }
    if (2 == argc)
    {
        bool ok = true;
        int eventType;
        ok &= luaval_to_int(L, 2, (int*)&eventType, "lua_ax_fairygui_removeEventListener");
        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'lua_ax_fairygui_removeEventListener'",
                                        nullptr);
            return 0;
        }
        int tag;
        ok &= luaval_to_int(L, 3, (int*)&tag, "lua_ax_fairygui_removeEventListener");
        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'lua_ax_fairygui_removeEventListener'",
                                        nullptr);
            return 0;
        }
        self->removeEventListener(eventType, fairygui::EventTag(tag));
        return 0;
    }
    luaL_error(
        L, "'removeEventListener' function of UIEventDispatcher has wrong number of arguments: %d, was expecting %d\n",
        argc, 1);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'removeEventListener'.", &conversionError);
    return 0;
#    endif
}

static int lua_ax_fairygui_hasEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;
    int argc                          = 0;
    fairygui::UIEventDispatcher* self = nullptr;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "fgui.UIEventDispatcher", 0, &conversionError))
        goto argumentError;
#    endif
    self = static_cast<fairygui::UIEventDispatcher*>(axlua::adapter::to_usertype(L, 1, 0));
#    if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'lua_ax_fairygui_hasEventListener'\n", NULL);
        return 0;
    }
#    endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
        bool ok = true;
        int eventType;
        ok &= luaval_to_int(L, 2, (int*)&eventType, "lua_ax_fairygui_hasEventListener");
        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'lua_ax_fairygui_hasEventListener'", nullptr);
            return 0;
        }
        self->hasEventListener(eventType);
        return 0;
    }
    if (2 == argc)
    {
        bool ok = true;
        int eventType;
        ok &= luaval_to_int(L, 2, (int*)&eventType, "lua_ax_fairygui_hasEventListener");
        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'lua_ax_fairygui_hasEventListener'", nullptr);
            return 0;
        }
        int tag;
        ok &= luaval_to_int(L, 3, (int*)&tag, "lua_ax_fairygui_hasEventListener");
        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'lua_ax_fairygui_hasEventListener'", nullptr);
            return 0;
        }
        self->hasEventListener(eventType, fairygui::EventTag(tag));
        return 0;
    }
    luaL_error(L,
               "'hasEventListener' function of UIEventDispatcher has wrong number of arguments: %d, was expecting %d\n",
               argc, 1);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'lua_ax_fairygui_hasEventListener'.", &conversionError);
    return 0;
#    endif
}

static int lua_ax_fairygui_play(lua_State* L)
{
    if (nullptr == L)
        return 0;
    int argc                   = 0;
    fairygui::Transition* self = nullptr;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "fgui.Transition", 0, &conversionError))
        goto argumentError;
#    endif
    self = static_cast<fairygui::Transition*>(axlua::adapter::to_usertype(L, 1, 0));
#    if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'lua_ax_fairygui_play'\n", NULL);
        return 0;
    }
#    endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#    if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(L, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#    endif
        LUA_FUNCTION handler = (axlua::adapter::ref_function(L, 2, 0));
        self->play([=]() { handleFairyguiEventNoParams(handler); });
        AxluaCallbackRegistry::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }
    if (3 == argc)
    {
#    if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(L, 4, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#    endif
        LUA_FUNCTION handler = (axlua::adapter::ref_function(L, 4, 0));
        bool ok              = true;
        int times;
        ok &= luaval_to_int(L, 2, (int*)&times, "lua_ax_fairygui_play");
        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'lua_ax_fairygui_play'", nullptr);
            return 0;
        }
        double delay;
        ok &= luaval_to_number(L, 3, (double*)&delay, "lua_ax_fairygui_play");
        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'lua_ax_fairygui_play'", nullptr);
            return 0;
        }
        self->play(times, (float)delay, [=]() { handleFairyguiEventNoParams(handler); });
        AxluaCallbackRegistry::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }
    luaL_error(L, "'addEventListener' function of Transition has wrong number of arguments: %d, was expecting %d\n",
               argc, 1);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'lua_ax_fairygui_play'.", &conversionError);
#    endif
    return 0;
}

static int lua_ax_fairygui_UIObjectFactory_setPackageItemExtension(lua_State* luaState)
{
    int argc = 0;
    bool ok  = true;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif
#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(luaState, 1, "fgui.UIObjectFactory", 0, &conversionError))
        goto argumentError;
#    endif
    argc = lua_gettop(luaState) - 1;
    if (argc == 3 || argc == 4)
    {
#    if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(luaState, 3, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#    endif
        LUA_FUNCTION handler = (axlua::adapter::ref_function(luaState, 3, 0));
        bool ok              = true;
        std::string url;
        ok &= luaval_to_std_string(luaState, 2, &url, "lua_ax_fairygui_UIObjectFactory_setPackageItemExtension");
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'lua_ax_fairygui_UIObjectFactory_setPackageItemExtension'",
                nullptr);
            return 0;
        }
        std::string retType;
        ok &= luaval_to_std_string(luaState, 4, &retType, "lua_ax_fairygui_UIObjectFactory_setPackageItemExtension");
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'lua_ax_fairygui_UIObjectFactory_setPackageItemExtension'",
                nullptr);
            return 0;
        }
        fairygui::UIObjectFactory::setPackageItemExtension(url, [=]() -> fairygui::GComponent* {
            fairygui::GComponent* ret = nullptr;
            if (retType == "GButton")
                ret = fairygui::GButton::create();
            else if (retType == "GLabel")
                ret = fairygui::GLabel::create();
            else if (retType == "GProgressBar")
                ret = fairygui::GProgressBar::create();
            else if (retType == "GSlider")
                ret = fairygui::GSlider::create();
            else if (retType == "GScrollBar")
                ret = fairygui::GScrollBar::create();
            else if (retType == "GComboBox")
                ret = fairygui::GComboBox::create();
            else
                ret = fairygui::GComponent::create();
            LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
            stack->pushObject(ret, "fgui.GComponent");
            stack->executeFunctionByHandler(handler, 1);
            return ret;
        });
        ax::Object* self = nullptr;
        if (argc == 4)
        {
            self = (ax::Object*)axlua::adapter::to_usertype(luaState, 5, 0);
        }
        if (self)
        {
            AxluaCallbackRegistry::getInstance()->addCustomHandler((void*)self, handler);
        }
        else
        {
            AxluaCallbackRegistry::getInstance()->addCustomHandler((void*)Director::getInstance(), handler);
        }
        return 0;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d\n ",
               "fgui.UIObjectFactory:setPackageItemExtension", argc, 2);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(
        luaState, "#ferror in function 'lua_ax_fairygui_UIObjectFactory_setPackageItemExtension'.", &conversionError);
    return 0;
#    endif
}

static int lua_ax_fairygui_GList_setItemRenderer(lua_State* L)
{
    if (nullptr == L)
        return 0;
    int argc              = 0;
    fairygui::GList* self = nullptr;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "fgui.GObject", 0, &conversionError))
        goto argumentError;
#    endif
    self = static_cast<fairygui::GList*>(axlua::adapter::to_usertype(L, 1, 0));
#    if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'lua_ax_fairygui_GList_setItemRenderer'\n", NULL);
        return 0;
    }
#    endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#    if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(L, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#    endif
        LUA_FUNCTION handler = (axlua::adapter::ref_function(L, 2, 0));
        self->itemRenderer   = [=](int index, fairygui::GObject* obj) {
            LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
            stack->pushInt(index);
            object_to_luaval<fairygui::GObject>(stack->getLuaState(), "fgui.GObject", (fairygui::GObject*)obj);
            stack->executeFunctionByHandler(handler, 2);
            stack->clean();
        };
        AxluaCallbackRegistry::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }
    luaL_error(L, "'setItemRenderer' function of GObject has wrong number of arguments: %d, was expecting %d\n", argc,
               1);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'lua_ax_fairygui_GList_setItemRenderer'.", &conversionError);
    return 0;
#    endif
}

int lua_ax_fairygui_EventContext_getDataValue(lua_State* luaState)
{
    int argc                    = 0;
    fairygui::EventContext* obj = nullptr;
    bool ok                     = true;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif
#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "fgui.EventContext", 0, &conversionError))
        goto argumentError;
#    endif
    obj = (fairygui::EventContext*)axlua::adapter::to_usertype(luaState, 1, 0);
#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'lua_ax_fairygui_EventContext_getDataValue'",
                                    nullptr);
        return 0;
    }
#    endif
    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'lua_ax_fairygui_EventContext_getDataValue'", nullptr);
            return 0;
        }
        const cocos2d::Value& ret = obj->getDataValue();
        value_to_luaval(luaState, ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:getDataValue",
               argc, 0);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'lua_ax_fairygui_EventContext_getDataValue'.",
                                &conversionError);
    return 0;
#    endif
}

int lua_ax_fairygui_EventContext_getData(lua_State* luaState)
{
    int argc                    = 0;
    fairygui::EventContext* obj = nullptr;
    bool ok                     = true;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif
#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "fgui.EventContext", 0, &conversionError))
        goto argumentError;
#    endif
    obj = (fairygui::EventContext*)axlua::adapter::to_usertype(luaState, 1, 0);
#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'lua_ax_fairygui_EventContext_getData'",
                                    nullptr);
        return 0;
    }
#    endif
    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'lua_ax_fairygui_EventContext_getData'", nullptr);
            return 0;
        }
        void* ret = obj->getData();
        object_to_luaval<ax::Object>(luaState, /*"cc."*/ "ax.Object", (ax::Object*)ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:getData", argc,
               0);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'lua_ax_fairygui_EventContext_getData'.",
                                &conversionError);
    return 0;
#    endif
}

static void extendUIEventDispatcher(lua_State* L)
{
    lua_pushstring(L, "fgui.UIEventDispatcher");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "removeEventListener", lua_ax_fairygui_removeEventListener);
        axlua::adapter::set_function(L, "hasEventListener", lua_ax_fairygui_hasEventListener);
    }
    lua_pop(L, 1);
}

static void extendGObject(lua_State* L)
{
    lua_pushstring(L, "fgui.GObject");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "addClickListener", lua_ax_fairygui_addClickListener);
        axlua::adapter::set_function(L, "removeClickListener", lua_ax_fairygui_removeClickListener);
        axlua::adapter::set_function(L, "getData", lua_ax_fairygui_GObject_getData);
    }
    lua_pop(L, 1);
}

static void extendTransition(lua_State* L)
{
    lua_pushstring(L, "fgui.Transition");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "play", lua_ax_fairygui_play);
    }
    lua_pop(L, 1);
}

static void extendUIObjectFactory(lua_State* L)
{
    lua_pushstring(L, "fgui.UIObjectFactory");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "setPackageItemExtension",
                                     lua_ax_fairygui_UIObjectFactory_setPackageItemExtension);
    }
    lua_pop(L, 1);
}

static void extendGList(lua_State* L)
{
    lua_pushstring(L, "fgui.GList");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "setItemRenderer", lua_ax_fairygui_GList_setItemRenderer);
    }
    lua_pop(L, 1);
}

static void extendEventContext(lua_State* L)
{
    lua_pushstring(L, "fgui.EventContext");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "getDataValue", lua_ax_fairygui_EventContext_getDataValue);
        axlua::adapter::set_function(L, "getData", lua_ax_fairygui_EventContext_getData);
    }
    lua_pop(L, 1);
}

int lua_ax_fairygui_GMovieClip_setPlaySettings(lua_State* luaState)
{
    int argc                  = 0;
    fairygui::GMovieClip* obj = nullptr;
    bool ok                   = true;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif
#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "fgui.GMovieClip", 0, &conversionError))
        goto argumentError;
#    endif
    obj = (fairygui::GMovieClip*)axlua::adapter::to_usertype(luaState, 1, 0);
#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'",
                                    nullptr);
        return 0;
    }
#    endif
    argc = lua_gettop(luaState) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
            return 0;
        }
        obj->setPlaySettings();
        lua_settop(luaState, 1);
        return 1;
    }
    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int(luaState, 2, (int*)&arg0, "fgui.GMovieClip:setPlaySettings");
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
            return 0;
        }
        obj->setPlaySettings(arg0);
        lua_settop(luaState, 1);
        return 1;
    }
    if (argc == 2)
    {
        int arg0;
        int arg1;
        ok &= luaval_to_int(luaState, 2, (int*)&arg0, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int(luaState, 3, (int*)&arg1, "fgui.GMovieClip:setPlaySettings");
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
            return 0;
        }
        obj->setPlaySettings(arg0, arg1);
        lua_settop(luaState, 1);
        return 1;
    }
    if (argc == 3)
    {
        int arg0;
        int arg1;
        int arg2;
        ok &= luaval_to_int(luaState, 2, (int*)&arg0, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int(luaState, 3, (int*)&arg1, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int(luaState, 4, (int*)&arg2, "fgui.GMovieClip:setPlaySettings");
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
            return 0;
        }
        obj->setPlaySettings(arg0, arg1, arg2);
        lua_settop(luaState, 1);
        return 1;
    }
    if (argc == 4)
    {
        int arg0;
        int arg1;
        int arg2;
        int arg3;
        ok &= luaval_to_int(luaState, 2, (int*)&arg0, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int(luaState, 3, (int*)&arg1, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int(luaState, 4, (int*)&arg2, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int(luaState, 5, (int*)&arg3, "fgui.GMovieClip:setPlaySettings");
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
            return 0;
        }
        obj->setPlaySettings(arg0, arg1, arg2, arg3);
        lua_settop(luaState, 1);
        return 1;
    }
    if (argc == 5)
    {
#    if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(luaState, 6, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#    endif
        int arg0;
        int arg1;
        int arg2;
        int arg3;
        ok &= luaval_to_int(luaState, 2, (int*)&arg0, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int(luaState, 3, (int*)&arg1, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int(luaState, 4, (int*)&arg2, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int(luaState, 5, (int*)&arg3, "fgui.GMovieClip:setPlaySettings");
        if (!ok)
        {
            axlua::adapter::raise_error(
                luaState, "invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
            return 0;
        }
        LUA_FUNCTION handler = (axlua::adapter::ref_function(luaState, 6, 0));
        obj->setPlaySettings(arg0, arg1, arg2, arg3, [=](void) { handleFairyguiEventNoParams(handler); });
        AxluaCallbackRegistry::getInstance()->addCustomHandler((void*)obj, handler);
        lua_settop(luaState, 1);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:setPlaySettings",
               argc, 0);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'.",
                                &conversionError);
#    endif
    return 0;
}

static void extendGMovieClip(lua_State* L)
{
    lua_pushstring(L, "fgui.GMovieClip");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
        axlua::adapter::set_function(L, "setPlaySettings", lua_ax_fairygui_GMovieClip_setPlaySettings);
    lua_pop(L, 1);
}

int lua_ax_fairygui_GTweener_onUpdate(lua_State* luaState)
{
    int argc                = 0;
    fairygui::GTweener* obj = nullptr;
    bool ok                 = true;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif
#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "fgui.GTweener", 0, &conversionError))
        goto argumentError;
#    endif
    obj = (fairygui::GTweener*)axlua::adapter::to_usertype(luaState, 1, 0);
#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'lua_ax_fairygui_GTweener_onUpdate'", nullptr);
        return 0;
    }
#    endif
    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
#    if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#    endif
        LUA_FUNCTION handler = (axlua::adapter::ref_function(luaState, 2, 0));
        auto&& ret           = obj->onUpdate([=](fairygui::GTweener* sender) {
            LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
            object_to_luaval<fairygui::GTweener>(stack->getLuaState(), "fgui.GTweener", (fairygui::GTweener*)sender);
            stack->executeFunctionByHandler(handler, 1);
            stack->clean();
        });
        AxluaCallbackRegistry::getInstance()->addCustomHandler((void*)obj, handler);
        object_to_luaval<fairygui::GTweener>(luaState, "fgui.GTweener", (fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:onUpdate", argc,
               1);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'lua_ax_fairygui_GTweener_onUpdate'.", &conversionError);
#    endif
    return 0;
}

int lua_ax_fairygui_GTweener_onStart(lua_State* luaState)
{
    int argc                = 0;
    fairygui::GTweener* obj = nullptr;
    bool ok                 = true;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif
#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "fgui.GTweener", 0, &conversionError))
        goto argumentError;
#    endif
    obj = (fairygui::GTweener*)axlua::adapter::to_usertype(luaState, 1, 0);
#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'lua_ax_fairygui_GTweener_onStart'", nullptr);
        return 0;
    }
#    endif
    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
#    if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#    endif
        LUA_FUNCTION handler = (axlua::adapter::ref_function(luaState, 2, 0));
        auto&& ret           = obj->onStart([=](fairygui::GTweener* sender) {
            LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
            object_to_luaval<fairygui::GTweener>(stack->getLuaState(), "fgui.GTweener", (fairygui::GTweener*)sender);
            stack->executeFunctionByHandler(handler, 1);
            stack->clean();
        });
        AxluaCallbackRegistry::getInstance()->addCustomHandler((void*)obj, handler);
        object_to_luaval<fairygui::GTweener>(luaState, "fgui.GTweener", (fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:onStart", argc, 1);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'lua_ax_fairygui_GTweener_onStart'.", &conversionError);
#    endif
    return 0;
}

int lua_ax_fairygui_GTweener_onComplete(lua_State* luaState)
{
    int argc                = 0;
    fairygui::GTweener* obj = nullptr;
    bool ok                 = true;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif
#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "fgui.GTweener", 0, &conversionError))
        goto argumentError;
#    endif
    obj = (fairygui::GTweener*)axlua::adapter::to_usertype(luaState, 1, 0);
#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'lua_ax_fairygui_GTweener_onComplete'",
                                    nullptr);
        return 0;
    }
#    endif
    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
#    if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#    endif
        LUA_FUNCTION handler = (axlua::adapter::ref_function(luaState, 2, 0));
        auto&& ret           = obj->onComplete([=](void) { handleFairyguiEventNoParams(handler); });
        AxluaCallbackRegistry::getInstance()->addCustomHandler((void*)obj, handler);
        object_to_luaval<fairygui::GTweener>(luaState, "fgui.GTweener", (fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:onComplete", argc,
               1);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'lua_ax_fairygui_GTweener_onComplete'.",
                                &conversionError);
#    endif
    return 0;
}

int lua_ax_fairygui_GTweener_onComplete1(lua_State* luaState)
{
    int argc                = 0;
    fairygui::GTweener* obj = nullptr;
    bool ok                 = true;
#    if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#    endif
#    if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "fgui.GTweener", 0, &conversionError))
        goto argumentError;
#    endif
    obj = (fairygui::GTweener*)axlua::adapter::to_usertype(luaState, 1, 0);
#    if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'lua_ax_fairygui_GTweener_onComplete1'",
                                    nullptr);
        return 0;
    }
#    endif
    argc = lua_gettop(luaState) - 1;
    if (argc == 1)
    {
#    if _AX_DEBUG >= 1
        if (!axlua::adapter::is_function(luaState, 2, "LUA_FUNCTION", 0, &conversionError))
        {
            goto argumentError;
        }
#    endif
        LUA_FUNCTION handler = (axlua::adapter::ref_function(luaState, 2, 0));
        auto&& ret           = obj->onComplete1([=](fairygui::GTweener* sender) {
            LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
            object_to_luaval<fairygui::GTweener>(stack->getLuaState(), "fgui.GTweener", (fairygui::GTweener*)sender);
            stack->executeFunctionByHandler(handler, 1);
            stack->clean();
        });
        AxluaCallbackRegistry::getInstance()->addCustomHandler((void*)obj, handler);
        object_to_luaval<fairygui::GTweener>(luaState, "fgui.GTweener", (fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:onComplete1", argc,
               1);
    return 0;
#    if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'lua_ax_fairygui_GTweener_onComplete1'.",
                                &conversionError);
#    endif
    return 0;
}

static void extendGTweener(lua_State* L)
{
    lua_pushstring(L, "fgui.GTweener");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "onUpdate", lua_ax_fairygui_GTweener_onUpdate);
        axlua::adapter::set_function(L, "onStart", lua_ax_fairygui_GTweener_onStart);
        axlua::adapter::set_function(L, "onComplete", lua_ax_fairygui_GTweener_onComplete);
        axlua::adapter::set_function(L, "onComplete1", lua_ax_fairygui_GTweener_onComplete1);
    }
    lua_pop(L, 1);
}

static int register_all_fairygui_adapter(lua_State* L)
{
    if (nullptr == L)
        return 0;

    extendUIEventDispatcher(L);
    extendGObject(L);
    extendEventContext(L);
    extendTransition(L);
    extendUIObjectFactory(L);
    extendGList(L);
    extendGMovieClip(L);
    extendGTweener(L);

    return 1;
}

int register_fairygui_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_fairygui(L);
        register_all_fairygui_adapter(L);
    }
    lua_pop(L, 1);
    return 1;
}

#endif  // defined(AX_ENABLE_EXT_FAIRYGUI)
