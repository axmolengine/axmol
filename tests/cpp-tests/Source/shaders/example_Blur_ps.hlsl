#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float4 v_color : COLOR0;
};

Texture2D u_tex0 : register(t0, space1);

cbuffer fs_ub : register(b1, space0) {
    float2 resolution;
    float blurRadius;
    float sampleNum;
};

float4 blur(float2 p)
{
    if (blurRadius > 0.0 && sampleNum > 1.0)
    {
        float4 col = float4(0.0, 0.0, 0.0, 0.0);
        float2 unit = 1.0 / resolution.xy;

        float r = blurRadius;
        float sampleStep = r / sampleNum;

        float count = 0.0;

        for (float x = -r; x < r; x += sampleStep)
        {
            for (float y = -r; y < r; y += sampleStep)
            {
                float weight = (r - abs(x)) * (r - abs(y));
                col += u_tex0.Sample(LinearClamp, p + float2(x * unit.x, y * unit.y)) * weight;
                count += weight;
            }
        }

        return col / count;
    }

    return u_tex0.Sample(LinearClamp, p);
}

float4 main(PS_IN input) : SV_Target0 {
    float4 col = blur(input.v_texCoord);
    return float4(col) * input.v_color;
}
