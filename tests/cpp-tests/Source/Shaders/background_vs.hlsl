#include "base.hlsli"

struct VS_IN {
    float2 a_position : POSITION;
};

struct VS_OUT {
    float4 position : SV_Position;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.position = float4(input.a_position, 0.0, 1.0);
    return output;
}
