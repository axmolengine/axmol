#include "lua-bindings/auto/axlua_rhi_auto.hpp"
#include "axmol/rhi/RHITypes.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/rhi/Texture.h"
#include "axmol/rhi/VertexLayout.h"
#include "axmol/rhi/DriverBase.h"
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.BufferUsage";
    g_typeCast[typeName] = "axrhi.BufferUsage";
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.BufferType";
    g_typeCast[typeName] = "axrhi.BufferType";
    return 1;
}


int lua_register_ax_rhi_ShaderStage(lua_State* tolua_S)
{
    tolua_module(tolua_S, "ShaderStage", 0);
    tolua_beginmodule(tolua_S,"ShaderStage");
        tolua_constant(tolua_S, "UNKNOWN", -1);
        tolua_constant(tolua_S, "FRAGMENT", 0);
        tolua_constant(tolua_S, "VERTEX", 1);
        tolua_constant(tolua_S, "DEFAULT", 0);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::ShaderStage).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.ShaderStage";
    g_typeCast[typeName] = "axrhi.ShaderStage";
    return 1;
}


int lua_register_ax_rhi_VertexFormat(lua_State* tolua_S)
{
    tolua_module(tolua_S, "VertexFormat", 0);
    tolua_beginmodule(tolua_S,"VertexFormat");
        tolua_constant(tolua_S, "FLOAT4", 0);
        tolua_constant(tolua_S, "FLOAT3", 1);
        tolua_constant(tolua_S, "FLOAT2", 2);
        tolua_constant(tolua_S, "FLOAT", 3);
        tolua_constant(tolua_S, "INT4", 4);
        tolua_constant(tolua_S, "INT3", 5);
        tolua_constant(tolua_S, "INT2", 6);
        tolua_constant(tolua_S, "INT", 7);
        tolua_constant(tolua_S, "USHORT4", 8);
        tolua_constant(tolua_S, "USHORT2", 9);
        tolua_constant(tolua_S, "UBYTE4", 10);
        tolua_constant(tolua_S, "MAT4", 11);
        tolua_constant(tolua_S, "COUNT", 12);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::VertexFormat).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.VertexFormat";
    g_typeCast[typeName] = "axrhi.VertexFormat";
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
        tolua_constant(tolua_S, "ASTC5x5", 14);
        tolua_constant(tolua_S, "ASTC6x6", 15);
        tolua_constant(tolua_S, "ASTC8x5", 16);
        tolua_constant(tolua_S, "ASTC8x6", 17);
        tolua_constant(tolua_S, "ASTC8x8", 18);
        tolua_constant(tolua_S, "ASTC10x5", 19);
        tolua_constant(tolua_S, "RGBA8", 20);
        tolua_constant(tolua_S, "BGRA8", 21);
        tolua_constant(tolua_S, "RGB8", 22);
        tolua_constant(tolua_S, "RGB565", 23);
        tolua_constant(tolua_S, "RGBA4", 24);
        tolua_constant(tolua_S, "RGB5A1", 25);
        tolua_constant(tolua_S, "R8", 26);
        tolua_constant(tolua_S, "RG8", 27);
        tolua_constant(tolua_S, "RGBA32F", 28);
        tolua_constant(tolua_S, "D24S8", 29);
        tolua_constant(tolua_S, "COUNT", 30);
        tolua_constant(tolua_S, "NONE", -1);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(ax::rhi::PixelFormat).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.PixelFormat";
    g_typeCast[typeName] = "axrhi.PixelFormat";
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.TextureUsage";
    g_typeCast[typeName] = "axrhi.TextureUsage";
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.IndexFormat";
    g_typeCast[typeName] = "axrhi.IndexFormat";
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.PrimitiveType";
    g_typeCast[typeName] = "axrhi.PrimitiveType";
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.BlendFactor";
    g_typeCast[typeName] = "axrhi.BlendFactor";
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.ColorWriteMask";
    g_typeCast[typeName] = "axrhi.ColorWriteMask";
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.CullMode";
    g_typeCast[typeName] = "axrhi.CullMode";
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.Winding";
    g_typeCast[typeName] = "axrhi.Winding";
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.TextureType";
    g_typeCast[typeName] = "axrhi.TextureType";
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.SamplerAddressMode";
    g_typeCast[typeName] = "axrhi.SamplerAddressMode";
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.SamplerFilter";
    g_typeCast[typeName] = "axrhi.SamplerFilter";
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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.TextureCubeFace";
    g_typeCast[typeName] = "axrhi.TextureCubeFace";
    return 1;
}

