#include "base.hlsli"

struct PS_IN {
    float4 v_light : COLOR0;
    float4 v_dark : COLOR1;
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0 : register(t0, space1);

float4 main(PS_IN input) : SV_Target0
{
    float4 texColor = u_tex0.Sample(LinearClamp, input.v_texCoord);
    float alpha = texColor.a * input.v_light.a;
    float4 FragColor;
    FragColor.a = alpha;
    FragColor.rgb = ((texColor.a - 1.0) * input.v_dark.a + 1.0 - texColor.rgb) * input.v_dark.rgb + texColor.rgb * input.v_light.rgb;
    return FragColor;
}
