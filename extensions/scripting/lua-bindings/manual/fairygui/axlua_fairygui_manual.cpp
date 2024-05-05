#include "axlua_fairygui_manual.hpp"
#include "lua-bindings/auto/axlua_fairygui_auto.hpp"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/LuaEngine.h"

#include "fairygui/FairyGUI.h"
#include "fairygui/FairyGUIMacros.h"
#include "fairygui/GLabel.h"

static int handleFairyguiEvent(int handler, fairygui::EventContext* sender)
{
    LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
    object_to_luaval<fairygui::EventContext>(stack->getLuaState(), "fgui.EventContext", (fairygui::EventContext*)sender);
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

static int lua_ax_fairygui_addClickListener(lua_State* L)
{
    if (nullptr == L)
        return 0;
    int argc = 0;
    fairygui::GObject* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "fgui.GObject", 0, &tolua_err)) goto tolua_lerror;
#endif
    self = static_cast<fairygui::GObject*>(tolua_tousertype(L, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L, "invalid 'self' in function 'lua_ax_fairygui_addClickListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));
        self->addClickListener([=](fairygui::EventContext* sender) {
            handleFairyguiEvent(handler, sender);
        });
        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }
    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));
        bool ok = true;
        int tag;
        ok &= luaval_to_int32(L, 3, (int *)&tag, "lua_ax_fairygui_addClickListener");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_addClickListener'", nullptr);
            return 0;
        }
        self->addClickListener([=](fairygui::EventContext* sender) {
            handleFairyguiEvent(handler, sender);
        }, fairygui::EventTag(tag));
        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }
    luaL_error(L, "'addClickListener' function of GObject has wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addClickListener'.", &tolua_err);
    return 0;
#endif
}

static int lua_ax_fairygui_removeClickListener(lua_State* L)
{
    if (nullptr == L)
        return 0;
    int argc = 0;
    fairygui::GObject* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "fgui.GObject", 0, &tolua_err)) goto tolua_lerror;
#endif
    self = static_cast<fairygui::GObject*>(tolua_tousertype(L, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L, "invalid 'self' in function 'lua_ax_fairygui_removeClickListener'\n", NULL);
        return 0;
    }
#endif
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
        ok &= luaval_to_int32(L, 2, (int *)&tag, "lua_ax_fairygui_removeClickListener");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_removeClickListener'", nullptr);
            return 0;
        }
        self->removeClickListener(fairygui::EventTag(tag));
        return 0;
    }
    luaL_error(L, "'removeClickListener' function of GObject has wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'removeClickListener'.", &tolua_err);
    return 0;
#endif
}

int lua_ax_fairygui_GObject_getData(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GObject* cobj = nullptr;
    bool ok = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "fgui.GObject", 0, &tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GObject*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_fairygui_GObject_getData'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_ax_fairygui_GObject_getData'", nullptr);
            return 0;
        }
        void* ret = cobj->getData();
        object_to_luaval<cocos2d::Object>(tolua_S, /*"cc."*/"ax.Object", (cocos2d::Object*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GObject:getData", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_fairygui_GObject_getData'.", &tolua_err);
    return 0;
#endif
}

static int lua_ax_fairygui_addEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;
    int argc = 0;
    fairygui::UIEventDispatcher* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "fgui.UIEventDispatcher", 0, &tolua_err)) goto tolua_lerror;
