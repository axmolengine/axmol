#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float4 v_color : COLOR0;
};

cbuffer fs_ub : register(b1, space0) {
    float2 resolution;
    float4 u_Time;
    float2 u_screenSize;
};

Texture2D u_tex0 : register(t0, space1);

float3 noise(float2 uv)
{
    float2 p = abs(sin(uv * 13.0 + uv.x * u_Time.y * sin(uv.y)));
    float3 r;
    r.x = sin(0.2 * u_Time.y + sin(p.x * 0.5) * u_Time.y / cos(50.0)) * 10.0;
    r.y = 0.3 + 0.5 * abs(sin(u_Time.y * tan(5.0)));
    r.z = 0.0;
    return r;
}

float4 main(PS_IN input) : SV_Target0
{
    float intensity = 0.05;
    float3 n = noise(u_screenSize / sin(resolution * u_Time.y * 0.01));
    float4 tex = u_tex0.Sample(LinearClamp, input.v_texCoord);
    return float4(intensity * n + (1.0 - intensity) * tex.xyz, 1.0);
}
