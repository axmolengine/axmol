#include "scripting/lua-bindings/auto/lua_cocos2dx_extension_auto.hpp"
#include "cocos-ext.h"
#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"

int lua_cocos2dx_extension_AssetsManager_setStoragePath(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManager_setStoragePath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "cc.AssetsManager:setStoragePath"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_setStoragePath'", nullptr);
            return 0;
        }
        cobj->setStoragePath(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:setStoragePath",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_setStoragePath'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManager_setPackageUrl(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManager_setPackageUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "cc.AssetsManager:setPackageUrl"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_setPackageUrl'", nullptr);
            return 0;
        }
        cobj->setPackageUrl(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:setPackageUrl",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_setPackageUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManager_checkUpdate(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManager_checkUpdate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_checkUpdate'", nullptr);
            return 0;
        }
        bool ret = cobj->checkUpdate();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:checkUpdate",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_checkUpdate'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManager_getStoragePath(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManager_getStoragePath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_getStoragePath'", nullptr);
            return 0;
        }
        const char* ret = cobj->getStoragePath();
        tolua_pushstring(tolua_S,(const char*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:getStoragePath",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_getStoragePath'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManager_update(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManager_update'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_update'", nullptr);
            return 0;
        }
        cobj->update();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:update",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_update'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManager_setConnectionTimeout(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManager_setConnectionTimeout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "cc.AssetsManager:setConnectionTimeout");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_setConnectionTimeout'", nullptr);
            return 0;
        }
        cobj->setConnectionTimeout(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:setConnectionTimeout",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_setConnectionTimeout'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManager_setVersionFileUrl(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManager_setVersionFileUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "cc.AssetsManager:setVersionFileUrl"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_setVersionFileUrl'", nullptr);
            return 0;
        }
        cobj->setVersionFileUrl(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:setVersionFileUrl",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_setVersionFileUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManager_getPackageUrl(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManager_getPackageUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_getPackageUrl'", nullptr);
            return 0;
        }
        const char* ret = cobj->getPackageUrl();
        tolua_pushstring(tolua_S,(const char*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:getPackageUrl",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_getPackageUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManager_getConnectionTimeout(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManager_getConnectionTimeout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_getConnectionTimeout'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getConnectionTimeout();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:getConnectionTimeout",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_getConnectionTimeout'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManager_getVersion(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManager_getVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_getVersion'", nullptr);
            return 0;
        }
        std::string ret = cobj->getVersion();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:getVersion",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_getVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManager_getVersionFileUrl(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManager_getVersionFileUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_getVersionFileUrl'", nullptr);
            return 0;
        }
        const char* ret = cobj->getVersionFileUrl();
        tolua_pushstring(tolua_S,(const char*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:getVersionFileUrl",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_getVersionFileUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManager_deleteVersion(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManager*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManager_deleteVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_deleteVersion'", nullptr);
            return 0;
        }
        cobj->deleteVersion();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:deleteVersion",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_deleteVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManager_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"cc.AssetsManager",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 6)
    {
        const char* arg0;
        const char* arg1;
        const char* arg2;
        std::function<void (int)> arg3;
        std::function<void (int)> arg4;
        std::function<void ()> arg5;
        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "cc.AssetsManager:create"); arg0 = arg0_tmp.c_str();
        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "cc.AssetsManager:create"); arg1 = arg1_tmp.c_str();
        std::string arg2_tmp; ok &= luaval_to_std_string(tolua_S, 4, &arg2_tmp, "cc.AssetsManager:create"); arg2 = arg2_tmp.c_str();
        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_create'", nullptr);
            return 0;
        }
        cocos2d::extension::AssetsManager* ret = cocos2d::extension::AssetsManager::create(arg0, arg1, arg2, arg3, arg4, arg5);
        object_to_luaval<cocos2d::extension::AssetsManager>(tolua_S, "cc.AssetsManager",(cocos2d::extension::AssetsManager*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "cc.AssetsManager:create",argc, 6);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_extension_AssetsManager_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManager* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::AssetsManager();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.AssetsManager");
        return 1;
    }
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "cc.AssetsManager:AssetsManager"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::AssetsManager(arg0);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.AssetsManager");
        return 1;
    }
    if (argc == 2) 
    {
        const char* arg0;
        const char* arg1;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "cc.AssetsManager:AssetsManager"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "cc.AssetsManager:AssetsManager"); arg1 = arg1_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::AssetsManager(arg0, arg1);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.AssetsManager");
        return 1;
    }
    if (argc == 3) 
    {
        const char* arg0;
        const char* arg1;
        const char* arg2;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "cc.AssetsManager:AssetsManager"); arg0 = arg0_tmp.c_str();

        std::string arg1_tmp; ok &= luaval_to_std_string(tolua_S, 3, &arg1_tmp, "cc.AssetsManager:AssetsManager"); arg1 = arg1_tmp.c_str();

        std::string arg2_tmp; ok &= luaval_to_std_string(tolua_S, 4, &arg2_tmp, "cc.AssetsManager:AssetsManager"); arg2 = arg2_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManager_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::AssetsManager(arg0, arg1, arg2);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.AssetsManager");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManager:AssetsManager",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManager_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_extension_AssetsManager_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (AssetsManager)");
    return 0;
}

