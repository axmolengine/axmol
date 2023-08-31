#version 310 es
precision highp float;
precision highp int;


layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    FragColor =  texture(u_tex0, v_texCoord);
}
