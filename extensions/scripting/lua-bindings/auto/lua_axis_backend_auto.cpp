#include "scripting/lua-bindings/auto/lua_axis_backend_auto.hpp"
#include "renderer/backend/Types.h"
#include "renderer/backend/ProgramState.h"
#include "renderer/backend/Texture.h"
#include "renderer/backend/VertexLayout.h"
#include "renderer/backend/Device.h"
#include "renderer/backend/RenderTarget.h"
#include "scripting/lua-bindings/manual/tolua_fix.h"
#include "scripting/lua-bindings/manual/LuaBasicConversions.h"


int lua_register_axis_backend_BufferUsage(lua_State* tolua_S)
{
    tolua_module(tolua_S, "BufferUsage", 0);
    tolua_beginmodule(tolua_S,"BufferUsage");
        tolua_constant(tolua_S, "STATIC", 0);
        tolua_constant(tolua_S, "DYNAMIC", 1);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::BufferUsage).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.BufferUsage";
    g_typeCast[typeName] = "axb.BufferUsage";
    return 1;
}


int lua_register_axis_backend_BufferType(lua_State* tolua_S)
{
    tolua_module(tolua_S, "BufferType", 0);
    tolua_beginmodule(tolua_S,"BufferType");
        tolua_constant(tolua_S, "VERTEX", 0);
        tolua_constant(tolua_S, "INDEX", 1);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::BufferType).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.BufferType";
    g_typeCast[typeName] = "axb.BufferType";
    return 1;
}


int lua_register_axis_backend_ShaderStage(lua_State* tolua_S)
{
    tolua_module(tolua_S, "ShaderStage", 0);
    tolua_beginmodule(tolua_S,"ShaderStage");
        tolua_constant(tolua_S, "VERTEX", 0);
        tolua_constant(tolua_S, "FRAGMENT", 1);
        tolua_constant(tolua_S, "VERTEX_AND_FRAGMENT", 2);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::ShaderStage).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.ShaderStage";
    g_typeCast[typeName] = "axb.ShaderStage";
    return 1;
}


int lua_register_axis_backend_VertexFormat(lua_State* tolua_S)
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
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::VertexFormat).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.VertexFormat";
    g_typeCast[typeName] = "axb.VertexFormat";
    return 1;
}


int lua_register_axis_backend_PixelFormat(lua_State* tolua_S)
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
        tolua_constant(tolua_S, "A8", 26);
        tolua_constant(tolua_S, "L8", 27);
        tolua_constant(tolua_S, "LA8", 28);
        tolua_constant(tolua_S, "D24S8", 29);
        tolua_constant(tolua_S, "COUNT", 30);
        tolua_constant(tolua_S, "NONE", 65535);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::PixelFormat).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.PixelFormat";
    g_typeCast[typeName] = "axb.PixelFormat";
    return 1;
}


int lua_register_axis_backend_TextureUsage(lua_State* tolua_S)
{
    tolua_module(tolua_S, "TextureUsage", 0);
    tolua_beginmodule(tolua_S,"TextureUsage");
        tolua_constant(tolua_S, "READ", 0);
        tolua_constant(tolua_S, "WRITE", 1);
        tolua_constant(tolua_S, "RENDER_TARGET", 2);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::TextureUsage).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.TextureUsage";
    g_typeCast[typeName] = "axb.TextureUsage";
    return 1;
}


int lua_register_axis_backend_IndexFormat(lua_State* tolua_S)
{
    tolua_module(tolua_S, "IndexFormat", 0);
    tolua_beginmodule(tolua_S,"IndexFormat");
        tolua_constant(tolua_S, "U_SHORT", 1);
        tolua_constant(tolua_S, "U_INT", 2);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::IndexFormat).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.IndexFormat";
    g_typeCast[typeName] = "axb.IndexFormat";
    return 1;
}


int lua_register_axis_backend_VertexStepMode(lua_State* tolua_S)
{
    tolua_module(tolua_S, "VertexStepMode", 0);
    tolua_beginmodule(tolua_S,"VertexStepMode");
        tolua_constant(tolua_S, "VERTEX", 0);
        tolua_constant(tolua_S, "INSTANCE", 1);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::VertexStepMode).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.VertexStepMode";
    g_typeCast[typeName] = "axb.VertexStepMode";
    return 1;
}


int lua_register_axis_backend_PrimitiveType(lua_State* tolua_S)
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

    auto typeName = typeid(axis::backend::PrimitiveType).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.PrimitiveType";
    g_typeCast[typeName] = "axb.PrimitiveType";
    return 1;
}


int lua_register_axis_backend_TextureType(lua_State* tolua_S)
{
    tolua_module(tolua_S, "TextureType", 0);
    tolua_beginmodule(tolua_S,"TextureType");
        tolua_constant(tolua_S, "TEXTURE_2D", 0);
        tolua_constant(tolua_S, "TEXTURE_CUBE", 1);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::TextureType).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.TextureType";
    g_typeCast[typeName] = "axb.TextureType";
    return 1;
}


int lua_register_axis_backend_SamplerAddressMode(lua_State* tolua_S)
{
    tolua_module(tolua_S, "SamplerAddressMode", 0);
    tolua_beginmodule(tolua_S,"SamplerAddressMode");
        tolua_constant(tolua_S, "REPEAT", 0);
        tolua_constant(tolua_S, "MIRROR_REPEAT", 1);
        tolua_constant(tolua_S, "CLAMP_TO_EDGE", 2);
        tolua_constant(tolua_S, "DONT_CARE", 3);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::SamplerAddressMode).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.SamplerAddressMode";
    g_typeCast[typeName] = "axb.SamplerAddressMode";
    return 1;
}


int lua_register_axis_backend_SamplerFilter(lua_State* tolua_S)
{
    tolua_module(tolua_S, "SamplerFilter", 0);
    tolua_beginmodule(tolua_S,"SamplerFilter");
        tolua_constant(tolua_S, "NEAREST", 0);
        tolua_constant(tolua_S, "NEAREST_MIPMAP_NEAREST", 1);
        tolua_constant(tolua_S, "NEAREST_MIPMAP_LINEAR", 2);
        tolua_constant(tolua_S, "LINEAR", 3);
        tolua_constant(tolua_S, "LINEAR_MIPMAP_LINEAR", 4);
        tolua_constant(tolua_S, "LINEAR_MIPMAP_NEAREST", 5);
        tolua_constant(tolua_S, "DONT_CARE", 6);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::SamplerFilter).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.SamplerFilter";
    g_typeCast[typeName] = "axb.SamplerFilter";
    return 1;
}