int lua_register_cocos2dx_extension_AssetsManager(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"cc.AssetsManager");
    tolua_cclass(tolua_S,"AssetsManager","cc.AssetsManager","cc.Node",nullptr);

    tolua_beginmodule(tolua_S,"AssetsManager");
        tolua_function(tolua_S,"new",lua_cocos2dx_extension_AssetsManager_constructor);
        tolua_function(tolua_S,"setStoragePath",lua_cocos2dx_extension_AssetsManager_setStoragePath);
        tolua_function(tolua_S,"setPackageUrl",lua_cocos2dx_extension_AssetsManager_setPackageUrl);
        tolua_function(tolua_S,"checkUpdate",lua_cocos2dx_extension_AssetsManager_checkUpdate);
        tolua_function(tolua_S,"getStoragePath",lua_cocos2dx_extension_AssetsManager_getStoragePath);
        tolua_function(tolua_S,"update",lua_cocos2dx_extension_AssetsManager_update);
        tolua_function(tolua_S,"setConnectionTimeout",lua_cocos2dx_extension_AssetsManager_setConnectionTimeout);
        tolua_function(tolua_S,"setVersionFileUrl",lua_cocos2dx_extension_AssetsManager_setVersionFileUrl);
        tolua_function(tolua_S,"getPackageUrl",lua_cocos2dx_extension_AssetsManager_getPackageUrl);
        tolua_function(tolua_S,"getConnectionTimeout",lua_cocos2dx_extension_AssetsManager_getConnectionTimeout);
        tolua_function(tolua_S,"getVersion",lua_cocos2dx_extension_AssetsManager_getVersion);
        tolua_function(tolua_S,"getVersionFileUrl",lua_cocos2dx_extension_AssetsManager_getVersionFileUrl);
        tolua_function(tolua_S,"deleteVersion",lua_cocos2dx_extension_AssetsManager_deleteVersion);
        tolua_function(tolua_S,"create", lua_cocos2dx_extension_AssetsManager_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::extension::AssetsManager).name();
    g_luaType[typeName] = "cc.AssetsManager";
    g_typeCast["AssetsManager"] = "cc.AssetsManager";
    return 1;
}

int lua_cocos2dx_extension_EventAssetsManagerEx_getAssetsManagerEx(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getAssetsManagerEx'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getAssetsManagerEx'", nullptr);
            return 0;
        }
        cocos2d::extension::AssetsManagerEx* ret = cobj->getAssetsManagerEx();
        object_to_luaval<cocos2d::extension::AssetsManagerEx>(tolua_S, "cc.AssetsManagerEx",(cocos2d::extension::AssetsManagerEx*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getAssetsManagerEx",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getAssetsManagerEx'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getAssetId(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getAssetId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getAssetId'", nullptr);
            return 0;
        }
        std::string ret = cobj->getAssetId();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getAssetId",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getAssetId'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getCURLECode(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getCURLECode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getCURLECode'", nullptr);
            return 0;
        }
        int ret = cobj->getCURLECode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getCURLECode",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getCURLECode'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getMessage(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getMessage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getMessage'", nullptr);
            return 0;
        }
        std::string ret = cobj->getMessage();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getMessage",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getMessage'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getCURLMCode(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getCURLMCode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getCURLMCode'", nullptr);
            return 0;
        }
        int ret = cobj->getCURLMCode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getCURLMCode",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getCURLMCode'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getPercentByFile(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getPercentByFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getPercentByFile'", nullptr);
            return 0;
        }
        double ret = cobj->getPercentByFile();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getPercentByFile",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getPercentByFile'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getEventCode(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getEventCode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getEventCode'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getEventCode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getEventCode",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getEventCode'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getPercent(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getPercent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getPercent'", nullptr);
            return 0;
        }
        double ret = cobj->getPercent();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getPercent",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getPercent'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 4) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 5) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 6) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;
        std::string arg5;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 7,&arg5, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4, arg5);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 7) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;
        std::string arg5;
        std::string arg6;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 7,&arg5, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 8,&arg6, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 8) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;
        std::string arg5;
        std::string arg6;
        int arg7;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 7,&arg5, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 8,&arg6, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 9,(int *)&arg7, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 9) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;
        std::string arg5;
        std::string arg6;
        int arg7;
        int arg8;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 7,&arg5, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 8,&arg6, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 9,(int *)&arg7, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 10,(int *)&arg8, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:EventAssetsManagerEx",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_extension_EventAssetsManagerEx_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (EventAssetsManagerEx)");
    return 0;
}

int lua_register_cocos2dx_extension_EventAssetsManagerEx(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"cc.EventAssetsManagerEx");
    tolua_cclass(tolua_S,"EventAssetsManagerEx","cc.EventAssetsManagerEx","cc.EventCustom",nullptr);

    tolua_beginmodule(tolua_S,"EventAssetsManagerEx");
        tolua_function(tolua_S,"new",lua_cocos2dx_extension_EventAssetsManagerEx_constructor);
        tolua_function(tolua_S,"getAssetsManagerEx",lua_cocos2dx_extension_EventAssetsManagerEx_getAssetsManagerEx);
        tolua_function(tolua_S,"getAssetId",lua_cocos2dx_extension_EventAssetsManagerEx_getAssetId);
        tolua_function(tolua_S,"getCURLECode",lua_cocos2dx_extension_EventAssetsManagerEx_getCURLECode);
        tolua_function(tolua_S,"getMessage",lua_cocos2dx_extension_EventAssetsManagerEx_getMessage);
        tolua_function(tolua_S,"getCURLMCode",lua_cocos2dx_extension_EventAssetsManagerEx_getCURLMCode);
        tolua_function(tolua_S,"getPercentByFile",lua_cocos2dx_extension_EventAssetsManagerEx_getPercentByFile);
        tolua_function(tolua_S,"getEventCode",lua_cocos2dx_extension_EventAssetsManagerEx_getEventCode);
        tolua_function(tolua_S,"getPercent",lua_cocos2dx_extension_EventAssetsManagerEx_getPercent);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::extension::EventAssetsManagerEx).name();
    g_luaType[typeName] = "cc.EventAssetsManagerEx";
    g_typeCast["EventAssetsManagerEx"] = "cc.EventAssetsManagerEx";
    return 1;
}

int lua_cocos2dx_extension_Manifest_getManifestFileUrl(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_getManifestFileUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_getManifestFileUrl'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getManifestFileUrl();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:getManifestFileUrl",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_getManifestFileUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_Manifest_isVersionLoaded(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_isVersionLoaded'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_isVersionLoaded'", nullptr);
            return 0;
        }
        bool ret = cobj->isVersionLoaded();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:isVersionLoaded",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_isVersionLoaded'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_Manifest_isLoaded(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_isLoaded'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_isLoaded'", nullptr);
            return 0;
        }
        bool ret = cobj->isLoaded();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:isLoaded",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_isLoaded'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_Manifest_getPackageUrl(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_getPackageUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_getPackageUrl'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getPackageUrl();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:getPackageUrl",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_getPackageUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_Manifest_getVersion(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_getVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_getVersion'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getVersion();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:getVersion",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_getVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_Manifest_getVersionFileUrl(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_getVersionFileUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_getVersionFileUrl'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getVersionFileUrl();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:getVersionFileUrl",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_getVersionFileUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_Manifest_getSearchPaths(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_getSearchPaths'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_getSearchPaths'", nullptr);
            return 0;
        }
        std::vector<std::string> ret = cobj->getSearchPaths();
        ccvector_std_string_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:getSearchPaths",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_getSearchPaths'.",&tolua_err);
