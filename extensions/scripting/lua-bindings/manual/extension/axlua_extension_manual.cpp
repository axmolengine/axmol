/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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
#include "axlua_extension_manual.h"
#include "lua-bindings/auto/axlua_extension_auto.hpp"

#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/LuaValue.h"
#include "extensions/axmol-ext.h"
#include "lua-bindings/manual/LuaEngine.h"
#include "lua-bindings/manual/base/LuaScriptHandlerMgr.h"

USING_NS_AX;
USING_NS_AX_EXT;


class LuaRefMap : public Object
{
protected:
    ax::Map<std::string, ax::Object*> __map;

public:
    LuaRefMap() {
        
    }

    ~LuaRefMap()
    {
        AXLOGINFO("deallocing LuaRefMap: %p", this);
        __map.clear();
    }
    void setObject(Object* pObject, const std::string& key) {
        __map.insert(key, pObject);
    }


    void removeAllObjects() { __map.clear(); 
    }

    Object* objectForKey(const std::string& key) {
        Object* pRetObject = nullptr;
        pRetObject      = __map.at(key);
        return pRetObject;
    }

    void removeObjectForKey(const std::string& key)
    {
        AXASSERT(!key.empty(), "Invalid Argument!");
        auto pRetObject = __map.at(key);
        if (pRetObject != nullptr)
        {
            __map.erase(key);
        }
    }

};

class LuaAssetsManagerDelegateProtocol : public Object, public AssetsManagerDelegateProtocol
{
public:
    virtual ~LuaAssetsManagerDelegateProtocol() {}

    virtual void onProgress(int percent) override
    {
        int handler = ScriptHandlerMgr::getInstance()->getObjectHandler(
            (void*)this, ScriptHandlerMgr::HandlerType::ASSETSMANAGER_PROGRESS);
        if (0 != handler)
        {
            LuaAssetsManagerEventData eventData(percent);
            BasicScriptData data((void*)this, &eventData);
            LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::ASSETSMANAGER_PROGRESS, (void*)&data);
        }
    }

    virtual void onSuccess() override
    {
        int handler = ScriptHandlerMgr::getInstance()->getObjectHandler(
            (void*)this, ScriptHandlerMgr::HandlerType::ASSETSMANAGER_SUCCESS);
        if (0 != handler)
        {
            LuaAssetsManagerEventData eventData;
            BasicScriptData data((void*)this, &eventData);
            LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::ASSETSMANAGER_SUCCESS, (void*)&data);
        }
    }

    virtual void onError(AssetsManager::ErrorCode errorCode) override
    {
        int handler = ScriptHandlerMgr::getInstance()->getObjectHandler(
            (void*)this, ScriptHandlerMgr::HandlerType::ASSETSMANAGER_ERROR);
        if (0 != handler)
        {
            LuaAssetsManagerEventData eventData((int)errorCode);
            BasicScriptData data((void*)this, &eventData);
            LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::ASSETSMANAGER_ERROR, (void*)&data);
        }
    }
};

static int axlua_AssetsManager_setDelegate(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc            = 0;
    AssetsManager* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.AssetsManager", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (AssetsManager*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_AssetsManager_setDelegate'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L, 2, "LUA_FUNCTION", 0, &tolua_err) || !tolua_isnumber(L, 3, 0, &tolua_err))
        {
            goto tolua_lerror;
        }
#endif
        LuaAssetsManagerDelegateProtocol* delegate =
            dynamic_cast<LuaAssetsManagerDelegateProtocol*>(self->getDelegate());
        if (nullptr == delegate)
        {
            delegate = new LuaAssetsManagerDelegateProtocol();
            self->setUserObject(delegate);
            self->setDelegate(delegate);
            delegate->release();
        }

        LUA_FUNCTION handler                      = toluafix_ref_function(L, 2, 0);
        ScriptHandlerMgr::HandlerType handlerType = (ScriptHandlerMgr::HandlerType)(
            (int)tolua_tonumber(L, 3, 0) + (int)ScriptHandlerMgr::HandlerType::ASSETSMANAGER_PROGRESS);

        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)delegate, handler, handlerType);
        return 0;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.AssetsManager:setDelegate", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_AssetsManager_setDelegate'.", &tolua_err);
    return 0;