#endif
    self = static_cast<fairygui::UIEventDispatcher*>(tolua_tousertype(L, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L, "invalid 'self' in function 'lua_ax_fairygui_addEventListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 3, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 3, 0));
        bool ok = true;
        int eventType;
        ok &= luaval_to_int32(L, 2, (int *)&eventType, "lua_ax_fairygui_addEventListener");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_addEventListener'", nullptr);
            return 0;
        }
        self->addEventListener(eventType, [=](fairygui::EventContext* sender) {
            handleFairyguiEvent(handler, sender);
        });
        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }
    if (3 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 3, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 3, 0));
        bool ok = true;
        int eventType;
        ok &= luaval_to_int32(L, 2, (int *)&eventType, "lua_ax_fairygui_addEventListener");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_addEventListener'", nullptr);
            return 0;
        }
        int tag;
        ok &= luaval_to_int32(L, 4, (int *)&tag, "lua_ax_fairygui_addEventListener");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_addEventListener'", nullptr);
            return 0;
        }
        self->addEventListener(eventType, [=](fairygui::EventContext* sender) {
            handleFairyguiEvent(handler, sender);
        }, fairygui::EventTag(tag));
        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }
    luaL_error(L, "'addEventListener' function of UIEventDispatcher has wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'addEventListener'.", &tolua_err);
    return 0;
#endif
}

static int lua_ax_fairygui_removeEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;
    int argc = 0;
    fairygui::UIEventDispatcher* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "fgui.UIEventDispatcher", 0, &tolua_err)) goto tolua_lerror;
#endif
    self = static_cast<fairygui::UIEventDispatcher*>(tolua_tousertype(L, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L, "invalid 'self' in function 'lua_ax_fairygui_removeEventListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
        bool ok = true;
        int eventType;
        ok &= luaval_to_int32(L, 2, (int *)&eventType, "lua_ax_fairygui_removeEventListener");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_removeEventListener'", nullptr);
            return 0;
        }
        self->removeEventListener(eventType);
        return 0;
    }
    if (2 == argc)
    {
        bool ok = true;
        int eventType;
        ok &= luaval_to_int32(L, 2, (int *)&eventType, "lua_ax_fairygui_removeEventListener");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_removeEventListener'", nullptr);
            return 0;
        }
        int tag;
        ok &= luaval_to_int32(L, 3, (int *)&tag, "lua_ax_fairygui_removeEventListener");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_removeEventListener'", nullptr);
            return 0;
        }
        self->removeEventListener(eventType, fairygui::EventTag(tag));
        return 0;
    }
    luaL_error(L, "'removeEventListener' function of UIEventDispatcher has wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'removeEventListener'.", &tolua_err);
    return 0;
#endif
}

static int lua_ax_fairygui_hasEventListener(lua_State* L)
{
    if (nullptr == L)
        return 0;
    int argc = 0;
    fairygui::UIEventDispatcher* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "fgui.UIEventDispatcher", 0, &tolua_err)) goto tolua_lerror;
#endif
    self = static_cast<fairygui::UIEventDispatcher*>(tolua_tousertype(L, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L, "invalid 'self' in function 'lua_ax_fairygui_hasEventListener'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
        bool ok = true;
        int eventType;
        ok &= luaval_to_int32(L, 2, (int *)&eventType, "lua_ax_fairygui_hasEventListener");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_hasEventListener'", nullptr);
            return 0;
        }
        self->hasEventListener(eventType);
        return 0;
    }
    if (2 == argc)
    {
        bool ok = true;
        int eventType;
        ok &= luaval_to_int32(L, 2, (int *)&eventType, "lua_ax_fairygui_hasEventListener");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_hasEventListener'", nullptr);
            return 0;
        }
        int tag;
        ok &= luaval_to_int32(L, 3, (int *)&tag, "lua_ax_fairygui_hasEventListener");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_hasEventListener'", nullptr);
            return 0;
        }
        self->hasEventListener(eventType, fairygui::EventTag(tag));
        return 0;
    }
    luaL_error(L, "'hasEventListener' function of UIEventDispatcher has wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_ax_fairygui_hasEventListener'.", &tolua_err);
    return 0;
#endif
}

static int lua_ax_fairygui_play(lua_State* L)
{
    if (nullptr == L)
        return 0;
    int argc = 0;
    fairygui::Transition* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "fgui.Transition", 0, &tolua_err)) goto tolua_lerror;
