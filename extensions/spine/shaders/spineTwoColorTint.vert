#version 310 es

layout(location = POSITION) in vec4 a_position;
layout(location = COLOR0) in vec4 a_color;
layout(location = COLOR1) in vec4 a_color2;
layout(location = TEXCOORD0) in vec2 a_texCoord;

layout(location = COLOR0) out vec4 v_light;
layout(location = COLOR1) out vec4 v_dark;
layout(location = TEXCOORD0) out vec2 v_texCoord;

layout(std140) uniform vs_ub {
    mat4 u_PMatrix;
};

void main() {
    v_light = a_color;
    v_dark = a_color2;
    v_texCoord = a_texCoord;
    gl_Position = u_PMatrix * a_position;
}
