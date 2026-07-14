#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float v_fogFactor : TEXCOORD1;
};

cbuffer fs_ub : register(b1, space0) {
    float4 u_fogColor;
};

Texture2D u_tex0 : register(t0, space1);

float4 main(PS_IN input) : SV_Target0 {
    float4 finalColor = u_tex0.Sample(LinearClamp, input.v_texCoord);
    return lerp(u_fogColor, finalColor, input.v_fogFactor);
}