int lua_register_axis_backend_StencilOperation(lua_State* tolua_S)
{
    tolua_module(tolua_S, "StencilOperation", 0);
    tolua_beginmodule(tolua_S,"StencilOperation");
        tolua_constant(tolua_S, "KEEP", 0);
        tolua_constant(tolua_S, "ZERO", 1);
        tolua_constant(tolua_S, "REPLACE", 2);
        tolua_constant(tolua_S, "INVERT", 3);
        tolua_constant(tolua_S, "INCREMENT_WRAP", 4);
        tolua_constant(tolua_S, "DECREMENT_WRAP", 5);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::StencilOperation).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.StencilOperation";
    g_typeCast[typeName] = "axb.StencilOperation";
    return 1;
}


int lua_register_axis_backend_CompareFunction(lua_State* tolua_S)
{
    tolua_module(tolua_S, "CompareFunction", 0);
    tolua_beginmodule(tolua_S,"CompareFunction");
        tolua_constant(tolua_S, "NEVER", 0);
        tolua_constant(tolua_S, "LESS", 1);
        tolua_constant(tolua_S, "LESS_EQUAL", 2);
        tolua_constant(tolua_S, "GREATER", 3);
        tolua_constant(tolua_S, "GREATER_EQUAL", 4);
        tolua_constant(tolua_S, "EQUAL", 5);
        tolua_constant(tolua_S, "NOT_EQUAL", 6);
        tolua_constant(tolua_S, "ALWAYS", 7);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::CompareFunction).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.CompareFunction";
    g_typeCast[typeName] = "axb.CompareFunction";
    return 1;
}


int lua_register_axis_backend_BlendOperation(lua_State* tolua_S)
{
    tolua_module(tolua_S, "BlendOperation", 0);
    tolua_beginmodule(tolua_S,"BlendOperation");
        tolua_constant(tolua_S, "ADD", 0);
        tolua_constant(tolua_S, "SUBTRACT", 1);
        tolua_constant(tolua_S, "RESERVE_SUBTRACT", 2);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::BlendOperation).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.BlendOperation";
    g_typeCast[typeName] = "axb.BlendOperation";
    return 1;
}


int lua_register_axis_backend_BlendFactor(lua_State* tolua_S)
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
        tolua_constant(tolua_S, "BLEND_CLOLOR", 13);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::BlendFactor).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.BlendFactor";
    g_typeCast[typeName] = "axb.BlendFactor";
    return 1;
}


int lua_register_axis_backend_ColorWriteMask(lua_State* tolua_S)
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

    auto typeName = typeid(axis::backend::ColorWriteMask).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.ColorWriteMask";
    g_typeCast[typeName] = "axb.ColorWriteMask";
    return 1;
}


int lua_register_axis_backend_CullMode(lua_State* tolua_S)
{
    tolua_module(tolua_S, "CullMode", 0);
    tolua_beginmodule(tolua_S,"CullMode");
        tolua_constant(tolua_S, "NONE", 0);
        tolua_constant(tolua_S, "BACK", 1);
        tolua_constant(tolua_S, "FRONT", 2);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::CullMode).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.CullMode";
    g_typeCast[typeName] = "axb.CullMode";
    return 1;
}


int lua_register_axis_backend_Winding(lua_State* tolua_S)
{
    tolua_module(tolua_S, "Winding", 0);
    tolua_beginmodule(tolua_S,"Winding");
        tolua_constant(tolua_S, "CLOCK_WISE", 0);
        tolua_constant(tolua_S, "COUNTER_CLOCK_WISE", 1);
    tolua_endmodule(tolua_S);

    auto typeName = typeid(axis::backend::Winding).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.Winding";
    g_typeCast[typeName] = "axb.Winding";
    return 1;
}


int lua_register_axis_backend_TextureCubeFace(lua_State* tolua_S)
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

    auto typeName = typeid(axis::backend::TextureCubeFace).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.TextureCubeFace";
    g_typeCast[typeName] = "axb.TextureCubeFace";
    return 1;
}

int lua_axis_backend_Program_getMaxVertexLocation(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Program_getMaxVertexLocation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Program_getMaxVertexLocation'", nullptr);
            return 0;
        }
        int ret = cobj->getMaxVertexLocation();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Program:getMaxVertexLocation",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Program_getMaxVertexLocation'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Program_getMaxFragmentLocation(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Program_getMaxFragmentLocation'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Program_getMaxFragmentLocation'", nullptr);
            return 0;
        }
        int ret = cobj->getMaxFragmentLocation();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Program:getMaxFragmentLocation",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Program_getMaxFragmentLocation'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Program_setProgramType(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Program_setProgramType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "axb.Program:setProgramType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Program_setProgramType'", nullptr);
            return 0;
        }
        cobj->setProgramType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Program:setProgramType",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Program_setProgramType'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Program_getFragmentShader(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Program_getFragmentShader'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Program_getFragmentShader'", nullptr);
            return 0;
        }
        std::string_view ret = cobj->getFragmentShader();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Program:getFragmentShader",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Program_getFragmentShader'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Program_getUniformBufferSize(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Program_getUniformBufferSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        axis::backend::ShaderStage arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.Program:getUniformBufferSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Program_getUniformBufferSize'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getUniformBufferSize(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Program:getUniformBufferSize",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Program_getUniformBufferSize'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Program_getUniformLocation(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Program* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Program",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (axis::backend::Program*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Program_getUniformLocation'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            axis::backend::Uniform arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.Program:getUniformLocation");

            if (!ok) { break; }
            axis::backend::UniformLocation ret = cobj->getUniformLocation(arg0);
            uniformLocation_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axb.Program:getUniformLocation");

            if (!ok) { break; }
            axis::backend::UniformLocation ret = cobj->getUniformLocation(arg0);
            uniformLocation_to_luaval(tolua_S, ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axb.Program:getUniformLocation",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Program_getUniformLocation'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Program_getProgramType(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Program_getProgramType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Program_getProgramType'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getProgramType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Program:getProgramType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Program_getProgramType'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Program_getActiveAttributes(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Program_getActiveAttributes'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Program_getActiveAttributes'", nullptr);
            return 0;
        }
        tsl::robin_map<std::string, axis::backend::AttributeBindInfo, hlookup::string_hash, hlookup::equal_to> ret = cobj->getActiveAttributes();
        program_activeattrs_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Program:getActiveAttributes",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Program_getActiveAttributes'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Program_getAttributeLocation(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Program* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Program",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (axis::backend::Program*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Program_getAttributeLocation'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            axis::backend::Attribute arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.Program:getAttributeLocation");

            if (!ok) { break; }
            int ret = cobj->getAttributeLocation(arg0);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axb.Program:getAttributeLocation");

            if (!ok) { break; }
            int ret = cobj->getAttributeLocation(arg0);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axb.Program:getAttributeLocation",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Program_getAttributeLocation'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Program_getVertexShader(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Program* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Program*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Program_getVertexShader'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Program_getVertexShader'", nullptr);
            return 0;
        }
        std::string_view ret = cobj->getVertexShader();
        lua_pushlstring(tolua_S,ret.data(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Program:getVertexShader",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Program_getVertexShader'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Program_getBuiltinProgram(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axb.Program",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        unsigned int arg0;
        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "axb.Program:getBuiltinProgram");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Program_getBuiltinProgram'", nullptr);
            return 0;
        }
        axis::backend::Program* ret = axis::backend::Program::getBuiltinProgram(arg0);
        object_to_luaval<axis::backend::Program>(tolua_S, "axb.Program",(axis::backend::Program*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axb.Program:getBuiltinProgram",argc, 1);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Program_getBuiltinProgram'.",&tolua_err);
#endif
    return 0;
}
static int lua_axis_backend_Program_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Program)");
    return 0;
}

