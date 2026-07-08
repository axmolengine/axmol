#include "base.hlsli"

struct PS_IN {
    float3 v_reflect : TEXCOORD0;
};

TextureCube u_Env : register(t0, space1);

cbuffer fs_ub : register(b1, space0) {
    float4 u_color;
};

float4 main(PS_IN input) : SV_Target0
{
    return u_Env.Sample(LinearClamp, input.v_reflect) * u_color;
}
