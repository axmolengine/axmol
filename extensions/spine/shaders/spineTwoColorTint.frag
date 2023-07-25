#version 310 es

precision highp float;

uniform sampler2D u_texture;
in vec4 v_light;
in vec4 v_dark;
in vec2 v_texCoord;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(u_texture, v_texCoord);
    float alpha = texColor.a * v_light.a;
    FragColor.a = alpha;
    FragColor.rgb = ((texColor.a - 1.0) * v_dark.a + 1.0 - texColor.rgb) * v_dark.rgb + texColor.rgb * v_light.rgb;
}
