#include "lua-bindings/auto/axlua_rhi_auto.hpp"
#include "axmol/rhi/RHITypes.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/rhi/Texture.h"
#include "axmol/rhi/VertexLayout.h"
#include "axmol/rhi/GraphicsCore.h"
#include "axmol/rhi/RenderTarget.h"
#include "lua-bindings/manual/tolua_fix.h"
#include "lua-bindings/manual/LuaBasicConversions.h"


int lua_register_ax_rhi_BufferUsage(lua_State* tolua_S)
{
    tolua_module(tolua_S, "BufferUsage", 0);
    tolua_beginmodule(tolua_S,"BufferUsage");
        tolua_constant(tolua_S, "STATIC", 0);
        tolua_constant(tolua_S, "DYNAMIC", 1);
        tolua_constant(tolua_S, "IMMUTABLE", 2);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::BufferUsage).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.BufferUsage";
    g_typeCast[typeName] = "axr.BufferUsage";
    return 1;
}


int lua_register_ax_rhi_BufferType(lua_State* tolua_S)
{
    tolua_module(tolua_S, "BufferType", 0);
    tolua_beginmodule(tolua_S,"BufferType");
        tolua_constant(tolua_S, "ARRAY_BUFFER", 0);
        tolua_constant(tolua_S, "ELEMENT_ARRAY_BUFFER", 1);
        tolua_constant(tolua_S, "UNIFORM_BUFFER", 2);
        tolua_constant(tolua_S, "PIXEL_PACK_BUFFER", 3);
        tolua_constant(tolua_S, "COUNT", 4);
        tolua_constant(tolua_S, "VERTEX", 0);
        tolua_constant(tolua_S, "INDEX", 1);
        tolua_constant(tolua_S, "UNIFORM", 2);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::BufferType).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.BufferType";
    g_typeCast[typeName] = "axr.BufferType";
    return 1;
}


int lua_register_ax_rhi_ShaderStage(lua_State* tolua_S)
{
    tolua_module(tolua_S, "ShaderStage", 0);
    tolua_beginmodule(tolua_S,"ShaderStage");
        tolua_constant(tolua_S, "UNKNOWN", -1);
        tolua_constant(tolua_S, "FRAGMENT", 0);
        tolua_constant(tolua_S, "VERTEX", 1);
        tolua_constant(tolua_S, "COMPUTE", 2);
        tolua_constant(tolua_S, "DEFAULT", 0);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::ShaderStage).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.ShaderStage";
    g_typeCast[typeName] = "axr.ShaderStage";
    return 1;
}


int lua_register_ax_rhi_PixelFormat(lua_State* tolua_S)
{
    tolua_module(tolua_S, "PixelFormat", 0);
    tolua_beginmodule(tolua_S,"PixelFormat");
        tolua_constant(tolua_S, "PVRTC4", 0);
        tolua_constant(tolua_S, "PVRTC4A", 1);
        tolua_constant(tolua_S, "PVRTC2", 2);
        tolua_constant(tolua_S, "PVRTC2A", 3);
        tolua_constant(tolua_S, "ETC1", 4);
        tolua_constant(tolua_S, "ETC2_RGB", 5);
        tolua_constant(tolua_S, "ETC2_RGBA", 6);
        tolua_constant(tolua_S, "S3TC_DXT1", 7);
        tolua_constant(tolua_S, "S3TC_DXT3", 8);
        tolua_constant(tolua_S, "S3TC_DXT5", 9);
        tolua_constant(tolua_S, "ATC_RGB", 10);
        tolua_constant(tolua_S, "ATC_EXPLICIT_ALPHA", 11);
        tolua_constant(tolua_S, "ATC_INTERPOLATED_ALPHA", 12);
        tolua_constant(tolua_S, "ASTC4x4", 13);
        tolua_constant(tolua_S, "ASTC5x4", 14);
        tolua_constant(tolua_S, "ASTC5x5", 15);
        tolua_constant(tolua_S, "ASTC6x5", 16);
        tolua_constant(tolua_S, "ASTC6x6", 17);
        tolua_constant(tolua_S, "ASTC8x5", 18);
        tolua_constant(tolua_S, "ASTC8x6", 19);
        tolua_constant(tolua_S, "ASTC8x8", 20);
        tolua_constant(tolua_S, "ASTC10x5", 21);
        tolua_constant(tolua_S, "ASTC10x6", 22);
        tolua_constant(tolua_S, "ASTC10x8", 23);
        tolua_constant(tolua_S, "ASTC10x10", 24);
        tolua_constant(tolua_S, "ASTC12x10", 25);
        tolua_constant(tolua_S, "ASTC12x12", 26);
        tolua_constant(tolua_S, "RGBA8", 27);
        tolua_constant(tolua_S, "BGRA8", 28);
        tolua_constant(tolua_S, "RGB8", 29);
        tolua_constant(tolua_S, "RGB565", 30);
        tolua_constant(tolua_S, "RGBA4", 31);
        tolua_constant(tolua_S, "RGB5A1", 32);
        tolua_constant(tolua_S, "R8", 33);
        tolua_constant(tolua_S, "RG8", 34);
        tolua_constant(tolua_S, "RGBA32F", 35);
        tolua_constant(tolua_S, "D24S8", 36);
        tolua_constant(tolua_S, "COUNT", 37);
        tolua_constant(tolua_S, "NONE", -1);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::PixelFormat).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.PixelFormat";
    g_typeCast[typeName] = "axr.PixelFormat";
    return 1;
}


int lua_register_ax_rhi_TextureUsage(lua_State* tolua_S)
{
    tolua_module(tolua_S, "TextureUsage", 0);
    tolua_beginmodule(tolua_S,"TextureUsage");
        tolua_constant(tolua_S, "READ", 0);
        tolua_constant(tolua_S, "WRITE", 1);
        tolua_constant(tolua_S, "RENDER_TARGET", 2);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::TextureUsage).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.TextureUsage";
    g_typeCast[typeName] = "axr.TextureUsage";
    return 1;
}


int lua_register_ax_rhi_IndexFormat(lua_State* tolua_S)
{
    tolua_module(tolua_S, "IndexFormat", 0);
    tolua_beginmodule(tolua_S,"IndexFormat");
        tolua_constant(tolua_S, "U_SHORT", 1);
        tolua_constant(tolua_S, "U_INT", 2);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::IndexFormat).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.IndexFormat";
    g_typeCast[typeName] = "axr.IndexFormat";
    return 1;
}


int lua_register_ax_rhi_PrimitiveType(lua_State* tolua_S)
{
    tolua_module(tolua_S, "PrimitiveType", 0);
    tolua_beginmodule(tolua_S,"PrimitiveType");
        tolua_constant(tolua_S, "POINT", 0);
        tolua_constant(tolua_S, "LINE", 1);
        tolua_constant(tolua_S, "LINE_LOOP", 2);
        tolua_constant(tolua_S, "LINE_STRIP", 3);
        tolua_constant(tolua_S, "TRIANGLE", 4);
        tolua_constant(tolua_S, "TRIANGLE_STRIP", 5);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::PrimitiveType).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.PrimitiveType";
    g_typeCast[typeName] = "axr.PrimitiveType";
    return 1;
}


