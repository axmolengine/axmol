#version 310 es
precision highp float;
#include "base.glsl"

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec4  u_textColor;     // text color
    vec4  u_effectColor;   // effect color (rgb = color, a = intensity)
    float u_effectWidth;   // glow width in pixels
    float u_distanceSpread;// default: 6.0
    int   u_labelPass;     // 0: text, 1: glow, 2: shadow
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    float dist = texture(u_tex0, v_texCoord).x;
    float smoothing = fwidth(dist);

    if (u_labelPass == 0) {
        // Text pass: solid core
        float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);
        FragColor = v_color * vec4(u_textColor.rgb, u_textColor.a * alpha);
    }
    else if (u_labelPass == 1) {
        // Glow pass: soft halo around text
        // Use distance field falloff to create smooth glow
        // Map u_effectWidth (in "visual units") into SDF domain
        float glowWidth = clamp(u_effectWidth / u_distanceSpread, 0.0, 1.0);
        float pivot = 0.5 + (1.0 - glowWidth);
        float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);
        float mu = smoothstep(0.5, pivot, sqrt(dist));
        vec4 color = u_effectColor * (1.0 - alpha) + u_textColor * alpha;
        FragColor = v_color * vec4(color.rgb, max(alpha, mu) * color.a);
    }
    else {
        // Shadow pass: simple fill
        float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);
        FragColor = v_color * vec4(u_effectColor.rgb, u_effectColor.a * alpha);
    }
}
