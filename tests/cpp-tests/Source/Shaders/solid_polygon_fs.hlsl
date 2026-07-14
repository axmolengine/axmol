#include "base.hlsli"

struct PS_IN {
    float2 f_position : TEXCOORD0;
    float4 f_color : TEXCOORD1;
    float2 f_points[8] : TEXCOORD2;
    nointerpolation int f_count : TEXCOORD10;
    float f_radius : TEXCOORD11;
    float f_thickness : TEXCOORD12;
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

float cross2d(float2 v1, float2 v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

// Signed distance function for convex polygon
float sdConvexPolygon(float2 p, float2 v[8], int count) {
    float d = dot(p - v[0], p - v[0]);
    float side = -1.0;
    int j = count - 1;
    for (int i = 0; i < count; ++i) {
        float2 e = v[i] - v[j];
        float2 w = p - v[j];
        float we = dot(w, e);
        float2 b = w - e * clamp(we / dot(e, e), 0.0, 1.0);
        float bb = dot(b, b);
        if (bb < d) {
            d = bb;
        }
        float s = cross2d(w, e);
        if (s >= 0.0) {
            side = 1.0;
        }
        j = i;
    }
    return side * sqrt(d);
}

float4 main(PS_IN input) : SV_Target0 {
    float4 borderColor = input.f_color;
    float4 fillColor = 0.6 * borderColor;

    float dw = sdConvexPolygon(input.f_position, input.f_points, input.f_count);
    float d = abs(dw - input.f_radius);

    float4 back = float4(fillColor.rgb, fillColor.a * smoothstep(input.f_radius + input.f_thickness, input.f_radius, dw));
    float4 front = float4(borderColor.rgb, smoothstep(input.f_thickness, 0.0, d));

    return blend_colors(front, back);
}
