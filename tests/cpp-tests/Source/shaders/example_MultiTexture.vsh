#version 310 es

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec4 a_color;

layout(location = 0) out vec4 v_fragmentColor;
layout(location = 1) out vec2 v_texCoord;


layout(std140, binding = 0) uniform vs_ub {
    mat4 u_MVPMatrix;
};

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_fragmentColor = a_color;
    v_texCoord = a_texCoord;
}