int lua_register_axis_backend_Program(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axb.Program");
    tolua_cclass(tolua_S,"Program","axb.Program","ax.Ref",nullptr);

    tolua_beginmodule(tolua_S,"Program");
        tolua_function(tolua_S,"getMaxVertexLocation",lua_axis_backend_Program_getMaxVertexLocation);
        tolua_function(tolua_S,"getMaxFragmentLocation",lua_axis_backend_Program_getMaxFragmentLocation);
        tolua_function(tolua_S,"setProgramType",lua_axis_backend_Program_setProgramType);
        tolua_function(tolua_S,"getFragmentShader",lua_axis_backend_Program_getFragmentShader);
        tolua_function(tolua_S,"getUniformBufferSize",lua_axis_backend_Program_getUniformBufferSize);
        tolua_function(tolua_S,"getUniformLocation",lua_axis_backend_Program_getUniformLocation);
        tolua_function(tolua_S,"getProgramType",lua_axis_backend_Program_getProgramType);
        tolua_function(tolua_S,"getActiveAttributes",lua_axis_backend_Program_getActiveAttributes);
        tolua_function(tolua_S,"getAttributeLocation",lua_axis_backend_Program_getAttributeLocation);
        tolua_function(tolua_S,"getVertexShader",lua_axis_backend_Program_getVertexShader);
        tolua_function(tolua_S,"getBuiltinProgram", lua_axis_backend_Program_getBuiltinProgram);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(axis::backend::Program).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.Program";
    g_typeCast[typeName] = "axb.Program";
    return 1;
}

int lua_axis_backend_VertexLayout_getVertexStepMode(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::VertexLayout* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_VertexLayout_getVertexStepMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_VertexLayout_getVertexStepMode'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getVertexStepMode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.VertexLayout:getVertexStepMode",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_VertexLayout_getVertexStepMode'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_VertexLayout_isValid(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::VertexLayout* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_VertexLayout_isValid'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_VertexLayout_isValid'", nullptr);
            return 0;
        }
        bool ret = cobj->isValid();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.VertexLayout:isValid",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_VertexLayout_isValid'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_VertexLayout_setLayout(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::VertexLayout* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_VertexLayout_setLayout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        unsigned int arg0;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "axb.VertexLayout:setLayout");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_VertexLayout_setLayout'", nullptr);
            return 0;
        }
        cobj->setLayout(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.VertexLayout:setLayout",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_VertexLayout_setLayout'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_VertexLayout_setAttribute(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::VertexLayout* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_VertexLayout_setAttribute'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 5) 
    {
        std::string_view arg0;
        unsigned int arg1;
        axis::backend::VertexFormat arg2;
        unsigned int arg3;
        bool arg4;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axb.VertexLayout:setAttribute");

        ok &= luaval_to_uint32(tolua_S, 3,&arg1, "axb.VertexLayout:setAttribute");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "axb.VertexLayout:setAttribute");

        ok &= luaval_to_uint32(tolua_S, 5,&arg3, "axb.VertexLayout:setAttribute");

        ok &= luaval_to_boolean(tolua_S, 6,&arg4, "axb.VertexLayout:setAttribute");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_VertexLayout_setAttribute'", nullptr);
            return 0;
        }
        cobj->setAttribute(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.VertexLayout:setAttribute",argc, 5);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_VertexLayout_setAttribute'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_VertexLayout_getStride(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::VertexLayout* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.VertexLayout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::VertexLayout*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_VertexLayout_getStride'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_VertexLayout_getStride'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getStride();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.VertexLayout:getStride",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_VertexLayout_getStride'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_VertexLayout_constructor(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::VertexLayout* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_VertexLayout_constructor'", nullptr);
            return 0;
        }
        cobj = new axis::backend::VertexLayout();
        tolua_pushusertype(tolua_S,(void*)cobj,"axb.VertexLayout");
        tolua_register_gc(tolua_S,lua_gettop(tolua_S));
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.VertexLayout:VertexLayout",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_VertexLayout_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_axis_backend_VertexLayout_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (VertexLayout)");
    return 0;
}

int lua_register_axis_backend_VertexLayout(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axb.VertexLayout");
    tolua_cclass(tolua_S,"VertexLayout","axb.VertexLayout","",nullptr);

    tolua_beginmodule(tolua_S,"VertexLayout");
        tolua_function(tolua_S,"new",lua_axis_backend_VertexLayout_constructor);
        tolua_function(tolua_S,"getVertexStepMode",lua_axis_backend_VertexLayout_getVertexStepMode);
        tolua_function(tolua_S,"isValid",lua_axis_backend_VertexLayout_isValid);
        tolua_function(tolua_S,"setLayout",lua_axis_backend_VertexLayout_setLayout);
        tolua_function(tolua_S,"setAttribute",lua_axis_backend_VertexLayout_setAttribute);
        tolua_function(tolua_S,"getStride",lua_axis_backend_VertexLayout_getStride);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(axis::backend::VertexLayout).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.VertexLayout";
    g_typeCast[typeName] = "axb.VertexLayout";
    return 1;
}

