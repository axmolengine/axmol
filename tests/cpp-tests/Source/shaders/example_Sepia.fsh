#version 310 es
precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(set = 1, binding = 0) uniform sampler2D u_tex0;

layout(location = SV_Target0) out vec4 FragColor;

void main(void)
{
	vec4 c = texture(u_tex0, v_texCoord);
    vec4 final = c;
    final.r = (c.r * 0.393) + (c.g * 0.769) + (c.b * 0.189);
    final.g = (c.r * 0.349) + (c.g * 0.686) + (c.b * 0.168);
    final.b = (c.r * 0.272) + (c.g * 0.534) + (c.b * 0.131);

	FragColor = final;
}
