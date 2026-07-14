#include "base.hlsli"

struct PS_IN {
    float4 gl_FragCoord : SV_Position;
};

cbuffer fs_ub : register(b1, space0) {
    float2 center;
    float2 resolution;
    float2 u_screenSize;
    float4 u_Time;
};

float u(float x) { return (x > 0.0) ? 1.0 : 0.0; }

float4 main(PS_IN input) : SV_Target0 {
    float time = u_Time[1];
#if AXSLC_UV_TOP
    float2 fragCoord = float2(input.gl_FragCoord.x, u_screenSize.y - input.gl_FragCoord.y);
#else
    float2 fragCoord = input.gl_FragCoord.xy;
#endif
    float2 p = 2.0 * (fragCoord - center.xy) / resolution.xy;

    float a = atan2(p.x, p.y);
    float r = length(p) * 0.75;

    float w = cos(3.1415927 * time - r * 2.0);
    float h = 0.5 + 0.5 * cos(12.0 * a - w * 7.0 + r * 8.0);
    float d = 0.25 + 0.75 * pow(h, 1.0 * r) * (0.7 + 0.3 * w);

    float rd = 1.0 - r / d;
    float col = 0.0;
    if (rd > 0.0)
        col = u(d - r) * sqrt(rd) * r * 2.5;
    col *= 1.25 + 0.25 * cos((12.0 * a - w * 7.0 + r * 8.0) / 2.0);
    col *= 1.0 - 0.35 * (0.5 + 0.5 * sin(r * 30.0)) * (0.5 + 0.5 * cos(12.0 * a - w * 7.0 + r * 8.0));
    return float4(
        col,
        col - h * 0.5 + r * 0.2 + 0.35 * h * (1.0 - r),
        col - h * r + 0.1 * h * (1.0 - r),
        1.0);
}
