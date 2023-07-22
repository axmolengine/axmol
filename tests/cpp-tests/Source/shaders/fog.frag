#version 310 es
precision highp float;
precision highp int;
layout(location = 0) in vec2 v_texture_coord;
layout(location = 1) in float v_fogFactor;


layout(binding = 0) uniform sampler2D u_tex0;

layout(std140, binding = 0) uniform fs_ub {
    vec4 u_fogColor;
};

layout(location = 0) out vec4 FragColor;

void main (void)
{
    vec4 finalColor = texture(u_tex0, v_texture_coord);
    FragColor     = mix(u_fogColor, finalColor, v_fogFactor );        //out put finalColor
}