#endif
}

static void extendAssetsManager(lua_State* L)
{
    lua_pushstring(L, "ax.AssetsManager");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "setDelegate", axlua_AssetsManager_setDelegate);
    }
    lua_pop(L, 1);
}

static void extendManifest(lua_State* L)
{
    lua_pushstring(L, "ax.Manifest");
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_pop(L, 1);
}

static int axlua_Extension_EventListenerAssetsManagerEx_create(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = 0;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(L, 1, "ax.EventListenerAssetsManagerEx", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 2)
    {

#if _AX_DEBUG >= 1
        if (!tolua_isusertype(L, 2, "ax.AssetsManagerEx", 0, &tolua_err) ||
            !toluafix_isfunction(L, 3, "LUA_FUNCTION", 0, &tolua_err))
            goto tolua_lerror;
#endif
        ax::extension::AssetsManagerEx* assetManager =
            static_cast<ax::extension::AssetsManagerEx*>(tolua_tousertype(L, 2, nullptr));

        LUA_FUNCTION handler = toluafix_ref_function(L, 3, 0);

        ax::extension::EventListenerAssetsManagerEx* ret =
            ax::extension::EventListenerAssetsManagerEx::create(assetManager, [=](EventAssetsManagerEx* event) {
                auto stack = LuaEngine::getInstance()->getLuaStack();
                int id     = event ? (int)event->_ID : -1;
                int* luaID = event ? &event->_luaID : nullptr;
                toluafix_pushusertype_ccobject(stack->getLuaState(), id, luaID, (void*)event,
                                               "ax.EventAssetsManagerEx");
                stack->executeFunctionByHandler(handler, 1);
            });

        int id     = (ret) ? (int)ret->_ID : -1;
        int* luaID = (ret) ? &ret->_luaID : nullptr;
        toluafix_pushusertype_ccobject(L, id, luaID, (void*)ret, "ax.EventListenerAssetsManagerEx");
        return 1;
    }

    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "create", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_Extension_EventListenerAssetsManagerEx_create'.", &tolua_err);
    return 0;
#endif
}

static void extendEventListenerAssetsManagerEx(lua_State* L)
{
    lua_pushstring(L, "ax.EventListenerAssetsManagerEx");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "create", axlua_Extension_EventListenerAssetsManagerEx_create);
    }
    lua_pop(L, 1);
}

int axlua_extension_ParticleSystem3D_getParticlePool(lua_State* tolua_S)
{
    int argc                        = 0;
    ax::ParticleSystem3D* cobj = nullptr;
    bool ok                         = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.ParticleSystem3D", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::ParticleSystem3D*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_extension_ParticleSystem3D_getParticlePool'",
                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            tolua_error(tolua_S,
                        "invalid arguments in function 'axlua_extension_ParticleSystem3D_getParticlePool'",
                        nullptr);
            return 0;
        }
        const ax::ParticlePool& ret = cobj->getParticlePool();
        tolua_pushusertype(tolua_S, (void*)&ret, "ax.ParticlePool");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.ParticleSystem3D:getParticlePool", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_extension_ParticleSystem3D_getParticlePool'.", &tolua_err);
#endif

    return 0;
}

static void extendParticleSystem3D(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.ParticleSystem3D");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "getParticlePool", axlua_extension_ParticleSystem3D_getParticlePool);
    }
    lua_pop(tolua_S, 1);
}

int axlua_extension_ParticlePool_getActiveDataList(lua_State* tolua_S)
{
    int argc                    = 0;
    ax::ParticlePool* cobj = nullptr;
    bool ok                     = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.ParticlePool", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::ParticlePool*)tolua_tousertype(tolua_S, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_extension_ParticlePool_getActiveDataList'",
                    nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 0)
    {
        if (!ok)
        {
            tolua_error(tolua_S,
                        "invalid arguments in function 'axlua_extension_ParticlePool_getActiveDataList'",
                        nullptr);
            return 0;
        }
        const ParticlePool::PoolList& ret = cobj->getActiveDataList();

        lua_newtable(tolua_S);
        if (ret.empty())
            return 1;

        int index = 1;
        for (const auto& obj : ret)
        {
            if (nullptr == obj)
                continue;

            lua_pushnumber(tolua_S, (lua_Number)index);
            tolua_pushusertype(tolua_S, (void*)&ret, "ax.ParticlePool");
            lua_rawset(tolua_S, -3);
            ++index;
        }

        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",
               "ax.ParticlePool:getActiveParticleList", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_extension_ParticlePool_getActiveParticleList'.",
                &tolua_err);
