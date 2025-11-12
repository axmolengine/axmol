#version 310 es

layout(location = POSITION) in vec2 a_position;

layout(location = COLOR0) out vec4 v_color;

layout(std140, set = 0, binding = 0) uniform vs_ub {
    vec4 u_color;
    mat4 u_MVPMatrix;
};

void main()
{
    gl_Position = u_MVPMatrix * vec4(a_position, 0.0, 1.0);
    v_color = u_color;
}
