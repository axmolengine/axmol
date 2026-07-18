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

float4 main(PS_IN input) : SV_Target0 {
    float time = u_Time[1];
#if AXSLC_UV_TOP
    float2 fragCoord = float2(input.gl_FragCoord.x, u_screenSize.y - input.gl_FragCoord.y);
#else
    float2 fragCoord = input.gl_FragCoord.xy;
#endif
    float2 p = 2.0 * (fragCoord - center.xy) / resolution.xy;
    float2 cc = float2(cos(0.25 * time), sin(0.25 * time * 1.423));

    float dmin = 1000.0;
    float2 z = p * float2(1.33, 1.0);
    for (int i = 0; i < 64; i++)
    {
        z = cc + float2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y);
        float m2 = dot(z, z);
        if (m2 > 100.0) break;
        dmin = min(dmin, m2);
    }

    float color = sqrt(sqrt(dmin)) * 0.7;
    return float4(color, color, color, 1.0);
}