#endif
    self = static_cast<fairygui::Transition*>(tolua_tousertype(L, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L, "invalid 'self' in function 'lua_ax_fairygui_play'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));
        self->play([=]() {
            handleFairyguiEventNoParams(handler);
        });
        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }
    if (3 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 4, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 4, 0));
        bool ok = true;
        int times;
        ok &= luaval_to_int32(L, 2, (int *)&times, "lua_ax_fairygui_play");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_play'", nullptr);
            return 0;
        }
        double delay;
        ok &= luaval_to_number(L, 3, (double *)&delay, "lua_ax_fairygui_play");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'lua_ax_fairygui_play'", nullptr);
            return 0;
        }
        self->play(times, (float)delay, [=]() {
            handleFairyguiEventNoParams(handler);
        });
        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }
    luaL_error(L, "'addEventListener' function of Transition has wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_ax_fairygui_play'.", &tolua_err);
#endif
    return 0;
}

static int lua_ax_fairygui_UIObjectFactory_setPackageItemExtension(lua_State* tolua_S)
{
    int argc = 0;
    bool ok = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S, 1, "fgui.UIObjectFactory", 0, &tolua_err)) goto tolua_lerror;
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 3 || argc == 4)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 3, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(tolua_S, 3, 0));
        bool ok = true;
        std::string url;
        ok &= luaval_to_std_string(tolua_S, 2, &url, "lua_ax_fairygui_UIObjectFactory_setPackageItemExtension");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_ax_fairygui_UIObjectFactory_setPackageItemExtension'", nullptr);
            return 0;
        }
        std::string retType;
        ok &= luaval_to_std_string(tolua_S, 4, &retType, "lua_ax_fairygui_UIObjectFactory_setPackageItemExtension");
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_ax_fairygui_UIObjectFactory_setPackageItemExtension'", nullptr);
            return 0;
        }
        fairygui::UIObjectFactory::setPackageItemExtension(url, [=]()->fairygui::GComponent* {
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
            stack->executeFunctionByHandler(handler,1);
            return ret;
        });
        cocos2d::Object* self = nullptr;
        if (argc == 4) {
            self = (cocos2d::Object*)tolua_tousertype(tolua_S, 5, 0);
        }
        if (self)
        {
            ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        } else {
            ScriptHandlerMgr::getInstance()->addCustomHandler((void*)Director::getInstance(), handler);
        }
        return 0;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "fgui.UIObjectFactory:setPackageItemExtension", argc, 2);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_fairygui_UIObjectFactory_setPackageItemExtension'.", &tolua_err);
    return 0;
#endif
}

static int lua_ax_fairygui_GList_setItemRenderer(lua_State* L)
{
    if (nullptr == L)
        return 0;
    int argc = 0;
    fairygui::GList* self = nullptr;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "fgui.GObject", 0, &tolua_err)) goto tolua_lerror;
#endif
    self = static_cast<fairygui::GList*>(tolua_tousertype(L, 1, 0));
#if _AX_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L, "invalid 'self' in function 'lua_ax_fairygui_GList_setItemRenderer'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(L, 2, 0));
        self->itemRenderer = [=](int index, fairygui::GObject* obj) {
            LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
            stack->pushInt(index);
            object_to_luaval<fairygui::GObject>(stack->getLuaState(), "fgui.GObject", (fairygui::GObject*)obj);
            stack->executeFunctionByHandler(handler, 2);
            stack->clean();
        };
        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)self, handler);
        return 0;
    }
    luaL_error(L, "'setItemRenderer' function of GObject has wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'lua_ax_fairygui_GList_setItemRenderer'.", &tolua_err);
    return 0;
#endif
}

int lua_ax_fairygui_EventContext_getDataValue(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::EventContext* cobj = nullptr;
    bool ok = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "fgui.EventContext", 0, &tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::EventContext*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_fairygui_EventContext_getDataValue'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_ax_fairygui_EventContext_getDataValue'", nullptr);
            return 0;
        }
        const cocos2d::Value& ret = cobj->getDataValue();
        ccvalue_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:getDataValue", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_fairygui_EventContext_getDataValue'.", &tolua_err);
    return 0;