#endif

    return 0;
}

static void extendParticlePool(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.ParticlePool");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S, -1))
    {
        tolua_function(tolua_S, "getActiveDataList", axlua_extension_ParticlePool_getActiveDataList);
    }
    lua_pop(tolua_S, 1);
}



class LuaScrollViewDelegate:public Object, public ScrollViewDelegate
{
public:
    virtual ~LuaScrollViewDelegate()
    {}
    
    virtual void scrollViewDidScroll(ScrollView* view) override
    {
        if (nullptr != view)
        {
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)view, ScriptHandlerMgr::HandlerType::SCROLLVIEW_SCROLL);
            if (0 != handler)
            {
                CommonScriptData data(handler,"");
                ScriptEvent event(kCommonEvent,(void*)&data);
                LuaEngine::getInstance()->sendEvent(event);
            }
            
        }
    }
    
    virtual void scrollViewDidZoom(ScrollView* view) override
    {
        if (nullptr != view)
        {
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)view, ScriptHandlerMgr::HandlerType::SCROLLVIEW_ZOOM);
            if (0 != handler)
            {
                CommonScriptData data(handler,"");
                ScriptEvent event(kCommonEvent,(void*)&data);
                LuaEngine::getInstance()->sendEvent(event);
            }
        }
    }
};

static int axlua_extension_ScrollView_setDelegate(lua_State* tolua_S)
{
    if (nullptr == tolua_S)
        return 0;
    
    int argc = 0;
    ScrollView* self = nullptr;
    
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif
    
    self = (ScrollView*)  tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(tolua_S,"invalid 'self' in function 'axlua_extension_ScrollView_setDelegate'\n", nullptr);
        return 0;
    }
#endif
    
    argc = lua_gettop(tolua_S) - 1;
    
    if (0 == argc)
    {
        LuaScrollViewDelegate* delegate = new (std::nothrow) LuaScrollViewDelegate();
        if (nullptr == delegate)
            return 0;
        
        self->setUserObject(delegate);
        self->setDelegate(delegate);
        
        delegate->release();
        
        return 0;
    }
    
    luaL_error(tolua_S, "'setDelegate' function of ScrollView wrong number of arguments: %d, was expecting %d\n", argc, 0);
    return 0;
    
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'setDelegate'.",&tolua_err);
    return 0;
#endif
}

static int axlua_extension_ScrollView_registerScriptHandler(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;
    
    int argc = 0;
    ScrollView* self = nullptr;
    
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif
    
    self = static_cast<ScrollView*>(tolua_tousertype(tolua_S,1,0));
    
#if _AX_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(tolua_S,"invalid 'self' in function 'axlua_extension_ScrollView_registerScriptHandler'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(tolua_S,2,"LUA_FUNCTION",0,&tolua_err) ||
            !tolua_isnumber(tolua_S, 3, 0, &tolua_err) )
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (  toluafix_ref_function(tolua_S,2,0));
        ScriptHandlerMgr::HandlerType handlerType = (ScriptHandlerMgr::HandlerType) ((int)tolua_tonumber(tolua_S,3,0) + (int)ScriptHandlerMgr::HandlerType::SCROLLVIEW_SCROLL);
        
        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, handlerType);
        return 0;
    }
    
    luaL_error(tolua_S, "%s function of ScrollView has wrong number of arguments: %d, was expecting %d\n", "ax.ScrollView:registerScriptHandler",argc, 2);
    return 0;
    
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'axlua_extension_ScrollView_registerScriptHandler'.",&tolua_err);
    return 0;
#endif
}

