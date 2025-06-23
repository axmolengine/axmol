#version 310 es
precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    vec4 c = texture(u_tex0, v_texCoord);
    FragColor = v_color * vec4(c.r, c.r, c.r, c.g);
}
