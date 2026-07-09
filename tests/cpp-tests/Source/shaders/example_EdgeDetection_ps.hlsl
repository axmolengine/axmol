#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float4 v_color : COLOR0;
};

Texture2D u_tex0 : register(t0, space1);

cbuffer fs_ub : register(b1, space0) {
    float2 resolution;
};

float lookup(float2 p, float dx, float dy)
{
    float2 uv = p.xy + float2(dx, dy) / resolution.xy;
    float4 c = u_tex0.Sample(LinearClamp, uv.xy);
    return 0.2126 * c.r + 0.7152 * c.g + 0.0722 * c.b;
}

float4 main(PS_IN input) : SV_Target0 {
    float2 p = input.v_texCoord.xy;

    float gx = 0.0;
    gx += -1.0 * lookup(p, -1.0, -1.0);
    gx += -2.0 * lookup(p, -1.0,  0.0);
    gx += -1.0 * lookup(p, -1.0,  1.0);
    gx +=  1.0 * lookup(p,  1.0, -1.0);
    gx +=  2.0 * lookup(p,  1.0,  0.0);
    gx +=  1.0 * lookup(p,  1.0,  1.0);

    float gy = 0.0;
    gy += -1.0 * lookup(p, -1.0, -1.0);
    gy += -2.0 * lookup(p,  0.0, -1.0);
    gy += -1.0 * lookup(p,  1.0, -1.0);
    gy +=  1.0 * lookup(p, -1.0,  1.0);
    gy +=  2.0 * lookup(p,  0.0,  1.0);
    gy +=  1.0 * lookup(p,  1.0,  1.0);

    float g = gx * gx + gy * gy;

    return float4(1.0 - g, 1.0 - g, 1.0 - g, 1.0);
}
