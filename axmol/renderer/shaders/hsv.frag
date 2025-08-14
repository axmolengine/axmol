#version 310 es
precision highp float;
precision highp int;

#include "colorUtils.glsl"

layout(location = TEXCOORD0) in vec2 v_texCoord;
layout(location = COLOR0) in vec4 v_color;
layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec3 u_hsv;
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    vec4 outColor = texture(u_tex0, v_texCoord);
    outColor.rgb = transformHSV(outColor.rgb, u_hsv);

    FragColor = outColor * v_color.a;
}
