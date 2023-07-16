#version 310 es


layout(location = 0) in vec4 a_position;

layout(location = 0) out vec4 v_fragmentColor;

layout(std140) uniform vs_ub {
    vec4 u_color;
    mat4 u_MVPMatrix;
};

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_fragmentColor = u_color;
}
