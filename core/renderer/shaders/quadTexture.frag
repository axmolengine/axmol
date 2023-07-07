#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
layout (location = 0) in mediump vec2 TextureCoordOut;
layout (location = 1) in mediump vec4 ColorOut;
#else
layout (location = 2) in vec4 ColorOut;
layout (location = 3) in vec2 TextureCoordOut;
#endif
layout(std140, binding = 0) uniform Block_0 {
    vec4 u_color;
};
layout(location = 4, binding = 0) uniform sampler2D u_tex0;
layout (location = 0) out vec4 FragColor;
void main(void)
{
    FragColor = texture(u_tex0, TextureCoordOut) * ColorOut * u_color;
}
