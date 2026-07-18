#include "base.hlsli"

struct PS_IN {
    float4 gl_FragCoord : SV_Position;
};

cbuffer fs_ub {
    float2 center;
    float2 resolution;
    float2 u_screenSize;
    float4 u_Time;
};

float snoise(float3 uv, float res)
{
    const float3 s = float3(1e0, 1e2, 1e4);

    uv *= res;

    float3 uv0 = floor(fmod(uv, res)) * s;
    float3 uv1 = floor(fmod(uv + float3(1.0, 1.0, 1.0), res)) * s;

    float3 f = frac(uv);
    f = f * f * (3.0 - 2.0 * f);

    float4 v = float4(uv0.x + uv0.y + uv0.z, uv1.x + uv0.y + uv0.z,
                       uv0.x + uv1.y + uv0.z, uv1.x + uv1.y + uv0.z);

    float4 r = frac(sin(v * 1e-3) * 1e5);
    float r0 = lerp(lerp(r.x, r.y, f.x), lerp(r.z, r.w, f.x), f.y);

    r = frac(sin((v + uv1.z - uv0.z) * 1e-3) * 1e5);
    float r1 = lerp(lerp(r.x, r.y, f.x), lerp(r.z, r.w, f.x), f.y);

    return lerp(r0, r1, f.z) * 2.0 - 1.0;
}

float4 main(PS_IN input) : SV_Target0 {
#if AXSLC_UV_TOP
    float2 fragCoord = float2(input.gl_FragCoord.x, u_screenSize.y - input.gl_FragCoord.y);
#else
    float2 fragCoord = input.gl_FragCoord.xy;
#endif
    float2 iResolution = resolution;
    float iGlobalTime = u_Time[1];

    float2 p = (fragCoord.xy - center.xy) / iResolution.xy;
    p.x *= iResolution.x / iResolution.y;

    float color = 3.0 - (3.0 * length(2.0 * p));

    float3 coord = float3(atan2(p.x, p.y) / 6.2832 + 0.5, length(p) * 0.4, 0.5);

    for (int i = 1; i <= 3; i++)
    {
        float power = pow(2.0, float(i));
        color += (1.5 / power) * snoise(coord + float3(0.0, -iGlobalTime * 0.05, iGlobalTime * 0.01), power * 16.0);
    }
    return float4(color, pow(max(color, 0.0), 2.0) * 0.4, pow(max(color, 0.0), 3.0) * 0.15, 1.0);
}
