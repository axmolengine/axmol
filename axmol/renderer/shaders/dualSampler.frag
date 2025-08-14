#version 310 es
precision highp float;
precision highp int;


layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;
layout(binding = 1) uniform sampler2D u_tex1;

layout(location = SV_Target0) out vec4 FragColor;

void main() {
    vec4 texColor = vec4(texture(u_tex0, v_texCoord).rgb, texture(u_tex1, v_texCoord).r);

    texColor.rgb *= texColor.a; // Premultiply with Alpha channel

    FragColor = v_color * texColor;
}
