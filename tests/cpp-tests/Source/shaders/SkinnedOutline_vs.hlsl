static const int SKINNING_JOINT_COUNT = 60;

struct VS_IN
{
    float3 a_position : POSITION;
    float3 a_normal : NORMAL;
    float4 a_blendWeight : BLENDWEIGHT;
    float4 a_blendIndex : BLENDINDICES;
};

cbuffer vs_ub : register(b0, space0)
{
    float OutlineWidth;
    float4 u_matrixPalette[SKINNING_JOINT_COUNT * 3];
    float4x4 u_MVPMatrix;
};

float3 SkinnedVec3(float4 v, float4 blendWeight, float4 blendIndex)
{
    float4 matrixPalette1 = float4(0, 0, 0, 0);
    float4 matrixPalette2 = float4(0, 0, 0, 0);
    float4 matrixPalette3 = float4(0, 0, 0, 0);

    float bw = blendWeight[0];
    int index = int(blendIndex[0]) * 3;
    matrixPalette1 = u_matrixPalette[index] * bw;
    matrixPalette2 = u_matrixPalette[index + 1] * bw;
    matrixPalette3 = u_matrixPalette[index + 2] * bw;

    bw = blendWeight[1];
    if (bw > 0.0)
    {
        index = int(blendIndex[1]) * 3;
        matrixPalette1 += u_matrixPalette[index] * bw;
        matrixPalette2 += u_matrixPalette[index + 1] * bw;
        matrixPalette3 += u_matrixPalette[index + 2] * bw;
    }

    bw = blendWeight[2];
    if (bw > 0.0)
    {
        index = int(blendIndex[2]) * 3;
        matrixPalette1 += u_matrixPalette[index] * bw;
        matrixPalette2 += u_matrixPalette[index + 1] * bw;
        matrixPalette3 += u_matrixPalette[index + 2] * bw;
    }

    bw = blendWeight[3];
    if (bw > 0.0)
    {
        index = int(blendIndex[3]) * 3;
        matrixPalette1 += u_matrixPalette[index] * bw;
        matrixPalette2 += u_matrixPalette[index + 1] * bw;
        matrixPalette3 += u_matrixPalette[index + 2] * bw;
    }

    float3 result;
    result.x = dot(v, matrixPalette1);
    result.y = dot(v, matrixPalette2);
    result.z = dot(v, matrixPalette3);
    return result;
}

float4 main(VS_IN input) : SV_Position
{
    float4 pos = mul(u_MVPMatrix, float4(SkinnedVec3(float4(input.a_position, 1.0), input.a_blendWeight, input.a_blendIndex), 1.0));

    float4 normalproj = mul(u_MVPMatrix, float4(SkinnedVec3(float4(input.a_normal, 0.0), input.a_blendWeight, input.a_blendIndex), 0.0));
    normalproj = normalize(normalproj);
    pos.xy += normalproj.xy * (OutlineWidth * (pos.z * 0.5 + 0.5));

    return pos;
}
