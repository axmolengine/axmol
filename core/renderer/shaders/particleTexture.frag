#version 310 es
precision highp float;
precision highp int;


layout(location = 0) in vec2 TextureCoordOut;
layout(location = 1) in vec4 ColorOut;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140, binding = 0) uniform fs_ub {
    vec4 u_color;
};

layout(location = 0) out vec4 FragColor;

void main(void)
{
    FragColor = texture(u_tex0, TextureCoordOut) * ColorOut * u_color;
}
