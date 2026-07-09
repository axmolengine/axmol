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

float noise(float t)
{
    return 0.0;
}
float noise(float2 t)
{
    return 0.0;
}

float3 lensflare(float2 uv, float2 pos)
{
    float2 main = uv - pos;
    float2 uvd = uv * (length(uv));

    float ang = atan2(main.x, main.y);
    float dist = length(main);
    dist = pow(dist, 0.1);
    float n = noise(float2(ang * 16.0, dist * 32.0));

    float f0 = 1.0 / (length(uv - pos) * 16.0 + 1.0);

    f0 = f0 + f0 * (sin(noise((pos.x + pos.y) * 2.2 + ang * 4.0 + 5.954) * 16.0) * 0.1 + dist * 0.1 + 0.8);

    float f1 = max(0.01 - pow(length(uv + 1.2 * pos), 1.9), 0.0) * 7.0;

    float f2 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.8 * pos), 2.0)), 0.0) * 0.25;
    float f22 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.85 * pos), 2.0)), 0.0) * 0.23;
    float f23 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.9 * pos), 2.0)), 0.0) * 0.21;

    float2 uvx = lerp(uv, uvd, -0.5);

    float f4 = max(0.01 - pow(length(uvx + 0.4 * pos), 2.4), 0.0) * 6.0;
    float f42 = max(0.01 - pow(length(uvx + 0.45 * pos), 2.4), 0.0) * 5.0;
    float f43 = max(0.01 - pow(length(uvx + 0.5 * pos), 2.4), 0.0) * 3.0;

    uvx = lerp(uv, uvd, -0.4);

    float f5 = max(0.01 - pow(length(uvx + 0.2 * pos), 5.5), 0.0) * 2.0;
    float f52 = max(0.01 - pow(length(uvx + 0.4 * pos), 5.5), 0.0) * 2.0;
    float f53 = max(0.01 - pow(length(uvx + 0.6 * pos), 5.5), 0.0) * 2.0;

    uvx = lerp(uv, uvd, -0.5);

    float f6 = max(0.01 - pow(length(uvx - 0.3 * pos), 1.6), 0.0) * 6.0;
    float f62 = max(0.01 - pow(length(uvx - 0.325 * pos), 1.6), 0.0) * 3.0;
    float f63 = max(0.01 - pow(length(uvx - 0.35 * pos), 1.6), 0.0) * 5.0;

    float3 c = float3(0.0, 0.0, 0.0);

    c.r += f2 + f4 + f5 + f6;
    c.g += f22 + f42 + f52 + f62;
    c.b += f23 + f43 + f53 + f63;
    c = c * 1.3 - float3(length(uvd) * 0.05, length(uvd) * 0.05, length(uvd) * 0.05);
    c += float3(f0, f0, f0);

    return c;
}

float3 cc(float3 color, float factor, float factor2)
{
    float w = color.x + color.y + color.z;
    return lerp(color, float3(w, w, w) * factor, w * factor2);
}

float4 main(PS_IN input) : SV_Target0 {
    float2 fragCoord = input.gl_FragCoord.xy;
    float2 iResolution = resolution;
    float iGlobalTime = u_Time[1];

    float2 uv = (fragCoord.xy - center.xy) / iResolution.xy;
    uv.x *= iResolution.x / iResolution.y;
    float3 hover = float3(-0.5, -0.5, -0.5);
    hover.x *= iResolution.x / iResolution.y;
    hover.x = sin(iGlobalTime) * 0.5;
    hover.y = sin(iGlobalTime * 0.913) * 0.5;

    float3 color = float3(1.4, 1.2, 1.0) * lensflare(uv, hover.xy);
    color -= noise(fragCoord.xy) * 0.015;
    color = cc(color, 0.5, 0.1);
    return float4(color, 1.0);
}
