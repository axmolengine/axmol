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
#include "lua-bindings/manual/3d/axlua_3d_manual.h"
#include "lua-bindings/auto/axlua_3d_auto.hpp"
#include "lua-bindings/manual/LuaBasicConversions.h"
#include "lua-bindings/manual/LuaEngine.h"
#include "3d/Bundle3D.h"
#include "3d/MeshRenderer.h"
#include "3d/Terrain.h"

int axlua_3d_MeshRenderer_getAABB(lua_State* L)
{
    int argc                = 0;
    ax::MeshRenderer* cobj = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.MeshRenderer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::MeshRenderer*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_MeshRenderer_getAABB'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 0)
    {
        ax::AABB* ret = const_cast<ax::AABB*>(&(cobj->getAABB()));
        object_to_luaval<ax::AABB>(L, "ax.AABB", (ax::AABB*)ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.MeshRenderer:getAABB", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_MeshRenderer_getAABB'.", &tolua_err);
#endif
    return 0;
}

int axlua_3d_MeshRenderer_createAsync(lua_State* L)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(L, 1, "ax.MeshRenderer", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(L) - 1;

    do
    {
        if (argc == 3)
        {
            std::string modelPath;
            ok &= luaval_to_std_string(L, 2, &modelPath, "ax.MeshRenderer:createAsync");
            if (!ok)
                break;
            std::string texturePath;
            ok &= luaval_to_std_string(L, 3, &texturePath, "ax.MeshRenderer:createAsync");
            if (!ok)
                break;

#if _AX_DEBUG >= 1
            if (!toluafix_isfunction(L, 4, "LUA_FUNCTION", 0, &tolua_err))
            {
                goto tolua_lerror;
            }
#endif
            LUA_FUNCTION handler = toluafix_ref_function(L, 4, 0);

            ax::MeshRenderer::createAsync(
                modelPath, texturePath,
                [=](ax::MeshRenderer* mesh, void* callbackparam) {
                    auto stack = LuaEngine::getInstance()->getLuaStack();
                    int id     = (mesh) ? (int)mesh->_ID : -1;
                    int* luaID = (mesh) ? &mesh->_luaID : nullptr;
                    toluafix_pushusertype_object(stack->getLuaState(), id, luaID, (void*)mesh, "ax.MeshRenderer");
                    stack->executeFunctionByHandler(handler, 1);
                },
                nullptr);

            lua_settop(L, 1);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 2)
        {
            std::string modelPath;
            ok &= luaval_to_std_string(L, 2, &modelPath, "ax.MeshRenderer:createAsync");
            if (!ok)
                break;

#if _AX_DEBUG >= 1
            if (!toluafix_isfunction(L, 3, "LUA_FUNCTION", 0, &tolua_err))
            {
                goto tolua_lerror;
            }
#endif
            LUA_FUNCTION handler = toluafix_ref_function(L, 3, 0);

            ax::MeshRenderer::createAsync(
                modelPath,
                [=](ax::MeshRenderer* mesh, void* callbackparam) {
                    auto stack = LuaEngine::getInstance()->getLuaStack();
                    int id     = (mesh) ? (int)mesh->_ID : -1;
                    int* luaID = (mesh) ? &mesh->_luaID : nullptr;
                    toluafix_pushusertype_object(stack->getLuaState(), id, luaID, (void*)mesh, "ax.MeshRenderer");
                    stack->executeFunctionByHandler(handler, 1);
                },
                nullptr);

            lua_settop(L, 1);
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d", "ax.MeshRenderer:createAsync", argc, 3);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_MeshRenderer_createAsync'.", &tolua_err);
#endif
    return 0;
}

static void extendMeshRenderer(lua_State* L)
{
    lua_pushstring(L, "ax.MeshRenderer");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "getAABB", axlua_3d_MeshRenderer_getAABB);
        tolua_function(L, "createAsync", axlua_3d_MeshRenderer_createAsync);
    }
    lua_pop(L, 1);
}

bool luaval_to_terraindata(lua_State* L, int lo, ax::Terrain::TerrainData* outValue, const char* funcName = "")
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;
    tolua_Error tolua_err;
    if (!tolua_istable(L, lo, 0, &tolua_err))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &tolua_err, funcName);
        ok = false;
