#version 310 es
precision highp float;
precision highp int;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(set = 1, binding = 0) uniform sampler2D u_sampler0; 
layout(set = 1, binding = 1) uniform sampler2D u_sampler1; 
layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec4 u_color;
    float offset;
    float duration;
};

layout(location = SV_Target0) out vec4 FragColor;

void main(void)
{
    vec4 color = duration*vec4(0,0.8,0.4,1.0);
    //blend two texture
    FragColor = u_color*texture(u_sampler0, vec2(v_texCoord.x- 2.0 * offset,v_texCoord.y)) * vec4(0.3,0.3,0.3,1)+texture(u_sampler1,vec2(v_texCoord.x-offset,v_texCoord.y)).r*color;
}
