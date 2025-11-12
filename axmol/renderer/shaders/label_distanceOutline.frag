#version 310 es
precision highp float;
#include "base.glsl"

// Empirical correction factor to visually match SDF outline thickness
// with non‑SDF (bitmap/vector) text rendering. Adjusting this value
// makes the two rendering methods look nearly identical in outline weight.
const float outlineScale = 0.425;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(set = 1, binding = 0) uniform sampler2D u_tex0;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec4 u_textColor;
    vec4 u_effectColor;
    float u_effectWidth;   // outline thickness in pixels
    int u_labelPass; // 0: text, 1: outline, 2: shadow
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    float dist = texture(u_tex0, v_texCoord).x;
    float smoothing = fwidth(dist);
    float textAlpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);

    if (u_labelPass == 0) {
        // Text pass: draw solid text core
        FragColor = v_color * vec4(u_textColor.rgb, u_textColor.a * textAlpha);
    }
    else if (u_labelPass == 1) {
        // Outline pass: only draw outer ring, exclude text core
        // clamp(u_effectWidth * outlineScale, 0.0, u_distanceSpread * 0.5);
        float pivot = clamp(0.5 - (u_effectWidth * outlineScale), 0.0, 1.0);
        float outlineAlpha = smoothstep(pivot - smoothing, pivot + smoothing, dist);

        float alpha = outlineAlpha * (1.0 - textAlpha); // exclude inner text
        FragColor = v_color * vec4(u_effectColor.rgb, u_effectColor.a * alpha);
    }
    else {
        // Shadow pass: pure color fill
        FragColor = v_color * vec4(u_effectColor.rgb, u_effectColor.a * textAlpha);
    }
}
