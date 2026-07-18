#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0;

float4 main(PS_IN input) : SV_Target0
{
    float4 c = u_tex0.Sample(LinearClamp, input.v_texCoord);
    c = input.v_color * c;
    float luma = 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b;
    float4 FragColor;
    FragColor.xyz = float3(luma, luma, luma);
    FragColor.w = c.w;
    return FragColor;
}
