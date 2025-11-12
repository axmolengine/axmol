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
     c = v_color * c;
    FragColor.xyz = vec3(0.2126*c.r + 0.7152*c.g + 0.0722*c.b);
    FragColor.w = c.w;
}
