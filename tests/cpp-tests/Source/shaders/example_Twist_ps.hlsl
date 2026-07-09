#include "base.hlsli"

struct PS_IN {
    float4 gl_FragCoord : SV_Position;
};

Texture2D tex0 : register(t0, space1);
Texture2D tex1 : register(t1, space1);

cbuffer fs_ub : register(b1, space0) {
    float2 resolution;
    float2 u_screenSize;
    float4 u_Time;
    float4 u_CosTime;
};

float4 main(PS_IN input) : SV_Target0 {
    float2 fragCoord = input.gl_FragCoord.xy;
    float time = u_Time[1];
    float2 p = -1.0 + 2.0 * fragCoord / resolution.xy;
    float2 uv;

    float a = atan2(p.y, p.x);
    float r = sqrt(dot(p, p));

    uv.x = r - u_Time[2];
    uv.y = sin(a * 10.0 + 2.0) * u_CosTime[0];

    float3 col = (0.5 + 0.5 * uv.y) * tex0.Sample(LinearClamp, uv).xyz;

    return float4(col, 1.0);
}
