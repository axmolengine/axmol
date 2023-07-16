#version 310 es
precision highp float;
precision highp int;


layout(location = 0) in vec2 v_texCoord;
layout(location = 1) in vec4 v_color;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = v_color;
}

