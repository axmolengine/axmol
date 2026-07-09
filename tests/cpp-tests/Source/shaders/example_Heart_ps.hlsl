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

float4 main(PS_IN input) : SV_Target0 {
    float time = u_Time[1];
    float2 fragCoord = input.gl_FragCoord.xy;
    float2 p = 2.0 * (fragCoord - center.xy) / resolution.xy;

    float tt = fmod(time, 2.0) / 2.0;
    float ss = pow(tt, 0.2) * 0.5 + 0.5;
    ss -= ss * 0.2 * sin(tt * 6.2831 * 5.0) * exp(-tt * 6.0);
    p *= float2(0.5, 1.5) + ss * float2(0.5, -0.5);

    float a = atan2(p.x, p.y) / 3.141593;
    float r = length(p);

    float h = abs(a);
    float d = (13.0 * h - 22.0 * h * h + 10.0 * h * h * h) / (6.0 - 5.0 * h);

    float f = step(r, d) * pow(1.0 - r / d, 0.25);

    return float4(f, 0.0, 0.0, 1.0);
}