int lua_ax_rhi_Program_getUniformLocation(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Program",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Program_getUniformLocation'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            ax::rhi::Uniform arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axrhi.Program:getUniformLocation");

            if (!ok) { break; }
            ax::rhi::UniformLocation ret = cobj->getUniformLocation(arg0);
            uniformLocation_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axrhi.Program:getUniformLocation");

            if (!ok) { break; }
            ax::rhi::UniformLocation ret = cobj->getUniformLocation(arg0);
            uniformLocation_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axrhi.Program:getUniformLocation",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getUniformLocation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getVertexInputDesc(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Program",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Program_getVertexInputDesc'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            ax::rhi::VertexInputKind arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axrhi.Program:getVertexInputDesc");

            if (!ok) { break; }
            const ax::rhi::VertexInputDesc* ret = cobj->getVertexInputDesc(arg0);
            #pragma warning NO CONVERSION FROM NATIVE FOR VertexInputDesc*;
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axrhi.Program:getVertexInputDesc");

            if (!ok) { break; }
            const ax::rhi::VertexInputDesc* ret = cobj->getVertexInputDesc(arg0);
            #pragma warning NO CONVERSION FROM NATIVE FOR VertexInputDesc*;
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axrhi.Program:getVertexInputDesc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getVertexInputDesc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getMaxVertexLocation(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Program_getMaxVertexLocation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Program_getMaxVertexLocation'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaxVertexLocation();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Program:getMaxVertexLocation",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getMaxVertexLocation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getMaxFragmentLocation(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Program_getMaxFragmentLocation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Program_getMaxFragmentLocation'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaxFragmentLocation();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Program:getMaxFragmentLocation",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getMaxFragmentLocation'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getVertexShaderSource(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Program_getVertexShaderSource'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Program_getVertexShaderSource'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getVertexShaderSource();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Program:getVertexShaderSource",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getVertexShaderSource'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getFragmentShaderSource(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Program_getFragmentShaderSource'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Program_getFragmentShaderSource'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getFragmentShaderSource();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Program:getFragmentShaderSource",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getFragmentShaderSource'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getProgramType(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Program_getProgramType'", nullptr);
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
        auto&& ret = cobj->getProgramType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Program:getProgramType",argc, 0);
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
    ax::rhi::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Program_getProgramId'", nullptr);
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
        auto&& ret = cobj->getProgramId();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Program:getProgramId",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getProgramId'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getUniformBufferSize(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Program_getUniformBufferSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::rhi::ShaderStage arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axrhi.Program:getUniformBufferSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Program_getUniformBufferSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getUniformBufferSize(arg0);
        #pragma warning NO CONVERSION FROM NATIVE FOR ??;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Program:getUniformBufferSize",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getUniformBufferSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Program_getVertexLayout(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Program_getVertexLayout'", nullptr);
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
        auto&& ret = cobj->getVertexLayout();
        object_to_luaval<ax::rhi::VertexLayout>(tolua_S, "axrhi.VertexLayout",(ax::rhi::VertexLayout*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Program:getVertexLayout",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Program_getVertexLayout'.",&tolua_err);
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
    tolua_usertype(tolua_S,"axrhi.Program");
    tolua_cclass(tolua_S,"Program","axrhi.Program","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Program");
        tolua_function(tolua_S,"getUniformLocation",lua_ax_rhi_Program_getUniformLocation);
        tolua_function(tolua_S,"getVertexInputDesc",lua_ax_rhi_Program_getVertexInputDesc);
        tolua_function(tolua_S,"getMaxVertexLocation",lua_ax_rhi_Program_getMaxVertexLocation);
        tolua_function(tolua_S,"getMaxFragmentLocation",lua_ax_rhi_Program_getMaxFragmentLocation);
        tolua_function(tolua_S,"getVertexShaderSource",lua_ax_rhi_Program_getVertexShaderSource);
        tolua_function(tolua_S,"getFragmentShaderSource",lua_ax_rhi_Program_getFragmentShaderSource);
        tolua_function(tolua_S,"getProgramType",lua_ax_rhi_Program_getProgramType);
        tolua_function(tolua_S,"getProgramId",lua_ax_rhi_Program_getProgramId);
        tolua_function(tolua_S,"getUniformBufferSize",lua_ax_rhi_Program_getUniformBufferSize);
        tolua_function(tolua_S,"getVertexLayout",lua_ax_rhi_Program_getVertexLayout);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::rhi::Program).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.Program";
    g_typeCast[typeName] = "axrhi.Program";
    return 1;
}

int lua_ax_rhi_VertexLayout_getDesc(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::VertexLayout* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_VertexLayout_getDesc'", nullptr);
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
        auto&& ret = cobj->getDesc();
        #pragma warning NO CONVERSION FROM NATIVE FOR VertexLayoutDesc;
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.VertexLayout:getDesc",argc, 0);
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
    ax::rhi::VertexLayout* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_VertexLayout_getStride'", nullptr);
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
        auto&& ret = cobj->getStride();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.VertexLayout:getStride",argc, 0);
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
    ax::rhi::VertexLayout* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_VertexLayout_getInstanceStride'", nullptr);
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
        auto&& ret = cobj->getInstanceStride();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.VertexLayout:getInstanceStride",argc, 0);
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
    ax::rhi::VertexLayout* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_VertexLayout_getHash'", nullptr);
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
        auto&& ret = cobj->getHash();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.VertexLayout:getHash",argc, 0);
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
    ax::rhi::VertexLayout* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_VertexLayout_getBuiltinId'", nullptr);
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
        auto&& ret = cobj->getBuiltinId();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.VertexLayout:getBuiltinId",argc, 0);
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
    tolua_usertype(tolua_S,"axrhi.VertexLayout");
    tolua_cclass(tolua_S,"VertexLayout","axrhi.VertexLayout","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"VertexLayout");
        tolua_function(tolua_S,"getDesc",lua_ax_rhi_VertexLayout_getDesc);
        tolua_function(tolua_S,"getStride",lua_ax_rhi_VertexLayout_getStride);
        tolua_function(tolua_S,"getInstanceStride",lua_ax_rhi_VertexLayout_getInstanceStride);
        tolua_function(tolua_S,"getHash",lua_ax_rhi_VertexLayout_getHash);
        tolua_function(tolua_S,"getBuiltinId",lua_ax_rhi_VertexLayout_getBuiltinId);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::rhi::VertexLayout).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.VertexLayout";
    g_typeCast[typeName] = "axrhi.VertexLayout";
    return 1;
}

int lua_ax_rhi_ProgramState_clone(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_ProgramState_clone'", nullptr);
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
        auto&& ret = cobj->clone();
        object_to_luaval<ax::rhi::ProgramState>(tolua_S, "axrhi.ProgramState",(ax::rhi::ProgramState*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.ProgramState:clone",argc, 0);
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
    ax::rhi::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_ProgramState_getProgram'", nullptr);
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
        auto&& ret = cobj->getProgram();
        object_to_luaval<ax::rhi::Program>(tolua_S, "axrhi.Program",(ax::rhi::Program*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.ProgramState:getProgram",argc, 0);
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
    ax::rhi::ProgramState* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_ProgramState_getVertexInputDesc'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            ax::rhi::VertexInputKind arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axrhi.ProgramState:getVertexInputDesc");

            if (!ok) { break; }
            const ax::rhi::VertexInputDesc* ret = cobj->getVertexInputDesc(arg0);
            #pragma warning NO CONVERSION FROM NATIVE FOR VertexInputDesc*;
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axrhi.ProgramState:getVertexInputDesc");

            if (!ok) { break; }
            const ax::rhi::VertexInputDesc* ret = cobj->getVertexInputDesc(arg0);
            #pragma warning NO CONVERSION FROM NATIVE FOR VertexInputDesc*;
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axrhi.ProgramState:getVertexInputDesc",argc, 1);
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
    ax::rhi::ProgramState* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_ProgramState_setTexture'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 3) {
            ax::rhi::UniformLocation arg0;
            ok &= luaval_to_uniformLocation(tolua_S, 2, arg0, "axrhi.ProgramState:setTexture");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "axrhi.ProgramState:setTexture");

            if (!ok) { break; }
            ax::rhi::Texture* arg2;
            ok &= luaval_to_object<ax::rhi::Texture>(tolua_S, 4, "axrhi.Texture",&arg2, "axrhi.ProgramState:setTexture");

            if (!ok) { break; }
            cobj->setTexture(arg0, arg1, arg2);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            ax::rhi::Texture* arg0;
            ok &= luaval_to_object<ax::rhi::Texture>(tolua_S, 2, "axrhi.Texture",&arg0, "axrhi.ProgramState:setTexture");

            if (!ok) { break; }
            cobj->setTexture(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axrhi.ProgramState:setTexture",argc, 1);
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
    ax::rhi::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_ProgramState_setParameterAutoBinding'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axrhi.ProgramState:setParameterAutoBinding");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axrhi.ProgramState:setParameterAutoBinding");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ProgramState_setParameterAutoBinding'", nullptr);
            return 0;
        }
        cobj->setParameterAutoBinding(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.ProgramState:setParameterAutoBinding",argc, 2);
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
    ax::rhi::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_ProgramState_getBuiltinVertexLayout'", nullptr);
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
        auto&& ret = cobj->getBuiltinVertexLayout();
        object_to_luaval<ax::rhi::VertexLayout>(tolua_S, "axrhi.VertexLayout",(ax::rhi::VertexLayout*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.ProgramState:getBuiltinVertexLayout",argc, 0);
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
    ax::rhi::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_ProgramState_getBatchId'", nullptr);
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
        auto&& ret = cobj->getBatchId();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.ProgramState:getBatchId",argc, 0);
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
    ax::rhi::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_ProgramState_isBatchable'", nullptr);
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
        auto&& ret = cobj->isBatchable();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.ProgramState:isBatchable",argc, 0);
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
    ax::rhi::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_ProgramState_updateBatchId'", nullptr);
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
        cobj->updateBatchId();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.ProgramState:updateBatchId",argc, 0);
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
    ax::rhi::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::rhi::Program* arg0;

        ok &= luaval_to_object<ax::rhi::Program>(tolua_S, 2, "axrhi.Program",&arg0, "axrhi.ProgramState:ProgramState");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_ProgramState_constructor'", nullptr);
            return 0;
        }
        cobj = new ax::rhi::ProgramState(arg0);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_object(tolua_S, ID, luaID, (void*)cobj,"axrhi.ProgramState");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.ProgramState:ProgramState",argc, 1);
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
    tolua_usertype(tolua_S,"axrhi.ProgramState");
    tolua_cclass(tolua_S,"ProgramState","axrhi.ProgramState","ax.Object",nullptr);

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
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.ProgramState";
    g_typeCast[typeName] = "axrhi.ProgramState";
    return 1;
}

int lua_ax_rhi_Texture_updateSamplerDesc(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_updateSamplerDesc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::rhi::SamplerDesc arg0;

        ok &= luaval_to_samplerDesc(tolua_S, 2, arg0, "axrhi.Texture:updateSamplerDesc");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_updateSamplerDesc'", nullptr);
            return 0;
        }
        cobj->updateSamplerDesc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:updateSamplerDesc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_updateSamplerDesc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_updateTextureDesc(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_updateTextureDesc'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::rhi::TextureDesc arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR TextureDesc
        ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_updateTextureDesc'", nullptr);
            return 0;
        }
        cobj->updateTextureDesc(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:updateTextureDesc",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_updateTextureDesc'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_getPixelFormat(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_getPixelFormat'", nullptr);
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
        int ret = (int)cobj->getPixelFormat();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:getPixelFormat",argc, 0);
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
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_getTextureUsage'", nullptr);
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
        int ret = (int)cobj->getTextureUsage();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:getTextureUsage",argc, 0);
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
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_getTextureType'", nullptr);
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
        int ret = (int)cobj->getTextureType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:getTextureType",argc, 0);
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
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_hasMipmaps'", nullptr);
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
        auto&& ret = cobj->hasMipmaps();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:hasMipmaps",argc, 0);
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
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_getWidth'", nullptr);
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
        auto&& ret = cobj->getWidth();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:getWidth",argc, 0);
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
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_getHeight'", nullptr);
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
        auto&& ret = cobj->getHeight();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:getHeight",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_getHeight'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_updateData(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_updateData'", nullptr);
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

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "axrhi.Texture:updateData");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "axrhi.Texture:updateData");

        ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "axrhi.Texture:updateData");

        ok &= luaval_to_int32(tolua_S, 6,(int *)&arg4, "axrhi.Texture:updateData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_updateData'", nullptr);
            return 0;
        }
        cobj->updateData(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:updateData",argc, 5);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_updateData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_updateSubData(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_updateSubData'", nullptr);
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

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axrhi.Texture:updateSubData");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "axrhi.Texture:updateSubData");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "axrhi.Texture:updateSubData");

        ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "axrhi.Texture:updateSubData");

        ok &= luaval_to_int32(tolua_S, 6,(int *)&arg4, "axrhi.Texture:updateSubData");

        #pragma warning NO CONVERSION TO NATIVE FOR void*
        ok = false;

        ok &= luaval_to_int32(tolua_S, 8,(int *)&arg6, "axrhi.Texture:updateSubData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_updateSubData'", nullptr);
            return 0;
        }
        cobj->updateSubData(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:updateSubData",argc, 7);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_updateSubData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_updateFaceData(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_updateFaceData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        ax::rhi::TextureCubeFace arg0;
        const void* arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axrhi.Texture:updateFaceData");

        #pragma warning NO CONVERSION TO NATIVE FOR void*
        ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_updateFaceData'", nullptr);
            return 0;
        }
        cobj->updateFaceData(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:updateFaceData",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_updateFaceData'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_invalidate(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_invalidate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_invalidate'", nullptr);
            return 0;
        }
        cobj->invalidate();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:invalidate",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_invalidate'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_shouldGenMipmaps(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_shouldGenMipmaps'", nullptr);
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
        auto&& ret = cobj->shouldGenMipmaps();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    if (argc == 1)
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axrhi.Texture:shouldGenMipmaps");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_shouldGenMipmaps'", nullptr);
            return 0;
        }
        auto&& ret = cobj->shouldGenMipmaps(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:shouldGenMipmaps",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_shouldGenMipmaps'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_Texture_zeroTexData(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::Texture* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.Texture",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::Texture*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_Texture_zeroTexData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_Texture_zeroTexData'", nullptr);
            return 0;
        }
        cobj->zeroTexData();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.Texture:zeroTexData",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_Texture_zeroTexData'.",&tolua_err);
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
    tolua_usertype(tolua_S,"axrhi.Texture");
    tolua_cclass(tolua_S,"Texture","axrhi.Texture","ax.Object",nullptr);

    tolua_beginmodule(tolua_S,"Texture");
        tolua_function(tolua_S,"updateSamplerDesc",lua_ax_rhi_Texture_updateSamplerDesc);
        tolua_function(tolua_S,"updateTextureDesc",lua_ax_rhi_Texture_updateTextureDesc);
        tolua_function(tolua_S,"getPixelFormat",lua_ax_rhi_Texture_getPixelFormat);
        tolua_function(tolua_S,"getTextureUsage",lua_ax_rhi_Texture_getTextureUsage);
        tolua_function(tolua_S,"getTextureType",lua_ax_rhi_Texture_getTextureType);
        tolua_function(tolua_S,"hasMipmaps",lua_ax_rhi_Texture_hasMipmaps);
        tolua_function(tolua_S,"getWidth",lua_ax_rhi_Texture_getWidth);
        tolua_function(tolua_S,"getHeight",lua_ax_rhi_Texture_getHeight);
        tolua_function(tolua_S,"updateData",lua_ax_rhi_Texture_updateData);
        tolua_function(tolua_S,"updateSubData",lua_ax_rhi_Texture_updateSubData);
        tolua_function(tolua_S,"updateFaceData",lua_ax_rhi_Texture_updateFaceData);
        tolua_function(tolua_S,"invalidate",lua_ax_rhi_Texture_invalidate);
        tolua_function(tolua_S,"shouldGenMipmaps",lua_ax_rhi_Texture_shouldGenMipmaps);
        tolua_function(tolua_S,"zeroTexData",lua_ax_rhi_Texture_zeroTexData);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::rhi::Texture).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.Texture";
    g_typeCast[typeName] = "axrhi.Texture";
    return 1;
}