#endif
    }

    if (ok)
    {
        lua_pushstring(L, "_chunkSize");
        lua_gettable(L, lo);
        if (!lua_isnil(L, lua_gettop(L)))
        {
            luaval_to_size(L, lua_gettop(L), &(outValue->_chunkSize));
        }
        else
        {
            outValue->_chunkSize = ax::Size(32, 32);
        }
        lua_pop(L, 1);

        lua_pushstring(L, "_heightMapSrc");
        lua_gettable(L, lo);
        outValue->_heightMapSrc = tolua_tocppstring(L, -1, "");
        lua_pop(L, 1);

        lua_pushstring(L, "_alphaMapSrc");
        lua_gettable(L, lo);
        outValue->_alphaMapSrc = const_cast<char*>(tolua_tocppstring(L, -1, ""));
        lua_pop(L, 1);

        lua_pushstring(L, "_detailMaps");
        lua_gettable(L, lo);
        if (lua_istable(L, -1))
        {
            size_t len = lua_objlen(L, -1);
            for (size_t i = 0; i < len; i++)
            {
                lua_pushnumber(L, i + 1);
                lua_gettable(L, -2);
                if (lua_istable(L, -1))
                {
                    lua_pushstring(L, "_detailMapSrc");
                    lua_gettable(L, -2);
                    outValue->_detailMaps[i]._detailMapSrc = tolua_tocppstring(L, -1, "");
                    lua_pop(L, 1);

                    lua_pushstring(L, "_detailMapSize");
                    lua_gettable(L, -2);
                    outValue->_detailMaps[i]._detailMapSize = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
                    lua_pop(L, 1);
                }
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);

        lua_pushstring(L, "_mapHeight");
        lua_gettable(L, lo);
        outValue->_mapHeight = lua_isnil(L, -1) ? 2.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "_mapScale");
        lua_gettable(L, lo);
        outValue->_mapScale = lua_isnil(L, -1) ? 0.1f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "_detailMapAmount");
        lua_gettable(L, lo);
        outValue->_detailMapAmount = lua_isnil(L, -1) ? 0 : (int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_pushstring(L, "_skirtHeightRatio");
        lua_gettable(L, lo);
        outValue->_skirtHeightRatio = lua_isnil(L, -1) ? 1.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    return ok;
}

void terraindata_to_luaval(lua_State* L, const ax::Terrain::TerrainData& inValue)
{
    if (nullptr == L)
        return;

    lua_newtable(L);
    lua_pushstring(L, "_chunkSize");
    size_to_luaval(L, inValue._chunkSize);
    lua_rawset(L, -3);

    if (inValue._heightMapSrc.length() > 0)
    {
        lua_pushstring(L, "_heightMapSrc");
        lua_pushstring(L, inValue._heightMapSrc.c_str());
        lua_rawset(L, -3);
    }

    if (!inValue._alphaMapSrc.empty())
    {
        lua_pushstring(L, "_alphaMapSrc");
        lua_pushstring(L, inValue._alphaMapSrc.c_str());
        lua_rawset(L, -3);
    }

    lua_pushstring(L, "_detailMaps");
    lua_newtable(L);
    for (int i = 0; i < 4; i++)
    {

        lua_pushnumber(L, (lua_Number)i + 1);
        lua_newtable(L);

        lua_pushstring(L, "_detailMapSrc");
        lua_pushstring(L, inValue._detailMaps[i]._detailMapSrc.c_str());
        lua_rawset(L, -3);

        lua_pushstring(L, "_detailMapSize");
        lua_pushnumber(L, (lua_Number)inValue._detailMaps[i]._detailMapSize);
        lua_rawset(L, -3);

        lua_rawset(L, -3);
    }
    lua_rawset(L, -3);

    lua_pushstring(L, "_mapHeight");
    lua_pushnumber(L, (lua_Number)inValue._mapHeight);
    lua_rawset(L, -3);

    lua_pushstring(L, "_mapScale");
    lua_pushnumber(L, (lua_Number)inValue._mapScale);
    lua_rawset(L, -3);

    lua_pushstring(L, "_detailMapAmount");
    lua_pushnumber(L, (lua_Number)inValue._detailMapAmount);
    lua_rawset(L, -3);

    lua_pushstring(L, "_skirtHeightRatio");
    lua_pushnumber(L, (lua_Number)inValue._skirtHeightRatio);
    lua_rawset(L, -3);
}

int axlua_3d_Terrain_create(lua_State* L)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(L, 1, "ax.Terrain", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 1)
    {
        ax::Terrain::TerrainData arg0;
        ok &= luaval_to_terraindata(L, 2, &arg0);
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'axlua_3d_Terrain_create'", nullptr);
            return 0;
        }
        ax::Terrain* ret = ax::Terrain::create(arg0);
        object_to_luaval<ax::Terrain>(L, "ax.Terrain", (ax::Terrain*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::Terrain::TerrainData arg0;
        ax::Terrain::CrackFixedType arg1;

        ok &= luaval_to_terraindata(L, 2, &arg0);
        ok &= luaval_to_int32(L, 3, (int*)&arg1, "ax.Terrain:create");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'axlua_3d_Terrain_create'", nullptr);
            return 0;
        }
        ax::Terrain* ret = ax::Terrain::create(arg0, arg1);
        object_to_luaval<ax::Terrain>(L, "ax.Terrain", (ax::Terrain*)ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Terrain:create", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_Terrain_create'.", &tolua_err);
#endif
    return 0;
}