int lua_register_ax_rhi_BlendFactor(lua_State* tolua_S)
{
    tolua_module(tolua_S, "BlendFactor", 0);
    tolua_beginmodule(tolua_S,"BlendFactor");
        tolua_constant(tolua_S, "ZERO", 0);
        tolua_constant(tolua_S, "ONE", 1);
        tolua_constant(tolua_S, "SRC_COLOR", 2);
        tolua_constant(tolua_S, "ONE_MINUS_SRC_COLOR", 3);
        tolua_constant(tolua_S, "SRC_ALPHA", 4);
        tolua_constant(tolua_S, "ONE_MINUS_SRC_ALPHA", 5);
        tolua_constant(tolua_S, "DST_COLOR", 6);
        tolua_constant(tolua_S, "ONE_MINUS_DST_COLOR", 7);
        tolua_constant(tolua_S, "DST_ALPHA", 8);
        tolua_constant(tolua_S, "ONE_MINUS_DST_ALPHA", 9);
        tolua_constant(tolua_S, "CONSTANT_ALPHA", 10);
        tolua_constant(tolua_S, "SRC_ALPHA_SATURATE", 11);
        tolua_constant(tolua_S, "ONE_MINUS_CONSTANT_ALPHA", 12);
        tolua_constant(tolua_S, "BLEND_COLOR", 13);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::BlendFactor).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.BlendFactor";
    g_typeCast[typeName] = "axr.BlendFactor";
    return 1;
}


int lua_register_ax_rhi_ColorWriteMask(lua_State* tolua_S)
{
    tolua_module(tolua_S, "ColorWriteMask", 0);
    tolua_beginmodule(tolua_S,"ColorWriteMask");
        tolua_constant(tolua_S, "RED_BIT", 0);
        tolua_constant(tolua_S, "GREEN_BIT", 1);
        tolua_constant(tolua_S, "BLUE_BIT", 2);
        tolua_constant(tolua_S, "ALPHA_BIT", 3);
        tolua_constant(tolua_S, "NONE", 0);
        tolua_constant(tolua_S, "RED", 1);
        tolua_constant(tolua_S, "GREEN", 2);
        tolua_constant(tolua_S, "BLUE", 4);
        tolua_constant(tolua_S, "ALPHA", 8);
        tolua_constant(tolua_S, "ALL", 15);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::ColorWriteMask).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.ColorWriteMask";
    g_typeCast[typeName] = "axr.ColorWriteMask";
    return 1;
}


int lua_register_ax_rhi_CullMode(lua_State* tolua_S)
{
    tolua_module(tolua_S, "CullMode", 0);
    tolua_beginmodule(tolua_S,"CullMode");
        tolua_constant(tolua_S, "NONE", 0);
        tolua_constant(tolua_S, "BACK", 1);
        tolua_constant(tolua_S, "FRONT", 2);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::CullMode).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.CullMode";
    g_typeCast[typeName] = "axr.CullMode";
    return 1;
}


int lua_register_ax_rhi_Winding(lua_State* tolua_S)
{
    tolua_module(tolua_S, "Winding", 0);
    tolua_beginmodule(tolua_S,"Winding");
        tolua_constant(tolua_S, "CLOCK_WISE", 0);
        tolua_constant(tolua_S, "COUNTER_CLOCK_WISE", 1);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::Winding).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.Winding";
    g_typeCast[typeName] = "axr.Winding";
    return 1;
}


int lua_register_ax_rhi_TextureType(lua_State* tolua_S)
{
    tolua_module(tolua_S, "TextureType", 0);
    tolua_beginmodule(tolua_S,"TextureType");
        tolua_constant(tolua_S, "TEXTURE_2D", 0);
        tolua_constant(tolua_S, "TEXTURE_CUBE", 1);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::TextureType).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.TextureType";
    g_typeCast[typeName] = "axr.TextureType";
    return 1;
}


int lua_register_ax_rhi_SamplerAddressMode(lua_State* tolua_S)
{
    tolua_module(tolua_S, "SamplerAddressMode", 0);
    tolua_beginmodule(tolua_S,"SamplerAddressMode");
        tolua_constant(tolua_S, "REPEAT", 0);
        tolua_constant(tolua_S, "MIRROR", 1);
        tolua_constant(tolua_S, "CLAMP", 2);
        tolua_constant(tolua_S, "BORDER", 3);
        tolua_constant(tolua_S, "CLAMP_TO_EDGE", 2);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::SamplerAddressMode).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.SamplerAddressMode";
    g_typeCast[typeName] = "axr.SamplerAddressMode";
    return 1;
}


int lua_register_ax_rhi_SamplerFilter(lua_State* tolua_S)
{
    tolua_module(tolua_S, "SamplerFilter", 0);
    tolua_beginmodule(tolua_S,"SamplerFilter");
        tolua_constant(tolua_S, "MIN_NEAREST", 0);
        tolua_constant(tolua_S, "MIN_LINEAR", 1);
        tolua_constant(tolua_S, "MIN_ANISOTROPIC", 2);
        tolua_constant(tolua_S, "MAG_NEAREST", 0);
        tolua_constant(tolua_S, "MAG_LINEAR", 1);
        tolua_constant(tolua_S, "MIP_NEAREST", 0);
        tolua_constant(tolua_S, "MIP_LINEAR", 1);
        tolua_constant(tolua_S, "MIP_DEFAULT", 3);
        tolua_constant(tolua_S, "NEAREST", 0);
        tolua_constant(tolua_S, "LINEAR", 1);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::SamplerFilter).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.SamplerFilter";
    g_typeCast[typeName] = "axr.SamplerFilter";
    return 1;
}


int lua_register_ax_rhi_TextureCubeFace(lua_State* tolua_S)
{
    tolua_module(tolua_S, "TextureCubeFace", 0);
    tolua_beginmodule(tolua_S,"TextureCubeFace");
        tolua_constant(tolua_S, "POSITIVE_X", 0);
        tolua_constant(tolua_S, "NEGATIVE_X", 1);
        tolua_constant(tolua_S, "POSITIVE_Y", 2);
        tolua_constant(tolua_S, "NEGATIVE_Y", 3);
        tolua_constant(tolua_S, "POSITIVE_Z", 4);
        tolua_constant(tolua_S, "NEGATIVE_Z", 5);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::TextureCubeFace).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.TextureCubeFace";
    g_typeCast[typeName] = "axr.TextureCubeFace";
    return 1;
}

