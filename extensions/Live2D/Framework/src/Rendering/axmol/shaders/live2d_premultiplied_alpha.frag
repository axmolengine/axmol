#version 310 es
#ifdef GLES2
precision mediump float;
#else
precision highp float;
#endif

layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(location = SV_Target0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec4 u_baseColor;
};

void main()
{
    FragColor = texture(u_tex0 , v_texCoord) * u_baseColor;
}
