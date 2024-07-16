#include "lua-bindings/auto/axlua_audioengine_auto.hpp"
#if defined(AX_ENABLE_AUDIO) && (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_MAC || defined(_WIN32) || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX || AX_TARGET_PLATFORM == AX_PLATFORM_WASM)
#include "audio/AudioEngine.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"

int lua_ax_audioengine_AudioProfile_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::AudioProfile* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioProfile_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::AudioProfile();
        tolua_pushusertype(tolua_S,(void*)cobj,"ax.AudioProfile");
        tolua_register_gc(tolua_S,lua_gettop(tolua_S));
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AudioProfile:AudioProfile",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioProfile_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_audioengine_AudioProfile_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (AudioProfile)");
    return 0;
}

int lua_register_ax_audioengine_AudioProfile(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.AudioProfile");
    tolua_cclass(tolua_S,"AudioProfile","ax.AudioProfile","",nullptr);

    tolua_beginmodule(tolua_S,"AudioProfile");
        tolua_function(tolua_S,"new",lua_ax_audioengine_AudioProfile_constructor);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::AudioProfile).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.AudioProfile";
    g_typeCast[typeName] = "ax.AudioProfile";
    return 1;
}

int lua_ax_audioengine_AudioEngine_lazyInit(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_lazyInit'", nullptr);
            return 0;
        }
        auto&& ret = ax::AudioEngine::lazyInit();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:lazyInit",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_lazyInit'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_end(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_end'", nullptr);
            return 0;
        }
        ax::AudioEngine::end();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:end",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_end'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_getDefaultProfile(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_getDefaultProfile'", nullptr);
            return 0;
        }
        auto&& ret = ax::AudioEngine::getDefaultProfile();
        object_to_luaval<ax::AudioProfile>(tolua_S, "ax.AudioProfile",(ax::AudioProfile*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:getDefaultProfile",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_getDefaultProfile'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_play2d(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 2)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            ax::AudioPlayerSettings arg1;
            #pragma warning NO CONVERSION TO NATIVE FOR AudioPlayerSettings
		ok = false;
            if (!ok) { break; }
            int ret = ax::AudioEngine::play2d(arg0, arg1);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 3)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            ax::AudioPlayerSettings arg1;
            #pragma warning NO CONVERSION TO NATIVE FOR AudioPlayerSettings
		ok = false;
            if (!ok) { break; }
            const ax::AudioProfile* arg2;
            ok &= luaval_to_object<const ax::AudioProfile>(tolua_S, 4, "ax.AudioProfile",&arg2, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            int ret = ax::AudioEngine::play2d(arg0, arg1, arg2);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 1)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            int ret = ax::AudioEngine::play2d(arg0);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            int ret = ax::AudioEngine::play2d(arg0, arg1);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 3)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            double arg2;
            ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            int ret = ax::AudioEngine::play2d(arg0, arg1, arg2);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 4)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            bool arg1;
            ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            double arg2;
            ok &= luaval_to_number(tolua_S, 4,&arg2, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            const ax::AudioProfile* arg3;
            ok &= luaval_to_object<const ax::AudioProfile>(tolua_S, 5, "ax.AudioProfile",&arg3, "ax.AudioEngine:play2d");
            if (!ok) { break; }
            int ret = ax::AudioEngine::play2d(arg0, arg1, arg2, arg3);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.AudioEngine:play2d",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_play2d'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_setLoop(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        int arg0;
        bool arg1;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:setLoop");
        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ax.AudioEngine:setLoop");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_setLoop'", nullptr);
            return 0;
        }
        ax::AudioEngine::setLoop(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:setLoop",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_setLoop'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_isLoop(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:isLoop");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_isLoop'", nullptr);
            return 0;
        }
        auto&& ret = ax::AudioEngine::isLoop(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:isLoop",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_isLoop'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_setVolume(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        int arg0;
        double arg1;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:setVolume");
        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.AudioEngine:setVolume");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_setVolume'", nullptr);
            return 0;
        }
        ax::AudioEngine::setVolume(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:setVolume",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_setVolume'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_getVolume(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:getVolume");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_getVolume'", nullptr);
            return 0;
        }
        auto&& ret = ax::AudioEngine::getVolume(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:getVolume",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_getVolume'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_pause(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:pause");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_pause'", nullptr);
            return 0;
        }
        ax::AudioEngine::pause(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:pause",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_pause'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_pauseAll(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_pauseAll'", nullptr);
            return 0;
        }
        ax::AudioEngine::pauseAll();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:pauseAll",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_pauseAll'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_resume(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:resume");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_resume'", nullptr);
            return 0;
        }
        ax::AudioEngine::resume(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:resume",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_resume'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_resumeAll(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_resumeAll'", nullptr);
            return 0;
        }
        ax::AudioEngine::resumeAll();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:resumeAll",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_resumeAll'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_stop(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:stop");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_stop'", nullptr);
            return 0;
        }
        ax::AudioEngine::stop(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:stop",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_stop'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_stopAll(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_stopAll'", nullptr);
            return 0;
        }
        ax::AudioEngine::stopAll();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:stopAll",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_stopAll'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_setCurrentTime(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        int arg0;
        double arg1;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:setCurrentTime");
        ok &= luaval_to_number(tolua_S, 3,&arg1, "ax.AudioEngine:setCurrentTime");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_setCurrentTime'", nullptr);
            return 0;
        }
        auto&& ret = ax::AudioEngine::setCurrentTime(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:setCurrentTime",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_setCurrentTime'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_getCurrentTime(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:getCurrentTime");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_getCurrentTime'", nullptr);
            return 0;
        }
        auto&& ret = ax::AudioEngine::getCurrentTime(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:getCurrentTime",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_getCurrentTime'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_getDuration(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:getDuration");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_getDuration'", nullptr);
            return 0;
        }
        auto&& ret = ax::AudioEngine::getDuration(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:getDuration",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_getDuration'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_getState(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:getState");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_getState'", nullptr);
            return 0;
        }
        int ret = (int)ax::AudioEngine::getState(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:getState",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_getState'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_getMaxAudioInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_getMaxAudioInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::AudioEngine::getMaxAudioInstance();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:getMaxAudioInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_getMaxAudioInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_setMaxAudioInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:setMaxAudioInstance");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_setMaxAudioInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::AudioEngine::setMaxAudioInstance(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:setMaxAudioInstance",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_setMaxAudioInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_uncache(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        std::string_view arg0;
        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.AudioEngine:uncache");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_uncache'", nullptr);
            return 0;
        }
        ax::AudioEngine::uncache(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:uncache",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_uncache'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_uncacheAll(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_uncacheAll'", nullptr);
            return 0;
        }
        ax::AudioEngine::uncacheAll();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:uncacheAll",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_uncacheAll'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_getProfile(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 1)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.AudioEngine:getProfile");
            if (!ok) { break; }
            ax::AudioProfile* ret = ax::AudioEngine::getProfile(arg0);
            object_to_luaval<ax::AudioProfile>(tolua_S, "ax.AudioProfile",(ax::AudioProfile*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 1)
        {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ax.AudioEngine:getProfile");
            if (!ok) { break; }
            ax::AudioProfile* ret = ax::AudioEngine::getProfile(arg0);
            object_to_luaval<ax::AudioProfile>(tolua_S, "ax.AudioProfile",(ax::AudioProfile*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.AudioEngine:getProfile",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_getProfile'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_preload(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 2)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.AudioEngine:preload");
            if (!ok) { break; }
            std::function<void (bool)> arg1;
            do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
            if (!ok) { break; }
            ax::AudioEngine::preload(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 1)
        {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "ax.AudioEngine:preload");
            if (!ok) { break; }
            ax::AudioEngine::preload(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ax.AudioEngine:preload",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_preload'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_getPlayingAudioCount(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_getPlayingAudioCount'", nullptr);
            return 0;
        }
        auto&& ret = ax::AudioEngine::getPlayingAudioCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:getPlayingAudioCount",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_getPlayingAudioCount'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_setEnabled(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        bool arg0;
        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ax.AudioEngine:setEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_setEnabled'", nullptr);
            return 0;
        }
        ax::AudioEngine::setEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:setEnabled",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_setEnabled'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_audioengine_AudioEngine_isEnabled(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ax.AudioEngine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_audioengine_AudioEngine_isEnabled'", nullptr);
            return 0;
        }
        auto&& ret = ax::AudioEngine::isEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.AudioEngine:isEnabled",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_audioengine_AudioEngine_isEnabled'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_audioengine_AudioEngine_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (AudioEngine)");
    return 0;
}

