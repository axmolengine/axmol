#version 310 es
precision highp float;
#include "base.glsl"

// Maximum distance (in pixels) encoded in the SDF texture.
// Must match the spread value used when generating the font atlas with FreeType.
const float spread = 6.0;

// Global correction factor to visually match SDF outline thickness
// with non‑SDF (bitmap/vector) rendering. Adjust this to make outlines
// look consistent across different rendering methods.
const float outlineScale = 1.5;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec4 u_textColor;
    vec4 u_effectColor;
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    float dist = texture(u_tex0, v_texCoord).x;
    float smoothing = fwidth(dist);
    float outlineSize = clamp(u_effectColor.w * outlineScale, 0.0, spread * 0.5);
    float thickness = outlineSize / (2.0 * spread);
    float pivot = 0.5 - thickness;

    float alpha  = smoothstep(pivot - smoothing, pivot + smoothing, dist);
    float border = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);

    FragColor = v_color * vec4(mix(u_effectColor.rgb, u_textColor.rgb, border), alpha);
}
