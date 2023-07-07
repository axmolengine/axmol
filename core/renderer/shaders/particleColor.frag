#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
layout (location = 0) in mediump vec4 ColorOut;
#else
layout (location = 1) in vec4 ColorOut;
#endif
layout(std140, binding = 0) uniform Block_0 {
    vec4 u_color;
};
layout (location = 0) out vec4 FragColor;
void main(void)
{
    FragColor = ColorOut * u_color;
}
