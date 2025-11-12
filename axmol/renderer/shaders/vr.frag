#version 310 es
precision highp float;
precision highp int;

layout(location = TEXCOORD0) in vec2 v_texCoord;
layout(location = TEXCOORD1) in float v_vignette;
layout(set = 1, binding = 0) uniform sampler2D u_tex0;

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    FragColor = v_vignette * texture(u_tex0, v_texCoord);
}