int lua_register_ax_audioengine_AudioEngine(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ax.AudioEngine");
    tolua_cclass(tolua_S,"AudioEngine","ax.AudioEngine","",nullptr);

    tolua_beginmodule(tolua_S,"AudioEngine");
        tolua_function(tolua_S,"lazyInit", lua_ax_audioengine_AudioEngine_lazyInit);
        tolua_function(tolua_S,"endToLua", lua_ax_audioengine_AudioEngine_end);
        tolua_function(tolua_S,"getDefaultProfile", lua_ax_audioengine_AudioEngine_getDefaultProfile);
        tolua_function(tolua_S,"play2d", lua_ax_audioengine_AudioEngine_play2d);
        tolua_function(tolua_S,"setLoop", lua_ax_audioengine_AudioEngine_setLoop);
        tolua_function(tolua_S,"isLoop", lua_ax_audioengine_AudioEngine_isLoop);
        tolua_function(tolua_S,"setVolume", lua_ax_audioengine_AudioEngine_setVolume);
        tolua_function(tolua_S,"getVolume", lua_ax_audioengine_AudioEngine_getVolume);
        tolua_function(tolua_S,"pause", lua_ax_audioengine_AudioEngine_pause);
        tolua_function(tolua_S,"pauseAll", lua_ax_audioengine_AudioEngine_pauseAll);
        tolua_function(tolua_S,"resume", lua_ax_audioengine_AudioEngine_resume);
        tolua_function(tolua_S,"resumeAll", lua_ax_audioengine_AudioEngine_resumeAll);
        tolua_function(tolua_S,"stop", lua_ax_audioengine_AudioEngine_stop);
        tolua_function(tolua_S,"stopAll", lua_ax_audioengine_AudioEngine_stopAll);
        tolua_function(tolua_S,"setCurrentTime", lua_ax_audioengine_AudioEngine_setCurrentTime);
        tolua_function(tolua_S,"getCurrentTime", lua_ax_audioengine_AudioEngine_getCurrentTime);
        tolua_function(tolua_S,"getDuration", lua_ax_audioengine_AudioEngine_getDuration);
        tolua_function(tolua_S,"getState", lua_ax_audioengine_AudioEngine_getState);
        tolua_function(tolua_S,"getMaxAudioInstance", lua_ax_audioengine_AudioEngine_getMaxAudioInstance);
        tolua_function(tolua_S,"setMaxAudioInstance", lua_ax_audioengine_AudioEngine_setMaxAudioInstance);
        tolua_function(tolua_S,"uncache", lua_ax_audioengine_AudioEngine_uncache);
        tolua_function(tolua_S,"uncacheAll", lua_ax_audioengine_AudioEngine_uncacheAll);
        tolua_function(tolua_S,"getProfile", lua_ax_audioengine_AudioEngine_getProfile);
        tolua_function(tolua_S,"preload", lua_ax_audioengine_AudioEngine_preload);
        tolua_function(tolua_S,"getPlayingAudioCount", lua_ax_audioengine_AudioEngine_getPlayingAudioCount);
        tolua_function(tolua_S,"setEnabled", lua_ax_audioengine_AudioEngine_setEnabled);
        tolua_function(tolua_S,"isEnabled", lua_ax_audioengine_AudioEngine_isEnabled);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::AudioEngine).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.AudioEngine";
    g_typeCast[typeName] = "ax.AudioEngine";
    return 1;
}
TOLUA_API int register_all_ax_audioengine(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"ax",0);
	tolua_beginmodule(tolua_S,"ax");

	lua_register_ax_audioengine_AudioProfile(tolua_S);
	lua_register_ax_audioengine_AudioEngine(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

#endif