static int axlua_extension_ScrollView_unregisterScriptHandler(lua_State* tolua_S)
{
    if (NULL == tolua_S)
        return 0;
    
    int argc = 0;
    ScrollView* self = nullptr;
    
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"ax.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif
    
    self = static_cast<ScrollView*>(tolua_tousertype(tolua_S,1,0));
    
#if _AX_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(tolua_S,"invalid 'self' in function 'axlua_extension_ScrollView_unregisterScriptHandler'\n", NULL);
        return 0;
    }
#endif
    
    argc = lua_gettop(tolua_S) - 1;
    
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isnumber(tolua_S, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        ScriptHandlerMgr::HandlerType handlerType = (ScriptHandlerMgr::HandlerType) ((int)tolua_tonumber(tolua_S,2,0) + (int)ScriptHandlerMgr::HandlerType::SCROLLVIEW_SCROLL);
        ScriptHandlerMgr::getInstance()->removeObjectHandler((void*)self, handlerType);
        return 0;
    }
    
    luaL_error(tolua_S, "%s function of ScrollView  has wrong number of arguments: %d, was expecting %d\n", "ax.ScrollView:unregisterScriptHandler", argc, 1);
    return 0;
    
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'axlua_extension_ScrollView_unregisterScriptHandler'.",&tolua_err);
    return 0;
#endif
}

static void extendScrollView(lua_State* tolua_S)
{
    lua_pushstring(tolua_S, "ax.ScrollView");
    lua_rawget(tolua_S, LUA_REGISTRYINDEX);
    if (lua_istable(tolua_S,-1))
    {
        lua_pushstring(tolua_S,"setDelegate");
        lua_pushcfunction(tolua_S,axlua_extension_ScrollView_setDelegate );
        lua_rawset(tolua_S,-3);
        lua_pushstring(tolua_S,"registerScriptHandler");
        lua_pushcfunction(tolua_S,axlua_extension_ScrollView_registerScriptHandler );
        lua_rawset(tolua_S,-3);
        lua_pushstring(tolua_S,"unregisterScriptHandler");
        lua_pushcfunction(tolua_S,axlua_extension_ScrollView_unregisterScriptHandler );
        lua_rawset(tolua_S,-3);
    }
    lua_pop(tolua_S, 1);
}


#define KEY_TABLEVIEW_DATA_SOURCE  "TableViewDataSource"
#define KEY_TABLEVIEW_DELEGATE     "TableViewDelegate"

class LUA_TableViewDelegate:public Object, public TableViewDelegate
{
public:
    LUA_TableViewDelegate(){}
    
    virtual ~LUA_TableViewDelegate(){}
    
    
    virtual void scrollViewDidScroll(ScrollView* view) override
    {
        if (nullptr != view)
        {
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)view, ScriptHandlerMgr::HandlerType::SCROLLVIEW_SCROLL);
            if (0 != handler)
            {
                LuaTableViewEventData eventData;
                BasicScriptData data(view,&eventData);
                LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::SCROLLVIEW_SCROLL, (void*)&data);
            }
        }
    }
    
    virtual void scrollViewDidZoom(ScrollView* view) override
    {
        if (nullptr != view)
        {
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)view, ScriptHandlerMgr::HandlerType::SCROLLVIEW_ZOOM);
            if (0 != handler)
            {
                LuaTableViewEventData eventData;
                BasicScriptData data(view,&eventData);
                LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::SCROLLVIEW_ZOOM, (void*)&data);
            }
        }
    }
    
    virtual void tableCellTouched(TableView* table, TableViewCell* cell) override
    {
        if (nullptr != table && nullptr != cell)
        {
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)table, ScriptHandlerMgr::HandlerType::TABLECELL_TOUCHED);
            if (0 != handler)
            {
                LuaTableViewEventData eventData(cell);
                BasicScriptData data(table,&eventData);
                LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::TABLECELL_TOUCHED,(void*)&data);
            }
        }
    }
    
    virtual void tableCellHighlight(TableView* table, TableViewCell* cell) override
    {
        if (nullptr != table && nullptr != cell)
        {
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)table, ScriptHandlerMgr::HandlerType::TABLECELL_HIGHLIGHT);
            if (0 != handler)
            {
                LuaTableViewEventData eventData(cell);
                BasicScriptData data(table,&eventData);
                LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::TABLECELL_HIGHLIGHT,(void*)&data);
            }
        }
    }
    
    virtual void tableCellUnhighlight(TableView* table, TableViewCell* cell) override
    {
        if (nullptr != table && nullptr != cell)
        {
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)table, ScriptHandlerMgr::HandlerType::TABLECELL_UNHIGHLIGHT);
            if (0 != handler)
            {
                LuaTableViewEventData eventData(cell);
                BasicScriptData data(table,&eventData);
                LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::TABLECELL_UNHIGHLIGHT,(void*)&data);
            }
        }
    }
    
    virtual void tableCellWillRecycle(TableView* table, TableViewCell* cell) override
    {
        if (nullptr != table && nullptr != cell)
        {
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)table, ScriptHandlerMgr::HandlerType::TABLECELL_WILL_RECYCLE);
            if (0 != handler)
            {
                LuaTableViewEventData eventData(cell);
                BasicScriptData data(table,&eventData);
                LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::TABLECELL_WILL_RECYCLE,(void*)&data);
            }
        }
    }
};

