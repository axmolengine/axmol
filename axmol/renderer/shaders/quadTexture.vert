#version 310 es


layout(location = POSITION) in vec4 a_position;
layout(location = COLOR0) in vec4 a_color;
layout(location = TEXCOORD0) in vec2 a_texCoord;

layout(location = TEXCOORD0) out vec2 v_texCoord;
layout(location = COLOR0) out vec4 v_color;


layout(std140) uniform vs_ub {
    mat4 u_MVPMatrix;
};

void main()
{
    v_color = a_color;
    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
    gl_Position = u_MVPMatrix * a_position;
}