#endif

    return 0;
}
static int lua_cocos2dx_extension_Manifest_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Manifest)");
    return 0;
}

int lua_register_cocos2dx_extension_Manifest(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"cc.Manifest");
    tolua_cclass(tolua_S,"Manifest","cc.Manifest","cc.Ref",nullptr);

    tolua_beginmodule(tolua_S,"Manifest");
        tolua_function(tolua_S,"getManifestFileUrl",lua_cocos2dx_extension_Manifest_getManifestFileUrl);
        tolua_function(tolua_S,"isVersionLoaded",lua_cocos2dx_extension_Manifest_isVersionLoaded);
        tolua_function(tolua_S,"isLoaded",lua_cocos2dx_extension_Manifest_isLoaded);
        tolua_function(tolua_S,"getPackageUrl",lua_cocos2dx_extension_Manifest_getPackageUrl);
        tolua_function(tolua_S,"getVersion",lua_cocos2dx_extension_Manifest_getVersion);
        tolua_function(tolua_S,"getVersionFileUrl",lua_cocos2dx_extension_Manifest_getVersionFileUrl);
        tolua_function(tolua_S,"getSearchPaths",lua_cocos2dx_extension_Manifest_getSearchPaths);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::extension::Manifest).name();
    g_luaType[typeName] = "cc.Manifest";
    g_typeCast["Manifest"] = "cc.Manifest";
    return 1;
}

int lua_cocos2dx_extension_AssetsManagerEx_getState(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_getState'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_getState'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getState();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:getState",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_getState'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_getMaxConcurrentTask(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_getMaxConcurrentTask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_getMaxConcurrentTask'", nullptr);
            return 0;
        }
        const int ret = cobj->getMaxConcurrentTask();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:getMaxConcurrentTask",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_getMaxConcurrentTask'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_checkUpdate(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_checkUpdate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_checkUpdate'", nullptr);
            return 0;
        }
        cobj->checkUpdate();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:checkUpdate",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_checkUpdate'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_setVerifyCallback(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_setVerifyCallback'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<bool (const std::string&, cocos2d::extension::ManifestAsset)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_setVerifyCallback'", nullptr);
            return 0;
        }
        cobj->setVerifyCallback(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:setVerifyCallback",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_setVerifyCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_getStoragePath(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_getStoragePath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_getStoragePath'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getStoragePath();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:getStoragePath",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_getStoragePath'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_update(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_update'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_update'", nullptr);
            return 0;
        }
        cobj->update();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:update",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_update'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_setVersionCompareHandle(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_setVersionCompareHandle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<int (const std::string&, const std::string&)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_setVersionCompareHandle'", nullptr);
            return 0;
        }
        cobj->setVersionCompareHandle(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:setVersionCompareHandle",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_setVersionCompareHandle'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_setMaxConcurrentTask(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_setMaxConcurrentTask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "cc.AssetsManagerEx:setMaxConcurrentTask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_setMaxConcurrentTask'", nullptr);
            return 0;
        }
        cobj->setMaxConcurrentTask(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:setMaxConcurrentTask",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_setMaxConcurrentTask'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_getLocalManifest(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_getLocalManifest'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_getLocalManifest'", nullptr);
            return 0;
        }
        const cocos2d::extension::Manifest* ret = cobj->getLocalManifest();
        object_to_luaval<cocos2d::extension::Manifest>(tolua_S, "cc.Manifest",(cocos2d::extension::Manifest*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:getLocalManifest",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_getLocalManifest'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_getRemoteManifest(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_getRemoteManifest'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_getRemoteManifest'", nullptr);
            return 0;
        }
        const cocos2d::extension::Manifest* ret = cobj->getRemoteManifest();
        object_to_luaval<cocos2d::extension::Manifest>(tolua_S, "cc.Manifest",(cocos2d::extension::Manifest*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:getRemoteManifest",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_getRemoteManifest'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_downloadFailedAssets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_downloadFailedAssets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_downloadFailedAssets'", nullptr);
            return 0;
        }
        cobj->downloadFailedAssets();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:downloadFailedAssets",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_downloadFailedAssets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        std::string arg0;
        std::string arg1;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.AssetsManagerEx:create");
        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "cc.AssetsManagerEx:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_create'", nullptr);
            return 0;
        }
        cocos2d::extension::AssetsManagerEx* ret = cocos2d::extension::AssetsManagerEx::create(arg0, arg1);
        object_to_luaval<cocos2d::extension::AssetsManagerEx>(tolua_S, "cc.AssetsManagerEx",(cocos2d::extension::AssetsManagerEx*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "cc.AssetsManagerEx:create",argc, 2);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        std::string arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.AssetsManagerEx:AssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "cc.AssetsManagerEx:AssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::AssetsManagerEx(arg0, arg1);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.AssetsManagerEx");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:AssetsManagerEx",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_extension_AssetsManagerEx_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (AssetsManagerEx)");
    return 0;
}

int lua_register_cocos2dx_extension_AssetsManagerEx(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"cc.AssetsManagerEx");
    tolua_cclass(tolua_S,"AssetsManagerEx","cc.AssetsManagerEx","cc.Ref",nullptr);

    tolua_beginmodule(tolua_S,"AssetsManagerEx");
        tolua_function(tolua_S,"new",lua_cocos2dx_extension_AssetsManagerEx_constructor);
        tolua_function(tolua_S,"getState",lua_cocos2dx_extension_AssetsManagerEx_getState);
        tolua_function(tolua_S,"getMaxConcurrentTask",lua_cocos2dx_extension_AssetsManagerEx_getMaxConcurrentTask);
        tolua_function(tolua_S,"checkUpdate",lua_cocos2dx_extension_AssetsManagerEx_checkUpdate);
        tolua_function(tolua_S,"setVerifyCallback",lua_cocos2dx_extension_AssetsManagerEx_setVerifyCallback);
        tolua_function(tolua_S,"getStoragePath",lua_cocos2dx_extension_AssetsManagerEx_getStoragePath);
        tolua_function(tolua_S,"update",lua_cocos2dx_extension_AssetsManagerEx_update);
        tolua_function(tolua_S,"setVersionCompareHandle",lua_cocos2dx_extension_AssetsManagerEx_setVersionCompareHandle);
        tolua_function(tolua_S,"setMaxConcurrentTask",lua_cocos2dx_extension_AssetsManagerEx_setMaxConcurrentTask);
        tolua_function(tolua_S,"getLocalManifest",lua_cocos2dx_extension_AssetsManagerEx_getLocalManifest);
        tolua_function(tolua_S,"getRemoteManifest",lua_cocos2dx_extension_AssetsManagerEx_getRemoteManifest);
        tolua_function(tolua_S,"downloadFailedAssets",lua_cocos2dx_extension_AssetsManagerEx_downloadFailedAssets);
        tolua_function(tolua_S,"create", lua_cocos2dx_extension_AssetsManagerEx_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::extension::AssetsManagerEx).name();
    g_luaType[typeName] = "cc.AssetsManagerEx";
    g_typeCast["AssetsManagerEx"] = "cc.AssetsManagerEx";
    return 1;
}