int lua_ax_rhi_ShaderCache_removeAllShaders(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ShaderCache* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ShaderCache",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::ShaderCache*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ShaderCache_removeAllShaders'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ShaderCache_removeAllShaders'", nullptr);
            return 0;
        }
        obj->removeAllShaders();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ShaderCache:removeAllShaders",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ShaderCache_removeAllShaders'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ShaderCache_acquireVertexShaderModule(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ShaderCache* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ShaderCache",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::ShaderCache*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ShaderCache_acquireVertexShaderModule'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Data arg0;

        luaL_error(tolua_S, "ax::Data unsupported");;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ShaderCache_acquireVertexShaderModule'", nullptr);
            return 0;
        }
        auto&& ret = obj->acquireVertexShaderModule(arg0);
        object_to_luaval<ax::rhi::ShaderModule>(tolua_S, "axr.ShaderModule",(ax::rhi::ShaderModule*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ShaderCache:acquireVertexShaderModule",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ShaderCache_acquireVertexShaderModule'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ShaderCache_acquireFragmentShaderModule(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ShaderCache* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ShaderCache",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::ShaderCache*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ShaderCache_acquireFragmentShaderModule'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::Data arg0;

        luaL_error(tolua_S, "ax::Data unsupported");;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ShaderCache_acquireFragmentShaderModule'", nullptr);
            return 0;
        }
        auto&& ret = obj->acquireFragmentShaderModule(arg0);
        object_to_luaval<ax::rhi::ShaderModule>(tolua_S, "axr.ShaderModule",(ax::rhi::ShaderModule*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ShaderCache:acquireFragmentShaderModule",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ShaderCache_acquireFragmentShaderModule'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ShaderCache_removeUnusedShader(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ShaderCache* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ShaderCache",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::ShaderCache*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ShaderCache_removeUnusedShader'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ShaderCache_removeUnusedShader'", nullptr);
            return 0;
        }
        obj->removeUnusedShader();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ShaderCache:removeUnusedShader",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ShaderCache_removeUnusedShader'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ShaderCache_getInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.ShaderCache",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ShaderCache_getInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::rhi::ShaderCache::getInstance();
        object_to_luaval<ax::rhi::ShaderCache>(tolua_S, "axr.ShaderCache",(ax::rhi::ShaderCache*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.ShaderCache:getInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ShaderCache_getInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_ShaderCache_destroyInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.ShaderCache",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ShaderCache_destroyInstance'", nullptr);
            return 0;
        }
        ax::rhi::ShaderCache::destroyInstance();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.ShaderCache:destroyInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ShaderCache_destroyInstance'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_rhi_ShaderCache_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ShaderCache)");
    return 0;
}

int lua_register_ax_rhi_ShaderCache(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axr.ShaderCache");
    tolua_cclass(tolua_S,"ShaderCache","axr.ShaderCache","",nullptr);

    tolua_beginmodule(tolua_S,"ShaderCache");
        tolua_function(tolua_S,"removeAllShaders",lua_ax_rhi_ShaderCache_removeAllShaders);
        tolua_function(tolua_S,"acquireVertexShaderModule",lua_ax_rhi_ShaderCache_acquireVertexShaderModule);
        tolua_function(tolua_S,"acquireFragmentShaderModule",lua_ax_rhi_ShaderCache_acquireFragmentShaderModule);
        tolua_function(tolua_S,"removeUnusedShader",lua_ax_rhi_ShaderCache_removeUnusedShader);
        tolua_function(tolua_S,"getInstance", lua_ax_rhi_ShaderCache_getInstance);
        tolua_function(tolua_S,"destroyInstance", lua_ax_rhi_ShaderCache_destroyInstance);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::rhi::ShaderCache).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.ShaderCache";
    g_typeCast[typeName] = "axr.ShaderCache";
    return 1;
}

int lua_ax_rhi_Program_getUniformLocation(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Program",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Program_getUniformLocation'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 1) {
            ax::rhi::Uniform arg0;
            ok &= luaval_to_int(tolua_S, 2, &arg0, "axr.Program:getUniformLocation");

            if (!ok) { break; }
            ax::rhi::UniformLocation ret = obj->getUniformLocation(arg0);
            uniformLocation_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 1) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axr.Program:getUniformLocation");

            if (!ok) { break; }
            ax::rhi::UniformLocation ret = obj->getUniformLocation(arg0);
            uniformLocation_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axr.Program:getUniformLocation",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getUniformLocation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getUniformBufferSize(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Program_getUniformBufferSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Program_getUniformBufferSize'", nullptr);
            return 0;
        }
        auto&& ret = obj->getUniformBufferSize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Program:getUniformBufferSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getUniformBufferSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getProgramType(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Program_getProgramType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Program_getProgramType'", nullptr);
            return 0;
        }
        auto&& ret = obj->getProgramType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Program:getProgramType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getProgramType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getProgramId(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Program_getProgramId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Program_getProgramId'", nullptr);
            return 0;
        }
        auto&& ret = obj->getProgramId();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Program:getProgramId",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getProgramId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getVertexLayout(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Program_getVertexLayout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Program_getVertexLayout'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVertexLayout();
        object_to_luaval<ax::rhi::VertexLayout>(tolua_S, "axr.VertexLayout",(ax::rhi::VertexLayout*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Program:getVertexLayout",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getVertexLayout'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getVSModule(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Program_getVSModule'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Program_getVSModule'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVSModule();
        object_to_luaval<ax::rhi::ShaderModule>(tolua_S, "axr.ShaderModule",(ax::rhi::ShaderModule*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Program:getVSModule",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getVSModule'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getFSModule(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Program_getFSModule'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Program_getFSModule'", nullptr);
            return 0;
        }
        auto&& ret = obj->getFSModule();
        object_to_luaval<ax::rhi::ShaderModule>(tolua_S, "axr.ShaderModule",(ax::rhi::ShaderModule*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Program:getFSModule",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getFSModule'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_isValid(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Program_isValid'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Program_isValid'", nullptr);
            return 0;
        }
        auto&& ret = obj->isValid();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Program:isValid",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_isValid'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_rhi_Program_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Program)");
    return 0;
}

int lua_register_ax_rhi_Program(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axr.Program");
    tolua_cclass(tolua_S,"Program","axr.Program","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Program");
        tolua_function(tolua_S,"getUniformLocation",lua_ax_rhi_Program_getUniformLocation);
        tolua_function(tolua_S,"getUniformBufferSize",lua_ax_rhi_Program_getUniformBufferSize);
        tolua_function(tolua_S,"getProgramType",lua_ax_rhi_Program_getProgramType);
        tolua_function(tolua_S,"getProgramId",lua_ax_rhi_Program_getProgramId);
        tolua_function(tolua_S,"getVertexLayout",lua_ax_rhi_Program_getVertexLayout);
        tolua_function(tolua_S,"getVSModule",lua_ax_rhi_Program_getVSModule);
        tolua_function(tolua_S,"getFSModule",lua_ax_rhi_Program_getFSModule);
        tolua_function(tolua_S,"isValid",lua_ax_rhi_Program_isValid);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::rhi::Program).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.Program";
    g_typeCast[typeName] = "axr.Program";
    return 1;
}

