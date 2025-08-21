#version 310 es
precision highp float;
precision highp int;

#include "base.glsl"

layout(location = TEXCOORD0) in vec2 v_texCoord;
layout(location = NORMAL) in vec3 v_normal;
layout(binding = 0) uniform sampler2D u_alphaMap;
layout(binding = 1) uniform sampler2D u_tex0;
layout(binding = 2) uniform sampler2D u_tex1;
layout(binding = 3) uniform sampler2D u_tex2;
layout(binding = 4) uniform sampler2D u_tex3;
layout(binding = 5) uniform sampler2D u_lightMap;
layout(std140) uniform fs_ub {
    int u_has_alpha;
    int u_has_light_map;
    vfloat_def(u_detailSize, 4);
    vec3 u_lightDir;
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    vec4 lightColor;
    if(u_has_light_map<=0)
    {
        lightColor = vec4(1.0,1.0,1.0,1.0);
    }
    else
    {
        lightColor = texture(u_lightMap,v_texCoord);
    }
    float lightFactor = dot(-u_lightDir,v_normal);
    if(u_has_alpha<=0)
    {
        FragColor = texture(u_tex0, v_texCoord)*lightColor*lightFactor;
    }
    else
    {
        vec4 blendFactor =texture(u_alphaMap,v_texCoord);
        vec4 color = vec4(0.0,0.0,0.0,0.0);
        color = texture(u_tex0, v_texCoord*vfloat_at(u_detailSize, 0))*blendFactor.r +
        texture(u_tex1, v_texCoord*vfloat_at(u_detailSize, 1))*blendFactor.g + texture(u_tex2, v_texCoord*vfloat_at(u_detailSize, 2))*blendFactor.b
            + texture(u_tex3, v_texCoord*vfloat_at(u_detailSize, 3))*(1.0 - blendFactor.a);
        FragColor = vec4(color.rgb*lightColor.rgb*lightFactor, 1.0);
    }
}
