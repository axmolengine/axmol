#version 310 es
precision highp float;
precision highp int;

layout(location = TEXCOORD0) in vec3 v_reflect;
layout(set = 1, binding = 0) uniform samplerCube u_cubeTex;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec4 u_color;
};

layout(location = SV_Target0) out vec4 FragColor;

void main(void)
{
    FragColor = texture(u_cubeTex, v_reflect) * u_color;
}
