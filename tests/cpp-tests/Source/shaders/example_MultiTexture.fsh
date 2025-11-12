#version 310 es
precision highp float;
precision highp int;


layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(set = 1, binding = 0) uniform sampler2D u_tex0;
layout(set = 1, binding = 1) uniform sampler2D u_tex1;


layout(std140, set = 0, binding = 1) uniform fs_ub {
    float u_interpolate;
};

layout(location = SV_Target0) out vec4 FragColor;

void main() {
    vec4 color1 = texture(u_tex0, v_texCoord);
    vec4 color2 = texture(u_tex1, v_texCoord);
    FragColor = v_color * mix( color1, color2, u_interpolate);
}

