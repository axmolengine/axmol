#include "base.hlsli"

struct VS_IN {
    float3 a_position : POSITION;
    float3 a_normal : NORMAL;
};

struct VS_OUT {
    float3 v_reflect : TEXCOORD0;
    float4 position : SV_Position;
};

cbuffer vs_ub : register(b0, space0) {
    float4x4 u_MVPMatrix;
    float4x4 u_MVMatrix;
    float3x3 u_NormalMatrix;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.position = mul(u_MVPMatrix, float4(input.a_position, 1.0));

    float4 positionWorldViewSpace = mul(u_MVMatrix, float4(input.a_position, 1.0));
    float3 vEyeVertex = normalize(positionWorldViewSpace.xyz);

    float3 v_normalVector = mul(u_NormalMatrix, input.a_normal);
    output.v_reflect = normalize(reflect(-vEyeVertex, v_normalVector));

    return output;
}
