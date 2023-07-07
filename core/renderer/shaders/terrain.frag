#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
precision mediump float;
#endif
layout(std140, binding = 0) uniform Block_0 {
    vec3 u_color;
};
layout (location = 0) in vec2 v_texCoord;
layout (location = 1) in vec3 v_normal;
#ifdef GL_ES
layout(std140, binding = 1) uniform Block_1 {
    lowp int u_has_alpha;
    lowp int u_has_light_map;
};
#else
layout(std140, binding = 2) uniform Block_2 {
    int u_has_alpha;
    int u_has_light_map;
};
#endif
layout(location = 2, binding = 0) uniform sampler2D u_alphaMap;
layout(location = 3, binding = 0) uniform sampler2D u_tex0;
layout(location = 4, binding = 0) uniform sampler2D u_tex1;
layout(location = 5, binding = 0) uniform sampler2D u_tex2;
layout(location = 6, binding = 0) uniform sampler2D u_tex3;
layout(location = 7, binding = 0) uniform sampler2D u_lightMap;
layout(std140, binding = 3) uniform Block_3 {
    float u_detailSize[4];
    vec3 u_lightDir;
};
layout (location = 0) out vec4 FragColor;
void main()
{
vec4 lightColor;
if(u_has_light_map<=0)
{
    lightColor = vec4(1.0,1.0,1.0,1.0);
}else
{
    lightColor = texture(u_lightMap,v_texCoord);
}
float lightFactor = dot(-u_lightDir,v_normal);
if(u_has_alpha<=0)
{
    FragColor = texture(u_tex0, v_texCoord)*lightColor*lightFactor;
}else
{
    vec4 blendFactor =texture(u_alphaMap,v_texCoord);
    vec4 color = vec4(0.0,0.0,0.0,0.0);
    color = texture(u_tex0, v_texCoord*u_detailSize[0])*blendFactor.r +
    texture(u_tex1, v_texCoord*u_detailSize[1])*blendFactor.g + texture(u_tex2, v_texCoord*u_detailSize[2])*blendFactor.b
        + texture(u_tex3, v_texCoord*u_detailSize[3])*(1.0 - blendFactor.a);
    FragColor = vec4(color.rgb*lightColor.rgb*lightFactor, 1.0);
}
}
