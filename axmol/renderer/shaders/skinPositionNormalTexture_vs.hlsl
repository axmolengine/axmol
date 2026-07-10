#include "base.hlsli"

#define SKINNING_JOINT_COUNT 60

struct VS_IN {
    float3 a_position : POSITION;
    float4 a_blendWeight : BLENDWEIGHT;
    float4 a_blendIndex : BLENDINDICES;
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

cbuffer vs_ub : register(b0, space0) {
#ifdef USE_NORMAL_MAPPING
    vvec3_def(u_DirLightSourceDirection, MAX_DIRECTIONAL_LIGHT_NUM);
    vvec3_def(u_SpotLightSourceDirection, MAX_SPOT_LIGHT_NUM);
#endif
    vvec3_def(u_PointLightSourcePosition, MAX_POINT_LIGHT_NUM);
    vvec3_def(u_SpotLightSourcePosition, MAX_SPOT_LIGHT_NUM);
    float4 u_matrixPalette[SKINNING_JOINT_COUNT * 3];
    float4x4 u_MVMatrix;
    float3x3 u_NormalMatrix;
    float4x4 u_PMatrix;
};

void getPositionAndNormal(out float4 position, out float3 normal, out float3 tangent, out float3 binormal,
                          VS_IN input)
{
    float blendWeight = input.a_blendWeight[0];

    int matrixIndex = int(input.a_blendIndex[0]) * 3;
    float4 matrixPalette1 = u_matrixPalette[matrixIndex] * blendWeight;
    float4 matrixPalette2 = u_matrixPalette[matrixIndex + 1] * blendWeight;
    float4 matrixPalette3 = u_matrixPalette[matrixIndex + 2] * blendWeight;


    blendWeight = input.a_blendWeight[1];
    if (blendWeight > 0.0)
    {
        matrixIndex = int(input.a_blendIndex[1]) * 3;
        matrixPalette1 += u_matrixPalette[matrixIndex] * blendWeight;
        matrixPalette2 += u_matrixPalette[matrixIndex + 1] * blendWeight;
        matrixPalette3 += u_matrixPalette[matrixIndex + 2] * blendWeight;

        blendWeight = input.a_blendWeight[2];
        if (blendWeight > 0.0)
        {
            matrixIndex = int(input.a_blendIndex[2]) * 3;
            matrixPalette1 += u_matrixPalette[matrixIndex] * blendWeight;
            matrixPalette2 += u_matrixPalette[matrixIndex + 1] * blendWeight;
            matrixPalette3 += u_matrixPalette[matrixIndex + 2] * blendWeight;

            blendWeight = input.a_blendWeight[3];
            if (blendWeight > 0.0)
            {
                matrixIndex = int(input.a_blendIndex[3]) * 3;
                matrixPalette1 += u_matrixPalette[matrixIndex] * blendWeight;
                matrixPalette2 += u_matrixPalette[matrixIndex + 1] * blendWeight;
                matrixPalette3 += u_matrixPalette[matrixIndex + 2] * blendWeight;
            }
        }
    }

    float4 p = float4(input.a_position, 1.0);
    position.x = dot(p, matrixPalette1);
    position.y = dot(p, matrixPalette2);
    position.z = dot(p, matrixPalette3);
    position.w = p.w;

    float4 n = float4(input.a_normal, 0.0);
    normal.x = dot(n, matrixPalette1);
    normal.y = dot(n, matrixPalette2);
    normal.z = dot(n, matrixPalette3);
#ifdef USE_NORMAL_MAPPING
    float4 t = float4(input.a_tangent, 0.0);
    tangent.x = dot(t, matrixPalette1);
    tangent.y = dot(t, matrixPalette2);
    tangent.z = dot(t, matrixPalette3);
    float4 b = float4(input.a_binormal, 0.0);
    binormal.x = dot(b, matrixPalette1);
    binormal.y = dot(b, matrixPalette2);
    binormal.z = dot(b, matrixPalette3);
#else
    tangent = float3(0.0, 0.0, 0.0);
    binormal = float3(0.0, 0.0, 0.0);
#endif
}

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    float4 position;
    float3 normal;
    float3 tangent;
    float3 binormal;
    getPositionAndNormal(position, normal, tangent, binormal, input);
    float4 ePosition = mul(u_MVMatrix, position);

#ifdef USE_NORMAL_MAPPING
    float3 eTangent = normalize(mul(u_NormalMatrix, tangent));
    float3 eBinormal = normalize(mul(u_NormalMatrix, binormal));
    float3 eNormal = normalize(mul(u_NormalMatrix, normal));

    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_NUM; ++i)
    {
        float3 pointD = vvec3_at(u_DirLightSourceDirection, i);
        output.v_dirLightDirection[i].x = dot(eTangent, pointD);
        output.v_dirLightDirection[i].y = dot(eBinormal, pointD);
        output.v_dirLightDirection[i].z = dot(eNormal, pointD);
    }

    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        float3 pointLightDir = vvec3_at(u_PointLightSourcePosition, i) - ePosition.xyz;
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

        float3 pointP = vvec3_at(u_SpotLightSourceDirection, i);
        output.v_spotLightDirection[i].x = dot(eTangent, pointP);
        output.v_spotLightDirection[i].y = dot(eBinormal, pointP);
        output.v_spotLightDirection[i].z = dot(eNormal, pointP);
    }
#else
    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        output.v_vertexToPointLightDirection[i] = vvec3_at(u_PointLightSourcePosition, i) - ePosition.xyz;
    }

    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        output.v_vertexToSpotLightDirection[i] = vvec3_at(u_SpotLightSourcePosition, i) - ePosition.xyz;
    }

    output.v_normal = mul(u_NormalMatrix, normal);
#endif

    output.v_texCoord = input.a_texCoord;
    output.v_texCoord.y = 1.0 - output.v_texCoord.y;
    output.position = mul(u_PMatrix, ePosition);
    return output;
}
