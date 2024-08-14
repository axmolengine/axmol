#version 310 es


layout(location = POSITION) in vec4 a_position;
layout(location = COLOR0) in vec4 a_color;

layout(location = COLOR0) out vec4 v_color;


layout(std140) uniform vs_ub {
    mat4 u_MVPMatrix;
};

void main()
{
    v_color = a_color;
    gl_Position = u_MVPMatrix * a_position;
}

