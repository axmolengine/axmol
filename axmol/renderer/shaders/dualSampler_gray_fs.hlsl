#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2DArray u_tex0 : register(t0, space1);

float4 main(PS_IN input) : SV_Target0
{
    float3 uv0 = float3(input.v_texCoord, 0.0);
    float3 uv1 = float3(input.v_texCoord, 1.0);
    float4 texColor = u_tex0.Sample(LinearClamp, uv0);
    texColor.a = u_tex0.Sample(LinearClamp, uv1).r;
    texColor.rgb *= texColor.a; // premultiply alpha channel

    texColor = input.v_color * texColor;

    float luma = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
    float4 FragColor;
    FragColor.rgb = float3(luma, luma, luma);
    FragColor.a = texColor.a;
    return FragColor;
}