#endif
}

int lua_ax_fairygui_EventContext_getData(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::EventContext* cobj = nullptr;
    bool ok = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "fgui.EventContext", 0, &tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::EventContext*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'lua_ax_fairygui_EventContext_getData'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            tolua_error(tolua_S, "invalid arguments in function 'lua_ax_fairygui_EventContext_getData'", nullptr);
            return 0;
        }
        void* ret = cobj->getData();
        object_to_luaval<cocos2d::Object>(tolua_S, /*"cc."*/"ax.Object", (cocos2d::Object*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.EventContext:getData", argc, 0);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'lua_ax_fairygui_EventContext_getData'.", &tolua_err);
    return 0;
#endif
}

static void extendUIEventDispatcher(lua_State* L)
{
    lua_pushstring(L, "fgui.UIEventDispatcher");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "addEventListener", lua_ax_fairygui_addEventListener);
        tolua_function(L, "removeEventListener", lua_ax_fairygui_removeEventListener);
        tolua_function(L, "hasEventListener", lua_ax_fairygui_hasEventListener);
    }
    lua_pop(L, 1);
}

static void extendGObject(lua_State* L)
{
    lua_pushstring(L, "fgui.GObject");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "addClickListener", lua_ax_fairygui_addClickListener);
        tolua_function(L, "removeClickListener", lua_ax_fairygui_removeClickListener);
        tolua_function(L, "getData", lua_ax_fairygui_GObject_getData);
    }
    lua_pop(L, 1);
}

static void extendTransition(lua_State* L)
{
    lua_pushstring(L, "fgui.Transition");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "play", lua_ax_fairygui_play);
    }
    lua_pop(L, 1);
}

static void extendUIObjectFactory(lua_State* L)
{
    lua_pushstring(L, "fgui.UIObjectFactory");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "setPackageItemExtension", lua_ax_fairygui_UIObjectFactory_setPackageItemExtension);
    }
    lua_pop(L, 1);
}

static void extendGList(lua_State* L)
{
    lua_pushstring(L, "fgui.GList");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "setItemRenderer", lua_ax_fairygui_GList_setItemRenderer);
    }
    lua_pop(L, 1);
}

static void extendEventContext(lua_State* L)
{
    lua_pushstring(L, "fgui.EventContext");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "getDataValue", lua_ax_fairygui_EventContext_getDataValue);
        tolua_function(L, "getData", lua_ax_fairygui_EventContext_getData);
    }
    lua_pop(L, 1);
}

int lua_ax_fairygui_GMovieClip_setPlaySettings(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GMovieClip* cobj = nullptr;
    bool ok  = true;
    #if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    #if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GMovieClip",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GMovieClip*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
            return 0;
        }
        cobj->setPlaySettings();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GMovieClip:setPlaySettings");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
            return 0;
        }
        cobj->setPlaySettings(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        int arg0;
        int arg1;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GMovieClip:setPlaySettings");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
            return 0;
        }
        cobj->setPlaySettings(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3) 
    {
        int arg0;
        int arg1;
        int arg2;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "fgui.GMovieClip:setPlaySettings");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
            return 0;
        }
        cobj->setPlaySettings(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 4) 
    {
        int arg0;
        int arg1;
        int arg2;
        int arg3;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "fgui.GMovieClip:setPlaySettings");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
            return 0;
        }
        cobj->setPlaySettings(arg0, arg1, arg2, arg3);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 5) 
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 6, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        int arg0;
        int arg1;
        int arg2;
        int arg3;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "fgui.GMovieClip:setPlaySettings");
        ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "fgui.GMovieClip:setPlaySettings");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'", nullptr);
            return 0;
        }
        LUA_FUNCTION handler = (toluafix_ref_function(tolua_S, 6, 0));
        cobj->setPlaySettings(arg0, arg1, arg2, arg3,
            [=](void)
            {
                handleFairyguiEventNoParams(handler);
            });
        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)cobj, handler);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GMovieClip:setPlaySettings",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GMovieClip_setPlaySettings'.",&tolua_err);
