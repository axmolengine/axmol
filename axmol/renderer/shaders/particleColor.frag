#version 310 es
precision highp float;
precision highp int;


layout(location = COLOR0) in vec4 v_color;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec4 u_color;
};

layout(location = SV_Target0) out vec4 FragColor;

void main(void)
{
    FragColor = v_color * u_color;
}