int lua_axis_backend_ProgramState_setTexture(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::ProgramState* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (axis::backend::ProgramState*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_ProgramState_setTexture'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 3) {
            axis::backend::UniformLocation arg0;
            ok &= luaval_to_uniformLocation(tolua_S, 2, arg0, "axb.ProgramState:setTexture");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "axb.ProgramState:setTexture");

            if (!ok) { break; }
            axis::backend::TextureBackend* arg2;
            ok &= luaval_to_object<axis::backend::TextureBackend>(tolua_S, 4, "axb.TextureBackend",&arg2, "axb.ProgramState:setTexture");

            if (!ok) { break; }
            cobj->setTexture(arg0, arg1, arg2);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            axis::backend::TextureBackend* arg0;
            ok &= luaval_to_object<axis::backend::TextureBackend>(tolua_S, 2, "axb.TextureBackend",&arg0, "axb.ProgramState:setTexture");

            if (!ok) { break; }
            cobj->setTexture(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 4) {
            axis::backend::UniformLocation arg0;
            ok &= luaval_to_uniformLocation(tolua_S, 2, arg0, "axb.ProgramState:setTexture");

            if (!ok) { break; }
            int arg1;
            ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "axb.ProgramState:setTexture");

            if (!ok) { break; }
            int arg2;
            ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "axb.ProgramState:setTexture");

            if (!ok) { break; }
            axis::backend::TextureBackend* arg3;
            ok &= luaval_to_object<axis::backend::TextureBackend>(tolua_S, 5, "axb.TextureBackend",&arg3, "axb.ProgramState:setTexture");

            if (!ok) { break; }
            cobj->setTexture(arg0, arg1, arg2, arg3);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axb.ProgramState:setTexture",argc, 4);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_ProgramState_setTexture'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_ProgramState_updateUniformID(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_ProgramState_updateUniformID'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_ProgramState_updateUniformID'", nullptr);
            return 0;
        }
        cobj->updateUniformID();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.ProgramState:updateUniformID");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_ProgramState_updateUniformID'", nullptr);
            return 0;
        }
        cobj->updateUniformID(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.ProgramState:updateUniformID",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_ProgramState_updateUniformID'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_ProgramState_clone(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_ProgramState_clone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_ProgramState_clone'", nullptr);
            return 0;
        }
        axis::backend::ProgramState* ret = cobj->clone();
        object_to_luaval<axis::backend::ProgramState>(tolua_S, "axb.ProgramState",(axis::backend::ProgramState*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.ProgramState:clone",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_ProgramState_clone'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_ProgramState_setParameterAutoBinding(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_ProgramState_setParameterAutoBinding'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axb.ProgramState:setParameterAutoBinding");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axb.ProgramState:setParameterAutoBinding");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_ProgramState_setParameterAutoBinding'", nullptr);
            return 0;
        }
        cobj->setParameterAutoBinding(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.ProgramState:setParameterAutoBinding",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_ProgramState_setParameterAutoBinding'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_ProgramState_getProgram(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_ProgramState_getProgram'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_ProgramState_getProgram'", nullptr);
            return 0;
        }
        axis::backend::Program* ret = cobj->getProgram();
        object_to_luaval<axis::backend::Program>(tolua_S, "axb.Program",(axis::backend::Program*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.ProgramState:getProgram",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_ProgramState_getProgram'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_ProgramState_getUniformID(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::ProgramState*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_ProgramState_getUniformID'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_ProgramState_getUniformID'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getUniformID();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.ProgramState:getUniformID",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_ProgramState_getUniformID'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_ProgramState_getAttributeLocation(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::ProgramState* cobj = nullptr;
    bool ok  = true;
#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.ProgramState",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (axis::backend::ProgramState*)tolua_tousertype(tolua_S,1,0);
#if _AX_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_ProgramState_getAttributeLocation'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            axis::backend::Attribute arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.ProgramState:getAttributeLocation");

            if (!ok) { break; }
            int ret = cobj->getAttributeLocation(arg0);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            std::string_view arg0;
            ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axb.ProgramState:getAttributeLocation");

            if (!ok) { break; }
            int ret = cobj->getAttributeLocation(arg0);
            tolua_pushnumber(tolua_S,(lua_Number)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "axb.ProgramState:getAttributeLocation",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_ProgramState_getAttributeLocation'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_ProgramState_constructor(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::ProgramState* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        axis::backend::Program* arg0;

        ok &= luaval_to_object<axis::backend::Program>(tolua_S, 2, "axb.Program",&arg0, "axb.ProgramState:ProgramState");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_ProgramState_constructor'", nullptr);
            return 0;
        }
        cobj = new axis::backend::ProgramState(arg0);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"axb.ProgramState");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.ProgramState:ProgramState",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_ProgramState_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_axis_backend_ProgramState_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (ProgramState)");
    return 0;
}

int lua_register_axis_backend_ProgramState(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axb.ProgramState");
    tolua_cclass(tolua_S,"ProgramState","axb.ProgramState","ax.Ref",nullptr);

    tolua_beginmodule(tolua_S,"ProgramState");
        tolua_function(tolua_S,"new",lua_axis_backend_ProgramState_constructor);
        tolua_function(tolua_S,"setTexture",lua_axis_backend_ProgramState_setTexture);
        tolua_function(tolua_S,"updateUniformID",lua_axis_backend_ProgramState_updateUniformID);
        tolua_function(tolua_S,"clone",lua_axis_backend_ProgramState_clone);
        tolua_function(tolua_S,"setParameterAutoBinding",lua_axis_backend_ProgramState_setParameterAutoBinding);
        tolua_function(tolua_S,"getProgram",lua_axis_backend_ProgramState_getProgram);
        tolua_function(tolua_S,"getUniformID",lua_axis_backend_ProgramState_getUniformID);
        tolua_function(tolua_S,"getAttributeLocation",lua_axis_backend_ProgramState_getAttributeLocation);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(axis::backend::ProgramState).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.ProgramState";
    g_typeCast[typeName] = "axb.ProgramState";
    return 1;
}

