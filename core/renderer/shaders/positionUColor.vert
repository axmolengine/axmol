#version 310 es


layout(location = POSITION) in vec4 a_position;

layout(location = COLOR0) out vec4 v_color;

layout(std140) uniform vs_ub {
    vec4 u_color;
    mat4 u_MVPMatrix;
};

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_color = u_color;
}
