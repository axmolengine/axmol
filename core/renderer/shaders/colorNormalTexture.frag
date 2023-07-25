#version 310 es
precision highp float;
precision highp int;

#include "base.glsl"

layout(location = TEXCOORD0) in vec2 v_texCoord;

#ifdef USE_NORMAL_MAPPING
layout(location = DIRLIGHT) in vec3 v_dirLightDirection[MAX_DIRECTIONAL_LIGHT_NUM];
#endif
layout(location = POINTLIGHT) in vec3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM];
layout(location = SPOTLIGHT) in vec3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM];
#ifdef USE_NORMAL_MAPPING
layout(location = SPOTLIGHT_NORM) in vec3 v_spotLightDirection[MAX_SPOT_LIGHT_NUM];
#endif

#ifndef USE_NORMAL_MAPPING
layout(location = NORMAL) in vec3 v_normal;
#endif

#ifdef USE_NORMAL_MAPPING
layout(binding = 0) uniform sampler2D u_normalTex;
#endif

layout(binding = 1) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec3 u_DirLightSourceColor[MAX_DIRECTIONAL_LIGHT_NUM];
    vec3 u_DirLightSourceDirection[MAX_DIRECTIONAL_LIGHT_NUM];
    vec3 u_PointLightSourceColor[MAX_POINT_LIGHT_NUM];
    float u_PointLightSourceRangeInverse[MAX_POINT_LIGHT_NUM];
    vec3 u_SpotLightSourceColor[MAX_SPOT_LIGHT_NUM];
    vec3 u_SpotLightSourceDirection[MAX_SPOT_LIGHT_NUM];
    float u_SpotLightSourceInnerAngleCos[MAX_SPOT_LIGHT_NUM];
    float u_SpotLightSourceOuterAngleCos[MAX_SPOT_LIGHT_NUM];
    float u_SpotLightSourceRangeInverse[MAX_SPOT_LIGHT_NUM];
    vec3 u_AmbientLightSourceColor;
    vec4 u_color;
};

vec3 computeLighting(vec3 normalVector, vec3 lightDirection, vec3 lightColor, float attenuation)
{
    float diffuse = max(dot(normalVector, lightDirection), 0.0);
    vec3 diffuseColor = lightColor  * diffuse * attenuation;

    return diffuseColor;
}

layout(location = SV_Target0) out vec4 FragColor;

void main(void)
{

#ifdef USE_NORMAL_MAPPING
    vec3 normal  = normalize(2.0 * texture(u_normalTex, v_texCoord).xyz - 1.0);
#else
    vec3 normal  = normalize(v_normal);
#endif

    vec4 combinedColor = vec4(u_AmbientLightSourceColor, 1.0);

    // Directional light contribution
    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_NUM; ++i)
    {
#ifdef USE_NORMAL_MAPPING
        vec3 lightDirection = normalize(v_dirLightDirection[i] * 2.0);
#else
        vec3 lightDirection = normalize(u_DirLightSourceDirection[i] * 2.0);
#endif
        combinedColor.xyz += computeLighting(normal, -lightDirection, u_DirLightSourceColor[i], 1.0);
    }

    // Point light contribution
    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        vec3 ldir = v_vertexToPointLightDirection[i] * u_PointLightSourceRangeInverse[i];
        float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);
        combinedColor.xyz += computeLighting(normal, normalize(v_vertexToPointLightDirection[i]), u_PointLightSourceColor[i], attenuation);
    }

    // Spot light contribution
    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        // Compute range attenuation
        vec3 ldir = v_vertexToSpotLightDirection[i] * u_SpotLightSourceRangeInverse[i];
        float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);
        vec3 vertexToSpotLightDirection = normalize(v_vertexToSpotLightDirection[i]);

#ifdef USE_NORMAL_MAPPING
        vec3 spotLightDirection = normalize(v_spotLightDirection[i] * 2.0);
#else
        vec3 spotLightDirection = normalize(u_SpotLightSourceDirection[i] * 2.0);
#endif

        // \-lightDirection\ is used because light direction points in opposite direction to spot direction.
        float spotCurrentAngleCos = dot(spotLightDirection, -vertexToSpotLightDirection);

        // Apply spot attenuation
        attenuation *= smoothstep(u_SpotLightSourceOuterAngleCos[i], u_SpotLightSourceInnerAngleCos[i], spotCurrentAngleCos);
        attenuation = clamp(attenuation, 0.0, 1.0);
        combinedColor.xyz += computeLighting(normal, vertexToSpotLightDirection, u_SpotLightSourceColor[i], attenuation);
    }

    FragColor = texture(u_tex0, v_texCoord) * u_color * combinedColor;
}
