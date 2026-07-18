#include "base.hlsli"

struct PS_IN {
    float2 f_position : TEXCOORD0;
    float4 f_color : TEXCOORD1;
    float f_thickness : TEXCOORD2;
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
    float radius = 1.0;

    // distance to axis line segment
    float2 e = float2(radius, 0);
    float2 w = input.f_position;
    float we = dot(w, e);
    float2 b = w - e * clamp(we / dot(e, e), 0.0, 1.0);
    float da = length(b);

    // distance to circle
    float dw = length(w);
    float dc = abs(dw - radius);

    // union of circle and axis
    float d = min(da, dc);

    float4 borderColor = input.f_color;
    float4 fillColor = 0.6 * borderColor;

    // roll the fill alpha down at the border
    float4 back = float4(fillColor.rgb, fillColor.a * smoothstep(radius + input.f_thickness, radius, dw));

    // roll the border alpha down from 1 to 0 across the border thickness
    float4 front = float4(borderColor.rgb, smoothstep(input.f_thickness, 0.0, d));

    return blend_colors(front, back);
}
