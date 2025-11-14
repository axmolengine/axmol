#version 310 es
#include "base.glsl"
precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(set = SAMPLER_SET, binding = 0) uniform sampler2D u_tex0;

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    FragColor = v_color * texture(u_tex0, v_texCoord);
}
