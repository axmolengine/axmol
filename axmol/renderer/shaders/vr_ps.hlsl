#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float v_vignette : TEXCOORD1;
};

Texture2D u_tex0 : register(t0, space1);

float4 main(PS_IN input) : SV_Target0
{
    return input.v_vignette * u_tex0.Sample(LinearClamp, input.v_texCoord);
}
