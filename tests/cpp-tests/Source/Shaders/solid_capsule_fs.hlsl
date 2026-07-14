#include "base.hlsli"

struct PS_IN {
    float2 f_position : TEXCOORD0;
    float4 f_color : TEXCOORD1;
    float f_length : TEXCOORD2;
    float f_thickness : TEXCOORD3;
};

float4 blend_colors(float4 front, float4 back) {
    float3 cSrc = front.rgb;
    float alphaSrc = front.a;
    float3 cDst = back.rgb;
    float alphaDst = back.a;
    float3 cOut = cSrc * alphaSrc + cDst * alphaDst * (1.0 - alphaSrc);
    float alphaOut = alphaSrc + alphaDst * (1.0 - alphaSrc);
    cOut = cOut / alphaOut;
    return float4(cOut, alphaOut);
}

float4 main(PS_IN input) : SV_Target0 {
    float radius = 0.5 * (2.0 - input.f_length);

    float4 borderColor = input.f_color;
    float4 fillColor = 0.6 * borderColor;

    float2 v1 = float2(-0.5 * input.f_length, 0);
    float2 v2 = float2(0.5 * input.f_length, 0);

    // distance to line segment
    float2 e = v2 - v1;
    float2 w = input.f_position - v1;
    float we = dot(w, e);
    float2 b = w - e * clamp(we / dot(e, e), 0.0, 1.0);
    float dw = length(b);

    // SDF union of capsule and line segment
    float d = min(dw, abs(dw - radius));

    // roll the fill alpha down at the border
    float4 back = float4(fillColor.rgb, fillColor.a * smoothstep(radius + input.f_thickness, radius, dw));

    // roll the border alpha down from 1 to 0 across the border thickness
    float4 front = float4(borderColor.rgb, smoothstep(input.f_thickness, 0.0, d));

    return blend_colors(front, back);
}
