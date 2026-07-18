#include "base.hlsli"

struct VS_IN {
    float4 a_position : POSITION;
    float4 a_color : COLOR0;
    float2 a_texCoord : TEXCOORD0;
};

struct VS_OUT {
    float2 v_texCoord : TEXCOORD0;
    float4 v_color : COLOR0;
    float4 position : SV_Position;
};

cbuffer vs_ub {
    float4x4 u_PMatrix;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.v_color = input.a_color;
    output.v_texCoord = input.a_texCoord;
    output.v_texCoord.y = 1.0 - output.v_texCoord.y;
    output.position = mul(u_PMatrix, input.a_position);
    return output;
}