int lua_ax_rhi_ProgramState_clone(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ProgramState* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ProgramState_clone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ProgramState_clone'", nullptr);
            return 0;
        }
        auto&& ret = obj->clone();
        object_to_luaval<ax::rhi::ProgramState>(tolua_S, "axr.ProgramState",(ax::rhi::ProgramState*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ProgramState:clone",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ProgramState_clone'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ProgramState_getProgram(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ProgramState* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ProgramState_getProgram'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ProgramState_getProgram'", nullptr);
            return 0;
        }
        auto&& ret = obj->getProgram();
        object_to_luaval<ax::rhi::Program>(tolua_S, "axr.Program",(ax::rhi::Program*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ProgramState:getProgram",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ProgramState_getProgram'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ProgramState_getVertexInputDesc(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ProgramState* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ProgramState_getVertexInputDesc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::rhi::VertexSemantic arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR VertexSemantic
        ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ProgramState_getVertexInputDesc'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVertexInputDesc(arg0);
        #pragma warning NO CONVERSION FROM NATIVE FOR VertexInputDesc*;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ProgramState:getVertexInputDesc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ProgramState_getVertexInputDesc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ProgramState_setTexture(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ProgramState* obj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif
    obj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ProgramState_setTexture'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do {
        if (argc == 3) {
            ax::rhi::UniformLocation arg0;
            ok &= luaval_to_uniformLocation(tolua_S, 2, arg0, "axr.ProgramState:setTexture");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int(tolua_S, 3, &arg1, "axr.ProgramState:setTexture");

            if (!ok) { break; }
            ax::rhi::Texture* arg2;
            ok &= luaval_to_object<ax::rhi::Texture>(tolua_S, 4, "axr.Texture",&arg2, "axr.ProgramState:setTexture");

            if (!ok) { break; }
            obj->setTexture(arg0, arg1, arg2);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do {
        if (argc == 1) {
            ax::rhi::Texture* arg0;
            ok &= luaval_to_object<ax::rhi::Texture>(tolua_S, 2, "axr.Texture",&arg0, "axr.ProgramState:setTexture");

            if (!ok) { break; }
            obj->setTexture(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axr.ProgramState:setTexture",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ProgramState_setTexture'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ProgramState_setParameterAutoBinding(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ProgramState* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ProgramState_setParameterAutoBinding'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axr.ProgramState:setParameterAutoBinding");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axr.ProgramState:setParameterAutoBinding");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ProgramState_setParameterAutoBinding'", nullptr);
            return 0;
        }
        obj->setParameterAutoBinding(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ProgramState:setParameterAutoBinding",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ProgramState_setParameterAutoBinding'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ProgramState_getBuiltinVertexLayout(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ProgramState* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ProgramState_getBuiltinVertexLayout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ProgramState_getBuiltinVertexLayout'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBuiltinVertexLayout();
        object_to_luaval<ax::rhi::VertexLayout>(tolua_S, "axr.VertexLayout",(ax::rhi::VertexLayout*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ProgramState:getBuiltinVertexLayout",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ProgramState_getBuiltinVertexLayout'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ProgramState_getBatchId(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ProgramState* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ProgramState_getBatchId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ProgramState_getBatchId'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBatchId();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ProgramState:getBatchId",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ProgramState_getBatchId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ProgramState_isBatchable(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ProgramState* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ProgramState_isBatchable'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ProgramState_isBatchable'", nullptr);
            return 0;
        }
        auto&& ret = obj->isBatchable();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ProgramState:isBatchable",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ProgramState_isBatchable'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ProgramState_updateBatchId(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ProgramState* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_ProgramState_updateBatchId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ProgramState_updateBatchId'", nullptr);
            return 0;
        }
        obj->updateBatchId();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ProgramState:updateBatchId",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ProgramState_updateBatchId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_ProgramState_constructor(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ProgramState* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::rhi::Program* arg0;

        ok &= luaval_to_object<ax::rhi::Program>(tolua_S, 2, "axr.Program",&arg0, "axr.ProgramState:ProgramState");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ProgramState_constructor'", nullptr);
            return 0;
        }
        obj = new ax::rhi::ProgramState(arg0);
        obj->autorelease();
        int ID =  (int)obj->_ID ;
        int* luaID =  &obj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)obj,"axr.ProgramState");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.ProgramState:ProgramState",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_ProgramState_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_ax_rhi_ProgramState_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (ProgramState)");
    return 0;
}

int lua_register_ax_rhi_ProgramState(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axr.ProgramState");
    tolua_cclass(tolua_S,"ProgramState","axr.ProgramState","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"ProgramState");
        tolua_function(tolua_S,"new",lua_ax_rhi_ProgramState_constructor);
        tolua_function(tolua_S,"clone",lua_ax_rhi_ProgramState_clone);
        tolua_function(tolua_S,"getProgram",lua_ax_rhi_ProgramState_getProgram);
        tolua_function(tolua_S,"getVertexInputDesc",lua_ax_rhi_ProgramState_getVertexInputDesc);
        tolua_function(tolua_S,"setTexture",lua_ax_rhi_ProgramState_setTexture);
        tolua_function(tolua_S,"setParameterAutoBinding",lua_ax_rhi_ProgramState_setParameterAutoBinding);
        tolua_function(tolua_S,"getBuiltinVertexLayout",lua_ax_rhi_ProgramState_getBuiltinVertexLayout);
        tolua_function(tolua_S,"getBatchId",lua_ax_rhi_ProgramState_getBatchId);
        tolua_function(tolua_S,"isBatchable",lua_ax_rhi_ProgramState_isBatchable);
        tolua_function(tolua_S,"updateBatchId",lua_ax_rhi_ProgramState_updateBatchId);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::rhi::ProgramState).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.ProgramState";
    g_typeCast[typeName] = "axr.ProgramState";
    return 1;
}

