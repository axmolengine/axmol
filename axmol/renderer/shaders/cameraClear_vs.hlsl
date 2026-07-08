#include "base.hlsli"

struct VS_IN {
    float4 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
    float4 a_color : COLOR0;
};

struct VS_OUT {
    float2 v_texCoord : TEXCOORD0;
    float4 v_color : COLOR0;
    float4 position : SV_Position;
};

cbuffer vs_ub : register(b0, space0) {
    float depth;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.position = input.a_position;
    output.position.z = depth;
    output.position.w = 1.0;
    output.v_texCoord = input.a_texCoord;
    output.v_color = input.a_color;
    return output;
}
