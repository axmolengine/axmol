#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

cbuffer fs_ub : register(b1, space0) {
    float u_interpolate;
};

Texture2D u_tex0 : register(t0, space1);
Texture2D u_tex1 : register(t1, space1);

float4 main(PS_IN input) : SV_Target0 {
    float4 color1 = u_tex0.Sample(LinearClamp, input.v_texCoord);
    float4 color2 = u_tex1.Sample(LinearClamp, input.v_texCoord);
    return input.v_color * lerp(color1, color2, u_interpolate);
}