int lua_axis_backend_TextureBackend_getTextureFormat(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::TextureBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.TextureBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::TextureBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_TextureBackend_getTextureFormat'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_getTextureFormat'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getTextureFormat();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.TextureBackend:getTextureFormat",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_TextureBackend_getTextureFormat'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_TextureBackend_generateMipmaps(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::TextureBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.TextureBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::TextureBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_TextureBackend_generateMipmaps'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_generateMipmaps'", nullptr);
            return 0;
        }
        cobj->generateMipmaps();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.TextureBackend:generateMipmaps",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_TextureBackend_generateMipmaps'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_TextureBackend_updateSamplerDescriptor(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::TextureBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.TextureBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::TextureBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_TextureBackend_updateSamplerDescriptor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        axis::backend::SamplerDescriptor arg0;

        ok &= luaval_to_samplerDescriptor(tolua_S, 2, arg0, "axb.TextureBackend:updateSamplerDescriptor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_updateSamplerDescriptor'", nullptr);
            return 0;
        }
        cobj->updateSamplerDescriptor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.TextureBackend:updateSamplerDescriptor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_TextureBackend_updateSamplerDescriptor'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_TextureBackend_getHeight(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::TextureBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.TextureBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::TextureBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_TextureBackend_getHeight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_getHeight'", nullptr);
            return 0;
        }
        int ret = cobj->getHeight();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.TextureBackend:getHeight",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_TextureBackend_getHeight'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_TextureBackend_updateTextureDescriptor(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::TextureBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.TextureBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::TextureBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_TextureBackend_updateTextureDescriptor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        axis::backend::TextureDescriptor arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR TextureDescriptor
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_updateTextureDescriptor'", nullptr);
            return 0;
        }
        cobj->updateTextureDescriptor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        axis::backend::TextureDescriptor arg0;
        int arg1;

        #pragma warning NO CONVERSION TO NATIVE FOR TextureDescriptor
		ok = false;

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "axb.TextureBackend:updateTextureDescriptor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_updateTextureDescriptor'", nullptr);
            return 0;
        }
        cobj->updateTextureDescriptor(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.TextureBackend:updateTextureDescriptor",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_TextureBackend_updateTextureDescriptor'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_TextureBackend_getTextureUsage(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::TextureBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.TextureBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::TextureBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_TextureBackend_getTextureUsage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_getTextureUsage'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getTextureUsage();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.TextureBackend:getTextureUsage",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_TextureBackend_getTextureUsage'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_TextureBackend_getWidth(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::TextureBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.TextureBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::TextureBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_TextureBackend_getWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_getWidth'", nullptr);
            return 0;
        }
        int ret = cobj->getWidth();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.TextureBackend:getWidth",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_TextureBackend_getWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_TextureBackend_hasMipmaps(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::TextureBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.TextureBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::TextureBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_TextureBackend_hasMipmaps'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_hasMipmaps'", nullptr);
            return 0;
        }
        bool ret = cobj->hasMipmaps();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.TextureBackend:hasMipmaps",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_TextureBackend_hasMipmaps'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_TextureBackend_getCount(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::TextureBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.TextureBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::TextureBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_TextureBackend_getCount'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_getCount'", nullptr);
            return 0;
        }
        int ret = cobj->getCount();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.TextureBackend:getCount",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_TextureBackend_getCount'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_TextureBackend_getHandler(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::TextureBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.TextureBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::TextureBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_TextureBackend_getHandler'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_getHandler'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getHandler();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.TextureBackend:getHandler");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_getHandler'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getHandler(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.TextureBackend:getHandler",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_TextureBackend_getHandler'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_TextureBackend_getTextureType(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::TextureBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.TextureBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::TextureBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_TextureBackend_getTextureType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureBackend_getTextureType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getTextureType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.TextureBackend:getTextureType",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_TextureBackend_getTextureType'.",&tolua_err);
#endif

    return 0;
}
static int lua_axis_backend_TextureBackend_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (TextureBackend)");
    return 0;
}

int lua_register_axis_backend_TextureBackend(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axb.TextureBackend");
    tolua_cclass(tolua_S,"TextureBackend","axb.TextureBackend","ax.Ref",nullptr);

    tolua_beginmodule(tolua_S,"TextureBackend");
        tolua_function(tolua_S,"getTextureFormat",lua_axis_backend_TextureBackend_getTextureFormat);
        tolua_function(tolua_S,"generateMipmaps",lua_axis_backend_TextureBackend_generateMipmaps);
        tolua_function(tolua_S,"updateSamplerDescriptor",lua_axis_backend_TextureBackend_updateSamplerDescriptor);
        tolua_function(tolua_S,"getHeight",lua_axis_backend_TextureBackend_getHeight);
        tolua_function(tolua_S,"updateTextureDescriptor",lua_axis_backend_TextureBackend_updateTextureDescriptor);
        tolua_function(tolua_S,"getTextureUsage",lua_axis_backend_TextureBackend_getTextureUsage);
        tolua_function(tolua_S,"getWidth",lua_axis_backend_TextureBackend_getWidth);
        tolua_function(tolua_S,"hasMipmaps",lua_axis_backend_TextureBackend_hasMipmaps);
        tolua_function(tolua_S,"getCount",lua_axis_backend_TextureBackend_getCount);
        tolua_function(tolua_S,"getHandler",lua_axis_backend_TextureBackend_getHandler);
        tolua_function(tolua_S,"getTextureType",lua_axis_backend_TextureBackend_getTextureType);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(axis::backend::TextureBackend).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.TextureBackend";
    g_typeCast[typeName] = "axb.TextureBackend";
    return 1;
}

