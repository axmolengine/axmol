#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float4 v_color : COLOR0;
};

Texture2D u_tex0 : register(t0, space1);

float4 main(PS_IN input) : SV_Target0 {
    float4 c = u_tex0.Sample(LinearClamp, input.v_texCoord);
    float grey = 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b;
    return float4(grey, grey, grey, c.w);
}
