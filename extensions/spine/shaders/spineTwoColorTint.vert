#version 310 es

in vec4 a_position;
in vec4 a_color;
in vec4 a_color2;
in vec2 a_texCoords;

out vec4 v_light;
out vec4 v_dark;
out vec2 v_texCoord;

layout(std140) uniform vs_ub {
    mat4 u_PMatrix;
};

void main() {
    v_light = a_color;
    v_dark = a_color2;
    v_texCoord = a_texCoords;
    gl_Position = u_PMatrix * a_position;
}
