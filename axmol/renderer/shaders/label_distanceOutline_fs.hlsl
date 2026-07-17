#include "base.hlsli"

// Empirical correction factor to visually match SDF outline thickness
// with non‑SDF (bitmap/vector) text rendering. Adjusting this value
// makes the two rendering methods look nearly identical in outline weight.
static const float outlineScale = 0.425;

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0;

cbuffer fs_ub {
    float4 u_textColor;
    float4 u_effectColor;
    float u_effectWidth;   // outline thickness in pixels
    int u_labelPass; // 0: text, 1: outline, 2: shadow
};

float4 main(PS_IN input) : SV_Target0
{
    float dist = u_tex0.Sample(LinearClamp, input.v_texCoord).x;
    float smoothing = fwidth(dist);
    float textAlpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);

    if (u_labelPass == 0) {
        // Text pass: draw solid text core
        return input.v_color * float4(u_textColor.rgb, u_textColor.a * textAlpha);
    }
    else if (u_labelPass == 1) {
        // Outline pass: only draw outer ring, exclude text core
        // clamp(u_effectWidth * outlineScale, 0.0, u_distanceSpread * 0.5);
        float pivot = clamp(0.5 - (u_effectWidth * outlineScale), 0.0, 1.0);
        float outlineAlpha = smoothstep(pivot - smoothing, pivot + smoothing, dist);

        float alpha = outlineAlpha * (1.0 - textAlpha); // exclude inner text
        return input.v_color * float4(u_effectColor.rgb, u_effectColor.a * alpha);
    }
    else {
        // Shadow pass: pure color fill
        return input.v_color * float4(u_effectColor.rgb, u_effectColor.a * textAlpha);
    }
}
