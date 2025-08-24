#version 310 es


layout(location = POSITION) in vec3 a_position;
layout(location = NORMAL) in vec3 a_normal;
layout(location = TEXCOORD0) in vec2 a_texCoord;

layout(location = TEXCOORD0) out vec2 v_texCoord;


layout(std140) uniform vs_ub {
    mat4 u_MVPMatrix;
};

void main(void)
{
    gl_Position = u_MVPMatrix * vec4(a_position.xyz, 1.0f);
    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
}