int axlua_3d_Terrain_getHeight(lua_State* L)
{
    int argc               = 0;
    ax::Terrain* cobj = nullptr;
    bool ok                = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.Terrain", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::Terrain*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_Terrain_getHeight'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    do
    {
        if (argc == 1)
        {
            ax::Vec2 arg0;
            ok &= luaval_to_vec2(L, 2, &arg0, "ax.Terrain:getHeight");

            if (!ok)
            {
                break;
            }
            double ret = cobj->getHeight(arg0);
            tolua_pushnumber(L, (lua_Number)ret);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 2)
        {
            ax::Vec2 arg0;
            ok &= luaval_to_vec2(L, 2, &arg0, "ax.Terrain:getHeight");

            if (!ok)
            {
                break;
            }
            ax::Vec3* arg1;
            ok &= luaval_to_object<ax::Vec3>(L, 3, "ax.Vec3", &arg1, "ax.Terrain:getHeight");

            if (!ok)
            {
                break;
            }
            double ret = cobj->getHeight(arg0, arg1);
            tolua_pushnumber(L, (lua_Number)ret);
            vec3_to_luaval(L, *arg1);
            return 2;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 2)
        {
            double arg0;
            ok &= luaval_to_number(L, 2, &arg0, "ax.Terrain:getHeight");

            if (!ok)
            {
                break;
            }
            double arg1;
            ok &= luaval_to_number(L, 3, &arg1, "ax.Terrain:getHeight");

            if (!ok)
            {
                break;
            }
            double ret = cobj->getHeight(arg0, arg1);
            tolua_pushnumber(L, (lua_Number)ret);
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 3)
        {
            double arg0;
            ok &= luaval_to_number(L, 2, &arg0, "ax.Terrain:getHeight");

            if (!ok)
            {
                break;
            }
            double arg1;
            ok &= luaval_to_number(L, 3, &arg1, "ax.Terrain:getHeight");

            if (!ok)
            {
                break;
            }
            ax::Vec3 arg2;
            ok &= luaval_to_vec3(L, 4, &arg2);

            if (!ok)
            {
                break;
            }
            double ret = cobj->getHeight(arg0, arg1, &arg2);
            tolua_pushnumber(L, (lua_Number)ret);
            vec3_to_luaval(L, arg2);
            return 2;
        }
    } while (0);
    ok = true;
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Terrain:getHeight", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_Terrain_getHeight'.", &tolua_err);
#endif

    return 0;
}

int axlua_3d_Terrain_getIntersectionPoint(lua_State* tolua_S)
{
    int argc               = 0;
    ax::Terrain* cobj = nullptr;
    bool ok                = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S, 1, "ax.Terrain", 0, &tolua_err))
        goto tolua_lerror;
#endif
    cobj = (ax::Terrain*)tolua_tousertype(tolua_S, 1, 0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S, "invalid 'cobj' in function 'axlua_3d_Terrain_getIntersectionPoint'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S) - 1;
    do
    {
        if (argc == 2)
        {
            ax::Ray* arg0 = nullptr;
            ok &= luaval_to_object<ax::Ray>(tolua_S, 2, "ax.Ray", &arg0, "ax.Terrain:getIntersectionPoint");

            if (!ok)
            {
                break;
            }
            ax::Vec3 arg1;
            ok &= luaval_to_vec3(tolua_S, 3, &arg1, "ax.Terrain:getIntersectionPoint");

            if (!ok)
            {
                break;
            }
            bool ret = cobj->getIntersectionPoint(*arg0, arg1);
            tolua_pushboolean(tolua_S, (bool)ret);
            vec3_to_luaval(tolua_S, arg1);
            return 2;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 1)
        {
            ax::Ray* arg0;
            ok &= luaval_to_object<ax::Ray>(tolua_S, 2, "ax.Ray", &arg0, "ax.Terrain:getIntersectionPoint");

            if (!ok)
            {
                break;
            }
            ax::Vec3 ret = cobj->getIntersectionPoint(*arg0);
            vec3_to_luaval(tolua_S, ret);
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Terrain:getIntersectionPoint",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S, "#ferror in function 'axlua_3d_Terrain_getIntersectionPoint'.", &tolua_err);
#endif

    return 0;
}

static void extendTerrain(lua_State* L)
{
    lua_pushstring(L, "ax.Terrain");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "create", axlua_3d_Terrain_create);
        tolua_function(L, "getHeight", axlua_3d_Terrain_getHeight);
        tolua_function(L, "getIntersectionPoint", axlua_3d_Terrain_getIntersectionPoint);
    }
    lua_pop(L, 1);
}

int axlua_3d_Bundle3D_getTrianglesList(lua_State* L)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(L, 1, "ax.Bundle3D", 0, &tolua_err))
        goto tolua_lerror;
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(L, 2, &arg0, "ax.Bundle3D:getTrianglesList");
        if (!ok)
        {
            tolua_error(L, "invalid arguments in function 'axlua_3d_Bundle3D_getTrianglesList'", nullptr);
            return 0;
        }

        std::vector<ax::Vec3> ret = ax::Bundle3D::getTrianglesList(arg0);
        std_vector_vec3_to_luaval(L, ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Bundle3D:getTrianglesList", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_Bundle3D_getTrianglesList'.", &tolua_err);
