#version 310 es
precision highp float;
precision highp int;


layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;
layout(binding = 1) uniform sampler2D u_tex1;

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    vec4 texColor = texture(u_tex0, v_texCoord);
    texColor.a = texture(u_tex1, v_texCoord).r;
    texColor.rgb *= texColor.a; // premultiply alpha channel

    texColor = v_color * texColor;

    FragColor.rgb = vec3(0.2126*texColor.r + 0.7152*texColor.g + 0.0722*texColor.b);
    FragColor.a = texColor.a;
}
