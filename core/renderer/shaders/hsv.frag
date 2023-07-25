#version 310 es
precision highp float;
precision highp int;

#include "colorUtils.glsl"

layout(location = 0) in vec2 v_texCoord;  
layout(location = 1) in vec4 v_fragmentColor;
layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec3 u_hsv;
};

layout(location = 0) out vec4 FragColor;

void main() 
{
    vec4 outColor = texture(u_tex0, v_texCoord);
    outColor.rgb = transformHSV(outColor.rgb, u_hsv);

    FragColor = outColor * v_fragmentColor.a; 
} 
