#include "base.hlsli"

#include "colorUtils.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2DArray u_tex0;

cbuffer fs_ub {
    float3 u_hsv;
};

float4 main(PS_IN input) : SV_Target0
{
    float3 uv0 = float3(input.v_texCoord, 0.0);
    float3 uv1 = float3(input.v_texCoord, 1.0);
    float4 texColor = float4(u_tex0.Sample(LinearClamp, uv0).rgb, u_tex0.Sample(LinearClamp, uv1).r);
    texColor.rgb *= texColor.a; // Premultiply with Alpha channel

    texColor.rgb = transformHSV(texColor.rgb, u_hsv);

    return texColor * input.v_color;
}