int lua_axis_backend_Texture2DBackend_getHeight(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Texture2DBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Texture2DBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Texture2DBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Texture2DBackend_getHeight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Texture2DBackend_getHeight'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getHeight();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Texture2DBackend:getHeight",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Texture2DBackend_getHeight'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Texture2DBackend_getWidth(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Texture2DBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Texture2DBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Texture2DBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Texture2DBackend_getWidth'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Texture2DBackend_getWidth'", nullptr);
            return 0;
        }
        unsigned int ret = cobj->getWidth();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Texture2DBackend:getWidth",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Texture2DBackend_getWidth'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Texture2DBackend_updateData(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Texture2DBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Texture2DBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Texture2DBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Texture2DBackend_updateData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 4) 
    {
        unsigned char* arg0;
        unsigned int arg1;
        unsigned int arg2;
        unsigned int arg3;

        #pragma warning NO CONVERSION TO NATIVE FOR unsigned char*
		ok = false;

        ok &= luaval_to_uint32(tolua_S, 3,&arg1, "axb.Texture2DBackend:updateData");

        ok &= luaval_to_uint32(tolua_S, 4,&arg2, "axb.Texture2DBackend:updateData");

        ok &= luaval_to_uint32(tolua_S, 5,&arg3, "axb.Texture2DBackend:updateData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Texture2DBackend_updateData'", nullptr);
            return 0;
        }
        cobj->updateData(arg0, arg1, arg2, arg3);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 5) 
    {
        unsigned char* arg0;
        unsigned int arg1;
        unsigned int arg2;
        unsigned int arg3;
        int arg4;

        #pragma warning NO CONVERSION TO NATIVE FOR unsigned char*
		ok = false;

        ok &= luaval_to_uint32(tolua_S, 3,&arg1, "axb.Texture2DBackend:updateData");

        ok &= luaval_to_uint32(tolua_S, 4,&arg2, "axb.Texture2DBackend:updateData");

        ok &= luaval_to_uint32(tolua_S, 5,&arg3, "axb.Texture2DBackend:updateData");

        ok &= luaval_to_int32(tolua_S, 6,(int *)&arg4, "axb.Texture2DBackend:updateData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Texture2DBackend_updateData'", nullptr);
            return 0;
        }
        cobj->updateData(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Texture2DBackend:updateData",argc, 4);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Texture2DBackend_updateData'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Texture2DBackend_updateCompressedData(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Texture2DBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Texture2DBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Texture2DBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Texture2DBackend_updateCompressedData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 5) 
    {
        unsigned char* arg0;
        unsigned int arg1;
        unsigned int arg2;
        unsigned int arg3;
        unsigned int arg4;

        #pragma warning NO CONVERSION TO NATIVE FOR unsigned char*
		ok = false;

        ok &= luaval_to_uint32(tolua_S, 3,&arg1, "axb.Texture2DBackend:updateCompressedData");

        ok &= luaval_to_uint32(tolua_S, 4,&arg2, "axb.Texture2DBackend:updateCompressedData");

        ok &= luaval_to_uint32(tolua_S, 5,&arg3, "axb.Texture2DBackend:updateCompressedData");

        ok &= luaval_to_uint32(tolua_S, 6,&arg4, "axb.Texture2DBackend:updateCompressedData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Texture2DBackend_updateCompressedData'", nullptr);
            return 0;
        }
        cobj->updateCompressedData(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 6) 
    {
        unsigned char* arg0;
        unsigned int arg1;
        unsigned int arg2;
        unsigned int arg3;
        unsigned int arg4;
        int arg5;

        #pragma warning NO CONVERSION TO NATIVE FOR unsigned char*
		ok = false;

        ok &= luaval_to_uint32(tolua_S, 3,&arg1, "axb.Texture2DBackend:updateCompressedData");

        ok &= luaval_to_uint32(tolua_S, 4,&arg2, "axb.Texture2DBackend:updateCompressedData");

        ok &= luaval_to_uint32(tolua_S, 5,&arg3, "axb.Texture2DBackend:updateCompressedData");

        ok &= luaval_to_uint32(tolua_S, 6,&arg4, "axb.Texture2DBackend:updateCompressedData");

        ok &= luaval_to_int32(tolua_S, 7,(int *)&arg5, "axb.Texture2DBackend:updateCompressedData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Texture2DBackend_updateCompressedData'", nullptr);
            return 0;
        }
        cobj->updateCompressedData(arg0, arg1, arg2, arg3, arg4, arg5);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Texture2DBackend:updateCompressedData",argc, 5);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Texture2DBackend_updateCompressedData'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Texture2DBackend_updateSubData(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Texture2DBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Texture2DBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Texture2DBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Texture2DBackend_updateSubData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 6) 
    {
        unsigned int arg0;
        unsigned int arg1;
        unsigned int arg2;
        unsigned int arg3;
        unsigned int arg4;
        unsigned char* arg5;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "axb.Texture2DBackend:updateSubData");

        ok &= luaval_to_uint32(tolua_S, 3,&arg1, "axb.Texture2DBackend:updateSubData");

        ok &= luaval_to_uint32(tolua_S, 4,&arg2, "axb.Texture2DBackend:updateSubData");

        ok &= luaval_to_uint32(tolua_S, 5,&arg3, "axb.Texture2DBackend:updateSubData");

        ok &= luaval_to_uint32(tolua_S, 6,&arg4, "axb.Texture2DBackend:updateSubData");

        #pragma warning NO CONVERSION TO NATIVE FOR unsigned char*
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Texture2DBackend_updateSubData'", nullptr);
            return 0;
        }
        cobj->updateSubData(arg0, arg1, arg2, arg3, arg4, arg5);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 7) 
    {
        unsigned int arg0;
        unsigned int arg1;
        unsigned int arg2;
        unsigned int arg3;
        unsigned int arg4;
        unsigned char* arg5;
        int arg6;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "axb.Texture2DBackend:updateSubData");

        ok &= luaval_to_uint32(tolua_S, 3,&arg1, "axb.Texture2DBackend:updateSubData");

        ok &= luaval_to_uint32(tolua_S, 4,&arg2, "axb.Texture2DBackend:updateSubData");

        ok &= luaval_to_uint32(tolua_S, 5,&arg3, "axb.Texture2DBackend:updateSubData");

        ok &= luaval_to_uint32(tolua_S, 6,&arg4, "axb.Texture2DBackend:updateSubData");

        #pragma warning NO CONVERSION TO NATIVE FOR unsigned char*
		ok = false;

        ok &= luaval_to_int32(tolua_S, 8,(int *)&arg6, "axb.Texture2DBackend:updateSubData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Texture2DBackend_updateSubData'", nullptr);
            return 0;
        }
        cobj->updateSubData(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Texture2DBackend:updateSubData",argc, 6);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Texture2DBackend_updateSubData'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Texture2DBackend_updateCompressedSubData(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Texture2DBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Texture2DBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Texture2DBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Texture2DBackend_updateCompressedSubData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 7) 
    {
        unsigned int arg0;
        unsigned int arg1;
        unsigned int arg2;
        unsigned int arg3;
        unsigned int arg4;
        unsigned int arg5;
        unsigned char* arg6;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "axb.Texture2DBackend:updateCompressedSubData");

        ok &= luaval_to_uint32(tolua_S, 3,&arg1, "axb.Texture2DBackend:updateCompressedSubData");

        ok &= luaval_to_uint32(tolua_S, 4,&arg2, "axb.Texture2DBackend:updateCompressedSubData");

        ok &= luaval_to_uint32(tolua_S, 5,&arg3, "axb.Texture2DBackend:updateCompressedSubData");

        ok &= luaval_to_uint32(tolua_S, 6,&arg4, "axb.Texture2DBackend:updateCompressedSubData");

        ok &= luaval_to_uint32(tolua_S, 7,&arg5, "axb.Texture2DBackend:updateCompressedSubData");

        #pragma warning NO CONVERSION TO NATIVE FOR unsigned char*
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Texture2DBackend_updateCompressedSubData'", nullptr);
            return 0;
        }
        cobj->updateCompressedSubData(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 8) 
    {
        unsigned int arg0;
        unsigned int arg1;
        unsigned int arg2;
        unsigned int arg3;
        unsigned int arg4;
        unsigned int arg5;
        unsigned char* arg6;
        int arg7;

        ok &= luaval_to_uint32(tolua_S, 2,&arg0, "axb.Texture2DBackend:updateCompressedSubData");

        ok &= luaval_to_uint32(tolua_S, 3,&arg1, "axb.Texture2DBackend:updateCompressedSubData");

        ok &= luaval_to_uint32(tolua_S, 4,&arg2, "axb.Texture2DBackend:updateCompressedSubData");

        ok &= luaval_to_uint32(tolua_S, 5,&arg3, "axb.Texture2DBackend:updateCompressedSubData");

        ok &= luaval_to_uint32(tolua_S, 6,&arg4, "axb.Texture2DBackend:updateCompressedSubData");

        ok &= luaval_to_uint32(tolua_S, 7,&arg5, "axb.Texture2DBackend:updateCompressedSubData");

        #pragma warning NO CONVERSION TO NATIVE FOR unsigned char*
		ok = false;

        ok &= luaval_to_int32(tolua_S, 9,(int *)&arg7, "axb.Texture2DBackend:updateCompressedSubData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Texture2DBackend_updateCompressedSubData'", nullptr);
            return 0;
        }
        cobj->updateCompressedSubData(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Texture2DBackend:updateCompressedSubData",argc, 7);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Texture2DBackend_updateCompressedSubData'.",&tolua_err);
