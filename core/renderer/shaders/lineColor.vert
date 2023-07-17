#version 310 es

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;


layout(location = 0) out vec4 v_fragmentColor;

layout(std140) uniform vs_ub {
    mat4 u_MVPMatrix;
};

void main()
{
    gl_Position = u_MVPMatrix *  vec4(a_position, 1.0);
    v_fragmentColor = a_color;
}
