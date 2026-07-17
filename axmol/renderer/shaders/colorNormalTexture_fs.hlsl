#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
#ifdef USE_NORMAL_MAPPING
    float3 v_dirLightDirection[MAX_DIRECTIONAL_LIGHT_NUM] : DIRLIGHT;
#endif
    float3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM] : POINTLIGHT;
    float3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM] : SPOTLIGHT;
#ifdef USE_NORMAL_MAPPING
    float3 v_spotLightDirection[MAX_SPOT_LIGHT_NUM] : SPOTLIGHT_NORM;
#endif
#ifndef USE_NORMAL_MAPPING
    float3 v_normal : NORMAL;
#endif
};

Texture2D u_tex0;

#ifdef USE_NORMAL_MAPPING
Texture2D u_normalTex;
#endif

cbuffer fs_ub {
    vvec3_def(u_DirLightSourceColor, MAX_DIRECTIONAL_LIGHT_NUM);
    vvec3_def(u_DirLightSourceDirection, MAX_DIRECTIONAL_LIGHT_NUM);
    vvec3_def(u_PointLightSourceColor, MAX_POINT_LIGHT_NUM);
    vfloat_def(u_PointLightSourceRangeInverse, MAX_POINT_LIGHT_NUM);
    vvec3_def(u_SpotLightSourceColor, MAX_SPOT_LIGHT_NUM);
    vvec3_def(u_SpotLightSourceDirection, MAX_SPOT_LIGHT_NUM);
    vfloat_def(u_SpotLightSourceInnerAngleCos, MAX_SPOT_LIGHT_NUM);
    vfloat_def(u_SpotLightSourceOuterAngleCos, MAX_SPOT_LIGHT_NUM);
    vfloat_def(u_SpotLightSourceRangeInverse, MAX_SPOT_LIGHT_NUM);
    float3 u_AmbientLightSourceColor;
    float4 u_color;
};

float3 computeLighting(float3 normalVector, float3 lightDirection, float3 lightColor, float attenuation)
{
    float diffuse = max(dot(normalVector, lightDirection), 0.0);
    float3 diffuseColor = lightColor * diffuse * attenuation;

    return diffuseColor;
}

float4 main(PS_IN input) : SV_Target0
{

#ifdef USE_NORMAL_MAPPING
    float3 normal = normalize(2.0 * u_normalTex.Sample(LinearClamp, input.v_texCoord).xyz - 1.0);
#else
    float3 normal = normalize(input.v_normal);
#endif

    float4 combinedColor = float4(u_AmbientLightSourceColor, 1.0);

    // Directional light contribution
    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_NUM; ++i)
    {
#ifdef USE_NORMAL_MAPPING
        float3 lightDirection = normalize(input.v_dirLightDirection[i] * 2.0);
#else
        float3 lightDirection = normalize(vvec3_at(u_DirLightSourceDirection, i) * 2.0);
#endif
        combinedColor.xyz += computeLighting(normal, -lightDirection, vvec3_at(u_DirLightSourceColor, i), 1.0);
    }

    // Point light contribution
    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        float3 ldir = input.v_vertexToPointLightDirection[i] * vfloat_at(u_PointLightSourceRangeInverse, i);
        float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);
        combinedColor.xyz += computeLighting(normal, normalize(input.v_vertexToPointLightDirection[i]), vvec3_at(u_PointLightSourceColor, i), attenuation);
    }

    // Spot light contribution
    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        // Compute range attenuation
        float3 ldir = input.v_vertexToSpotLightDirection[i] * vfloat_at(u_SpotLightSourceRangeInverse, i);
        float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);
        float3 vertexToSpotLightDirection = normalize(input.v_vertexToSpotLightDirection[i]);

#ifdef USE_NORMAL_MAPPING
        float3 spotLightDirection = normalize(input.v_spotLightDirection[i] * 2.0);
#else
        float3 spotLightDirection = normalize(vvec3_at(u_SpotLightSourceDirection, i) * 2.0);
#endif

        // \-lightDirection\ is used because light direction points in opposite direction to spot direction.
        float spotCurrentAngleCos = dot(spotLightDirection, -vertexToSpotLightDirection);

        // Apply spot attenuation
        attenuation *= smoothstep(vfloat_at(u_SpotLightSourceOuterAngleCos, i), vfloat_at(u_SpotLightSourceInnerAngleCos, i), spotCurrentAngleCos);
        attenuation = clamp(attenuation, 0.0, 1.0);
        combinedColor.xyz += computeLighting(normal, vertexToSpotLightDirection, vvec3_at(u_SpotLightSourceColor, i), attenuation);
    }

    return u_tex0.Sample(LinearClamp, input.v_texCoord) * u_color * combinedColor;
}
