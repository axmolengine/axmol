#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
layout (location = 0) in lowp vec4 DestinationColor;
#else
layout (location = 1) in vec4 DestinationColor;
#endif
layout(std140, binding = 0) uniform Block_0 {
    vec4 u_color;
};
layout (location = 0) out vec4 FragColor;
void main(void)
{
    FragColor = u_color;
}