int lua_cocos2dx_extension_EventListenerAssetsManagerEx_init(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventListenerAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventListenerAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventListenerAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventListenerAssetsManagerEx_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        const cocos2d::extension::AssetsManagerEx* arg0;
        std::function<void (cocos2d::extension::EventAssetsManagerEx *)> arg1;

        ok &= luaval_to_object<const cocos2d::extension::AssetsManagerEx>(tolua_S, 2, "cc.AssetsManagerEx",&arg0, "cc.EventListenerAssetsManagerEx:init");

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventListenerAssetsManagerEx_init'", nullptr);
            return 0;
        }
        bool ret = cobj->init(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventListenerAssetsManagerEx:init",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventListenerAssetsManagerEx_init'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventListenerAssetsManagerEx_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventListenerAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventListenerAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventListenerAssetsManagerEx();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventListenerAssetsManagerEx");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventListenerAssetsManagerEx:EventListenerAssetsManagerEx",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventListenerAssetsManagerEx_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_extension_EventListenerAssetsManagerEx_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (EventListenerAssetsManagerEx)");
    return 0;
}

int lua_register_cocos2dx_extension_EventListenerAssetsManagerEx(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"cc.EventListenerAssetsManagerEx");
    tolua_cclass(tolua_S,"EventListenerAssetsManagerEx","cc.EventListenerAssetsManagerEx","cc.EventListenerCustom",nullptr);

    tolua_beginmodule(tolua_S,"EventListenerAssetsManagerEx");
        tolua_function(tolua_S,"new",lua_cocos2dx_extension_EventListenerAssetsManagerEx_constructor);
        tolua_function(tolua_S,"init",lua_cocos2dx_extension_EventListenerAssetsManagerEx_init);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::extension::EventListenerAssetsManagerEx).name();
    g_luaType[typeName] = "cc.EventListenerAssetsManagerEx";
    g_typeCast["EventListenerAssetsManagerEx"] = "cc.EventListenerAssetsManagerEx";
    return 1;
}

int lua_cocos2dx_extension_ParticleSystem3D_removeAffector(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_removeAffector'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "cc.ParticleSystem3D:removeAffector");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_removeAffector'", nullptr);
            return 0;
        }
        cobj->removeAffector(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:removeAffector",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_removeAffector'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_resumeParticleSystem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_resumeParticleSystem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_resumeParticleSystem'", nullptr);
            return 0;
        }
        cobj->resumeParticleSystem();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:resumeParticleSystem",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_resumeParticleSystem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_removeAllAffector(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_removeAllAffector'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_removeAllAffector'", nullptr);
            return 0;
        }
        cobj->removeAllAffector();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:removeAllAffector",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_removeAllAffector'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_addAffector(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_addAffector'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Particle3DAffector* arg0;

        ok &= luaval_to_object<cocos2d::Particle3DAffector>(tolua_S, 2, "cc.Particle3DAffector",&arg0, "cc.ParticleSystem3D:addAffector");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_addAffector'", nullptr);
            return 0;
        }
        cobj->addAffector(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:addAffector",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_addAffector'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_startParticleSystem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_startParticleSystem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_startParticleSystem'", nullptr);
            return 0;
        }
        cobj->startParticleSystem();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:startParticleSystem",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_startParticleSystem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_isEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_isEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_isEnabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:isEnabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_isEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_getRender(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_getRender'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_getRender'", nullptr);
            return 0;
        }
        cocos2d::Particle3DRender* ret = cobj->getRender();
        object_to_luaval<cocos2d::Particle3DRender>(tolua_S, "cc.Particle3DRender",(cocos2d::Particle3DRender*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:getRender",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_getRender'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_setEmitter(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_setEmitter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Particle3DEmitter* arg0;

        ok &= luaval_to_object<cocos2d::Particle3DEmitter>(tolua_S, 2, "cc.Particle3DEmitter",&arg0, "cc.ParticleSystem3D:setEmitter");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_setEmitter'", nullptr);
            return 0;
        }
        cobj->setEmitter(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:setEmitter",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_setEmitter'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_isKeepLocal(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_isKeepLocal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_isKeepLocal'", nullptr);
            return 0;
        }
        bool ret = cobj->isKeepLocal();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:isKeepLocal",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_isKeepLocal'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_setEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_setEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "cc.ParticleSystem3D:setEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_setEnabled'", nullptr);
            return 0;
        }
        cobj->setEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:setEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_setEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_getParticleQuota(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_getParticleQuota'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_getParticleQuota'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getParticleQuota();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:getParticleQuota",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_getParticleQuota'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_getBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_getBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_getBlendFunc'", nullptr);
            return 0;
        }
        const cocos2d::BlendFunc& ret = cobj->getBlendFunc();
        blendfunc_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:getBlendFunc",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_getBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_pauseParticleSystem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_pauseParticleSystem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_pauseParticleSystem'", nullptr);
            return 0;
        }
        cobj->pauseParticleSystem();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:pauseParticleSystem",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_pauseParticleSystem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_getState(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_getState'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_getState'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getState();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:getState",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_getState'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_getAliveParticleCount(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_getAliveParticleCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_getAliveParticleCount'", nullptr);
            return 0;
        }
        int ret = cobj->getAliveParticleCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:getAliveParticleCount",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_getAliveParticleCount'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_setParticleQuota(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_setParticleQuota'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "cc.ParticleSystem3D:setParticleQuota");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_setParticleQuota'", nullptr);
            return 0;
        }
        cobj->setParticleQuota(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:setParticleQuota",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_setParticleQuota'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_setBlendFunc(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_setBlendFunc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::BlendFunc arg0;

        ok &= luaval_to_blendfunc(tolua_S, 2, &arg0, "cc.ParticleSystem3D:setBlendFunc");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_setBlendFunc'", nullptr);
            return 0;
        }
        cobj->setBlendFunc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:setBlendFunc",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_setBlendFunc'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_setRender(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_setRender'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Particle3DRender* arg0;

        ok &= luaval_to_object<cocos2d::Particle3DRender>(tolua_S, 2, "cc.Particle3DRender",&arg0, "cc.ParticleSystem3D:setRender");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_setRender'", nullptr);
            return 0;
        }
        cobj->setRender(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:setRender",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_setRender'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_stopParticleSystem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_stopParticleSystem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_stopParticleSystem'", nullptr);
            return 0;
        }
        cobj->stopParticleSystem();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:stopParticleSystem",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_stopParticleSystem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_setKeepLocal(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.ParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_ParticleSystem3D_setKeepLocal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "cc.ParticleSystem3D:setKeepLocal");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_setKeepLocal'", nullptr);
            return 0;
        }
        cobj->setKeepLocal(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:setKeepLocal",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_setKeepLocal'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_ParticleSystem3D_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_ParticleSystem3D_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ParticleSystem3D();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.ParticleSystem3D");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.ParticleSystem3D:ParticleSystem3D",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_ParticleSystem3D_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_extension_ParticleSystem3D_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (ParticleSystem3D)");
    return 0;
}