static int axlua_extension_TableView_setDelegate(lua_State* L)
{
    if (nullptr == L)
        return 0;
    
    int argc = 0;
    TableView* self = nullptr;
    
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif
    
    self = (TableView*)  tolua_tousertype(L,1,0);
    
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L,"invalid 'self' in function 'axlua_extension_TableView_setDelegate'\n", nullptr);
        return 0;
    }
#endif
    
    argc = lua_gettop(L) - 1;
    
    if (0 == argc)
    {
        LUA_TableViewDelegate* delegate = new (std::nothrow) LUA_TableViewDelegate();
        if (nullptr == delegate)
            return 0;
        auto userDict = static_cast<LuaRefMap*>(self->getUserObject());

        if (nullptr == userDict)
        {
            userDict = new (std::nothrow) LuaRefMap();
            if (NULL == userDict)
                return 0;
            
            self->setUserObject(userDict);
            userDict->release();
        }
        
        userDict->setObject(delegate, KEY_TABLEVIEW_DELEGATE);
        self->setDelegate(delegate);
        delegate->release();
        
        return 0;
    }
    
    luaL_error(L, "'setDelegate' function of TableView wrong number of arguments: %d, was expecting %d\n", argc, 0);
    return 0;
    
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L,"#ferror in function 'setDelegate'.",&tolua_err);
    return 0;
#endif
}

class LUA_TableViewDataSource:public Object,public TableViewDataSource
{
public:
    LUA_TableViewDataSource(){}
    virtual ~LUA_TableViewDataSource(){}
    
    virtual Size tableCellSizeForIndex(TableView *table, ssize_t idx) override
    {
        if (nullptr != table )
        {
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)table, ScriptHandlerMgr::HandlerType::TABLECELL_SIZE_FOR_INDEX);
            if (0 != handler)
            {
                LuaTableViewEventData eventData(&idx);
                BasicScriptData data(table,&eventData);
                float width = 0.0;
                float height = 0.0;
                LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::TABLECELL_SIZE_FOR_INDEX, (void*)&data,2,[&](lua_State* L,int numReturn){
                    AXASSERT(numReturn == 2, "tableCellSizeForIndex return count error");
                    ValueVector vec;
                    height  = (float)tolua_tonumber(L, -1, 0);
                    lua_pop(L, 1);
                    width = (float)tolua_tonumber(L, -1, 0);
                    lua_pop(L, 1);
                });
                
