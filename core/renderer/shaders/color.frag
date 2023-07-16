#version 310 es
precision highp float;
precision highp int;


layout(location = 0) in vec4 DestinationColor;

layout(std140, binding = 0) uniform fs_ub {
    vec4 u_color;
};

layout(location = 0) out vec4 FragColor;

void main(void)
{
    FragColor = u_color;
}
