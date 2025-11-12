#version 310 es

layout(location = POSITION) in vec4 a_position;
layout(location = TEXCOORD0) in vec2 a_texCoord;

layout(location = TEXCOORD0) out vec2 v_texCoord;

layout(std140, set = 0, binding = 0) uniform vs_ub {
    mat4 u_matrix;
};

void main()
{
    vec4 pos = vec4(a_position.x, a_position.y, 0.0, 1.0);
    gl_Position = u_matrix * pos;
    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
}
