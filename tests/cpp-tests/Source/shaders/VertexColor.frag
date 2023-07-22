#version 310 es
precision highp float;
precision highp int;

layout(location = 0) in vec4 v_fragmentColor;

layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = v_fragmentColor;
}