int lua_ax_rhi_Texture_updateSamplerDesc(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_updateSamplerDesc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::rhi::SamplerDesc arg0;

        ok &= luaval_to_samplerDesc(tolua_S, 2, arg0, "axr.Texture:updateSamplerDesc");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_updateSamplerDesc'", nullptr);
            return 0;
        }
        obj->updateSamplerDesc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:updateSamplerDesc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_updateSamplerDesc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_getPixelFormat(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_getPixelFormat'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_getPixelFormat'", nullptr);
            return 0;
        }
        int ret = (int)obj->getPixelFormat();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:getPixelFormat",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_getPixelFormat'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_getTextureUsage(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_getTextureUsage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_getTextureUsage'", nullptr);
            return 0;
        }
        int ret = (int)obj->getTextureUsage();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:getTextureUsage",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_getTextureUsage'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_getTextureType(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_getTextureType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_getTextureType'", nullptr);
            return 0;
        }
        int ret = (int)obj->getTextureType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:getTextureType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_getTextureType'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_hasMipmaps(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_hasMipmaps'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_hasMipmaps'", nullptr);
            return 0;
        }
        auto&& ret = obj->hasMipmaps();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:hasMipmaps",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_hasMipmaps'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_getWidth(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_getWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_getWidth'", nullptr);
            return 0;
        }
        auto&& ret = obj->getWidth();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:getWidth",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_getWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_getHeight(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_getHeight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_getHeight'", nullptr);
            return 0;
        }
        auto&& ret = obj->getHeight();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:getHeight",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_getHeight'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_getArraySize(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_getArraySize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_getArraySize'", nullptr);
            return 0;
        }
        auto&& ret = obj->getArraySize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:getArraySize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_getArraySize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_getMipLevels(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_getMipLevels'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_getMipLevels'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMipLevels();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:getMipLevels",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_getMipLevels'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_updateData(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_updateData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 5)
    {
        const void* arg0;
        int arg1;
        int arg2;
        int arg3;
        int arg4;

        #pragma warning NO CONVERSION TO NATIVE FOR void*
        ok = false;

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axr.Texture:updateData");

        ok &= luaval_to_int(tolua_S, 4, &arg2, "axr.Texture:updateData");

        ok &= luaval_to_int(tolua_S, 5, &arg3, "axr.Texture:updateData");

        ok &= luaval_to_int(tolua_S, 6, &arg4, "axr.Texture:updateData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_updateData'", nullptr);
            return 0;
        }
        obj->updateData(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:updateData",argc, 5);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_updateData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_updateCompressedData(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_updateCompressedData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 6)
    {
        const void* arg0;
        int arg1;
        int arg2;
        unsigned int arg3;
        int arg4;
        int arg5;

        #pragma warning NO CONVERSION TO NATIVE FOR void*
        ok = false;

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axr.Texture:updateCompressedData");

        ok &= luaval_to_int(tolua_S, 4, &arg2, "axr.Texture:updateCompressedData");

        ok &= luaval_to_int(tolua_S, 5, &arg3, "axr.Texture:updateCompressedData");

        ok &= luaval_to_int(tolua_S, 6, &arg4, "axr.Texture:updateCompressedData");

        ok &= luaval_to_int(tolua_S, 7, &arg5, "axr.Texture:updateCompressedData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_updateCompressedData'", nullptr);
            return 0;
        }
        obj->updateCompressedData(arg0, arg1, arg2, arg3, arg4, arg5);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:updateCompressedData",argc, 6);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_updateCompressedData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_updateSubData(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_updateSubData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 7)
    {
        int arg0;
        int arg1;
        int arg2;
        int arg3;
        int arg4;
        const void* arg5;
        int arg6;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axr.Texture:updateSubData");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axr.Texture:updateSubData");

        ok &= luaval_to_int(tolua_S, 4, &arg2, "axr.Texture:updateSubData");

        ok &= luaval_to_int(tolua_S, 5, &arg3, "axr.Texture:updateSubData");

        ok &= luaval_to_int(tolua_S, 6, &arg4, "axr.Texture:updateSubData");

        #pragma warning NO CONVERSION TO NATIVE FOR void*
        ok = false;

        ok &= luaval_to_int(tolua_S, 8, &arg6, "axr.Texture:updateSubData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_updateSubData'", nullptr);
            return 0;
        }
        obj->updateSubData(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:updateSubData",argc, 7);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_updateSubData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_updateCompressedSubData(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_updateCompressedSubData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 8)
    {
        int arg0;
        int arg1;
        int arg2;
        int arg3;
        unsigned int arg4;
        int arg5;
        const void* arg6;
        int arg7;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axr.Texture:updateCompressedSubData");

        ok &= luaval_to_int(tolua_S, 3, &arg1, "axr.Texture:updateCompressedSubData");

        ok &= luaval_to_int(tolua_S, 4, &arg2, "axr.Texture:updateCompressedSubData");

        ok &= luaval_to_int(tolua_S, 5, &arg3, "axr.Texture:updateCompressedSubData");

        ok &= luaval_to_int(tolua_S, 6, &arg4, "axr.Texture:updateCompressedSubData");

        ok &= luaval_to_int(tolua_S, 7, &arg5, "axr.Texture:updateCompressedSubData");

        #pragma warning NO CONVERSION TO NATIVE FOR void*
        ok = false;

        ok &= luaval_to_int(tolua_S, 9, &arg7, "axr.Texture:updateCompressedSubData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_updateCompressedSubData'", nullptr);
            return 0;
        }
        obj->updateCompressedSubData(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:updateCompressedSubData",argc, 8);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_updateCompressedSubData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_updateFaceData(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_updateFaceData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::rhi::TextureCubeFace arg0;
        const void* arg1;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axr.Texture:updateFaceData");

        #pragma warning NO CONVERSION TO NATIVE FOR void*
        ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_updateFaceData'", nullptr);
            return 0;
        }
        obj->updateFaceData(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:updateFaceData",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_updateFaceData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_shouldGenMipmaps(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_Texture_shouldGenMipmaps'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_shouldGenMipmaps'", nullptr);
            return 0;
        }
        auto&& ret = obj->shouldGenMipmaps();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axr.Texture:shouldGenMipmaps");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_shouldGenMipmaps'", nullptr);
            return 0;
        }
        auto&& ret = obj->shouldGenMipmaps(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.Texture:shouldGenMipmaps",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_shouldGenMipmaps'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_rhi_Texture_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (Texture)");
    return 0;
}

int lua_register_ax_rhi_Texture(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axr.Texture");
    tolua_cclass(tolua_S,"Texture","axr.Texture","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Texture");
        tolua_function(tolua_S,"updateSamplerDesc",lua_ax_rhi_Texture_updateSamplerDesc);
        tolua_function(tolua_S,"getPixelFormat",lua_ax_rhi_Texture_getPixelFormat);
        tolua_function(tolua_S,"getTextureUsage",lua_ax_rhi_Texture_getTextureUsage);
        tolua_function(tolua_S,"getTextureType",lua_ax_rhi_Texture_getTextureType);
        tolua_function(tolua_S,"hasMipmaps",lua_ax_rhi_Texture_hasMipmaps);
        tolua_function(tolua_S,"getWidth",lua_ax_rhi_Texture_getWidth);
        tolua_function(tolua_S,"getHeight",lua_ax_rhi_Texture_getHeight);
        tolua_function(tolua_S,"getArraySize",lua_ax_rhi_Texture_getArraySize);
        tolua_function(tolua_S,"getMipLevels",lua_ax_rhi_Texture_getMipLevels);
        tolua_function(tolua_S,"updateData",lua_ax_rhi_Texture_updateData);
        tolua_function(tolua_S,"updateCompressedData",lua_ax_rhi_Texture_updateCompressedData);
        tolua_function(tolua_S,"updateSubData",lua_ax_rhi_Texture_updateSubData);
        tolua_function(tolua_S,"updateCompressedSubData",lua_ax_rhi_Texture_updateCompressedSubData);
        tolua_function(tolua_S,"updateFaceData",lua_ax_rhi_Texture_updateFaceData);
        tolua_function(tolua_S,"shouldGenMipmaps",lua_ax_rhi_Texture_shouldGenMipmaps);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::rhi::Texture).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.Texture";
    g_typeCast[typeName] = "axr.Texture";
    return 1;
}