int lua_register_cocos2dx_extension_ParticleSystem3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"cc.ParticleSystem3D");
    tolua_cclass(tolua_S,"ParticleSystem3D","cc.ParticleSystem3D","cc.Node",nullptr);

    tolua_beginmodule(tolua_S,"ParticleSystem3D");
        tolua_function(tolua_S,"new",lua_cocos2dx_extension_ParticleSystem3D_constructor);
        tolua_function(tolua_S,"removeAffector",lua_cocos2dx_extension_ParticleSystem3D_removeAffector);
        tolua_function(tolua_S,"resumeParticleSystem",lua_cocos2dx_extension_ParticleSystem3D_resumeParticleSystem);
        tolua_function(tolua_S,"removeAllAffector",lua_cocos2dx_extension_ParticleSystem3D_removeAllAffector);
        tolua_function(tolua_S,"addAffector",lua_cocos2dx_extension_ParticleSystem3D_addAffector);
        tolua_function(tolua_S,"startParticleSystem",lua_cocos2dx_extension_ParticleSystem3D_startParticleSystem);
        tolua_function(tolua_S,"isEnabled",lua_cocos2dx_extension_ParticleSystem3D_isEnabled);
        tolua_function(tolua_S,"getRender",lua_cocos2dx_extension_ParticleSystem3D_getRender);
        tolua_function(tolua_S,"setEmitter",lua_cocos2dx_extension_ParticleSystem3D_setEmitter);
        tolua_function(tolua_S,"isKeepLocal",lua_cocos2dx_extension_ParticleSystem3D_isKeepLocal);
        tolua_function(tolua_S,"setEnabled",lua_cocos2dx_extension_ParticleSystem3D_setEnabled);
        tolua_function(tolua_S,"getParticleQuota",lua_cocos2dx_extension_ParticleSystem3D_getParticleQuota);
        tolua_function(tolua_S,"getBlendFunc",lua_cocos2dx_extension_ParticleSystem3D_getBlendFunc);
        tolua_function(tolua_S,"pauseParticleSystem",lua_cocos2dx_extension_ParticleSystem3D_pauseParticleSystem);
        tolua_function(tolua_S,"getState",lua_cocos2dx_extension_ParticleSystem3D_getState);
        tolua_function(tolua_S,"getAliveParticleCount",lua_cocos2dx_extension_ParticleSystem3D_getAliveParticleCount);
        tolua_function(tolua_S,"setParticleQuota",lua_cocos2dx_extension_ParticleSystem3D_setParticleQuota);
        tolua_function(tolua_S,"setBlendFunc",lua_cocos2dx_extension_ParticleSystem3D_setBlendFunc);
        tolua_function(tolua_S,"setRender",lua_cocos2dx_extension_ParticleSystem3D_setRender);
        tolua_function(tolua_S,"stopParticleSystem",lua_cocos2dx_extension_ParticleSystem3D_stopParticleSystem);
        tolua_function(tolua_S,"setKeepLocal",lua_cocos2dx_extension_ParticleSystem3D_setKeepLocal);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ParticleSystem3D).name();
    g_luaType[typeName] = "cc.ParticleSystem3D";
    g_typeCast["ParticleSystem3D"] = "cc.ParticleSystem3D";
    return 1;
}

