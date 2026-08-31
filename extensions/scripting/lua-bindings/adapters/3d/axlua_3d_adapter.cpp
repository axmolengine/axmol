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
#include "lua-bindings/adapters/3d/axlua_3d_adapter.h"
#include "lua-bindings/generated/axlua_3d_gen.h"
#include "lua-bindings/runtime/axlua_conversions.h"
#include "lua-bindings/runtime/axlua_runtime.h"
#include "lua-bindings/runtime/LuaEngine.h"
#include "axmol/3d/Bundle3D.h"
#include "axmol/3d/MeshRenderer.h"
#include "axmol/3d/Terrain.h"

int axlua_3d_AABB_getCorners(lua_State* L);
int axlua_3d_AABB_updateMinMax(lua_State* L);
int axlua_3d_OBB_getCorners(lua_State* L);
int axlua_3d_Ray_intersects(lua_State* L);

int axlua_3d_MeshRenderer_createAsync(lua_State* L)
{
    int argc = 0;
    bool ok  = true;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(L, 1, "ax.MeshRenderer", 0, &conversionError))
        goto argumentError;
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
            if (!axlua::adapter::is_function(L, 4, "LUA_FUNCTION", 0, &conversionError))
            {
                goto argumentError;
            }
#endif
            axlua::Callback<void(ax::MeshRenderer*)> callback(L, 4);
            ax::MeshRenderer::createAsync(modelPath, texturePath,
                                          [callback = std::move(callback)](ax::MeshRenderer* mesh, void*) mutable {
                                              callback(mesh);
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
            if (!axlua::adapter::is_function(L, 3, "LUA_FUNCTION", 0, &conversionError))
            {
                goto argumentError;
            }
#endif
            axlua::Callback<void(ax::MeshRenderer*)> callback(L, 3);
            ax::MeshRenderer::createAsync(modelPath,
                                          [callback = std::move(callback)](ax::MeshRenderer* mesh, void*) mutable {
                                              callback(mesh);
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
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_3d_MeshRenderer_createAsync'.", &conversionError);
#endif
    return 0;
}

static void extendMeshRenderer(lua_State* L)
{
    axlua::adapter::push_literal(L, "ax.MeshRenderer");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "createAsync", axlua_3d_MeshRenderer_createAsync);
    }
    lua_pop(L, 1);
}

bool luaval_to_terraindata(lua_State* L, int lo, ax::Terrain::TerrainData* outValue, const char* funcName = "")
{
    if (nullptr == L || nullptr == outValue)
        return false;

    bool ok = true;
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_table(L, lo, 0, &conversionError))
    {
#if _AX_DEBUG >= 1
        luaval_to_native_err(L, "#ferror:", &conversionError, funcName);
        ok = false;
#endif
    }

    if (ok)
    {
        axlua::adapter::push_literal(L, "_chunkSize");
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

        axlua::adapter::push_literal(L, "_heightMapSrc");
        lua_gettable(L, lo);
        outValue->_heightMapSrc = axlua::adapter::to_string(L, -1, "");
        lua_pop(L, 1);

        axlua::adapter::push_literal(L, "_alphaMapSrc");
        lua_gettable(L, lo);
        outValue->_alphaMapSrc = const_cast<char*>(axlua::adapter::to_string(L, -1, ""));
        lua_pop(L, 1);

        axlua::adapter::push_literal(L, "_detailMaps");
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
                    axlua::adapter::push_literal(L, "_detailMapSrc");
                    lua_gettable(L, -2);
                    outValue->_detailMaps[i]._detailMapSrc = axlua::adapter::to_string(L, -1, "");
                    lua_pop(L, 1);

                    axlua::adapter::push_literal(L, "_detailMapSize");
                    lua_gettable(L, -2);
                    outValue->_detailMaps[i]._detailMapSize = lua_isnil(L, -1) ? 0.0f : (float)lua_tonumber(L, -1);
                    lua_pop(L, 1);
                }
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);

        axlua::adapter::push_literal(L, "_mapHeight");
        lua_gettable(L, lo);
        outValue->_mapHeight = lua_isnil(L, -1) ? 2.0f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        axlua::adapter::push_literal(L, "_mapScale");
        lua_gettable(L, lo);
        outValue->_mapScale = lua_isnil(L, -1) ? 0.1f : (float)lua_tonumber(L, -1);
        lua_pop(L, 1);

        axlua::adapter::push_literal(L, "_detailMapAmount");
        lua_gettable(L, lo);
        outValue->_detailMapAmount = lua_isnil(L, -1) ? 0 : (int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        axlua::adapter::push_literal(L, "_skirtHeightRatio");
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
    axlua::adapter::push_literal(L, "_chunkSize");
    size_to_luaval(L, inValue._chunkSize);
    lua_rawset(L, -3);

    if (inValue._heightMapSrc.length() > 0)
    {
        axlua::adapter::push_literal(L, "_heightMapSrc");
        lua_pushlstring(L, inValue._heightMapSrc.data(), inValue._heightMapSrc.size());
        lua_rawset(L, -3);
    }

    if (!inValue._alphaMapSrc.empty())
    {
        axlua::adapter::push_literal(L, "_alphaMapSrc");
        lua_pushlstring(L, inValue._alphaMapSrc.data(), inValue._alphaMapSrc.size());
        lua_rawset(L, -3);
    }

    axlua::adapter::push_literal(L, "_detailMaps");
    lua_newtable(L);
    for (int i = 0; i < 4; i++)
    {

        lua_pushnumber(L, (lua_Number)i + 1);
        lua_newtable(L);

        axlua::adapter::push_literal(L, "_detailMapSrc");
        lua_pushlstring(L, inValue._detailMaps[i]._detailMapSrc.data(), inValue._detailMaps[i]._detailMapSrc.size());
        lua_rawset(L, -3);

        axlua::adapter::push_literal(L, "_detailMapSize");
        lua_pushnumber(L, (lua_Number)inValue._detailMaps[i]._detailMapSize);
        lua_rawset(L, -3);

        lua_rawset(L, -3);
    }
    lua_rawset(L, -3);

    axlua::adapter::push_literal(L, "_mapHeight");
    lua_pushnumber(L, (lua_Number)inValue._mapHeight);
    lua_rawset(L, -3);

    axlua::adapter::push_literal(L, "_mapScale");
    lua_pushnumber(L, (lua_Number)inValue._mapScale);
    lua_rawset(L, -3);

    axlua::adapter::push_literal(L, "_detailMapAmount");
    lua_pushnumber(L, (lua_Number)inValue._detailMapAmount);
    lua_rawset(L, -3);

    axlua::adapter::push_literal(L, "_skirtHeightRatio");
    lua_pushnumber(L, (lua_Number)inValue._skirtHeightRatio);
    lua_rawset(L, -3);
}

