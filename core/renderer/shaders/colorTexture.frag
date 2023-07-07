#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
layout (location = 0) in mediump vec2 TextureCoordOut;
#else
layout (location = 1) in vec2 TextureCoordOut;
#endif
layout(std140, binding = 0) uniform Block_0 {
    vec4 u_color;
};
layout(location = 2, binding = 0) uniform sampler2D u_tex0; 
layout (location = 0) out vec4 FragColor;
void main(void)
{
    FragColor = texture(u_tex0, TextureCoordOut) * u_color;
}
