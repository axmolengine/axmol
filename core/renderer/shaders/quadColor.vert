#version 310 es

                                              
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;

layout(location = 0) out vec4 ColorOut;


layout(std140, binding = 0) uniform vs_ub {
    mat4 u_MVPMatrix;
};

void main()
{
    ColorOut = a_color;
    gl_Position = u_MVPMatrix * a_position;
}

