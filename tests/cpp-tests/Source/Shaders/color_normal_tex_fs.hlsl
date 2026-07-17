#define MAX_POINT_LIGHT_NUM 1
#define MAX_SPOT_LIGHT_NUM 1
#define MAX_DIRECTIONAL_LIGHT_NUM 1

#include "base.hlsli"

struct PS_IN
{
    float2 v_texCoord : TEXCOORD0;
    float3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM] : TEXCOORD1;
    float3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM] : TEXCOORD2;
    float3 v_normal : NORMAL;
};

Texture2D u_sampler0;

cbuffer fs_ub
{
    float4 u_DirLightSourceColor[MAX_DIRECTIONAL_LIGHT_NUM];
    float4 u_DirLightSourceDirection[MAX_DIRECTIONAL_LIGHT_NUM];
    float4 u_PointLightSourceColor[MAX_POINT_LIGHT_NUM];
    float4 u_PointLightSourceRangeInverse[MAX_POINT_LIGHT_NUM];
    float4 u_SpotLightSourceColor[MAX_SPOT_LIGHT_NUM];
    float4 u_SpotLightSourceDirection[MAX_SPOT_LIGHT_NUM];
    float4 u_SpotLightSourceInnerAngleCos[MAX_SPOT_LIGHT_NUM];
    float4 u_SpotLightSourceOuterAngleCos[MAX_SPOT_LIGHT_NUM];
    float4 u_SpotLightSourceRangeInverse[MAX_SPOT_LIGHT_NUM];
    float4 u_AmbientLightSourceColor;
    float4 u_color;
};

float3 computeLighting(float3 normalVector, float3 lightDirection, float3 lightColor, float attenuation)
{
    float diffuse = max(dot(normalVector, lightDirection), 0.0);
    return lightColor * diffuse * attenuation;
}

float4 main(PS_IN input) : SV_Target0
{
    float3 normal = normalize(input.v_normal);

    float4 combinedColor = float4(u_AmbientLightSourceColor.rgb, 1.0);

    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_NUM; ++i)
    {
        float3 lightDirection = normalize(u_DirLightSourceDirection[i].rgb * 2.0);
        combinedColor.xyz += computeLighting(normal, -lightDirection, u_DirLightSourceColor[i].rgb, 1.0);
    }

    for (int j = 0; j < MAX_POINT_LIGHT_NUM; ++j)
    {
        float3 ldir = input.v_vertexToPointLightDirection[j] * u_PointLightSourceRangeInverse[j].x;
        float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);
        combinedColor.xyz += computeLighting(normal, normalize(input.v_vertexToPointLightDirection[j]), u_PointLightSourceColor[j].rgb, attenuation);
    }

    for (int k = 0; k < MAX_SPOT_LIGHT_NUM; ++k)
    {
        float3 ldir = input.v_vertexToSpotLightDirection[k] * u_SpotLightSourceRangeInverse[k].x;
        float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);
        float3 vertexToSpotLightDirection = normalize(input.v_vertexToSpotLightDirection[k]);
        float3 spotLightDirection = normalize(u_SpotLightSourceDirection[k].rgb * 2.0);

        float spotCurrentAngleCos = dot(spotLightDirection, -vertexToSpotLightDirection);
        attenuation *= smoothstep(u_SpotLightSourceOuterAngleCos[k].x, u_SpotLightSourceInnerAngleCos[k].x, spotCurrentAngleCos);
        attenuation = clamp(attenuation, 0.0, 1.0);
        combinedColor.xyz += computeLighting(normal, vertexToSpotLightDirection, u_SpotLightSourceColor[k].rgb, attenuation);
    }

    return u_sampler0.Sample(LinearClamp, input.v_texCoord) * u_color * combinedColor;
}
