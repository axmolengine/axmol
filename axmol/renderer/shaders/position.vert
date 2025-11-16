#version 310 es


layout(location = POSITION) in vec3 a_position;

layout(std140, set = 0, binding = 0) uniform vs_ub {
    mat4 u_MVPMatrix;
};

void main()
{
    gl_Position = u_MVPMatrix * vec4(a_position, 1.0f);
}