int lua_ax_rhi_VertexLayout_getDesc(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::VertexLayout* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_VertexLayout_getDesc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_VertexLayout_getDesc'", nullptr);
            return 0;
        }
        auto&& ret = obj->getDesc();
        #pragma warning NO CONVERSION FROM NATIVE FOR VertexLayoutDesc;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.VertexLayout:getDesc",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_VertexLayout_getDesc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_VertexLayout_getStride(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::VertexLayout* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_VertexLayout_getStride'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_VertexLayout_getStride'", nullptr);
            return 0;
        }
        auto&& ret = obj->getStride();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.VertexLayout:getStride",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_VertexLayout_getStride'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_VertexLayout_getInstanceStride(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::VertexLayout* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_VertexLayout_getInstanceStride'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_VertexLayout_getInstanceStride'", nullptr);
            return 0;
        }
        auto&& ret = obj->getInstanceStride();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.VertexLayout:getInstanceStride",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_VertexLayout_getInstanceStride'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_VertexLayout_getHash(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::VertexLayout* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_VertexLayout_getHash'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_VertexLayout_getHash'", nullptr);
            return 0;
        }
        auto&& ret = obj->getHash();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.VertexLayout:getHash",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_VertexLayout_getHash'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_VertexLayout_getBuiltinId(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::VertexLayout* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_VertexLayout_getBuiltinId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_VertexLayout_getBuiltinId'", nullptr);
            return 0;
        }
        auto&& ret = obj->getBuiltinId();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.VertexLayout:getBuiltinId",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_VertexLayout_getBuiltinId'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_rhi_VertexLayout_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (VertexLayout)");
    return 0;
}

int lua_register_ax_rhi_VertexLayout(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axr.VertexLayout");
    tolua_cclass(tolua_S,"VertexLayout","axr.VertexLayout","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"VertexLayout");
        tolua_function(tolua_S,"getDesc",lua_ax_rhi_VertexLayout_getDesc);
        tolua_function(tolua_S,"getStride",lua_ax_rhi_VertexLayout_getStride);
        tolua_function(tolua_S,"getInstanceStride",lua_ax_rhi_VertexLayout_getInstanceStride);
        tolua_function(tolua_S,"getHash",lua_ax_rhi_VertexLayout_getHash);
        tolua_function(tolua_S,"getBuiltinId",lua_ax_rhi_VertexLayout_getBuiltinId);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::rhi::VertexLayout).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.VertexLayout";
    g_typeCast[typeName] = "axr.VertexLayout";
    return 1;
}

int lua_ax_rhi_GraphicsDevice_createRenderTarget(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_createRenderTarget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_createRenderTarget'", nullptr);
            return 0;
        }
        auto&& ret = obj->createRenderTarget();
        object_to_luaval<ax::rhi::RenderTarget>(tolua_S, "axr.RenderTarget",(ax::rhi::RenderTarget*)ret);
        return 1;
    }
    if (argc == 1)
    {
        ax::rhi::Texture* arg0;

        ok &= luaval_to_object<ax::rhi::Texture>(tolua_S, 2, "axr.Texture",&arg0, "axr.GraphicsDevice:createRenderTarget");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_createRenderTarget'", nullptr);
            return 0;
        }
        auto&& ret = obj->createRenderTarget(arg0);
        object_to_luaval<ax::rhi::RenderTarget>(tolua_S, "axr.RenderTarget",(ax::rhi::RenderTarget*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::rhi::Texture* arg0;
        ax::rhi::Texture* arg1;

        ok &= luaval_to_object<ax::rhi::Texture>(tolua_S, 2, "axr.Texture",&arg0, "axr.GraphicsDevice:createRenderTarget");

        ok &= luaval_to_object<ax::rhi::Texture>(tolua_S, 3, "axr.Texture",&arg1, "axr.GraphicsDevice:createRenderTarget");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_createRenderTarget'", nullptr);
            return 0;
        }
        auto&& ret = obj->createRenderTarget(arg0, arg1);
        object_to_luaval<ax::rhi::RenderTarget>(tolua_S, "axr.RenderTarget",(ax::rhi::RenderTarget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:createRenderTarget",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_createRenderTarget'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_createProgram(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_createProgram'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::Data arg0;
        ax::Data arg1;

        luaL_error(tolua_S, "ax::Data unsupported");;

        luaL_error(tolua_S, "ax::Data unsupported");;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_createProgram'", nullptr);
            return 0;
        }
        auto&& ret = obj->createProgram(arg0, arg1);
        object_to_luaval<ax::rhi::Program>(tolua_S, "axr.Program",(ax::rhi::Program*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:createProgram",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_createProgram'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_resetState(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_resetState'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_resetState'", nullptr);
            return 0;
        }
        obj->resetState();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:resetState",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_resetState'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_getVendor(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_getVendor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_getVendor'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVendor();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:getVendor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_getVendor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_getRenderer(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_getRenderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_getRenderer'", nullptr);
            return 0;
        }
        auto&& ret = obj->getRenderer();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:getRenderer",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_getRenderer'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_getVersion(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_getVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_getVersion'", nullptr);
            return 0;
        }
        auto&& ret = obj->getVersion();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:getVersion",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_getVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_getShaderVersion(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_getShaderVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_getShaderVersion'", nullptr);
            return 0;
        }
        auto&& ret = obj->getShaderVersion();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:getShaderVersion",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_getShaderVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_hasExtension(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_hasExtension'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axr.GraphicsDevice:hasExtension");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_hasExtension'", nullptr);
            return 0;
        }
        auto&& ret = obj->hasExtension(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:hasExtension",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_hasExtension'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_dumpExtensions(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_dumpExtensions'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_dumpExtensions'", nullptr);
            return 0;
        }
        auto&& ret = obj->dumpExtensions();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:dumpExtensions",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_dumpExtensions'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_checkForFeatureSupported(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_checkForFeatureSupported'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::rhi::FeatureType arg0;

        ok &= luaval_to_int(tolua_S, 2, &arg0, "axr.GraphicsDevice:checkForFeatureSupported");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_checkForFeatureSupported'", nullptr);
            return 0;
        }
        auto&& ret = obj->checkForFeatureSupported(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:checkForFeatureSupported",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_checkForFeatureSupported'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_getMaxTextureSize(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_getMaxTextureSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_getMaxTextureSize'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMaxTextureSize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:getMaxTextureSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_getMaxTextureSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_getMaxAttributes(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_getMaxAttributes'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_getMaxAttributes'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMaxAttributes();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:getMaxAttributes",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_getMaxAttributes'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_getMaxTextureUnits(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_getMaxTextureUnits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_getMaxTextureUnits'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMaxTextureUnits();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:getMaxTextureUnits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_getMaxTextureUnits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_getMaxSamplesAllowed(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_getMaxSamplesAllowed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_getMaxSamplesAllowed'", nullptr);
            return 0;
        }
        auto&& ret = obj->getMaxSamplesAllowed();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:getMaxSamplesAllowed",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_getMaxSamplesAllowed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_destroyStaleResources(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_destroyStaleResources'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_destroyStaleResources'", nullptr);
            return 0;
        }
        obj->destroyStaleResources();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:destroyStaleResources",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_destroyStaleResources'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_GraphicsDevice_waitForGPU(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::GraphicsDevice* obj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axr.GraphicsDevice",0,&tolua_err)) goto tolua_lerror;
