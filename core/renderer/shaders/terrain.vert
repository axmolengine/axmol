#version 310 es
precision highp float;
precision highp int;
layout (location = 0) in vec4 a_position;
layout (location = 1) in vec2 a_texCoord;
layout (location = 2) in vec3 a_normal;
#ifdef GL_ES
layout (location = 0) out mediump vec2 v_texCoord;
layout (location = 1) out mediump vec3 v_normal;
#else
layout (location = 2) out vec2 v_texCoord;
layout (location = 3) out vec3 v_normal;
#endif
layout(std140, binding = 0) uniform Block_0 {
    mat4 u_MVPMatrix;
};
void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_texCoord = a_texCoord;
    v_normal = a_normal;
}
