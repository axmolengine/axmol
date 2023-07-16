#version 310 es
precision highp float;
precision highp int;


layout(location = 0) in vec4 ColorOut;

layout(std140, binding = 0) uniform fs_ub {
    vec4 u_color;
};

layout(location = 0) out vec4 FragColor;

void main(void)
{
    FragColor = ColorOut * u_color;
}
