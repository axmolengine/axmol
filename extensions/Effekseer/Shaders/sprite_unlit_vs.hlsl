#include "base.hlsli"

cbuffer vs_ub
{
    float4x4 mCamera;
    float4x4 mCameraProj;
    float4 mUVInversed;

    // Unused
    float4 mflipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
}

#if defined(ENABLE_LIGHTING) || defined(ENABLE_DISTORTION)

struct VS_IN
{
    float3 a_position : POSITION0;
    float4 a_color : NORMAL0;
    float4 a_normal : NORMAL1;
    float4 a_tangent : NORMAL2;
    float2 a_texCoord1 : TEXCOORD0;
    float2 a_texCoord2 : TEXCOORD1;
};

struct VS_OUT
{
    float4 position : SV_Position;
    float4 v_color : COLOR;
    float2 v_texCoord : TEXCOORD0;

#if defined(ENABLE_LIGHTING)
    float3 v_worldN : TEXCOORD1;
    float3 v_worldB : TEXCOORD2;
    float3 v_worldT : TEXCOORD3;
    float4 v_posP : TEXCOORD4;
#else
    float4 v_projBinormal : TEXCOORD1;
    float4 v_projTangent : TEXCOORD2;
    float4 v_posP : TEXCOORD3;
#endif
};

#else

struct VS_IN
{
    float3 a_position : POSITION0;
    float4 a_color : NORMAL0;
    float2 a_texCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 position : SV_Position;
    float4 v_color : COLOR;
    float2 v_texCoord : TEXCOORD0;
    float4 v_posP : TEXCOORD4;
};

#endif

VS_OUT main(VS_IN input)
{
    VS_OUT output;

#if defined(ENABLE_LIGHTING) || defined(ENABLE_DISTORTION)
    float4 worldNormal = float4((input.a_normal.xyz - float3(0.5, 0.5, 0.5)) * 2.0, 0.0);
    float4 worldTangent = float4((input.a_tangent.xyz - float3(0.5, 0.5, 0.5)) * 2.0, 0.0);
    float4 worldBinormal = float4(cross(worldNormal.xyz, worldTangent.xyz), 0.0);
#endif

    float4 worldPos = float4(input.a_position, 1.0);
    output.position = mul(mCameraProj, worldPos);
    output.v_color = input.a_color;

    // UV
#if defined(ENABLE_LIGHTING) || defined(ENABLE_DISTORTION)
    float2 uv1 = input.a_texCoord1;
#else
    float2 uv1 = input.a_texCoord;
#endif
    uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
    output.v_texCoord = uv1;

#if defined(ENABLE_LIGHTING)
    // NBT
    output.v_worldN = worldNormal.xyz;
    output.v_worldB = worldBinormal.xyz;
    output.v_worldT = worldTangent.xyz;

#elif defined(ENABLE_DISTORTION)
    output.v_projTangent = mul(mCameraProj, worldPos + worldTangent);
    output.v_projBinormal = mul(mCameraProj, worldPos + worldBinormal);
#endif

    output.v_posP = output.position;

    return output;
}
