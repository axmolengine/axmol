#include "base.hlsli"

struct VS_IN {
    float4 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
};

struct VS_OUT {
    float2 v_texCoord : TEXCOORD0;
    float4 v_position : POSITION;
    float4 position : SV_Position;
};

cbuffer vs_ub : register(b0, space0) {
    float4x4 u_model_matrix;
    float4x4 u_MVPMatrix;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.position = mul(u_MVPMatrix, input.a_position);
    output.v_texCoord = input.a_texCoord;
    output.v_position = mul(u_model_matrix, input.a_position);
    return output;
}
