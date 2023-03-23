#include <string_view>

/*
* refer to: https://github.com/doyoulikerock/D3D11NV12Rendering/blob/master/D3D11NV12Rendering/PixelShader.hlsl
// Converting 8-bit YUV to RGB888
static const float3x3 YUVtoRGBCoeffMatrix =
{
    1.164383,  1.164383, 1.164383,
    0.000000, -0.391762, 2.017232,
    1.596027, -0.812968, 0.000000
};
*/

const std::string_view videoTextureNV12_frag = R"(
#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform sampler2D u_tex0; // Y sample
uniform sampler2D u_tex1; // UV sample
uniform vec2 uv_scale;
uniform float out_w;

const mat3 YUVtoRGBCoeff = mat3(
    1.16438356, 1.16438356,  1.16438356,
    0.00000000, -0.213237017, 2.11241937,
    1.79265225, -0.533004045, 0.00000000
);

const vec3 YUVOffsets = vec3(0.0627451017, 0.501960814, 0.501960814);

vec3 YuvToRgb(vec3 YUV)
{
    YUV -= YUVOffsets;
    return YUVtoRGBCoeff * YUV;
}

void main()
{
    vec3 YUV;

    /* For dual sampler */
    //vec2 tXY = v_texCoord;
    //YUV.x = texture2D(u_tex0, tXY).x;
    //tXY.y += 0.015625; // why needs adjust 1.0/64 ?
    //YUV.yz = texture2D(u_tex1, tXY).xw;

    /* For single sampler */
    vec2 tXY = v_texCoord * uv_scale;
    YUV.x = texture2D(u_tex0, tXY).x;
    
    tXY.y *= 0.5;
    tXY.y += 2.0 / 3.0;
    
    float UVOffs = floor(v_texCoord.x * out_w / 2.0) * 2;
    float UPos = ((UVOffs * uv_scale.x) + 0.5) / out_w;
    float VPos = ((UVOffs * uv_scale.x) + 1.5) / out_w;
    
    YUV.y = texture2D(u_tex0, vec2(UPos, tXY.y)).x;
    YUV.z = texture2D(u_tex0, vec2(VPos, tXY.y)).x;

    /* Convert YUV to RGB */
    vec4 OutColor;
    OutColor.xyz = YuvToRgb(YUV);
    OutColor.w = 1.0;

    gl_FragColor = v_fragmentColor * OutColor;
}
)"sv;

// refer to:
// https://docs.microsoft.com/en-us/windows/win32/medfound/recommended-8-bit-yuv-formats-for-video-rendering#yuy2
const std::string_view videoTextureYUY2_frag = R"(

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform sampler2D u_tex0; // Y sample
uniform sampler2D u_tex1; // UV sample
uniform vec2 uv_scale;
uniform float out_w; // texture width

const mat3 YUVtoRGBCoeff = mat3(
    1.16438356, 1.16438356,  1.16438356,
    0.00000000, -0.213237017, 2.11241937,
    1.79265225, -0.533004045, 0.00000000
);

const vec3 YUVOffsets = vec3(0.0627451017, 0.501960814, 0.501960814);

vec3 YuvToRgb(vec3 YUV)
{
    YUV -= YUVOffsets;
    return YUVtoRGBCoeff * YUV;
}

void main()
{
    vec2 tXY = v_texCoord * uv_scale;

    vec3 YUV;
    
    /* For dual sampler */
    YUV.yz = texture2D(u_tex1, tXY).yw;
    YUV.x = texture2D(u_tex0, tXY).x;
	
    /* For single sampler */
    //YUV.yz = texture2D(u_tex0, tXY).yw;
    //
    //vec4 YUY2P = texture2D(u_tex0, tXY);
    //float Pos = v_texCoord.x * out_w;
    //YUV.x = floor(mod(Pos, 2.0)) == 0.0 ? YUY2P.z : YUY2P.x;

    /* Convert YUV to RGB */
    vec4 OutColor;
    OutColor.xyz = YuvToRgb(YUV);
    OutColor.w = 1.0;

    gl_FragColor = v_fragmentColor * OutColor;
}
)"sv;


const std::string_view videoTextureBGRA_frag = R"(
#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform sampler2D u_tex0;

void main()
{
    gl_FragColor = v_fragmentColor * texture2D(u_tex0, v_texCoord).bgra;
}
)"sv;
