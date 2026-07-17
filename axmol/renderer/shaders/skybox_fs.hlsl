#include "base.hlsli"

struct PS_IN {
    float3 v_reflect : TEXCOORD0;
};

TextureCube u_Env;

cbuffer fs_ub {
    float4 u_color;
};

float4 main(PS_IN input) : SV_Target0
{
    return u_Env.Sample(LinearClamp, input.v_reflect) * u_color;
}
