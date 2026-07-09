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
#if AXSLC_UV_TOP
    float2 fragCoord = float2(input.gl_FragCoord.x, u_screenSize.y - input.gl_FragCoord.y);
#else
    float2 fragCoord = input.gl_FragCoord.xy;
#endif
    float2 p = 2.0 * (fragCoord - center.xy) / resolution.xy;
    p.x *= resolution.x / resolution.y;

    float zoo = 0.62 + 0.38 * sin(0.1 * u_Time[1]);
    float coa = cos(0.1 * (1.0 - zoo) * u_Time[1]);
    float sia = sin(0.1 * (1.0 - zoo) * u_Time[1]);
    zoo = pow(zoo, 8.0);
    float2 xy = float2(p.x * coa - p.y * sia, p.x * sia + p.y * coa);
    float2 cc = float2(-0.745, 0.186) + xy * zoo;

    float2 z = float2(0.0, 0.0);
    float2 z2 = z * z;
    float m2;
    float co = 0.0;

    for (int i = 0; i < 256; i++)
    {
        z = cc + float2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y);
        m2 = dot(z, z);
        if (m2 > 1024.0) break;
        co += 1.0;
    }
    co = co + 1.0 - log2(0.5 * log2(m2));

    co = sqrt(co / 256.0);
    return float4(0.5 + 0.5 * cos(6.2831 * co + 0.0),
                  0.5 + 0.5 * cos(6.2831 * co + 0.4),
                  0.5 + 0.5 * cos(6.2831 * co + 0.7),
                  1.0);
}
