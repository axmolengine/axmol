#version 310 es
precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;
layout(set = 1, binding = 0) uniform sampler2D u_normalMap;

layout(set = 1, binding = 1) uniform sampler2D u_tex0;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    float u_kBump;
    vec4  u_lightPosInLocalSpace;
    vec2  u_contentSize;
    vec3  u_diffuseL;
};

layout(location = SV_Target0) out vec4 FragColor;

void main(void)
{
    vec4 texColor=texture(u_tex0, v_texCoord);
    vec3 normal=texture(u_normalMap, v_texCoord).rgb;
	normal=normal*2.0-1.0;
	normal.y=-normal.y;
	if(u_kBump!=1.0)
	{
        //if the vertex.z mult kBump, then the normal.z should div kBump and re-normalize
        normal=vec3(normal.x,normal.y,normal.z/u_kBump);
        normal=normalize(normal);
    }
	vec4 curPixelPosInLocalSpace=vec4(v_texCoord.x*u_contentSize.x,(1.0-v_texCoord.y)*u_contentSize.y,0.0,1.0);
	vec4 lightDir=normalize(curPixelPosInLocalSpace-u_lightPosInLocalSpace);
	vec3 posToLight=-lightDir.xyz;
	float normDotPosToLight=max(0.0,dot(normal,posToLight));
	vec4 diffuse=vec4(normDotPosToLight*u_diffuseL,1.0);
	vec4 ambient=vec4(0.5,0.5,0.5,1);
	FragColor=texColor*vec4(vec3(diffuse+ambient),diffuse.a);
}