int axlua_3d_Terrain_create(lua_State* L)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(L, 1, "ax.Terrain", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 1)
    {
        ax::Terrain::TerrainData arg0;
        ok &= luaval_to_terraindata(L, 2, &arg0);
        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'axlua_3d_Terrain_create'", nullptr);
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
        ok &= luaval_to_int(L, 3, (int*)&arg1, "ax.Terrain:create");
        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'axlua_3d_Terrain_create'", nullptr);
            return 0;
        }
        ax::Terrain* ret = ax::Terrain::create(arg0, arg1);
        object_to_luaval<ax::Terrain>(L, "ax.Terrain", (ax::Terrain*)ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Terrain:create", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_3d_Terrain_create'.", &conversionError);
#endif
    return 0;
}

int axlua_3d_Terrain_getHeight(lua_State* L)
{
    int argc         = 0;
    ax::Terrain* obj = nullptr;
    bool ok          = true;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(L, 1, "ax.Terrain", 0, &conversionError))
        goto argumentError;
#endif
    obj = (ax::Terrain*)axlua::adapter::to_usertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(L, "invalid 'obj' in function 'axlua_3d_Terrain_getHeight'", nullptr);
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
            double ret = obj->getHeight(arg0);
            lua_pushnumber(L, (lua_Number)ret);
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
            double ret = obj->getHeight(arg0, arg1);
            lua_pushnumber(L, (lua_Number)ret);
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
            double ret = obj->getHeight(arg0, arg1);
            lua_pushnumber(L, (lua_Number)ret);
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
            double ret = obj->getHeight(arg0, arg1, &arg2);
            lua_pushnumber(L, (lua_Number)ret);
            vec3_to_luaval(L, arg2);
            return 2;
        }
    } while (0);
    ok = true;
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Terrain:getHeight", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_3d_Terrain_getHeight'.", &conversionError);
#endif

    return 0;
}