int lua_ax_rhi_DriverBase_createDefaultRenderTarget(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_createDefaultRenderTarget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_createDefaultRenderTarget'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createDefaultRenderTarget();
        object_to_luaval<ax::rhi::RenderTarget>(tolua_S, "axrhi.RenderTarget",(ax::rhi::RenderTarget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:createDefaultRenderTarget",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_createDefaultRenderTarget'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_createRenderTarget(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_createRenderTarget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_createRenderTarget'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createRenderTarget();
        object_to_luaval<ax::rhi::RenderTarget>(tolua_S, "axrhi.RenderTarget",(ax::rhi::RenderTarget*)ret);
        return 1;
    }
    if (argc == 1)
    {
        ax::rhi::Texture* arg0;

        ok &= luaval_to_object<ax::rhi::Texture>(tolua_S, 2, "axrhi.Texture",&arg0, "axrhi.DriverBase:createRenderTarget");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_createRenderTarget'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createRenderTarget(arg0);
        object_to_luaval<ax::rhi::RenderTarget>(tolua_S, "axrhi.RenderTarget",(ax::rhi::RenderTarget*)ret);
        return 1;
    }
    if (argc == 2)
    {
        ax::rhi::Texture* arg0;
        ax::rhi::Texture* arg1;

        ok &= luaval_to_object<ax::rhi::Texture>(tolua_S, 2, "axrhi.Texture",&arg0, "axrhi.DriverBase:createRenderTarget");

        ok &= luaval_to_object<ax::rhi::Texture>(tolua_S, 3, "axrhi.Texture",&arg1, "axrhi.DriverBase:createRenderTarget");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_createRenderTarget'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createRenderTarget(arg0, arg1);
        object_to_luaval<ax::rhi::RenderTarget>(tolua_S, "axrhi.RenderTarget",(ax::rhi::RenderTarget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:createRenderTarget",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_createRenderTarget'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_createProgram(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_createProgram'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axrhi.DriverBase:createProgram");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axrhi.DriverBase:createProgram");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_createProgram'", nullptr);
            return 0;
        }
        auto&& ret = cobj->createProgram(arg0, arg1);
        object_to_luaval<ax::rhi::Program>(tolua_S, "axrhi.Program",(ax::rhi::Program*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:createProgram",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_createProgram'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_resetState(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_resetState'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_resetState'", nullptr);
            return 0;
        }
        cobj->resetState();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:resetState",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_resetState'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_getVendor(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_getVendor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_getVendor'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getVendor();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:getVendor",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_getVendor'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_getRenderer(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_getRenderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_getRenderer'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getRenderer();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:getRenderer",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_getRenderer'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_getVersion(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_getVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_getVersion'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getVersion();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:getVersion",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_getVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_getShaderVersion(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_getShaderVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_getShaderVersion'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getShaderVersion();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:getShaderVersion",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_getShaderVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_hasExtension(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_hasExtension'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        std::string_view arg0;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axrhi.DriverBase:hasExtension");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_hasExtension'", nullptr);
            return 0;
        }
        auto&& ret = cobj->hasExtension(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:hasExtension",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_hasExtension'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_dumpExtensions(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_dumpExtensions'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_dumpExtensions'", nullptr);
            return 0;
        }
        auto&& ret = cobj->dumpExtensions();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:dumpExtensions",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_dumpExtensions'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_checkForFeatureSupported(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_checkForFeatureSupported'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1)
    {
        ax::rhi::FeatureType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axrhi.DriverBase:checkForFeatureSupported");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_checkForFeatureSupported'", nullptr);
            return 0;
        }
        auto&& ret = cobj->checkForFeatureSupported(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:checkForFeatureSupported",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_checkForFeatureSupported'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_getMaxTextureSize(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_getMaxTextureSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_getMaxTextureSize'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaxTextureSize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:getMaxTextureSize",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_getMaxTextureSize'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_getMaxAttributes(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_getMaxAttributes'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_getMaxAttributes'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaxAttributes();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:getMaxAttributes",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_getMaxAttributes'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_getMaxTextureUnits(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_getMaxTextureUnits'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_getMaxTextureUnits'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaxTextureUnits();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:getMaxTextureUnits",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_getMaxTextureUnits'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_getMaxSamplesAllowed(lua_State* tolua_S)
{
    int argc = 0;
    ax::rhi::DriverBase* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (ax::rhi::DriverBase*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_ax_rhi_DriverBase_getMaxSamplesAllowed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_getMaxSamplesAllowed'", nullptr);
            return 0;
        }
        auto&& ret = cobj->getMaxSamplesAllowed();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axrhi.DriverBase:getMaxSamplesAllowed",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_getMaxSamplesAllowed'.",&tolua_err);
#endif

    return 0;
}
int lua_ax_rhi_DriverBase_getInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_getInstance'", nullptr);
            return 0;
        }
        auto&& ret = ax::rhi::DriverBase::getInstance();
        object_to_luaval<ax::rhi::DriverBase>(tolua_S, "axrhi.DriverBase",(ax::rhi::DriverBase*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axrhi.DriverBase:getInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_getInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_ax_rhi_DriverBase_destroyInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axrhi.DriverBase",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_ax_rhi_DriverBase_destroyInstance'", nullptr);
            return 0;
        }
        ax::rhi::DriverBase::destroyInstance();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axrhi.DriverBase:destroyInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_ax_rhi_DriverBase_destroyInstance'.",&tolua_err);