#endif
    return 0;
}

void extendBundle3D(lua_State* L)
{
    lua_pushstring(L, "ax.Bundle3D");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        tolua_function(L, "getTrianglesList", axlua_3d_Bundle3D_getTrianglesList);
    }
    lua_pop(L, 1);
}

static int register_all_ax_3d_manual(lua_State* L)
{
    if (nullptr == L)
        return 0;

    extendMeshRenderer(L);
    extendTerrain(L);
    extendBundle3D(L);
    return 0;
}

int axlua_3d_AABB_reset(lua_State* L)
{
    int argc            = 0;
    ax::AABB* cobj = nullptr;
    bool ok             = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.AABB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::AABB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_AABB_reset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 0)
    {
        if (!ok)
            return 0;
        cobj->reset();
        lua_settop(L, 1);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AABB:reset", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_AABB_reset'.", &tolua_err);
#endif

    return 0;
}
int axlua_3d_AABB_set(lua_State* L)
{
    int argc            = 0;
    ax::AABB* cobj = nullptr;
    bool ok             = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.AABB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::AABB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_AABB_set'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 2)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;

        ok &= luaval_to_vec3(L, 2, &arg0, "ax.AABB:set");

        ok &= luaval_to_vec3(L, 3, &arg1, "ax.AABB:set");
        if (!ok)
            return 0;
        cobj->set(arg0, arg1);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AABB:set", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_AABB_set'.", &tolua_err);