                return Size(width, height);
            }
        }
        
        return Size::ZERO;
    }
    
    virtual TableViewCell* tableCellAtIndex(TableView *table, ssize_t idx) override
    {
        if (nullptr != table )
        {
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)table, ScriptHandlerMgr::HandlerType::TABLECELL_AT_INDEX);
            if (0 != handler)
            {
                LuaTableViewEventData eventData(&idx);
                BasicScriptData data(table,&eventData);
                TableViewCell* viewCell = nullptr;
                LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::TABLECELL_AT_INDEX, (void*)&data, 1, [&](lua_State* L, int numReturn){
                    AXASSERT(numReturn == 1, "tableCellAtIndex return count error");
                    viewCell = static_cast<TableViewCell*>(tolua_tousertype(L, -1, nullptr));
                    lua_pop(L, 1);
                });
                
                return viewCell;
            }
        }
        
        return NULL;
    }
    
    virtual ssize_t numberOfCellsInTableView(TableView *table) override
    {
        if (nullptr != table )
        {
            int handler = ScriptHandlerMgr::getInstance()->getObjectHandler((void*)table, ScriptHandlerMgr::HandlerType::TABLEVIEW_NUMS_OF_CELLS);
            if (0 != handler)
            {
                LuaTableViewEventData eventData;
                BasicScriptData data(table,&eventData);
                ssize_t counts = 0;
                LuaEngine::getInstance()->handleEvent(ScriptHandlerMgr::HandlerType::TABLEVIEW_NUMS_OF_CELLS, (void*)&data,1, [&](lua_State* L, int numReturn){
                    AXASSERT(numReturn == 1, "numberOfCellsInTableView return count error");
                    counts = (ssize_t)tolua_tonumber(L, -1, 0);
                    lua_pop(L, 1);
                });
                return counts;
            }
        }
        return 0;
    }
};

static int axlua_extension_TableView_setDataSource(lua_State* L)
{
    if (nullptr == L)
        return 0;
    
    int argc = 0;
    TableView* self = nullptr;
    
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif
    
    self = (TableView*)  tolua_tousertype(L,1,0);
    
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L,"invalid 'self' in function 'axlua_extension_TableView_setDataSource'\n", nullptr);
        return 0;
    }
#endif
    
    argc = lua_gettop(L) - 1;
    
    if (0 == argc)
    {
        LUA_TableViewDataSource* dataSource = new (std::nothrow) LUA_TableViewDataSource();
        if (nullptr == dataSource)
            return 0;
        
        auto userDict = static_cast<LuaRefMap*>(self->getUserObject());
        if (nullptr == userDict)
        {
            userDict = new LuaRefMap();
            if (NULL == userDict)
                return 0;
            
            self->setUserObject(userDict);
            userDict->release();
        }
        
        userDict->setObject(dataSource,KEY_TABLEVIEW_DATA_SOURCE);
        
        self->setDataSource(dataSource);
        
        dataSource->release();
        
        return 0;
    }
    
    luaL_error(L, "'setDataSource' function of TableView wrong number of arguments: %d, was expecting %d\n", argc, 0);
    return 0;
    
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L,"#ferror in function 'setDataSource'.",&tolua_err);
    return 0;
#endif
}

static int axlua_extension_TableView_create(lua_State* L)
{
    if (nullptr == L)
        return 0;
    
    int argc = 0;
    bool ok = true;
    
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertable(L,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif
    
    argc = lua_gettop(L) - 1;
    
    if (2 == argc || 1 == argc)
    {
        LUA_TableViewDataSource* dataSource = new (std::nothrow) LUA_TableViewDataSource();
        Size size;
        ok &= luaval_to_size(L, 2, &size, "ax.TableView:create");
        
        TableView* ret = nullptr;
        
        if (1 == argc)
        {
            ret = TableView::create(dataSource, size);
        }
        else
        {
#if _AX_DEBUG >= 1
            if (!tolua_isusertype(L,3,"ax.Node",0,&tolua_err)) goto tolua_lerror;
#endif
            Node* node = static_cast<Node*>(tolua_tousertype(L, 3, nullptr));
            ret = TableView::create(dataSource, size, node);
        }
        
        if (nullptr ==  ret)
            return 0;
        
        ret->reloadData();


        LuaRefMap* userDict = new LuaRefMap();
        userDict->setObject(dataSource, KEY_TABLEVIEW_DATA_SOURCE);
        ret->setUserObject(userDict);
        dataSource->release();
        userDict->release();
       
        int  nID = (int)ret->_ID;
        int* pLuaID =  &ret->_luaID;
        toluafix_pushusertype_ccobject(L, nID, pLuaID, (void*)ret,"ax.TableView");
        
        return 1;
    }
    luaL_error(L, "'create' function of TableView wrong number of arguments: %d, was expecting %d\n", argc, 1);
    return 0;
    
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L,"#ferror in function 'create'.",&tolua_err);
    return 0;
#endif
}

