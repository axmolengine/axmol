#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
layout (location = 0) in mediump vec2 v_texCoord;
layout (location = 1) in mediump vec4 v_color;
#else
layout (location = 2) in vec2 v_texCoord;
layout (location = 3) in vec4 v_color;
#endif
layout (location = 0) out vec4 FragColor;
void main()
{
    FragColor = v_color;
}