#endif
    return 0;
}
static int lua_ax_rhi_DriverBase_finalize(lua_State* tolua_S)
{
    AXLOGV("luabindings: finalizing LUA object (DriverBase)");
    return 0;
}

int lua_register_ax_rhi_DriverBase(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axrhi.DriverBase");
    tolua_cclass(tolua_S,"DriverBase","axrhi.DriverBase","",nullptr);

    tolua_beginmodule(tolua_S,"DriverBase");
        tolua_function(tolua_S,"createDefaultRenderTarget",lua_ax_rhi_DriverBase_createDefaultRenderTarget);
        tolua_function(tolua_S,"createRenderTarget",lua_ax_rhi_DriverBase_createRenderTarget);
        tolua_function(tolua_S,"createProgram",lua_ax_rhi_DriverBase_createProgram);
        tolua_function(tolua_S,"resetState",lua_ax_rhi_DriverBase_resetState);
        tolua_function(tolua_S,"getVendor",lua_ax_rhi_DriverBase_getVendor);
        tolua_function(tolua_S,"getRenderer",lua_ax_rhi_DriverBase_getRenderer);
        tolua_function(tolua_S,"getVersion",lua_ax_rhi_DriverBase_getVersion);
        tolua_function(tolua_S,"getShaderVersion",lua_ax_rhi_DriverBase_getShaderVersion);
        tolua_function(tolua_S,"hasExtension",lua_ax_rhi_DriverBase_hasExtension);
        tolua_function(tolua_S,"dumpExtensions",lua_ax_rhi_DriverBase_dumpExtensions);
        tolua_function(tolua_S,"checkForFeatureSupported",lua_ax_rhi_DriverBase_checkForFeatureSupported);
        tolua_function(tolua_S,"getMaxTextureSize",lua_ax_rhi_DriverBase_getMaxTextureSize);
        tolua_function(tolua_S,"getMaxAttributes",lua_ax_rhi_DriverBase_getMaxAttributes);
        tolua_function(tolua_S,"getMaxTextureUnits",lua_ax_rhi_DriverBase_getMaxTextureUnits);
        tolua_function(tolua_S,"getMaxSamplesAllowed",lua_ax_rhi_DriverBase_getMaxSamplesAllowed);
        tolua_function(tolua_S,"getInstance", lua_ax_rhi_DriverBase_getInstance);
        tolua_function(tolua_S,"destroyInstance", lua_ax_rhi_DriverBase_destroyInstance);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(ax::rhi::DriverBase).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axrhi.DriverBase";
    g_typeCast[typeName] = "axrhi.DriverBase";
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
    lua_register_ax_rhi_VertexFormat(tolua_S);
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
    lua_register_ax_rhi_Program(tolua_S);
    lua_register_ax_rhi_VertexLayout(tolua_S);
    lua_register_ax_rhi_ProgramState(tolua_S);
    lua_register_ax_rhi_Texture(tolua_S);
    lua_register_ax_rhi_DriverBase(tolua_S);

    tolua_endmodule(tolua_S);
    return 1;
}

