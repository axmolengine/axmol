#include "base.hlsli"

struct VS_IN {
    float4 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
    AX_INSTANCE_INPUT(TEXCOORD1);
};

struct VS_OUT {
    float2 v_texCoord : TEXCOORD0;
    float4 position : SV_Position;
};

cbuffer vs_ub {
    float4x4 u_MVPMatrix;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    output.position = AX_INSTANCE_TRANSFORM(input, input.a_position, u_MVPMatrix);
    output.v_texCoord = input.a_texCoord;
    output.v_texCoord.y = 1.0 - output.v_texCoord.y;
    return output;
}