int axlua_3d_Terrain_getIntersectionPoint(lua_State* luaState)
{
    int argc         = 0;
    ax::Terrain* obj = nullptr;
    bool ok          = true;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(luaState, 1, "ax.Terrain", 0, &conversionError))
        goto argumentError;
#endif
    obj = (ax::Terrain*)axlua::adapter::to_usertype(luaState, 1, 0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(luaState, "invalid 'obj' in function 'axlua_3d_Terrain_getIntersectionPoint'",
                                    nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(luaState) - 1;
    do
    {
        if (argc == 2)
        {
            ax::Ray* arg0 = nullptr;
            ok &= luaval_to_object<ax::Ray>(luaState, 2, "ax.Ray", &arg0, "ax.Terrain:getIntersectionPoint");

            if (!ok)
            {
                break;
            }
            ax::Vec3 arg1;
            ok &= luaval_to_vec3(luaState, 3, &arg1, "ax.Terrain:getIntersectionPoint");

            if (!ok)
            {
                break;
            }
            bool ret = obj->getIntersectionPoint(*arg0, arg1);
            lua_pushboolean(luaState, (bool)ret);
            vec3_to_luaval(luaState, arg1);
            return 2;
        }
    } while (0);
    ok = true;
    do
    {
        if (argc == 1)
        {
            ax::Ray* arg0;
            ok &= luaval_to_object<ax::Ray>(luaState, 2, "ax.Ray", &arg0, "ax.Terrain:getIntersectionPoint");

            if (!ok)
            {
                break;
            }
            ax::Vec3 ret = obj->getIntersectionPoint(*arg0);
            vec3_to_luaval(luaState, ret);
            return 1;
        }
    } while (0);
    ok = true;
    luaL_error(luaState, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Terrain:getIntersectionPoint",
               argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(luaState, "#ferror in function 'axlua_3d_Terrain_getIntersectionPoint'.",
                                &conversionError);
#endif

    return 0;
}

static void extendTerrain(lua_State* L)
{
    axlua::adapter::push_literal(L, "ax.Terrain");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "create", axlua_3d_Terrain_create);
        axlua::adapter::set_function(L, "getHeight", axlua_3d_Terrain_getHeight);
        axlua::adapter::set_function(L, "getIntersectionPoint", axlua_3d_Terrain_getIntersectionPoint);
    }
    lua_pop(L, 1);
}

int axlua_3d_Bundle3D_getTrianglesList(lua_State* L)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertable(L, 1, "ax.Bundle3D", 0, &conversionError))
        goto argumentError;
#endif

    argc = lua_gettop(L) - 1;

    if (argc == 1)
    {
        std::string arg0;
        ok &= luaval_to_std_string(L, 2, &arg0, "ax.Bundle3D:getTrianglesList");
        if (!ok)
        {
            axlua::adapter::raise_error(L, "invalid arguments in function 'axlua_3d_Bundle3D_getTrianglesList'",
                                        nullptr);
            return 0;
        }

        std::vector<ax::Vec3> ret = ax::Bundle3D::getTrianglesList(arg0);
        vec3span_to_luaval(L, ret);
        return 1;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d\n ", "ax.Bundle3D:getTrianglesList", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_3d_Bundle3D_getTrianglesList'.", &conversionError);
#endif
    return 0;
}

void extendBundle3D(lua_State* L)
{
    axlua::adapter::push_literal(L, "ax.Bundle3D");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "getTrianglesList", axlua_3d_Bundle3D_getTrianglesList);
    }
    lua_pop(L, 1);
}

void extendAABB(lua_State* L)
{
    axlua::adapter::push_literal(L, "ax.AABB");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
    {
        axlua::adapter::set_function(L, "getCorners", axlua_3d_AABB_getCorners);
        axlua::adapter::set_function(L, "updateMinMax", axlua_3d_AABB_updateMinMax);
    }
    lua_pop(L, 1);
}

