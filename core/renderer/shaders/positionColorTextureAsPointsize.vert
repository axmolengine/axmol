#version 310 es


layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;

layout(location = 2) in vec2 a_texCoord;

layout(location = 0) out vec4 v_fragmentColor;

layout(std140) uniform vs_ub {
    float u_alpha;
    mat4 u_MVPMatrix;
};

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    gl_PointSize = a_texCoord.x;
    v_fragmentColor = vec4(a_color.rgb * a_color.a * u_alpha, a_color.a * u_alpha);
}
