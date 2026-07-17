#include "base.hlsli"

struct VS_IN {
    float4 a_position : POSITION;
    float4 a_color : COLOR0;
};

struct VS_OUT {
    float4 v_color : COLOR0;
    float4 position : SV_Position;
};

cbuffer vs_ub {
    float4x4 u_MVPMatrix;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.position = mul(u_MVPMatrix, input.a_position);
    output.v_color = input.a_color;
    return output;
}
