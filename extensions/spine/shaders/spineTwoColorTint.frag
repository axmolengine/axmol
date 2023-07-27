#version 310 es

precision highp float;

layout(binding = 0) uniform sampler2D u_tex0;

layout(location = COLOR0) in vec4 v_light;
layout(location = COLOR1) in vec4 v_dark;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(location = SV_Target0) out vec4 FragColor;

void main() {
    vec4 texColor = texture(u_tex0, v_texCoord);
    float alpha = texColor.a * v_light.a;
    FragColor.a = alpha;
    FragColor.rgb = ((texColor.a - 1.0) * v_dark.a + 1.0 - texColor.rgb) * v_dark.rgb + texColor.rgb * v_light.rgb;
}