#endif

    return 0;
}
static int lua_axis_backend_Texture2DBackend_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Texture2DBackend)");
    return 0;
}

int lua_register_axis_backend_Texture2DBackend(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axb.Texture2DBackend");
    tolua_cclass(tolua_S,"Texture2DBackend","axb.Texture2DBackend","axb.TextureBackend",nullptr);

    tolua_beginmodule(tolua_S,"Texture2DBackend");
        tolua_function(tolua_S,"getHeight",lua_axis_backend_Texture2DBackend_getHeight);
        tolua_function(tolua_S,"getWidth",lua_axis_backend_Texture2DBackend_getWidth);
        tolua_function(tolua_S,"updateData",lua_axis_backend_Texture2DBackend_updateData);
        tolua_function(tolua_S,"updateCompressedData",lua_axis_backend_Texture2DBackend_updateCompressedData);
        tolua_function(tolua_S,"updateSubData",lua_axis_backend_Texture2DBackend_updateSubData);
        tolua_function(tolua_S,"updateCompressedSubData",lua_axis_backend_Texture2DBackend_updateCompressedSubData);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(axis::backend::Texture2DBackend).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.Texture2DBackend";
    g_typeCast[typeName] = "axb.Texture2DBackend";
    return 1;
}

int lua_axis_backend_TextureCubemapBackend_updateFaceData(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::TextureCubemapBackend* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.TextureCubemapBackend",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::TextureCubemapBackend*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_TextureCubemapBackend_updateFaceData'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        axis::backend::TextureCubeFace arg0;
        void* arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.TextureCubemapBackend:updateFaceData");

        #pragma warning NO CONVERSION TO NATIVE FOR void*
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureCubemapBackend_updateFaceData'", nullptr);
            return 0;
        }
        cobj->updateFaceData(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3) 
    {
        axis::backend::TextureCubeFace arg0;
        void* arg1;
        int arg2;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.TextureCubemapBackend:updateFaceData");

        #pragma warning NO CONVERSION TO NATIVE FOR void*
		ok = false;

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "axb.TextureCubemapBackend:updateFaceData");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_TextureCubemapBackend_updateFaceData'", nullptr);
            return 0;
        }
        cobj->updateFaceData(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.TextureCubemapBackend:updateFaceData",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_TextureCubemapBackend_updateFaceData'.",&tolua_err);
#endif

    return 0;
}
static int lua_axis_backend_TextureCubemapBackend_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (TextureCubemapBackend)");
    return 0;
}

int lua_register_axis_backend_TextureCubemapBackend(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axb.TextureCubemapBackend");
    tolua_cclass(tolua_S,"TextureCubemapBackend","axb.TextureCubemapBackend","axb.TextureBackend",nullptr);

    tolua_beginmodule(tolua_S,"TextureCubemapBackend");
        tolua_function(tolua_S,"updateFaceData",lua_axis_backend_TextureCubemapBackend_updateFaceData);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(axis::backend::TextureCubemapBackend).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.TextureCubemapBackend";
    g_typeCast[typeName] = "axb.TextureCubemapBackend";
    return 1;
}

