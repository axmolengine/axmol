#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
// #extension GL_OES_standard_derivatives : enable
layout (location = 0) in mediump vec4 v_color;
layout (location = 1) in mediump vec2 v_texCoord;
#else
layout (location = 2) in vec4 v_color;
layout (location = 3) in vec2 v_texCoord;
#endif
layout (location = 0) out vec4 FragColor;
void main()
{
// #if defined GL_OES_standard_derivatives
// FragColor = v_color*smoothstep(0.0, length(fwidth(v_texCoord)), 1.0 - length(v_texCoord));
// #else
    FragColor = v_color*step(0.0, 1.0 - length(v_texCoord));
// #endif
}
