#include "base.hlsli"

struct VS_IN {
    float4 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
};

struct VS_OUT {
    float2 v_texCoord : TEXCOORD0;
    float4 position : SV_Position;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.position = input.a_position;
    output.v_texCoord = input.a_texCoord;
    return output;
}