int lua_cocos2dx_extension_PUParticleSystem3D_initWithFilePath(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_initWithFilePath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:initWithFilePath");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_initWithFilePath'", nullptr);
            return 0;
        }
        bool ret = cobj->initWithFilePath(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:initWithFilePath",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_initWithFilePath'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_getParticleSystemScaleVelocity(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_getParticleSystemScaleVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_getParticleSystemScaleVelocity'", nullptr);
            return 0;
        }
        double ret = cobj->getParticleSystemScaleVelocity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:getParticleSystemScaleVelocity",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_getParticleSystemScaleVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_setEmittedSystemQuota(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_setEmittedSystemQuota'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:setEmittedSystemQuota");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_setEmittedSystemQuota'", nullptr);
            return 0;
        }
        cobj->setEmittedSystemQuota(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:setEmittedSystemQuota",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_setEmittedSystemQuota'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_getDefaultDepth(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDefaultDepth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDefaultDepth'", nullptr);
            return 0;
        }
        double ret = cobj->getDefaultDepth();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:getDefaultDepth",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDefaultDepth'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_getEmittedSystemQuota(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_getEmittedSystemQuota'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_getEmittedSystemQuota'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getEmittedSystemQuota();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:getEmittedSystemQuota",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_getEmittedSystemQuota'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_initWithFilePathAndMaterialPath(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_initWithFilePathAndMaterialPath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        std::string arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:initWithFilePathAndMaterialPath");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "cc.PUParticleSystem3D:initWithFilePathAndMaterialPath");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_initWithFilePathAndMaterialPath'", nullptr);
            return 0;
        }
        bool ret = cobj->initWithFilePathAndMaterialPath(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:initWithFilePathAndMaterialPath",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_initWithFilePathAndMaterialPath'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_clearAllParticles(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_clearAllParticles'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_clearAllParticles'", nullptr);
            return 0;
        }
        cobj->clearAllParticles();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:clearAllParticles",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_clearAllParticles'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_getMaterialName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_getMaterialName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_getMaterialName'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getMaterialName();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:getMaterialName",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_getMaterialName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_calulateRotationOffset(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_calulateRotationOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_calulateRotationOffset'", nullptr);
            return 0;
        }
        cobj->calulateRotationOffset();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:calulateRotationOffset",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_calulateRotationOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_getMaxVelocity(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_getMaxVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_getMaxVelocity'", nullptr);
            return 0;
        }
        double ret = cobj->getMaxVelocity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:getMaxVelocity",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_getMaxVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_forceUpdate(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_forceUpdate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:forceUpdate");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_forceUpdate'", nullptr);
            return 0;
        }
        cobj->forceUpdate(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:forceUpdate",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_forceUpdate'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_getTimeElapsedSinceStart(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_getTimeElapsedSinceStart'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_getTimeElapsedSinceStart'", nullptr);
            return 0;
        }
        double ret = cobj->getTimeElapsedSinceStart();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:getTimeElapsedSinceStart",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_getTimeElapsedSinceStart'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_removeAllBehaviourTemplate(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_removeAllBehaviourTemplate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_removeAllBehaviourTemplate'", nullptr);
            return 0;
        }
        cobj->removeAllBehaviourTemplate();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:removeAllBehaviourTemplate",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_removeAllBehaviourTemplate'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_getEmittedEmitterQuota(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_getEmittedEmitterQuota'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_getEmittedEmitterQuota'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getEmittedEmitterQuota();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:getEmittedEmitterQuota",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_getEmittedEmitterQuota'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_forceEmission(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_forceEmission'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        cocos2d::PUEmitter* arg0;
        unsigned int arg1;

        ok &= luaval_to_object<cocos2d::PUEmitter>(tolua_S, 2, "cc.PUEmitter",&arg0, "cc.PUParticleSystem3D:forceEmission");

        ok &= luaval_to_uint32(tolua_S, 3,&arg1, "cc.PUParticleSystem3D:forceEmission");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_forceEmission'", nullptr);
            return 0;
        }
        cobj->forceEmission(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:forceEmission",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_forceEmission'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_addListener(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_addListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::PUListener* arg0;

        ok &= luaval_to_object<cocos2d::PUListener>(tolua_S, 2, "cc.PUListener",&arg0, "cc.PUParticleSystem3D:addListener");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_addListener'", nullptr);
            return 0;
        }
        cobj->addListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:addListener",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_addListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_isMarkedForEmission(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_isMarkedForEmission'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_isMarkedForEmission'", nullptr);
            return 0;
        }
        bool ret = cobj->isMarkedForEmission();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:isMarkedForEmission",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_isMarkedForEmission'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_getDefaultWidth(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDefaultWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDefaultWidth'", nullptr);
            return 0;
        }
        double ret = cobj->getDefaultWidth();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:getDefaultWidth",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDefaultWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_setEmittedEmitterQuota(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_setEmittedEmitterQuota'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:setEmittedEmitterQuota");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_setEmittedEmitterQuota'", nullptr);
            return 0;
        }
        cobj->setEmittedEmitterQuota(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:setEmittedEmitterQuota",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_setEmittedEmitterQuota'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_setMarkedForEmission(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_setMarkedForEmission'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:setMarkedForEmission");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_setMarkedForEmission'", nullptr);
            return 0;
        }
        cobj->setMarkedForEmission(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:setMarkedForEmission",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_setMarkedForEmission'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_clone(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_clone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_clone'", nullptr);
            return 0;
        }
        cocos2d::PUParticleSystem3D* ret = cobj->clone();
        object_to_luaval<cocos2d::PUParticleSystem3D>(tolua_S, "cc.PUParticleSystem3D",(cocos2d::PUParticleSystem3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:clone",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_clone'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_addEmitter(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_addEmitter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::PUEmitter* arg0;

        ok &= luaval_to_object<cocos2d::PUEmitter>(tolua_S, 2, "cc.PUEmitter",&arg0, "cc.PUParticleSystem3D:addEmitter");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_addEmitter'", nullptr);
            return 0;
        }
        cobj->addEmitter(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:addEmitter",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_addEmitter'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_addBehaviourTemplate(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_addBehaviourTemplate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::PUBehaviour* arg0;

        ok &= luaval_to_object<cocos2d::PUBehaviour>(tolua_S, 2, "cc.PUBehaviour",&arg0, "cc.PUParticleSystem3D:addBehaviourTemplate");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_addBehaviourTemplate'", nullptr);
            return 0;
        }
        cobj->addBehaviourTemplate(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:addBehaviourTemplate",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_addBehaviourTemplate'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_setDefaultWidth(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_setDefaultWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:setDefaultWidth");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_setDefaultWidth'", nullptr);
            return 0;
        }
        cobj->setDefaultWidth(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:setDefaultWidth",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_setDefaultWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_copyAttributesTo(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_copyAttributesTo'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::PUParticleSystem3D* arg0;

        ok &= luaval_to_object<cocos2d::PUParticleSystem3D>(tolua_S, 2, "cc.PUParticleSystem3D",&arg0, "cc.PUParticleSystem3D:copyAttributesTo");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_copyAttributesTo'", nullptr);
            return 0;
        }
        cobj->copyAttributesTo(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:copyAttributesTo",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_copyAttributesTo'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_setMaterialName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_setMaterialName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:setMaterialName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_setMaterialName'", nullptr);
            return 0;
        }
        cobj->setMaterialName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:setMaterialName",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_setMaterialName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_getParentParticleSystem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_getParentParticleSystem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_getParentParticleSystem'", nullptr);
            return 0;
        }
        cocos2d::PUParticleSystem3D* ret = cobj->getParentParticleSystem();
        object_to_luaval<cocos2d::PUParticleSystem3D>(tolua_S, "cc.PUParticleSystem3D",(cocos2d::PUParticleSystem3D*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:getParentParticleSystem",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_getParentParticleSystem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_removeListener(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_removeListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::PUListener* arg0;

        ok &= luaval_to_object<cocos2d::PUListener>(tolua_S, 2, "cc.PUListener",&arg0, "cc.PUParticleSystem3D:removeListener");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_removeListener'", nullptr);
            return 0;
        }
        cobj->removeListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:removeListener",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_removeListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_setMaxVelocity(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_setMaxVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:setMaxVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_setMaxVelocity'", nullptr);
            return 0;
        }
        cobj->setMaxVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:setMaxVelocity",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_setMaxVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_getDefaultHeight(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDefaultHeight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDefaultHeight'", nullptr);
            return 0;
        }
        double ret = cobj->getDefaultHeight();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:getDefaultHeight",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDefaultHeight'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_getDerivedPosition(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDerivedPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDerivedPosition'", nullptr);
            return 0;
        }
        cocos2d::Vec3 ret = cobj->getDerivedPosition();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:getDerivedPosition",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDerivedPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_rotationOffset(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_rotationOffset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Vec3 arg0;

        ok &= luaval_to_vec3(tolua_S, 2, &arg0, "cc.PUParticleSystem3D:rotationOffset");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_rotationOffset'", nullptr);
            return 0;
        }
        cobj->rotationOffset(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:rotationOffset",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_rotationOffset'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_removeAllEmitter(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_removeAllEmitter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_removeAllEmitter'", nullptr);
            return 0;
        }
        cobj->removeAllEmitter();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:removeAllEmitter",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_removeAllEmitter'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_setParticleSystemScaleVelocity(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_setParticleSystemScaleVelocity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:setParticleSystemScaleVelocity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_setParticleSystemScaleVelocity'", nullptr);
            return 0;
        }
        cobj->setParticleSystemScaleVelocity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:setParticleSystemScaleVelocity",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_setParticleSystemScaleVelocity'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_getDerivedScale(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDerivedScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDerivedScale'", nullptr);
            return 0;
        }
        cocos2d::Vec3 ret = cobj->getDerivedScale();
        vec3_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:getDerivedScale",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_getDerivedScale'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_setDefaultHeight(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_setDefaultHeight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:setDefaultHeight");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_setDefaultHeight'", nullptr);
            return 0;
        }
        cobj->setDefaultHeight(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:setDefaultHeight",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_setDefaultHeight'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_removeAllListener(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_removeAllListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_removeAllListener'", nullptr);
            return 0;
        }
        cobj->removeAllListener();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:removeAllListener",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_removeAllListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_initSystem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_initSystem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:initSystem");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_initSystem'", nullptr);
            return 0;
        }
        bool ret = cobj->initSystem(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:initSystem",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_initSystem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_makeParticleLocal(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_makeParticleLocal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::PUParticle3D* arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR PUParticle3D*
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_makeParticleLocal'", nullptr);
            return 0;
        }
        bool ret = cobj->makeParticleLocal(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:makeParticleLocal",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_makeParticleLocal'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_removerAllObserver(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_removerAllObserver'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_removerAllObserver'", nullptr);
            return 0;
        }
        cobj->removerAllObserver();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:removerAllObserver",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_removerAllObserver'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_setDefaultDepth(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_setDefaultDepth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:setDefaultDepth");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_setDefaultDepth'", nullptr);
            return 0;
        }
        cobj->setDefaultDepth(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:setDefaultDepth",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_setDefaultDepth'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_addObserver(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::PUParticleSystem3D*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_PUParticleSystem3D_addObserver'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::PUObserver* arg0;

        ok &= luaval_to_object<cocos2d::PUObserver>(tolua_S, 2, "cc.PUObserver",&arg0, "cc.PUParticleSystem3D:addObserver");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_addObserver'", nullptr);
            return 0;
        }
        cobj->addObserver(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:addObserver",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_addObserver'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"cc.PUParticleSystem3D",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 1)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:create");
            if (!ok) { break; }
            cocos2d::PUParticleSystem3D* ret = cocos2d::PUParticleSystem3D::create(arg0);
            object_to_luaval<cocos2d::PUParticleSystem3D>(tolua_S, "cc.PUParticleSystem3D",(cocos2d::PUParticleSystem3D*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::PUParticleSystem3D* ret = cocos2d::PUParticleSystem3D::create();
            object_to_luaval<cocos2d::PUParticleSystem3D>(tolua_S, "cc.PUParticleSystem3D",(cocos2d::PUParticleSystem3D*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.PUParticleSystem3D:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "cc.PUParticleSystem3D:create");
            if (!ok) { break; }
            cocos2d::PUParticleSystem3D* ret = cocos2d::PUParticleSystem3D::create(arg0, arg1);
            object_to_luaval<cocos2d::PUParticleSystem3D>(tolua_S, "cc.PUParticleSystem3D",(cocos2d::PUParticleSystem3D*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "cc.PUParticleSystem3D:create",argc, 2);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_extension_PUParticleSystem3D_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::PUParticleSystem3D* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_PUParticleSystem3D_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::PUParticleSystem3D();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.PUParticleSystem3D");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.PUParticleSystem3D:PUParticleSystem3D",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_PUParticleSystem3D_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_extension_PUParticleSystem3D_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PUParticleSystem3D)");
    return 0;
}

int lua_register_cocos2dx_extension_PUParticleSystem3D(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"cc.PUParticleSystem3D");
    tolua_cclass(tolua_S,"PUParticleSystem3D","cc.PUParticleSystem3D","cc.ParticleSystem3D",nullptr);

    tolua_beginmodule(tolua_S,"PUParticleSystem3D");
        tolua_function(tolua_S,"new",lua_cocos2dx_extension_PUParticleSystem3D_constructor);
        tolua_function(tolua_S,"initWithFilePath",lua_cocos2dx_extension_PUParticleSystem3D_initWithFilePath);
        tolua_function(tolua_S,"getParticleSystemScaleVelocity",lua_cocos2dx_extension_PUParticleSystem3D_getParticleSystemScaleVelocity);
        tolua_function(tolua_S,"setEmittedSystemQuota",lua_cocos2dx_extension_PUParticleSystem3D_setEmittedSystemQuota);
        tolua_function(tolua_S,"getDefaultDepth",lua_cocos2dx_extension_PUParticleSystem3D_getDefaultDepth);
        tolua_function(tolua_S,"getEmittedSystemQuota",lua_cocos2dx_extension_PUParticleSystem3D_getEmittedSystemQuota);
        tolua_function(tolua_S,"initWithFilePathAndMaterialPath",lua_cocos2dx_extension_PUParticleSystem3D_initWithFilePathAndMaterialPath);
        tolua_function(tolua_S,"clearAllParticles",lua_cocos2dx_extension_PUParticleSystem3D_clearAllParticles);
        tolua_function(tolua_S,"getMaterialName",lua_cocos2dx_extension_PUParticleSystem3D_getMaterialName);
        tolua_function(tolua_S,"calulateRotationOffset",lua_cocos2dx_extension_PUParticleSystem3D_calulateRotationOffset);
        tolua_function(tolua_S,"getMaxVelocity",lua_cocos2dx_extension_PUParticleSystem3D_getMaxVelocity);
        tolua_function(tolua_S,"forceUpdate",lua_cocos2dx_extension_PUParticleSystem3D_forceUpdate);
        tolua_function(tolua_S,"getTimeElapsedSinceStart",lua_cocos2dx_extension_PUParticleSystem3D_getTimeElapsedSinceStart);
        tolua_function(tolua_S,"removeAllBehaviourTemplate",lua_cocos2dx_extension_PUParticleSystem3D_removeAllBehaviourTemplate);
        tolua_function(tolua_S,"getEmittedEmitterQuota",lua_cocos2dx_extension_PUParticleSystem3D_getEmittedEmitterQuota);
        tolua_function(tolua_S,"forceEmission",lua_cocos2dx_extension_PUParticleSystem3D_forceEmission);
        tolua_function(tolua_S,"addListener",lua_cocos2dx_extension_PUParticleSystem3D_addListener);
        tolua_function(tolua_S,"isMarkedForEmission",lua_cocos2dx_extension_PUParticleSystem3D_isMarkedForEmission);
        tolua_function(tolua_S,"getDefaultWidth",lua_cocos2dx_extension_PUParticleSystem3D_getDefaultWidth);
        tolua_function(tolua_S,"setEmittedEmitterQuota",lua_cocos2dx_extension_PUParticleSystem3D_setEmittedEmitterQuota);
        tolua_function(tolua_S,"setMarkedForEmission",lua_cocos2dx_extension_PUParticleSystem3D_setMarkedForEmission);
        tolua_function(tolua_S,"clone",lua_cocos2dx_extension_PUParticleSystem3D_clone);
        tolua_function(tolua_S,"addEmitter",lua_cocos2dx_extension_PUParticleSystem3D_addEmitter);
        tolua_function(tolua_S,"addBehaviourTemplate",lua_cocos2dx_extension_PUParticleSystem3D_addBehaviourTemplate);
        tolua_function(tolua_S,"setDefaultWidth",lua_cocos2dx_extension_PUParticleSystem3D_setDefaultWidth);
        tolua_function(tolua_S,"copyAttributesTo",lua_cocos2dx_extension_PUParticleSystem3D_copyAttributesTo);
        tolua_function(tolua_S,"setMaterialName",lua_cocos2dx_extension_PUParticleSystem3D_setMaterialName);
        tolua_function(tolua_S,"getParentParticleSystem",lua_cocos2dx_extension_PUParticleSystem3D_getParentParticleSystem);
        tolua_function(tolua_S,"removeListener",lua_cocos2dx_extension_PUParticleSystem3D_removeListener);
        tolua_function(tolua_S,"setMaxVelocity",lua_cocos2dx_extension_PUParticleSystem3D_setMaxVelocity);
        tolua_function(tolua_S,"getDefaultHeight",lua_cocos2dx_extension_PUParticleSystem3D_getDefaultHeight);
        tolua_function(tolua_S,"getDerivedPosition",lua_cocos2dx_extension_PUParticleSystem3D_getDerivedPosition);
        tolua_function(tolua_S,"rotationOffset",lua_cocos2dx_extension_PUParticleSystem3D_rotationOffset);
        tolua_function(tolua_S,"removeAllEmitter",lua_cocos2dx_extension_PUParticleSystem3D_removeAllEmitter);
        tolua_function(tolua_S,"setParticleSystemScaleVelocity",lua_cocos2dx_extension_PUParticleSystem3D_setParticleSystemScaleVelocity);
        tolua_function(tolua_S,"getDerivedScale",lua_cocos2dx_extension_PUParticleSystem3D_getDerivedScale);
        tolua_function(tolua_S,"setDefaultHeight",lua_cocos2dx_extension_PUParticleSystem3D_setDefaultHeight);
        tolua_function(tolua_S,"removeAllListener",lua_cocos2dx_extension_PUParticleSystem3D_removeAllListener);
        tolua_function(tolua_S,"initSystem",lua_cocos2dx_extension_PUParticleSystem3D_initSystem);
        tolua_function(tolua_S,"makeParticleLocal",lua_cocos2dx_extension_PUParticleSystem3D_makeParticleLocal);
        tolua_function(tolua_S,"removerAllObserver",lua_cocos2dx_extension_PUParticleSystem3D_removerAllObserver);
        tolua_function(tolua_S,"setDefaultDepth",lua_cocos2dx_extension_PUParticleSystem3D_setDefaultDepth);
        tolua_function(tolua_S,"addObserver",lua_cocos2dx_extension_PUParticleSystem3D_addObserver);
        tolua_function(tolua_S,"create", lua_cocos2dx_extension_PUParticleSystem3D_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::PUParticleSystem3D).name();
    g_luaType[typeName] = "cc.PUParticleSystem3D";
    g_typeCast["PUParticleSystem3D"] = "cc.PUParticleSystem3D";
    return 1;
}
TOLUA_API int register_all_cocos2dx_extension(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"cc",0);
	tolua_beginmodule(tolua_S,"cc");

	lua_register_cocos2dx_extension_AssetsManagerEx(tolua_S);
	lua_register_cocos2dx_extension_EventListenerAssetsManagerEx(tolua_S);
	lua_register_cocos2dx_extension_AssetsManager(tolua_S);
	lua_register_cocos2dx_extension_Manifest(tolua_S);
	lua_register_cocos2dx_extension_EventAssetsManagerEx(tolua_S);
	lua_register_cocos2dx_extension_ParticleSystem3D(tolua_S);
	lua_register_cocos2dx_extension_PUParticleSystem3D(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

