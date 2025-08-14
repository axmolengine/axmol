#version 310 es
precision highp float;
precision highp int;

#include "base.glsl"
#include "colorUtils.glsl"

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0; // Y sample: LumaTexture
layout(binding = 1) uniform sampler2D u_tex1; // UV sample: ChromaTexture

layout(std140) uniform fs_ub {
    mat4 colorTransform;
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    vec3 YUV;

    YUV.x = texture(u_tex0, v_texCoord).x; // Y
    YUV.yz = texture(u_tex1, v_texCoord).RG8_CHANNEL; // CbCr

    /* Convert YUV to RGB */
    vec4 OutColor;
    OutColor.xyz = trasnformYUV(YUV, colorTransform);
    OutColor.w = 1.0;

    FragColor = v_color * OutColor;
}
