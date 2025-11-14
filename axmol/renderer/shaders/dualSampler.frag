#version 310 es
precision highp float;
precision highp int;
precision highp sampler2DArray;


layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(set = 1, binding = 0) uniform sampler2DArray u_tex0;

layout(location = SV_Target0) out vec4 FragColor;

void main() {
    vec3 uv0 = vec3(v_texCoord, 0.0);
    vec3 uv1 = vec3(v_texCoord, 1.0);

    vec3 rgb   = texture(u_tex0, uv0).rgb;
    float a    = texture(u_tex0, uv1).r;

    vec4 texColor = vec4(rgb, a);
    texColor.rgb *= texColor.a;

    FragColor = v_color * texColor;
}
