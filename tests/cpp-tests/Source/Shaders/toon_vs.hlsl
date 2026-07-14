#include "base.hlsli"

struct VS_IN {
    float4 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
    float3 a_normal : NORMAL;
};

struct VS_OUT {
    float2 v_texCoord : TEXCOORD0;
    float3 v_normal : NORMAL;
    float4 position : SV_Position;
};

cbuffer vs_ub : register(b0, space0) {
    float4x4 u_MVPMatrix;
    float3x3 u_NormalMatrix;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.position = mul(u_MVPMatrix, input.a_position);
    output.v_texCoord = input.a_texCoord;
    output.v_normal = mul(u_NormalMatrix, input.a_normal);
    return output;
}