#endif
    return 0;
}

static void extendGMovieClip(lua_State* L)
{
    lua_pushstring(L, "fgui.GMovieClip");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
        tolua_function(L, "setPlaySettings", lua_ax_fairygui_GMovieClip_setPlaySettings);
    lua_pop(L, 1);
}

int lua_ax_fairygui_GTweener_onUpdate(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_onUpdate'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(tolua_S, 2, 0));
        auto&& ret = cobj->onUpdate(
            [=](fairygui::GTweener* sender)
            {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                object_to_luaval<fairygui::GTweener>(stack->getLuaState(), "fgui.GTweener", (fairygui::GTweener*)sender);
                stack->executeFunctionByHandler(handler, 1);
                stack->clean();
            });
        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)cobj, handler);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:onUpdate",argc, 1);
    return 0;
    #if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_onUpdate'.",&tolua_err);
#endif
    return 0;
}

int lua_ax_fairygui_GTweener_onStart(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;
    #if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    #if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_onStart'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(tolua_S, 2, 0));
        auto&& ret = cobj->onStart(
            [=](fairygui::GTweener* sender)
            {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                object_to_luaval<fairygui::GTweener>(stack->getLuaState(), "fgui.GTweener", (fairygui::GTweener*)sender);
                stack->executeFunctionByHandler(handler, 1);
                stack->clean();
            });
        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)cobj, handler);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:onStart",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_onStart'.",&tolua_err);
#endif
    return 0;
}

int lua_ax_fairygui_GTweener_onComplete(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_onComplete'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(tolua_S, 2, 0));
        auto&& ret = cobj->onComplete(
            [=](void)
            {
                handleFairyguiEventNoParams(handler);
            });
        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)cobj, handler);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:onComplete",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_onComplete'.",&tolua_err);
#endif
    return 0;
}

int lua_ax_fairygui_GTweener_onComplete1(lua_State* tolua_S)
{
    int argc = 0;
    fairygui::GTweener* cobj = nullptr;
    bool ok  = true;
    #if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif
#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"fgui.GTweener",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (fairygui::GTweener*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_fairygui_GTweener_onComplete1'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S, 2, "LUA_FUNCTION", 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (toluafix_ref_function(tolua_S, 2, 0));
        auto&& ret = cobj->onComplete1(
            [=](fairygui::GTweener* sender)
            {
                LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
                object_to_luaval<fairygui::GTweener>(stack->getLuaState(), "fgui.GTweener", (fairygui::GTweener*)sender);
                stack->executeFunctionByHandler(handler, 1);
                stack->clean();
            });
        ScriptHandlerMgr::getInstance()->addCustomHandler((void*)cobj, handler);
        object_to_luaval<fairygui::GTweener>(tolua_S, "fgui.GTweener",(fairygui::GTweener*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "fgui.GTweener:onComplete1",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_fairygui_GTweener_onComplete1'.",&tolua_err);
#endif
    return 0;
}

static void extendGTweener(lua_State* L)
{
    lua_pushstring(L, "fgui.GTweener");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
        tolua_function(L, "onUpdate", lua_ax_fairygui_GTweener_onUpdate);
        tolua_function(L, "onStart", lua_ax_fairygui_GTweener_onStart);
        tolua_function(L, "onComplete", lua_ax_fairygui_GTweener_onComplete);
        tolua_function(L, "onComplete1", lua_ax_fairygui_GTweener_onComplete1);
    lua_pop(L, 1);
}

static int register_all_fairygui_manual(lua_State* L)
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
        register_all_fairygui_manual(L);
    }
    lua_pop(L, 1);
    return 1;
}
