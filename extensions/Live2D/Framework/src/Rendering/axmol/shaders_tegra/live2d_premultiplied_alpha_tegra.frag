#version 310 es
#extension GL_NV_shader_framebuffer_fetch : enable
precision mediump float;

layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(location = SV_Target0) out vec4 FragColor;

layout(set = 1, binding = 0) uniform sampler2D u_tex0;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec4 u_baseColor;
};

void main()
{
    FragColor = texture(u_tex0 , v_texCoord) * u_baseColor;
}
