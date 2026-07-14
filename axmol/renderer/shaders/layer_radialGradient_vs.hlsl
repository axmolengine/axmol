#include "base.hlsli"

struct VS_IN {
    float3 a_position : POSITION;
};

struct VS_OUT {
    float4 v_position : TEXCOORD0;
    float4 position : SV_Position;
};

cbuffer vs_ub : register(b0, space0) {
    float4x4 u_MVPMatrix;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.v_position = float4(input.a_position, 1.0);
    output.position = mul(u_MVPMatrix, float4(input.a_position, 1.0));
    return output;
}
