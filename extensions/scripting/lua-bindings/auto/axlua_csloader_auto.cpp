#include "lua-bindings/auto/axlua_csloader_auto.hpp"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_csloader_CSLoader_init(lua_State* tolua_S)
{
    int argc = 0;
    ax::CSLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::CSLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_csloader_CSLoader_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_init'", nullptr);
            return 0;
        }
        cobj->init();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CSLoader:init",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_init'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_csloader_CSLoader_createNodeFromJson(lua_State* tolua_S)
{
    int argc = 0;
    ax::CSLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::CSLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_csloader_CSLoader_createNodeFromJson'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.CSLoader:createNodeFromJson");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_createNodeFromJson'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createNodeFromJson(arg0);
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CSLoader:createNodeFromJson",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_createNodeFromJson'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_csloader_CSLoader_loadNodeWithFile(lua_State* tolua_S)
{
    int argc = 0;
    ax::CSLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::CSLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_csloader_CSLoader_loadNodeWithFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.CSLoader:loadNodeWithFile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_loadNodeWithFile'", nullptr);
            return 0;
        }
        auto&& ret = cobj->loadNodeWithFile(arg0);
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CSLoader:loadNodeWithFile",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_loadNodeWithFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_csloader_CSLoader_loadNodeWithContent(lua_State* tolua_S)
{
    int argc = 0;
    ax::CSLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::CSLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_csloader_CSLoader_loadNodeWithContent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.CSLoader:loadNodeWithContent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_loadNodeWithContent'", nullptr);
            return 0;
        }
        auto&& ret = cobj->loadNodeWithContent(arg0);
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CSLoader:loadNodeWithContent",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_loadNodeWithContent'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_csloader_CSLoader_setRecordJsonPath(lua_State* tolua_S)
{
    int argc = 0;
    ax::CSLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::CSLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_csloader_CSLoader_setRecordJsonPath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.CSLoader:setRecordJsonPath");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_setRecordJsonPath'", nullptr);
            return 0;
        }
        cobj->setRecordJsonPath(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CSLoader:setRecordJsonPath",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_setRecordJsonPath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_csloader_CSLoader_isRecordJsonPath(lua_State* tolua_S)
{
    int argc = 0;
    ax::CSLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::CSLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_csloader_CSLoader_isRecordJsonPath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_isRecordJsonPath'", nullptr);
            return 0;
        }
        auto&& ret = cobj->isRecordJsonPath();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CSLoader:isRecordJsonPath",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_isRecordJsonPath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_csloader_CSLoader_setJsonPath(lua_State* tolua_S)
{
    int argc = 0;
    ax::CSLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::CSLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_csloader_CSLoader_setJsonPath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ax.CSLoader:setJsonPath");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_setJsonPath'", nullptr);
            return 0;
        }
        cobj->setJsonPath(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CSLoader:setJsonPath",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_setJsonPath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_csloader_CSLoader_getJsonPath(lua_State* tolua_S)
{
    int argc = 0;
    ax::CSLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::CSLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_csloader_CSLoader_getJsonPath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_getJsonPath'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getJsonPath();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CSLoader:getJsonPath",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_getJsonPath'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_csloader_CSLoader_createNodeWithFlatBuffersFile(lua_State* tolua_S)
{
    int argc = 0;
    ax::CSLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::CSLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_csloader_CSLoader_createNodeWithFlatBuffersFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.CSLoader:createNodeWithFlatBuffersFile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_createNodeWithFlatBuffersFile'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createNodeWithFlatBuffersFile(arg0);
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CSLoader:createNodeWithFlatBuffersFile",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_createNodeWithFlatBuffersFile'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_csloader_CSLoader_bindCallback(lua_State* tolua_S)
{
    int argc = 0;
    ax::CSLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::CSLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_csloader_CSLoader_bindCallback'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 4) 
    {
        std::string_view arg0;
        std::string_view arg1;
        ax::ui::Widget* arg2;
        ax::Node* arg3;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.CSLoader:bindCallback");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "ax.CSLoader:bindCallback");

        ok &= luaval_to_object<ax::ui::Widget>(tolua_S, 4, "axui.Widget",&arg2, "ax.CSLoader:bindCallback");

        ok &= luaval_to_object<ax::Node>(tolua_S, 5, "ax.Node",&arg3, "ax.CSLoader:bindCallback");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_bindCallback'", nullptr);
            return 0;
        }
        auto&& ret = cobj->bindCallback(arg0, arg1, arg2, arg3);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CSLoader:bindCallback",argc, 4);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_bindCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_csloader_CSLoader_createNodeWithFlatBuffersForSimulator(lua_State* tolua_S)
{
    int argc = 0;
    ax::CSLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::CSLoader*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_csloader_CSLoader_createNodeWithFlatBuffersForSimulator'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.CSLoader:createNodeWithFlatBuffersForSimulator");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_createNodeWithFlatBuffersForSimulator'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createNodeWithFlatBuffersForSimulator(arg0);
        object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CSLoader:createNodeWithFlatBuffersForSimulator",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_createNodeWithFlatBuffersForSimulator'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_csloader_CSLoader_getInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_getInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::CSLoader::getInstance();
        object_to_luaval<ax::CSLoader>(tolua_S, "ax.CSLoader",(ax::CSLoader*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.CSLoader:getInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_getInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_csloader_CSLoader_destroyInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_destroyInstance'", nullptr);
            return 0;
        }
        ax::CSLoader::destroyInstance();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.CSLoader:destroyInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_destroyInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_csloader_CSLoader_createNodeWithVisibleSize(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.CSLoader",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 2)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.CSLoader:createNodeWithVisibleSize");
            if (!ok) { break; }
            std::function<void (ax::Object *)> arg1;
            do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
            if (!ok) { break; }
            ax::Node* ret = ax::CSLoader::createNodeWithVisibleSize(arg0, arg1);
            object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 1)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.CSLoader:createNodeWithVisibleSize");
            if (!ok) { break; }
            ax::Node* ret = ax::CSLoader::createNodeWithVisibleSize(arg0);
            object_to_luaval<ax::Node>(tolua_S, "ax.Node",(ax::Node*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.CSLoader:createNodeWithVisibleSize",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_createNodeWithVisibleSize'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_csloader_CSLoader_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::CSLoader* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_csloader_CSLoader_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::CSLoader();
        tolua_pushusertype(tolua_S,(void*)cobj,"ax.CSLoader");
        tolua_register_gc(tolua_S,lua_gettop(tolua_S));
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.CSLoader:CSLoader",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_csloader_CSLoader_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_csloader_CSLoader_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (CSLoader)");
    return 0;
}

int lua_register_ax_csloader_CSLoader(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.CSLoader");
    tolua_cclass(tolua_S,"CSLoader","ax.CSLoader","",nullptr);

    tolua_beginmodule(tolua_S,"CSLoader");
        tolua_function(tolua_S,"new",lua_ax_csloader_CSLoader_constructor);
        tolua_function(tolua_S,"init",lua_ax_csloader_CSLoader_init);
        tolua_function(tolua_S,"createNodeFromJson",lua_ax_csloader_CSLoader_createNodeFromJson);
        tolua_function(tolua_S,"loadNodeWithFile",lua_ax_csloader_CSLoader_loadNodeWithFile);
        tolua_function(tolua_S,"loadNodeWithContent",lua_ax_csloader_CSLoader_loadNodeWithContent);
        tolua_function(tolua_S,"setRecordJsonPath",lua_ax_csloader_CSLoader_setRecordJsonPath);
        tolua_function(tolua_S,"isRecordJsonPath",lua_ax_csloader_CSLoader_isRecordJsonPath);
        tolua_function(tolua_S,"setJsonPath",lua_ax_csloader_CSLoader_setJsonPath);
        tolua_function(tolua_S,"getJsonPath",lua_ax_csloader_CSLoader_getJsonPath);
        tolua_function(tolua_S,"createNodeWithFlatBuffersFile",lua_ax_csloader_CSLoader_createNodeWithFlatBuffersFile);
        tolua_function(tolua_S,"bindCallback",lua_ax_csloader_CSLoader_bindCallback);
        tolua_function(tolua_S,"createNodeWithFlatBuffersForSimulator",lua_ax_csloader_CSLoader_createNodeWithFlatBuffersForSimulator);
        tolua_function(tolua_S,"getInstance", lua_ax_csloader_CSLoader_getInstance);
        tolua_function(tolua_S,"destroyInstance", lua_ax_csloader_CSLoader_destroyInstance);
        tolua_function(tolua_S,"createNodeWithVisibleSize", lua_ax_csloader_CSLoader_createNodeWithVisibleSize);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::CSLoader).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.CSLoader";
    g_typeCast[typeName] = "ax.CSLoader";
    return 1;
}
TOLUA_API int register_all_ax_csloader(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"ax",0);
	tolua_beginmodule(tolua_S,"ax");

	lua_register_ax_csloader_CSLoader(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

