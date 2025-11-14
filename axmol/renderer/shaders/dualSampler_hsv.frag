#version 310 es
precision highp float;
precision highp int;
precision highp sampler2DArray;

#include "colorUtils.glsl"

layout(location = TEXCOORD0) in vec2 v_texCoord;
layout(location = COLOR0) in vec4 v_color;

layout(set = 1, binding = 0) uniform sampler2DArray u_tex0;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec3 u_hsv;
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    vec3 uv0 = vec3(v_texCoord, 0.0);
    vec3 uv1 = vec3(v_texCoord, 1.0);
    vec4 texColor = vec4(texture(u_tex0, uv0).rgb, texture(u_tex0, uv1).r);
    texColor.rgb *= texColor.a; // Premultiply with Alpha channel

    texColor.rgb = transformHSV(texColor.rgb, u_hsv);

    FragColor = texColor * v_color;
}
