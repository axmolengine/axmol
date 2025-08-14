#version 310 es
precision highp float;
precision highp int;

#include "base.glsl"

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec4 u_textColor;
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    FragColor =  v_color * vec4(u_textColor.rgb,// RGB from uniform
        u_textColor.a * texture(u_tex0, v_texCoord).x// x from texture & uniform
    );
}
