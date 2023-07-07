#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
layout (location = 0) in lowp vec4 v_fragmentColor;
layout (location = 1) in mediump vec2 v_texCoord;
#else
layout (location = 2) in vec4 v_fragmentColor;
layout (location = 3) in vec2 v_texCoord;
#endif
layout(location = 4, binding = 0) uniform sampler2D u_tex0;
layout (location = 0) out vec4 FragColor;
void main()
{
    FragColor = v_fragmentColor * texture(u_tex0, v_texCoord).bgra;
}
