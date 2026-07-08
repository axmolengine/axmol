#include "base.hlsli"

#include "colorUtils.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float4 v_color : COLOR0;
};

Texture2D u_tex0 : register(t0, space1);

cbuffer fs_ub : register(b1, space0) {
    float3 u_hsv;
};

float4 main(PS_IN input) : SV_Target0
{
    float4 outColor = u_tex0.Sample(LinearClamp, input.v_texCoord);
    outColor.rgb = transformHSV(outColor.rgb, u_hsv);

    return outColor * input.v_color.a;
}
