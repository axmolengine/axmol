#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
    float4 pixelCoord : SV_Position;
};

cbuffer fs_ub {
    float2 resolution;
    float4 u_Time;
    float2 u_screenSize;
};

Texture2D u_tex0;

float3 noise(float2 uv)
{
    float2 p = abs(sin(uv * 13.0 + uv.x * u_Time.y * sin(uv.y)));
    float3 r;
    float2 s = sin(p * 0.5);
    r.x = sin(0.2 * u_Time.y + s.x * u_Time.y / cos(50.0)) * 10.0;
    r.y = sin(0.2 * u_Time.y + s.y * u_Time.y / cos(50.0)) * 10.0;
    r.z = 0.3 + 0.5 * abs(sin(u_Time.y * tan(5.0)));
    return r;
}

float4 main(PS_IN input) : SV_Target0
{
    float intensity = 0.05;
    float3 n = noise(input.pixelCoord.xy / sin(resolution * u_Time.y * 0.01));
    float4 tex = u_tex0.Sample(LinearClamp, input.v_texCoord);
    return float4(intensity * n + (1.0 - intensity) * tex.xyz, 1.0);
}