#endif

    return 0;
}
int axlua_3d_AABB_transform(lua_State* L)
{
    int argc            = 0;
    ax::AABB* cobj = nullptr;
    bool ok             = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.AABB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::AABB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_AABB_transform'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 1)
    {
        ax::Mat4 arg0;

        ok &= luaval_to_mat4(L, 2, &arg0, "ax.AABB:transform");
        if (!ok)
            return 0;
        cobj->transform(arg0);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AABB:transform", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_AABB_transform'.", &tolua_err);
#endif

    return 0;
}
int axlua_3d_AABB_getCenter(lua_State* L)
{
    int argc            = 0;
    ax::AABB* cobj = nullptr;
    bool ok             = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.AABB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::AABB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_AABB_getCenter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 0)
    {
        if (!ok)
            return 0;
        ax::Vec3 ret = cobj->getCenter();
        vec3_to_luaval(L, ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AABB:getCenter", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_AABB_getCenter'.", &tolua_err);
#endif

    return 0;
}
int axlua_3d_AABB_isEmpty(lua_State* L)
{
    int argc            = 0;
    ax::AABB* cobj = nullptr;
    bool ok             = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.AABB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::AABB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_AABB_isEmpty'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 0)
    {
        if (!ok)
            return 0;
        bool ret = cobj->isEmpty();
        tolua_pushboolean(L, (bool)ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AABB:isEmpty", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_AABB_isEmpty'.", &tolua_err);
#endif

    return 0;
}
int axlua_3d_AABB_getCorners(lua_State* L)
{
    int argc            = 0;
    ax::AABB* cobj = nullptr;
    bool ok             = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.AABB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::AABB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_AABB_getCorners'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 1)
    {
        ax::Vec3* arg0;

        ok &= luaval_to_object<ax::Vec3>(L, 2, "ax.Vec3", &arg0, "ax.AABB:getCorners");
        if (!ok)
            return 0;
        cobj->getCorners(arg0);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AABB:getCorners", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_AABB_getCorners'.", &tolua_err);
#endif

    return 0;
}
int axlua_3d_AABB_updateMinMax(lua_State* L)
{
    int argc            = 0;
    ax::AABB* cobj = nullptr;
    bool ok             = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.AABB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::AABB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_AABB_updateMinMax'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 2)
    {
        const ax::Vec3* arg0;
        ssize_t arg1;

        ok &= luaval_to_object<const ax::Vec3>(L, 2, "ax.Vec3", &arg0, "ax.AABB:updateMinMax");

        ok &= luaval_to_ssize_t(L, 3, &arg1, "ax.AABB:updateMinMax");
        if (!ok)
            return 0;
        cobj->updateMinMax(arg0, arg1);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AABB:updateMinMax", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_AABB_updateMinMax'.", &tolua_err);
#endif

    return 0;
}
int axlua_3d_AABB_containPoint(lua_State* L)
{
    int argc            = 0;
    ax::AABB* cobj = nullptr;
    bool ok             = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.AABB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::AABB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_AABB_containPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(L, 2, &arg0, "ax.AABB:containPoint");
        if (!ok)
            return 0;
        bool ret = cobj->containPoint(arg0);
        tolua_pushboolean(L, (bool)ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AABB:containPoint", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_AABB_containPoint'.", &tolua_err);
#endif

    return 0;
}
int axlua_3d_AABB_constructor(lua_State* L)
{
    int argc            = 0;
    ax::AABB* cobj = nullptr;
    bool ok             = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

    argc = lua_gettop(L) - 1;
    do
    {
        if (argc == 2)
        {
            ax::Vec3 arg0;
            ok &= luaval_to_vec3(L, 2, &arg0, "ax.AABB:AABB");

            if (!ok)
            {
                break;
            }
            ax::Vec3 arg1;
            ok &= luaval_to_vec3(L, 3, &arg1, "ax.AABB:AABB");

            if (!ok)
            {
                break;
            }
            cobj = new ax::AABB(arg0, arg1);
            tolua_pushusertype(L, (void*)cobj, "ax.AABB");
            tolua_register_gc(L, lua_gettop(L));
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 0)
        {
            cobj = new ax::AABB();
            tolua_pushusertype(L, (void*)cobj, "ax.AABB");
            tolua_register_gc(L, lua_gettop(L));
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AABB:AABB", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(L, "#ferror in function 'axlua_3d_AABB_constructor'.", &tolua_err);
#endif

    return 0;
}

int axlua_3d_get_AABB_min(lua_State* L)
{
    ax::AABB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.AABB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::AABB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_get_AABB_min'\n", nullptr);
        return 0;
    }
#endif
    vec3_to_luaval(L, self->_min);
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_get_AABB_min'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_set_AABB_min(lua_State* L)
{
    int argc            = 0;
    ax::AABB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.AABB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::AABB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_set_AABB_min'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        luaval_to_vec3(L, 2, &self->_min);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_set_AABB_min'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_get_AABB_max(lua_State* L)
{
    ax::AABB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.AABB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::AABB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_get_AABB_max'\n", nullptr);
        return 0;
    }
#endif
    vec3_to_luaval(L, self->_max);
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_get_AABB_max'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_set_AABB_max(lua_State* L)
{
    int argc            = 0;
    ax::AABB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.AABB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::AABB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_set_AABB_max'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        luaval_to_vec3(L, 2, &self->_max);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_set_Ray_direction'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_AABB_finalize(lua_State* L)
{
    ax::AABB* self = (ax::AABB*)tolua_tousertype(L, 1, 0);
    AX_SAFE_DELETE(self);
    return 0;
}

int lua_register_axis_3d_AABB(lua_State* L)
{
    tolua_usertype(L, "ax.AABB");
    tolua_cclass(L, "AABB", "ax.AABB", "", axlua_3d_AABB_finalize);

    tolua_beginmodule(L, "AABB");
    tolua_variable(L, "_min", axlua_3d_get_AABB_min, axlua_3d_set_AABB_min);
    tolua_variable(L, "_max", axlua_3d_get_AABB_max, axlua_3d_set_AABB_max);
    tolua_function(L, "new", axlua_3d_AABB_constructor);
    tolua_function(L, "reset", axlua_3d_AABB_reset);
    tolua_function(L, "set", axlua_3d_AABB_set);
    tolua_function(L, "transform", axlua_3d_AABB_transform);
    tolua_function(L, "getCenter", axlua_3d_AABB_getCenter);
    tolua_function(L, "isEmpty", axlua_3d_AABB_isEmpty);
    tolua_function(L, "getCorners", axlua_3d_AABB_getCorners);
    tolua_function(L, "updateMinMax", axlua_3d_AABB_updateMinMax);
    tolua_function(L, "containPoint", axlua_3d_AABB_containPoint);
    tolua_endmodule(L);
    auto typeName                                    = typeid(ax::AABB).name();
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.AABB";
    g_typeCast[typeName]                             = "ax.AABB";
    return 1;
}

int axlua_3d_OBB_reset(lua_State* L)
{
    int argc           = 0;
    ax::OBB* cobj = nullptr;
    bool ok            = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::OBB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_OBB_reset'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 0)
    {
        if (!ok)
            return 0;
        cobj->reset();
        lua_settop(L, 1);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.OBB:reset", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_OBB_reset'.", &tolua_err);
#endif

    return 0;
}
int axlua_3d_OBB_set(lua_State* L)
{
    int argc           = 0;
    ax::OBB* cobj = nullptr;
    bool ok            = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::OBB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_OBB_set'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 5)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;
        ax::Vec3 arg2;
        ax::Vec3 arg3;
        ax::Vec3 arg4;

        ok &= luaval_to_vec3(L, 2, &arg0, "ax.OBB:set");

        ok &= luaval_to_vec3(L, 3, &arg1, "ax.OBB:set");

        ok &= luaval_to_vec3(L, 4, &arg2, "ax.OBB:set");

        ok &= luaval_to_vec3(L, 5, &arg3, "ax.OBB:set");

        ok &= luaval_to_vec3(L, 6, &arg4, "ax.OBB:set");
        if (!ok)
            return 0;
        cobj->set(arg0, arg1, arg2, arg3, arg4);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.OBB:set", argc, 5);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_OBB_set'.", &tolua_err);
#endif

    return 0;
}
int axlua_3d_OBB_transform(lua_State* L)
{
    int argc           = 0;
    ax::OBB* cobj = nullptr;
    bool ok            = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::OBB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_OBB_transform'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 1)
    {
        ax::Mat4 arg0;

        ok &= luaval_to_mat4(L, 2, &arg0, "ax.OBB:transform");
        if (!ok)
            return 0;
        cobj->transform(arg0);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.OBB:transform", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_OBB_transform'.", &tolua_err);
#endif

    return 0;
}

int axlua_3d_OBB_containPoint(lua_State* L)
{
    int argc           = 0;
    ax::OBB* cobj = nullptr;
    bool ok            = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::OBB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_OBB_containPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 1)
    {
        ax::Vec3 arg0;

        ok &= luaval_to_vec3(L, 2, &arg0, "ax.OBB:containPoint");
        if (!ok)
            return 0;
        bool ret = cobj->containPoint(arg0);
        tolua_pushboolean(L, (bool)ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.OBB:containPoint", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_OBB_containPoint'.", &tolua_err);
#endif

    return 0;
}

int axlua_3d_OBB_constructor(lua_State* L)
{
    int argc           = 0;
    ax::OBB* cobj = nullptr;
    bool ok            = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

    argc = lua_gettop(L) - 1;
    do
    {
        if (argc == 1)
        {
            ax::AABB* arg0;
            ok &= luaval_to_object<ax::AABB>(L, 2, "ax.AABB", &arg0, "ax.OBB:OBB");

            if (!ok)
            {
                break;
            }
            cobj = new ax::OBB(*arg0);
            tolua_pushusertype(L, (void*)cobj, "ax.OBB");
            tolua_register_gc(L, lua_gettop(L));
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 0)
        {
            cobj = new ax::OBB();
            tolua_pushusertype(L, (void*)cobj, "ax.OBB");
            tolua_register_gc(L, lua_gettop(L));
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 2)
        {
            const ax::Vec3* arg0;
            ok &= luaval_to_object<const ax::Vec3>(L, 2, "ax.Vec3", &arg0, "ax.OBB:OBB");

            if (!ok)
            {
                break;
            }
            int arg1;
            ok &= luaval_to_int32(L, 3, (int*)&arg1, "ax.OBB:OBB");

            if (!ok)
            {
                break;
            }
            cobj = new ax::OBB(arg0, arg1);
            tolua_pushusertype(L, (void*)cobj, "ax.OBB");
            tolua_register_gc(L, lua_gettop(L));
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.OBB:OBB", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(L, "#ferror in function 'axlua_3d_OBB_constructor'.", &tolua_err);
#endif

    return 0;
}

int axlua_3d_OBB_intersects(lua_State* L)
{
    int argc           = 0;
    ax::OBB* self = nullptr;
    bool ok            = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::OBB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_OBB_intersects'\n", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
        ax::OBB* arg0;
        ok &= luaval_to_object<ax::OBB>(L, 2, "ax.OBB", &arg0, "ax.OBB:intersects");

        if (!ok)
            return 0;

        bool ret = self->intersects(*arg0);
        tolua_pushboolean(L, ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.OBB:intersects", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_OBB_intersects'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_get_OBB_center(lua_State* L)
{
    ax::OBB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::OBB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_get_OBB_center'\n", nullptr);
        return 0;
    }
#endif
    vec3_to_luaval(L, self->_center);
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_get_OBB_center'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_set_OBB_center(lua_State* L)
{
    int argc           = 0;
    ax::OBB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::OBB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_set_OBB_center'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        luaval_to_vec3(L, 2, &self->_center);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_set_OBB_center'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_get_OBB_xAxis(lua_State* L)
{
    ax::OBB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::OBB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_get_OBB_xAxis'\n", nullptr);
        return 0;
    }
#endif
    vec3_to_luaval(L, self->_xAxis);
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_get_OBB_xAxis'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_set_OBB_xAxis(lua_State* L)
{
    int argc           = 0;
    ax::OBB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::OBB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_set_OBB_xAxis'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        luaval_to_vec3(L, 2, &self->_xAxis);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_set_OBB_center'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_get_OBB_yAxis(lua_State* L)
{
    ax::OBB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::OBB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_get_OBB_yAxis'\n", nullptr);
        return 0;
    }
#endif
    vec3_to_luaval(L, self->_yAxis);
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_get_OBB_yAxis'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_set_OBB_yAxis(lua_State* L)
{
    int argc           = 0;
    ax::OBB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::OBB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_set_OBB_yAxis'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        luaval_to_vec3(L, 2, &self->_yAxis);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_set_OBB_yAxis'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_get_OBB_zAxis(lua_State* L)
{
    ax::OBB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::OBB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_get_OBB_zAxis'\n", nullptr);
        return 0;
    }
#endif
    vec3_to_luaval(L, self->_zAxis);
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_get_OBB_zAxis'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_set_OBB_zAxis(lua_State* L)
{
    int argc           = 0;
    ax::OBB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::OBB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_set_OBB_zAxis'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        luaval_to_vec3(L, 2, &self->_zAxis);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_set_OBB_zAxis'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_get_OBB_extents(lua_State* L)
{
    ax::OBB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::OBB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_get_OBB_extents'\n", nullptr);
        return 0;
    }
#endif
    vec3_to_luaval(L, self->_extents);
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_get_OBB_extents'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_set_OBB_extents(lua_State* L)
{
    int argc           = 0;
    ax::OBB* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::OBB*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_set_OBB_extents'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        luaval_to_vec3(L, 2, &self->_extents);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_set_OBB_extents'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_OBB_getCorners(lua_State* L)
{
    int argc           = 0;
    ax::OBB* cobj = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.OBB", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::OBB*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_OBB_getCorners'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 1)
    {
        ax::Vec3* arg0;
#if _AX_DEBUG >= 1
        if (!tolua_istable(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif

        size_t len = lua_objlen(L, 2);
        if (len == 0)
        {
            luaL_error(L, "Table's len equal 0");
            return 0;
        }

        arg0 = new ax::Vec3[len];
        for (int i = 1; i <= len; i++)
        {
            lua_pushnumber(L, i);
            lua_gettable(L, 2);
            if (lua_isnil(L, -1))
            {
                arg0[i - 1] = ax::Vec3(0, 0, 0);
            }
            else
            {
                luaval_to_vec3(L, -1, &arg0[i - 1], "ax.OBB:getCorners");
            }
            lua_pop(L, 1);
        }

        cobj->getCorners(arg0);

        lua_newtable(L);

        for (int i = 1; i <= len; i++)
        {
            lua_pushnumber(L, i);
            vec3_to_luaval(L, arg0[i - 1]);
            lua_rawset(L, -3);
        }
        AX_SAFE_DELETE_ARRAY(arg0);

        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.OBB:getCorners", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_OBB_getCorners'.", &tolua_err);
#endif

    return 0;
}

int axlua_3d_OBB_finalize(lua_State* L)
{
    ax::OBB* self = (ax::OBB*)tolua_tousertype(L, 1, 0);
    AX_SAFE_DELETE(self);
    return 0;
}

int lua_register_axis_3d_OBB(lua_State* L)
{
    tolua_usertype(L, "ax.OBB");
    tolua_cclass(L, "OBB", "ax.OBB", "", axlua_3d_OBB_finalize);

    tolua_beginmodule(L, "OBB");
    tolua_variable(L, "_center", axlua_3d_get_OBB_center, axlua_3d_set_OBB_center);
    tolua_variable(L, "_xAxis", axlua_3d_get_OBB_xAxis, axlua_3d_set_OBB_xAxis);
    tolua_variable(L, "_yAxis", axlua_3d_get_OBB_yAxis, axlua_3d_set_OBB_yAxis);
    tolua_variable(L, "_zAxis", axlua_3d_get_OBB_zAxis, axlua_3d_set_OBB_zAxis);
    tolua_variable(L, "_extents", axlua_3d_get_OBB_extents, axlua_3d_set_OBB_extents);
    tolua_function(L, "new", axlua_3d_OBB_constructor);
    tolua_function(L, "reset", axlua_3d_OBB_reset);
    tolua_function(L, "set", axlua_3d_OBB_set);
    tolua_function(L, "transform", axlua_3d_OBB_transform);
    tolua_function(L, "containPoint", axlua_3d_OBB_containPoint);
    tolua_function(L, "intersects", axlua_3d_OBB_intersects);
    tolua_function(L, "getCorners", axlua_3d_OBB_getCorners);
    tolua_endmodule(L);
    auto typeName                                    = typeid(ax::OBB).name();
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.OBB";
    g_typeCast[typeName]                             = "ax.OBB";
    return 1;
}

int axlua_3d_Ray_set(lua_State* L)
{
    int argc           = 0;
    ax::Ray* cobj = nullptr;
    bool ok            = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.Ray", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::Ray*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_Ray_set'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 2)
    {
        ax::Vec3 arg0;
        ax::Vec3 arg1;

        ok &= luaval_to_vec3(L, 2, &arg0, "ax.Ray:set");

        ok &= luaval_to_vec3(L, 3, &arg1, "ax.Ray:set");
        if (!ok)
            return 0;
        cobj->set(arg0, arg1);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Ray:set", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_Ray_set'.", &tolua_err);
#endif

    return 0;
}
int axlua_3d_Ray_transform(lua_State* L)
{
    int argc           = 0;
    ax::Ray* cobj = nullptr;
    bool ok            = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(L, 1, "ax.Ray", 0, &tolua_err))
        goto tolua_lerror;
#endif

    cobj = (ax::Ray*)tolua_tousertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(L, "invalid 'cobj' in function 'axlua_3d_Ray_transform'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 1)
    {
        ax::Mat4 arg0;

        ok &= luaval_to_mat4(L, 2, &arg0, "ax.Ray:transform");
        if (!ok)
            return 0;
        cobj->transform(arg0);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Ray:transform", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_Ray_transform'.", &tolua_err);
#endif

    return 0;
}

int axlua_3d_Ray_intersects(lua_State* L)
{
    int argc           = 0;
    ax::Ray* self = nullptr;
    bool ok            = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Ray", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::Ray*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_Ray_intersects'\n", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(L) - 1;
    if (1 == argc)
    {
        ax::OBB* arg0 = nullptr;
        ok &= luaval_to_object<ax::OBB>(L, 2, "ax.OBB", &arg0, "ax.Ray:intersects");

        if (!ok)
            return 0;

        float distance;
        bool ret = self->intersects(*arg0, &distance);
        tolua_pushboolean(L, ret);
        tolua_pushnumber(L, (lua_Number)distance);
        return 2;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Ray:intersects", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_Ray_intersects'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_Ray_constructor(lua_State* L)
{
    int argc           = 0;
    ax::Ray* cobj = nullptr;
    bool ok            = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

    argc = lua_gettop(L) - 1;
    do
    {
        if (argc == 2)
        {
            ax::Vec3 arg0;
            ok &= luaval_to_vec3(L, 2, &arg0, "ax.Ray:Ray");

            if (!ok)
            {
                break;
            }
            ax::Vec3 arg1;
            ok &= luaval_to_vec3(L, 3, &arg1, "ax.Ray:Ray");

            if (!ok)
            {
                break;
            }
            cobj = new ax::Ray(arg0, arg1);
            tolua_pushusertype(L, (void*)cobj, "ax.Ray");
            tolua_register_gc(L, lua_gettop(L));
            return 1;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 0)
        {
            cobj = new ax::Ray();
            tolua_pushusertype(L, (void*)cobj, "ax.Ray");
            tolua_register_gc(L, lua_gettop(L));
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Ray:Ray", argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(L, "#ferror in function 'axlua_3d_Ray_constructor'.", &tolua_err);
#endif

    return 0;
}

int axlua_3d_get_Ray_origin(lua_State* L)
{
    ax::Ray* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Ray", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::Ray*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_get_Ray_origin'\n", nullptr);
        return 0;
    }
#endif
    vec3_to_luaval(L, self->_origin);
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_get_Ray_origin'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_set_Ray_origin(lua_State* L)
{
    int argc           = 0;
    ax::Ray* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Ray", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::Ray*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_set_Ray_origin'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        luaval_to_vec3(L, 2, &self->_origin);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_set_Ray_origin'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_get_Ray_direction(lua_State* L)
{
    ax::Ray* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Ray", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::Ray*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_get_Ray_direction'\n", nullptr);
        return 0;
    }
#endif
    vec3_to_luaval(L, self->_direction);
    return 1;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_get_Ray_direction'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_set_Ray_direction(lua_State* L)
{
    int argc           = 0;
    ax::Ray* self = nullptr;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(L, 1, "ax.Ray", 0, &tolua_err))
        goto tolua_lerror;
#endif

    self = (ax::Ray*)tolua_tousertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        tolua_error(L, "invalid 'self' in function 'axlua_3d_set_Ray_direction'\n", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!tolua_istable(L, 2, 0, &tolua_err))
            goto tolua_lerror;
#endif
        luaval_to_vec3(L, 2, &self->_direction);
        return 0;
    }

    return 0;

#if _AX_DEBUG >= 1
tolua_lerror:
    tolua_error(L, "#ferror in function 'axlua_3d_set_Ray_direction'.", &tolua_err);
    return 0;
#endif
}

int axlua_3d_Ray_finalize(lua_State* L)
{
    ax::Ray* self = (ax::Ray*)tolua_tousertype(L, 1, 0);
    AX_SAFE_DELETE(self);
    return 0;
}

int lua_register_axis_3d_Ray(lua_State* L)
{
    tolua_usertype(L, "ax.Ray");
    tolua_cclass(L, "Ray", "ax.Ray", "", axlua_3d_Ray_finalize);

    tolua_beginmodule(L, "Ray");
    tolua_variable(L, "_origin", axlua_3d_get_Ray_origin, axlua_3d_set_Ray_origin);
    tolua_variable(L, "_direction", axlua_3d_get_Ray_direction, axlua_3d_set_Ray_direction);
    tolua_function(L, "new", axlua_3d_Ray_constructor);
    tolua_function(L, "set", axlua_3d_Ray_set);
    tolua_function(L, "transform", axlua_3d_Ray_transform);
    tolua_function(L, "intersects", axlua_3d_Ray_intersects);
    tolua_endmodule(L);
    auto typeName                                    = typeid(ax::Ray).name();
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "ax.Ray";
    g_typeCast[typeName]                             = "ax.Ray";
    return 1;
}

int register_all_cocos3d_manual_class(lua_State* L)
{
    tolua_module(L, "ax", 0);
    tolua_beginmodule(L, "ax");
    lua_register_axis_3d_AABB(L);
    lua_register_axis_3d_OBB(L);
    lua_register_axis_3d_Ray(L);
    tolua_endmodule(L);

    return 0;
}

int register_cocos3d_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_3d(L);
        register_all_ax_3d_manual(L);
        register_all_cocos3d_manual_class(L);
    }
    lua_pop(L, 1);

    return 1;
}
