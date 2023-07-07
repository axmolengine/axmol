#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
precision lowp float;
#endif
layout (location = 0) in vec4 v_fragmentColor;
layout (location = 0) out vec4 FragColor;
void main()
{
    FragColor = v_fragmentColor;
}
