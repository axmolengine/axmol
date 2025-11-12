#version 310 es
#extension GL_NV_shader_framebuffer_fetch : enable
precision mediump float;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;
layout(location = TEXCOORD1) in vec4 v_myPos;

layout(location = SV_Target0) out vec4 FragColor;

layout(set = 1, binding = 0) uniform sampler2D u_tex0;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec4 u_channelFlag;
    vec4 u_baseColor;
};

void main()
{
    float isInside = 
          step(u_baseColor.x, v_myPos.x/v_myPos.w)
        * step(u_baseColor.y, v_myPos.y/v_myPos.w)
        * step(v_myPos.x/v_myPos.w, u_baseColor.z)
        * step(v_myPos.y/v_myPos.w, u_baseColor.w);

    FragColor = u_channelFlag * texture(u_tex0 , v_texCoord).a * isInside;
}