#endif

    obj = (ax::rhi::GraphicsDevice*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!obj)
    {
        tolua_error(tolua_S,"invalid 'obj' in function 'lua_ax_rhi_GraphicsDevice_waitForGPU'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsDevice_waitForGPU'", nullptr);
            return 0;
        }
        obj->waitForGPU();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axr.GraphicsDevice:waitForGPU",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsDevice_waitForGPU'.",&tolua_err);
#endif

    return 0;
}
static int lua_ax_rhi_GraphicsDevice_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GraphicsDevice)");
    return 0;
}

int lua_register_ax_rhi_GraphicsDevice(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axr.GraphicsDevice");
    tolua_cclass(tolua_S,"GraphicsDevice","axr.GraphicsDevice","",nullptr);

    tolua_beginmodule(tolua_S,"GraphicsDevice");
        tolua_function(tolua_S,"createRenderTarget",lua_ax_rhi_GraphicsDevice_createRenderTarget);
        tolua_function(tolua_S,"createProgram",lua_ax_rhi_GraphicsDevice_createProgram);
        tolua_function(tolua_S,"resetState",lua_ax_rhi_GraphicsDevice_resetState);
        tolua_function(tolua_S,"getVendor",lua_ax_rhi_GraphicsDevice_getVendor);
        tolua_function(tolua_S,"getRenderer",lua_ax_rhi_GraphicsDevice_getRenderer);
        tolua_function(tolua_S,"getVersion",lua_ax_rhi_GraphicsDevice_getVersion);
        tolua_function(tolua_S,"getShaderVersion",lua_ax_rhi_GraphicsDevice_getShaderVersion);
        tolua_function(tolua_S,"hasExtension",lua_ax_rhi_GraphicsDevice_hasExtension);
        tolua_function(tolua_S,"dumpExtensions",lua_ax_rhi_GraphicsDevice_dumpExtensions);
        tolua_function(tolua_S,"checkForFeatureSupported",lua_ax_rhi_GraphicsDevice_checkForFeatureSupported);
        tolua_function(tolua_S,"getMaxTextureSize",lua_ax_rhi_GraphicsDevice_getMaxTextureSize);
        tolua_function(tolua_S,"getMaxAttributes",lua_ax_rhi_GraphicsDevice_getMaxAttributes);
        tolua_function(tolua_S,"getMaxTextureUnits",lua_ax_rhi_GraphicsDevice_getMaxTextureUnits);
        tolua_function(tolua_S,"getMaxSamplesAllowed",lua_ax_rhi_GraphicsDevice_getMaxSamplesAllowed);
        tolua_function(tolua_S,"destroyStaleResources",lua_ax_rhi_GraphicsDevice_destroyStaleResources);
        tolua_function(tolua_S,"waitForGPU",lua_ax_rhi_GraphicsDevice_waitForGPU);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::rhi::GraphicsDevice).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.GraphicsDevice";
    g_typeCast[typeName] = "axr.GraphicsDevice";
    return 1;
}

int lua_ax_rhi_GraphicsCore_setPreferredBackend(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::rhi::GraphicsBackend arg0;
        ok &= luaval_to_int(tolua_S, 2, &arg0, "axr.GraphicsCore:setPreferredBackend");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_setPreferredBackend'", nullptr);
            return 0;
        }
        ax::rhi::GraphicsCore::setPreferredBackend(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:setPreferredBackend",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_setPreferredBackend'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_setVulkanMinAndroidApiLevel(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int(tolua_S, 2, &arg0, "axr.GraphicsCore:setVulkanMinAndroidApiLevel");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_setVulkanMinAndroidApiLevel'", nullptr);
            return 0;
        }
        ax::rhi::GraphicsCore::setVulkanMinAndroidApiLevel(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:setVulkanMinAndroidApiLevel",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_setVulkanMinAndroidApiLevel'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_setVulkanInterop(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::rhi::VulkanInterop* arg0;
        ok &= luaval_to_object<ax::rhi::VulkanInterop>(tolua_S, 2, "axr.VulkanInterop",&arg0, "axr.GraphicsCore:setVulkanInterop");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_setVulkanInterop'", nullptr);
            return 0;
        }
        ax::rhi::GraphicsCore::setVulkanInterop(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:setVulkanInterop",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_setVulkanInterop'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_setBackendPriority(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        ax::rhi::GraphicsBackend arg0;
        int arg1;
        ok &= luaval_to_int(tolua_S, 2, &arg0, "axr.GraphicsCore:setBackendPriority");
        ok &= luaval_to_int(tolua_S, 3, &arg1, "axr.GraphicsCore:setBackendPriority");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_setBackendPriority'", nullptr);
            return 0;
        }
        ax::rhi::GraphicsCore::setBackendPriority(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:setBackendPriority",argc, 2);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_setBackendPriority'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_getBackendPriority(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        ax::rhi::GraphicsBackend arg0;
        ok &= luaval_to_int(tolua_S, 2, &arg0, "axr.GraphicsCore:getBackendPriority");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_getBackendPriority'", nullptr);
            return 0;
        }
        auto&& ret = ax::rhi::GraphicsCore::getBackendPriority(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:getBackendPriority",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_getBackendPriority'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_initialize(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_initialize'", nullptr);
            return 0;
        }
        ax::rhi::GraphicsCore::initialize();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:initialize",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_initialize'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_activate(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_activate'", nullptr);
            return 0;
        }
        ax::rhi::GraphicsCore::activate();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:activate",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_activate'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_shutdown(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_shutdown'", nullptr);
            return 0;
        }
        ax::rhi::GraphicsCore::shutdown();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:shutdown",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_shutdown'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_device(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_device'", nullptr);
            return 0;
        }
        auto&& ret = ax::rhi::GraphicsCore::device();
        object_to_luaval<ax::rhi::GraphicsDevice>(tolua_S, "axr.GraphicsDevice",(ax::rhi::GraphicsDevice*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:device",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_device'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_backend(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_backend'", nullptr);
            return 0;
        }
        int ret = (int)ax::rhi::GraphicsCore::backend();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:backend",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_backend'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_isOpenGL(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_isOpenGL'", nullptr);
            return 0;
        }
        auto&& ret = ax::rhi::GraphicsCore::isOpenGL();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:isOpenGL",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_isOpenGL'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_isMetal(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_isMetal'", nullptr);
            return 0;
        }
        auto&& ret = ax::rhi::GraphicsCore::isMetal();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:isMetal",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_isMetal'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_isD3D11(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_isD3D11'", nullptr);
            return 0;
        }
        auto&& ret = ax::rhi::GraphicsCore::isD3D11();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:isD3D11",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_isD3D11'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_isD3D12(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_isD3D12'", nullptr);
            return 0;
        }
        auto&& ret = ax::rhi::GraphicsCore::isD3D12();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:isD3D12",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_isD3D12'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_isVulkan(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_isVulkan'", nullptr);
            return 0;
        }
        auto&& ret = ax::rhi::GraphicsCore::isVulkan();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:isVulkan",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_isVulkan'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_shaderLanguage(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_shaderLanguage'", nullptr);
            return 0;
        }
        auto&& ret = ax::rhi::GraphicsCore::shaderLanguage();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:shaderLanguage",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_shaderLanguage'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_shaderProfile(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_shaderProfile'", nullptr);
            return 0;
        }
        auto&& ret = ax::rhi::GraphicsCore::shaderProfile();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:shaderProfile",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_shaderProfile'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_shaderILProfile(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_shaderILProfile'", nullptr);
            return 0;
        }
        auto&& ret = ax::rhi::GraphicsCore::shaderILProfile();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:shaderILProfile",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_shaderILProfile'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_GraphicsCore_setShaderILProfile(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axr.GraphicsCore",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        int arg0;
        ok &= luaval_to_int(tolua_S, 2, &arg0, "axr.GraphicsCore:setShaderILProfile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_GraphicsCore_setShaderILProfile'", nullptr);
            return 0;
        }
        ax::rhi::GraphicsCore::setShaderILProfile(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axr.GraphicsCore:setShaderILProfile",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_GraphicsCore_setShaderILProfile'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_rhi_GraphicsCore_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (GraphicsCore)");
    return 0;
}