static int axlua_extension_TableView_registerScriptHandler(lua_State* L)
{
    if (NULL == L)
        return 0;
    
    int argc = 0;
    TableView* self = nullptr;
    
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L,1,"",0,&tolua_err)) goto tolua_lerror;
#endif
    
    self = static_cast<TableView*>(tolua_tousertype(L,1,0));
    
#if _AX_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L,"invalid 'self' in function 'axlua_extension_TableView_registerScriptHandler'\n", NULL);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!toluafix_isfunction(L,2,"LUA_FUNCTION",0,&tolua_err) ||
            !tolua_isnumber(L, 3, 0, &tolua_err) )
        {
            goto tolua_lerror;
        }
#endif
        LUA_FUNCTION handler = (  toluafix_ref_function(L,2,0));
        ScriptHandlerMgr::HandlerType handlerType = (ScriptHandlerMgr::HandlerType) ((int)tolua_tonumber(L,3,0) + (int)ScriptHandlerMgr::HandlerType::SCROLLVIEW_SCROLL);
        
        ScriptHandlerMgr::getInstance()->addObjectHandler((void*)self, handler, handlerType);
        return 0;
    }
    
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.TableView:registerScriptHandler",argc, 2);
    return 0;
    
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L,"#ferror in function 'axlua_extension_TableView_registerScriptHandler'.",&tolua_err);
    return 0;
#endif
}

static int axlua_extension_TableView_unregisterScriptHandler(lua_State* L)
{
    if (NULL == L)
        return 0;
    
    int argc = 0;
    TableView* self = nullptr;
    
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L,1,"ax.TableView",0,&tolua_err)) goto tolua_lerror;
#endif
    
    self = static_cast<TableView*>(tolua_tousertype(L,1,0));
    
#if _AX_DEBUG >= 1
    if (nullptr == self) {
        tolua_error(L,"invalid 'self' in function 'axlua_extension_TableView_unregisterScriptHandler'\n", NULL);
        return 0;
    }
#endif
    
    argc = lua_gettop(L) - 1;
    
    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_isnumber(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        ScriptHandlerMgr::HandlerType handlerType = (ScriptHandlerMgr::HandlerType) ((int)tolua_tonumber(L,2,0) + (int)ScriptHandlerMgr::HandlerType::SCROLLVIEW_SCROLL);
        ScriptHandlerMgr::getInstance()->removeObjectHandler((void*)self, handlerType);
        return 0;
    }
    
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n", "ax.TableView:unregisterScriptHandler", argc, 0);
    return 0;
    
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L,"#ferror in function 'axlua_extension_TableView_unregisterScriptHandler'.",&tolua_err);
    return 0;
#endif
}

static void extendTableView(lua_State* L)
{
    lua_pushstring(L, "ax.TableView");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L,-1))
    {
        tolua_function(L, "setDelegate", axlua_extension_TableView_setDelegate);
        tolua_function(L, "setDataSource", axlua_extension_TableView_setDataSource);
        tolua_function(L, "create", axlua_extension_TableView_create);
        tolua_function(L, "registerScriptHandler", axlua_extension_TableView_registerScriptHandler);
        tolua_function(L, "unregisterScriptHandler", axlua_extension_TableView_unregisterScriptHandler);
    }
    lua_pop(L, 1);
}

int register_all_ax_extension_manual(lua_State* tolua_S)
{
    extendAssetsManager(tolua_S);
    extendManifest(tolua_S);
    extendEventListenerAssetsManagerEx(tolua_S);
    extendParticleSystem3D(tolua_S);
    extendParticlePool(tolua_S);
    extendScrollView(tolua_S);
    extendTableView(tolua_S);
    return 0;
}

int register_extension_module(lua_State* tolua_S)
{
    lua_getglobal(tolua_S, "_G");
    if (lua_istable(tolua_S, -1))  // stack:...,_G,
    {
        register_all_ax_extension(tolua_S);
        register_all_ax_extension_manual(tolua_S);
    }
    lua_pop(tolua_S, 1);
    return 1;
}
