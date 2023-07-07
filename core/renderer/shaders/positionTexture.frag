#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
layout (location = 0) in mediump vec2 v_texCoord;
#else
layout (location = 1) in vec2 v_texCoord;
#endif
layout(location = 2, binding = 0) uniform sampler2D u_tex0;
layout (location = 0) out vec4 FragColor;
void main()
{
    FragColor =  texture(u_tex0, v_texCoord);
}
