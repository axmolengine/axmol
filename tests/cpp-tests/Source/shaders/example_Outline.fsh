#version 310 es
precision highp float;
precision highp int;
/*
  Created by guanghui on 4/8/14.
http://www.idevgames.com/forums/thread-3010.html
*/

layout(location = TEXCOORD0) in vec2 v_texCoord;
layout(location = COLOR0) in vec4 v_color;


layout(set = 1, binding = 0) uniform sampler2D u_tex0;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec3 u_outlineColor;
    float u_threshold;
    float u_radius;
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    float radius = u_radius;
    vec4 accum = vec4(0.0);
    vec4 normal = vec4(0.0);
    
    normal = texture(u_tex0, vec2(v_texCoord.x, v_texCoord.y));
    
    accum += texture(u_tex0, vec2(v_texCoord.x - radius, v_texCoord.y - radius));
    accum += texture(u_tex0, vec2(v_texCoord.x + radius, v_texCoord.y - radius));
    accum += texture(u_tex0, vec2(v_texCoord.x + radius, v_texCoord.y + radius));
    accum += texture(u_tex0, vec2(v_texCoord.x - radius, v_texCoord.y + radius));
    
    accum *= u_threshold;
    accum.rgb =  u_outlineColor * accum.a;
    accum.a = 1.0;
    
    normal = ( accum * (1.0 - normal.a)) + (normal * normal.a);
    
    FragColor = v_color * normal;
}

