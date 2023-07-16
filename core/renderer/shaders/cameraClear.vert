#version 310 es

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texCoord;

layout(location = 0) out vec2 v_texCoord;
layout(location = 1) out vec4 v_color;

layout(std140, binding = 0) uniform vs_ub {
    float depth;
};

void main()
{
    gl_Position = a_position;
    gl_Position.z = depth;
    gl_Position.w = 1.0;
    v_texCoord = a_texCoord;
    v_color = a_color;
}

