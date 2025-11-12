#version 450

layout (location = POSITION) in vec4 a_position;
layout (location = TEXCOORD0) in vec2 a_texCoord;
layout (location = TEXCOORD1) in mat4 a_instance;
layout (location = TEXCOORD0) out vec2 v_texCoord;

layout(std140, binding = 0, set = 0, binding = 0) uniform vs_ub {
    mat4 u_MVPMatrix;
};

void main(void)
{
    gl_Position = u_MVPMatrix * a_instance * a_position;
    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
}
