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
#ifdef AXSLC_TARGET_GLSL
    output.position = float4(input.a_position.xyz, 1.0);
#else
    output.position = float4(input.a_position.x, -input.a_position.y, input.a_position.z, 1.0);
#endif
    output.v_texCoord = input.a_texCoord;
    return output;
}
