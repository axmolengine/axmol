#version 310 es

layout(location = POSITION) in vec4 a_position;
layout(location = TEXCOORD0) in vec2 a_texCoord;
layout(location = NORMAL) in vec3 a_normal;
layout(location = TEXCOORD0) out vec2 v_texCoord;
layout(location = NORMAL) out vec3 v_normal;

layout(std140) uniform vs_ub {
    mat4 u_MVPMatrix;
};

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_texCoord = a_texCoord;
    v_normal = a_normal;
}
