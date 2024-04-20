#version 310 es

precision mediump float;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    float fontAlpha = texture(u_tex0, v_texCoord).x;
    FragColor = vec4(v_color.rgb, v_color.a * fontAlpha);
}
