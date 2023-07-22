#version 310 es
precision highp float;
precision highp int;


layout(location = 0) in vec4 v_fragmentColor;
layout(location = 1) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;
layout(binding = 1) uniform sampler2D u_tex1;


layout(std140, binding = 0) uniform fs_ub {
    float u_interpolate;
};

layout(location = 0) out vec4 FragColor;

void main() {
    vec4 color1 = texture(u_tex0, v_texCoord);
    vec4 color2 = texture(u_tex1, v_texCoord);
    FragColor = v_fragmentColor * mix( color1, color2, u_interpolate);
}

