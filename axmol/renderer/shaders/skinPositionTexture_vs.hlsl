#include "base.hlsli"

#define SKINNING_JOINT_COUNT 60

struct VS_IN {
    float3 a_position : POSITION;
    float4 a_blendWeight : BLENDWEIGHT;
    float4 a_blendIndex : BLENDINDICES;
    float2 a_texCoord : TEXCOORD0;
};

struct VS_OUT {
    float2 v_texCoord : TEXCOORD0;
    float4 position : SV_Position;
};

cbuffer vs_ub : register(b0, space0) {
    float4 u_matrixPalette[SKINNING_JOINT_COUNT * 3];
    float4x4 u_MVPMatrix;
};

float4 getPosition(VS_IN input)
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

    float4 _skinnedPosition;
    float4 position = float4(input.a_position, 1.0);
    _skinnedPosition.x = dot(position, matrixPalette1);
    _skinnedPosition.y = dot(position, matrixPalette2);
    _skinnedPosition.z = dot(position, matrixPalette3);
    _skinnedPosition.w = position.w;

    return _skinnedPosition;
}

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    float4 position = getPosition(input);
    output.position = mul(u_MVPMatrix, position);

    output.v_texCoord = input.a_texCoord;
    output.v_texCoord.y = 1.0 - output.v_texCoord.y;
    return output;
}
