#include "base.hlsli"

struct VS_IN {
    float4 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
    float4 a_color : COLOR0;
};

struct VS_OUT {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
    float4 position : SV_Position;
};

cbuffer vs_ub {
    float u_alpha;
    float4x4 u_MVPMatrix;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.v_color = float4(input.a_color.rgb * input.a_color.a * u_alpha, input.a_color.a * u_alpha);
    output.v_texCoord = input.a_texCoord;
    output.position = mul(u_MVPMatrix, input.a_position);
    return output;
}
