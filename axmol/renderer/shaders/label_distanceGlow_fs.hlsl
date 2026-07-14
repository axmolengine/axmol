#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0 : register(t0, space1);

cbuffer fs_ub : register(b1, space0) {
    float4  u_textColor;     // text color
    float4  u_effectColor;   // effect color (rgb = color, a = intensity)
    float u_effectWidth;   // glow width in pixels
    int   u_labelPass;     // 0: text, 1: glow, 2: shadow
};

float4 main(PS_IN input) : SV_Target0
{
    float dist = u_tex0.Sample(LinearClamp, input.v_texCoord).x;
    float smoothing = fwidth(dist);
    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);

    if (u_labelPass == 0) {
        // Text pass: solid core
        return input.v_color * float4(u_textColor.rgb, u_textColor.a * alpha);
    }
    else if (u_labelPass == 1) {
        // Glow pass: soft halo around text
        // Use distance field falloff to create smooth glow
        // Map u_effectWidth (in "visual units") into SDF domain
        float pivot = clamp(0.5 + (1.0 - u_effectWidth), 0.0, 1.0);
        float mu = smoothstep(0.0, pivot, dist);
        float4 color = u_effectColor * (1.0 - alpha) + u_textColor * alpha;
        return input.v_color * float4(color.rgb, max(alpha, mu) * color.a);
    }
    else {
        // Shadow pass: simple fill
        return input.v_color * float4(u_effectColor.rgb, u_effectColor.a * alpha);
    }
}
