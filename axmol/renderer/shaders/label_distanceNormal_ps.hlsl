#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0 : register(t0, space1);

cbuffer fs_ub : register(b1, space0) {
    float4 u_textColor;
};

float4 main(PS_IN input) : SV_Target0
{
    float dist = u_tex0.Sample(LinearClamp, input.v_texCoord).x;
    float smoothing = fwidth(dist);

    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist) * u_textColor.a;
    return input.v_color * float4(u_textColor.rgb, alpha);
}