int lua_axis_backend_Device_newDefaultRenderTarget(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Device* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Device",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Device*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Device_newDefaultRenderTarget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        axis::backend::TargetBufferFlags arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.Device:newDefaultRenderTarget");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Device_newDefaultRenderTarget'", nullptr);
            return 0;
        }
        axis::backend::RenderTarget* ret = cobj->newDefaultRenderTarget(arg0);
        object_to_luaval<axis::backend::RenderTarget>(tolua_S, "axb.RenderTarget",(axis::backend::RenderTarget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Device:newDefaultRenderTarget",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Device_newDefaultRenderTarget'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Device_newProgram(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Device* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Device",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Device*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Device_newProgram'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string_view arg0;
        std::string_view arg1;

        ok &= luaval_to_std_string_view(tolua_S, 2,&arg0, "axb.Device:newProgram");

        ok &= luaval_to_std_string_view(tolua_S, 3,&arg1, "axb.Device:newProgram");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Device_newProgram'", nullptr);
            return 0;
        }
        axis::backend::Program* ret = cobj->newProgram(arg0, arg1);
        object_to_luaval<axis::backend::Program>(tolua_S, "axb.Program",(axis::backend::Program*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Device:newProgram",argc, 2);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Device_newProgram'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Device_newRenderTarget(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Device* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Device",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Device*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Device_newRenderTarget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        axis::backend::TargetBufferFlags arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.Device:newRenderTarget");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Device_newRenderTarget'", nullptr);
            return 0;
        }
        axis::backend::RenderTarget* ret = cobj->newRenderTarget(arg0);
        object_to_luaval<axis::backend::RenderTarget>(tolua_S, "axb.RenderTarget",(axis::backend::RenderTarget*)ret);
        return 1;
    }
    if (argc == 2) 
    {
        axis::backend::TargetBufferFlags arg0;
        axis::backend::TextureBackend* arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.Device:newRenderTarget");

        ok &= luaval_to_object<axis::backend::TextureBackend>(tolua_S, 3, "axb.TextureBackend",&arg1, "axb.Device:newRenderTarget");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Device_newRenderTarget'", nullptr);
            return 0;
        }
        axis::backend::RenderTarget* ret = cobj->newRenderTarget(arg0, arg1);
        object_to_luaval<axis::backend::RenderTarget>(tolua_S, "axb.RenderTarget",(axis::backend::RenderTarget*)ret);
        return 1;
    }
    if (argc == 3) 
    {
        axis::backend::TargetBufferFlags arg0;
        axis::backend::TextureBackend* arg1;
        axis::backend::TextureBackend* arg2;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.Device:newRenderTarget");

        ok &= luaval_to_object<axis::backend::TextureBackend>(tolua_S, 3, "axb.TextureBackend",&arg1, "axb.Device:newRenderTarget");

        ok &= luaval_to_object<axis::backend::TextureBackend>(tolua_S, 4, "axb.TextureBackend",&arg2, "axb.Device:newRenderTarget");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Device_newRenderTarget'", nullptr);
            return 0;
        }
        axis::backend::RenderTarget* ret = cobj->newRenderTarget(arg0, arg1, arg2);
        object_to_luaval<axis::backend::RenderTarget>(tolua_S, "axb.RenderTarget",(axis::backend::RenderTarget*)ret);
        return 1;
    }
    if (argc == 4) 
    {
        axis::backend::TargetBufferFlags arg0;
        axis::backend::TextureBackend* arg1;
        axis::backend::TextureBackend* arg2;
        axis::backend::TextureBackend* arg3;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "axb.Device:newRenderTarget");

        ok &= luaval_to_object<axis::backend::TextureBackend>(tolua_S, 3, "axb.TextureBackend",&arg1, "axb.Device:newRenderTarget");

        ok &= luaval_to_object<axis::backend::TextureBackend>(tolua_S, 4, "axb.TextureBackend",&arg2, "axb.Device:newRenderTarget");

        ok &= luaval_to_object<axis::backend::TextureBackend>(tolua_S, 5, "axb.TextureBackend",&arg3, "axb.Device:newRenderTarget");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Device_newRenderTarget'", nullptr);
            return 0;
        }
        axis::backend::RenderTarget* ret = cobj->newRenderTarget(arg0, arg1, arg2, arg3);
        object_to_luaval<axis::backend::RenderTarget>(tolua_S, "axb.RenderTarget",(axis::backend::RenderTarget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Device:newRenderTarget",argc, 1);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Device_newRenderTarget'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Device_newDepthStencilState(lua_State* tolua_S)
{
    int argc = 0;
    axis::backend::Device* cobj = nullptr;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if _AX_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"axb.Device",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (axis::backend::Device*)tolua_tousertype(tolua_S,1,0);

#if _AX_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_axis_backend_Device_newDepthStencilState'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Device_newDepthStencilState'", nullptr);
            return 0;
        }
        axis::backend::DepthStencilState* ret = cobj->newDepthStencilState();
        object_to_luaval<axis::backend::DepthStencilState>(tolua_S, "axb.DepthStencilState",(axis::backend::DepthStencilState*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "axb.Device:newDepthStencilState",argc, 0);
    return 0;

#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Device_newDepthStencilState'.",&tolua_err);
#endif

    return 0;
}
int lua_axis_backend_Device_getInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if _AX_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if _AX_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"axb.Device",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_axis_backend_Device_getInstance'", nullptr);
            return 0;
        }
        axis::backend::Device* ret = axis::backend::Device::getInstance();
        object_to_luaval<axis::backend::Device>(tolua_S, "axb.Device",(axis::backend::Device*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "axb.Device:getInstance",argc, 0);
    return 0;
#if _AX_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_axis_backend_Device_getInstance'.",&tolua_err);
#endif
    return 0;
}
static int lua_axis_backend_Device_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Device)");
    return 0;
}

int lua_register_axis_backend_Device(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"axb.Device");
    tolua_cclass(tolua_S,"Device","axb.Device","ax.Ref",nullptr);

    tolua_beginmodule(tolua_S,"Device");
        tolua_function(tolua_S,"newDefaultRenderTarget",lua_axis_backend_Device_newDefaultRenderTarget);
        tolua_function(tolua_S,"newProgram",lua_axis_backend_Device_newProgram);
        tolua_function(tolua_S,"newRenderTarget",lua_axis_backend_Device_newRenderTarget);
        tolua_function(tolua_S,"newDepthStencilState",lua_axis_backend_Device_newDepthStencilState);
        tolua_function(tolua_S,"getInstance", lua_axis_backend_Device_getInstance);
    tolua_endmodule(tolua_S);
    auto typeName = typeid(axis::backend::Device).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "axb.Device";
    g_typeCast[typeName] = "axb.Device";
    return 1;
}
TOLUA_API int register_all_axis_backend(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"axb",0);
	tolua_beginmodule(tolua_S,"axb");

	lua_register_axis_backend_ColorWriteMask(tolua_S);
	lua_register_axis_backend_TextureType(tolua_S);
	lua_register_axis_backend_PrimitiveType(tolua_S);
	lua_register_axis_backend_SamplerAddressMode(tolua_S);
	lua_register_axis_backend_ProgramState(tolua_S);
	lua_register_axis_backend_IndexFormat(tolua_S);
	lua_register_axis_backend_TextureBackend(tolua_S);
	lua_register_axis_backend_TextureCubemapBackend(tolua_S);
	lua_register_axis_backend_SamplerFilter(tolua_S);
	lua_register_axis_backend_TextureCubeFace(tolua_S);
	lua_register_axis_backend_VertexLayout(tolua_S);
	lua_register_axis_backend_BlendFactor(tolua_S);
	lua_register_axis_backend_Device(tolua_S);
	lua_register_axis_backend_VertexFormat(tolua_S);
	lua_register_axis_backend_VertexStepMode(tolua_S);
	lua_register_axis_backend_StencilOperation(tolua_S);
	lua_register_axis_backend_CompareFunction(tolua_S);
	lua_register_axis_backend_BufferUsage(tolua_S);
	lua_register_axis_backend_TextureUsage(tolua_S);
	lua_register_axis_backend_BufferType(tolua_S);
	lua_register_axis_backend_CullMode(tolua_S);
	lua_register_axis_backend_Winding(tolua_S);
	lua_register_axis_backend_PixelFormat(tolua_S);
	lua_register_axis_backend_Program(tolua_S);
	lua_register_axis_backend_BlendOperation(tolua_S);
	lua_register_axis_backend_ShaderStage(tolua_S);
	lua_register_axis_backend_Texture2DBackend(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

