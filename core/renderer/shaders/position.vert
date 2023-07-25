#version 310 es


layout(location = POSITION) in vec4 a_position;

layout(location = 0) out vec4 v_position;


layout(std140) uniform vs_ub {
    mat4 u_MVPMatrix;
};

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_position = a_position;
}