int lua_register_ax_rhi_GraphicsCore(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axr.GraphicsCore");
    tolua_cclass(tolua_S,"GraphicsCore","axr.GraphicsCore","",nullptr);

    tolua_beginmodule(tolua_S,"GraphicsCore");
        tolua_function(tolua_S,"setPreferredBackend", lua_ax_rhi_GraphicsCore_setPreferredBackend);
        tolua_function(tolua_S,"setVulkanMinAndroidApiLevel", lua_ax_rhi_GraphicsCore_setVulkanMinAndroidApiLevel);
        tolua_function(tolua_S,"setVulkanInterop", lua_ax_rhi_GraphicsCore_setVulkanInterop);
        tolua_function(tolua_S,"setBackendPriority", lua_ax_rhi_GraphicsCore_setBackendPriority);
        tolua_function(tolua_S,"getBackendPriority", lua_ax_rhi_GraphicsCore_getBackendPriority);
        tolua_function(tolua_S,"initialize", lua_ax_rhi_GraphicsCore_initialize);
        tolua_function(tolua_S,"activate", lua_ax_rhi_GraphicsCore_activate);
        tolua_function(tolua_S,"shutdown", lua_ax_rhi_GraphicsCore_shutdown);
        tolua_function(tolua_S,"device", lua_ax_rhi_GraphicsCore_device);
        tolua_function(tolua_S,"backend", lua_ax_rhi_GraphicsCore_backend);
        tolua_function(tolua_S,"isOpenGL", lua_ax_rhi_GraphicsCore_isOpenGL);
        tolua_function(tolua_S,"isMetal", lua_ax_rhi_GraphicsCore_isMetal);
        tolua_function(tolua_S,"isD3D11", lua_ax_rhi_GraphicsCore_isD3D11);
        tolua_function(tolua_S,"isD3D12", lua_ax_rhi_GraphicsCore_isD3D12);
        tolua_function(tolua_S,"isVulkan", lua_ax_rhi_GraphicsCore_isVulkan);
        tolua_function(tolua_S,"shaderLanguage", lua_ax_rhi_GraphicsCore_shaderLanguage);
        tolua_function(tolua_S,"shaderProfile", lua_ax_rhi_GraphicsCore_shaderProfile);
        tolua_function(tolua_S,"shaderILProfile", lua_ax_rhi_GraphicsCore_shaderILProfile);
        tolua_function(tolua_S,"setShaderILProfile", lua_ax_rhi_GraphicsCore_setShaderILProfile);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::rhi::GraphicsCore).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axr.GraphicsCore";
    g_typeCast[typeName] = "axr.GraphicsCore";
    return 1;
}
TOLUA_API int register_all_ax_rhi(lua_State* tolua_S)
{
    tolua_open(tolua_S);

    tolua_module(tolua_S,"axrhi",0);
    tolua_beginmodule(tolua_S,"axrhi");

    lua_register_ax_rhi_BufferUsage(tolua_S);
    lua_register_ax_rhi_BufferType(tolua_S);
    lua_register_ax_rhi_ShaderStage(tolua_S);
    lua_register_ax_rhi_PixelFormat(tolua_S);
    lua_register_ax_rhi_TextureUsage(tolua_S);
    lua_register_ax_rhi_IndexFormat(tolua_S);
    lua_register_ax_rhi_PrimitiveType(tolua_S);
    lua_register_ax_rhi_BlendFactor(tolua_S);
    lua_register_ax_rhi_ColorWriteMask(tolua_S);
    lua_register_ax_rhi_CullMode(tolua_S);
    lua_register_ax_rhi_Winding(tolua_S);
    lua_register_ax_rhi_TextureType(tolua_S);
    lua_register_ax_rhi_SamplerAddressMode(tolua_S);
    lua_register_ax_rhi_SamplerFilter(tolua_S);
    lua_register_ax_rhi_TextureCubeFace(tolua_S);
    lua_register_ax_rhi_ShaderCache(tolua_S);
    lua_register_ax_rhi_Program(tolua_S);
    lua_register_ax_rhi_ProgramState(tolua_S);
    lua_register_ax_rhi_Texture(tolua_S);
    lua_register_ax_rhi_VertexLayout(tolua_S);
    lua_register_ax_rhi_GraphicsDevice(tolua_S);
    lua_register_ax_rhi_GraphicsCore(tolua_S);

    tolua_endmodule(tolua_S);
    return 1;
}

