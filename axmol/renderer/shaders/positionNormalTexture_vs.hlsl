#include "base.hlsli"

struct VS_IN {
    float4 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
    float3 a_normal : NORMAL;
#ifdef USE_NORMAL_MAPPING
    float3 a_tangent : TANGENT;
    float3 a_binormal : BINORMAL;
#endif
};

struct VS_OUT {
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
    float4 position : SV_Position;
};

cbuffer vs_ub {
#ifdef USE_NORMAL_MAPPING
    vvec3_def(u_DirLightSourceDirection, MAX_DIRECTIONAL_LIGHT_NUM);
    vvec3_def(u_SpotLightSourceDirection, MAX_SPOT_LIGHT_NUM);
#endif
    vvec3_def(u_PointLightSourcePosition, MAX_POINT_LIGHT_NUM);
    vvec3_def(u_SpotLightSourcePosition, MAX_SPOT_LIGHT_NUM);
    float4x4 u_MVPMatrix;
    float4x4 u_MVMatrix;
    float4x4 u_PMatrix;
    float3x3 u_NormalMatrix;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    float4 ePosition = mul(u_MVMatrix, input.a_position);
#ifdef USE_NORMAL_MAPPING
    float3 eTangent = normalize(mul(u_NormalMatrix, input.a_tangent));
    float3 eBinormal = normalize(mul(u_NormalMatrix, input.a_binormal));
    float3 eNormal = normalize(mul(u_NormalMatrix, input.a_normal));
    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_NUM; ++i)
    {
        output.v_dirLightDirection[i].x = dot(eTangent, vvec3_at(u_DirLightSourceDirection, i));
        output.v_dirLightDirection[i].y = dot(eBinormal, vvec3_at(u_DirLightSourceDirection, i));
        output.v_dirLightDirection[i].z = dot(eNormal, vvec3_at(u_DirLightSourceDirection, i));
    }

    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        float3 pointLightDir = vvec3_at(u_PointLightSourcePosition, i).xyz - ePosition.xyz;
        output.v_vertexToPointLightDirection[i].x = dot(eTangent, pointLightDir);
        output.v_vertexToPointLightDirection[i].y = dot(eBinormal, pointLightDir);
        output.v_vertexToPointLightDirection[i].z = dot(eNormal, pointLightDir);
    }

    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        float3 spotLightDir = vvec3_at(u_SpotLightSourcePosition, i) - ePosition.xyz;
        output.v_vertexToSpotLightDirection[i].x = dot(eTangent, spotLightDir);
        output.v_vertexToSpotLightDirection[i].y = dot(eBinormal, spotLightDir);
        output.v_vertexToSpotLightDirection[i].z = dot(eNormal, spotLightDir);

        output.v_spotLightDirection[i].x = dot(eTangent, vvec3_at(u_SpotLightSourceDirection, i));
        output.v_spotLightDirection[i].y = dot(eBinormal, vvec3_at(u_SpotLightSourceDirection, i));
        output.v_spotLightDirection[i].z = dot(eNormal, vvec3_at(u_SpotLightSourceDirection, i));
    }
#else
    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        output.v_vertexToPointLightDirection[i] = vvec3_at(u_PointLightSourcePosition, i).xyz - ePosition.xyz;
    }

    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        output.v_vertexToSpotLightDirection[i] = vvec3_at(u_SpotLightSourcePosition, i) - ePosition.xyz;
    }

    output.v_normal = mul(u_NormalMatrix, input.a_normal);
#endif

    output.v_texCoord = input.a_texCoord;
    output.v_texCoord.y = 1.0 - output.v_texCoord.y;
    output.position = mul(u_PMatrix, ePosition);
    return output;
}
