#include "base.hlsli"

struct VS_IN {
    float3 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
};

struct VS_OUT {
    float2 v_texCoord : TEXCOORD0;
    float v_fogFactor : TEXCOORD1;
    float4 position : SV_Position;
};

cbuffer vs_ub : register(b0, space0) {
    float u_fogDensity;
    float u_fogStart;
    float u_fogEnd;
    int u_fogEquation;
    float4x4 u_MVPMatrix;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    float4 clipPos = mul(u_MVPMatrix, float4(input.a_position, 1.0));
    output.position = clipPos;
    output.v_texCoord = input.a_texCoord;

    float fogFragCoord = abs(clipPos.z);

    if (u_fogEquation == 0)
        output.v_fogFactor = (u_fogEnd - fogFragCoord) / (u_fogEnd - u_fogStart);
    else if (u_fogEquation == 1)
        output.v_fogFactor = exp(-u_fogDensity * fogFragCoord);
    else if (u_fogEquation == 2)
        output.v_fogFactor = exp(-pow(u_fogDensity * fogFragCoord, 2.0));

    output.v_fogFactor = clamp(output.v_fogFactor, 0.0, 1.0);

    return output;
}
