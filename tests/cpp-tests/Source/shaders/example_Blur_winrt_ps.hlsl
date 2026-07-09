#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float4 v_color : COLOR0;
};

Texture2D u_tex0 : register(t0, space1);

cbuffer fs_ub : register(b1, space0) {
    float2 resolution;
};

float4 blur(float2 p)
{
    float4 col = float4(0.0, 0.0, 0.0, 0.0);
    float2 unit = 1.0 / resolution.xy;

    float count = 0.0;

    for (float x = -4.0; x <= 4.0; x += 2.0)
    {
        for (float y = -4.0; y <= 4.0; y += 2.0)
        {
            float weight = (4.0 - abs(x)) * (4.0 - abs(y));
            col += u_tex0.Sample(LinearClamp, p + float2(x * unit.x, y * unit.y)) * weight;
            count += weight;
        }
    }

    return col / count;
}

float4 main(PS_IN input) : SV_Target0 {
    float4 col = blur(input.v_texCoord);
    return float4(col) * input.v_color;
}
