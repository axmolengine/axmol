#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0;

float4 main(PS_IN input) : SV_Target0
{
    float4 c = u_tex0.Sample(LinearClamp, input.v_texCoord);
    return input.v_color * float4(c.r, c.r, c.r, 1);
}
