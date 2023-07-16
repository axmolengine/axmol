#version 310 es
precision highp float;
precision highp int;


layout(location = 0) in vec4 v_fragmentColor;
layout(location = 1) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;
layout(binding = 1) uniform sampler2D u_tex1;

layout(location = 0) out vec4 FragColor;

void main() {
    vec4 texColor = vec4(texture(u_tex0, v_texCoord).rgb, texture(u_tex1, v_texCoord).r);

    texColor.rgb *= texColor.a; // Premultiply with Alpha channel

    FragColor = v_fragmentColor * texColor;
}