void extendOBB(lua_State* L)
{
    axlua::adapter::push_literal(L, "ax.OBB");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
        axlua::adapter::set_function(L, "getCorners", axlua_3d_OBB_getCorners);
    lua_pop(L, 1);
}

void extendRay(lua_State* L)
{
    axlua::adapter::push_literal(L, "ax.Ray");
    lua_rawget(L, LUA_REGISTRYINDEX);
    if (lua_istable(L, -1))
        axlua::adapter::set_function(L, "intersects", axlua_3d_Ray_intersects);
    lua_pop(L, 1);
}

static int register_all_ax_3d_adapter(lua_State* L)
{
    if (nullptr == L)
        return 0;

    extendMeshRenderer(L);
    extendTerrain(L);
    extendBundle3D(L);
    extendAABB(L);
    extendOBB(L);
    extendRay(L);
    return 0;
}

int axlua_3d_AABB_getCorners(lua_State* L)
{
    int argc      = 0;
    ax::AABB* obj = nullptr;
    bool ok       = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(L, 1, "ax.AABB", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::AABB*)axlua::adapter::to_usertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(L, "invalid 'obj' in function 'axlua_3d_AABB_getCorners'", nullptr);
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
        obj->getCorners(arg0);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AABB:getCorners", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_3d_AABB_getCorners'.", &conversionError);
#endif

    return 0;
}
int axlua_3d_AABB_updateMinMax(lua_State* L)
{
    int argc      = 0;
    ax::AABB* obj = nullptr;
    bool ok       = true;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(L, 1, "ax.AABB", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::AABB*)axlua::adapter::to_usertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(L, "invalid 'obj' in function 'axlua_3d_AABB_updateMinMax'", nullptr);
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
        obj->updateMinMax(arg0, arg1);
        return 0;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.AABB:updateMinMax", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_3d_AABB_updateMinMax'.", &conversionError);
#endif

    return 0;
}

int axlua_3d_OBB_getCorners(lua_State* L)
{
    int argc     = 0;
    ax::OBB* obj = nullptr;

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

#if _AX_DEBUG >= 1
    if (!axlua::adapter::is_usertype(L, 1, "ax.OBB", 0, &conversionError))
        goto argumentError;
#endif

    obj = (ax::OBB*)axlua::adapter::to_usertype(L, 1, 0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        axlua::adapter::raise_error(L, "invalid 'obj' in function 'axlua_3d_OBB_getCorners'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(L) - 1;
    if (argc == 1)
    {
        ax::Vec3* arg0;
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_table(L, 2, 0, &conversionError))
            goto argumentError;
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

        obj->getCorners(arg0);

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
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_3d_OBB_getCorners'.", &conversionError);
#endif

    return 0;
}

int axlua_3d_Ray_intersects(lua_State* L)
{
    int argc      = 0;
    ax::Ray* self = nullptr;
    bool ok       = true;
#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
    if (!axlua::adapter::is_usertype(L, 1, "ax.Ray", 0, &conversionError))
        goto argumentError;
#endif

    self = (ax::Ray*)axlua::adapter::to_usertype(L, 1, 0);
#if _AX_DEBUG >= 1
    if (nullptr == self)
    {
        axlua::adapter::raise_error(L, "invalid 'self' in function 'axlua_3d_Ray_intersects'\n", nullptr);
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
        lua_pushboolean(L, ret);
        lua_pushnumber(L, (lua_Number)distance);
        return 2;
    }
    luaL_error(L, "%s has wrong number of arguments: %d, was expecting %d \n", "ax.Ray:intersects", argc, 1);
    return 0;
#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'axlua_3d_Ray_intersects'.", &conversionError);
    return 0;
#endif
}

int register_ax_3d_module(lua_State* L)
{
    lua_getglobal(L, "_G");
    if (lua_istable(L, -1))  // stack:...,_G,
    {
        register_all_ax_3d(L);
        register_all_ax_3d_adapter(L);
    }
    if (lua_gettop(L) > 0)
        lua_pop(L, 1);

    return 1;
}
