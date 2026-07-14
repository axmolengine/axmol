#include "base.hlsli"

struct PS_IN {
    float3 v_reflect : TEXCOORD0;
};

cbuffer fs_ub : register(b1, space0) {
    float4 u_color;
};

TextureCube u_cubeTex : register(t0, space1);

float4 main(PS_IN input) : SV_Target0 {
    return u_cubeTex.Sample(LinearClamp, input.v_reflect) * u_color;
}
