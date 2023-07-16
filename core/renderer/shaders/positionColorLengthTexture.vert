#version 310 es




layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec4 a_color;

layout(location = 0) out vec4 v_color;
layout(location = 1) out vec2 v_texCoord;



layout(std140, binding = 0) uniform vs_ub {
    float u_alpha;
    mat4 u_MVPMatrix;
};

void main()
{
    v_color = vec4(a_color.rgb * a_color.a * u_alpha, a_color.a * u_alpha);
    v_texCoord = a_texCoord;

    gl_Position = u_MVPMatrix * a_position;
}
