#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
};

cbuffer fs_ub {
    float4 baseColor;
};

Texture2D u_tex0;

float4 main(PS_IN input) : SV_Target0 {
    float4 c = u_tex0.Sample(LinearClamp, input.v_texCoord) * baseColor;
    return float4(c.rgb * c.a, c.a);
}
