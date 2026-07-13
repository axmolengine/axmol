#include "base.hlsli"

struct PS_IN {
    float4 position : SV_Position;
};

cbuffer fs_ub : register(b0, space0) {
    float2 u_resolution;
    float u_time;
    float3 u_baseColor;
};

float rand(float2 st) {
    return frac(sin(dot(st.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

float4 main(PS_IN input) : SV_Target0 {
    float2 uv = input.position.xy / u_resolution.xy;
    float noise = rand(uv + u_time * 0.1);
    float grainIntensity = 0.01;
    float3 color = u_baseColor + float3(noise * grainIntensity, noise * grainIntensity, noise * grainIntensity);
    return float4(color, 1.0);
}
