#version 310 es
precision highp float;
precision highp int;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec3 OutLineColor;
    vec4 u_color;
};

layout(location = SV_Target0) out vec4 FragColor;

void main(void)
{
    FragColor =  vec4(OutLineColor,1.0) * u_color;
}
