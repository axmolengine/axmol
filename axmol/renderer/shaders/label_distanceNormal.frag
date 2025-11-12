#version 310 es
precision highp float;
precision highp int;
#include "base.glsl"

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(set = 1, binding = 0) uniform sampler2D u_tex0;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec4 u_textColor;
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    float dist = texture(u_tex0, v_texCoord).x;
    float smoothing = fwidth(dist);

    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist) * u_textColor.a;
    FragColor = v_color * vec4(u_textColor.rgb,alpha);
}
