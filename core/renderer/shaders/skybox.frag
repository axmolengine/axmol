#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
layout (location = 0) in mediump vec3        v_reflect;
#else
layout (location = 1) in vec3        v_reflect;
#endif
layout(location = 3, binding = 0) uniform samplerCube u_Env;
layout(std140, binding = 0) uniform Block_0 {
    vec4 u_color;
};
layout (location = 0) out vec4 FragColor;
void main(void)
{
    FragColor = texture(u_Env, v_reflect) * u_color;
}
