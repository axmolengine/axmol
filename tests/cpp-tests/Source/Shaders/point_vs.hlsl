#include "base.hlsli"

struct VS_IN {
    float2 a_position : POSITION;
    float a_size : PSIZE;
    float4 a_color : COLOR0;
};

struct VS_OUT {
    float4 position : SV_Position;
    float4 f_color : COLOR0;
    float f_size : TEXCOORD0;
};

cbuffer vs_ub {
    float4x4 u_MVPMatrix;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.position = mul(u_MVPMatrix, float4(input.a_position, 0.0, 1.0));
    output.f_color = input.a_color;
    output.f_size = input.a_size;
    return output;
}
