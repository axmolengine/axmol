#version 310 es


layout(location = POSITION) in vec4 a_position;
layout(location = COLOR0) in vec4 a_color;

layout(location = TEXCOORD0) in vec2 a_texCoord;

layout(location = COLOR0) out vec4 v_color;

layout(std140) uniform vs_ub {
    float u_alpha;
    mat4 u_MVPMatrix;
};

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    gl_PointSize = a_texCoord.x;
    v_color = vec4(a_color.rgb * a_color.a * u_alpha, a_color.a * u_alpha);
}
