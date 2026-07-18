#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0;

cbuffer fs_ub {
    float4 u_textColor;
};

float4 main(PS_IN input) : SV_Target0
{
    return input.v_color * float4(u_textColor.rgb, // RGB from uniform
        u_textColor.a * u_tex0.Sample(LinearClamp, input.v_texCoord).x // x from texture & uniform
    );
}
