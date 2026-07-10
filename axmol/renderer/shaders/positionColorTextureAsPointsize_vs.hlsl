#include "base.hlsli"

struct VS_IN {
    float4 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
    float4 a_color : COLOR0;
};

struct VS_OUT {
    float4 v_color : COLOR0;
    float4 position : SV_Position;
    
    [[vk::builtin("PointSize")]] float pointSize : PSIZE;
};

cbuffer vs_ub : register(b0, space0) {
    float u_alpha;
    float4x4 u_MVPMatrix;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.position = mul(u_MVPMatrix, input.a_position);
    output.pointSize = input.a_texCoord.x;
    output.v_color = float4(input.a_color.rgb * input.a_color.a * u_alpha, input.a_color.a * u_alpha);
    return output;
}
