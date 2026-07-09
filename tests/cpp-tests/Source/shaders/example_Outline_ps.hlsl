#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float4 v_color : COLOR0;
};

Texture2D u_tex0 : register(t0, space1);

cbuffer fs_ub : register(b1, space0) {
    float3 u_outlineColor;
    float u_threshold;
    float u_radius;
};

float4 main(PS_IN input) : SV_Target0 {
    float radius = u_radius;
    float4 accum = float4(0.0, 0.0, 0.0, 0.0);
    float4 normal = float4(0.0, 0.0, 0.0, 0.0);

    normal = u_tex0.Sample(LinearClamp, float2(input.v_texCoord.x, input.v_texCoord.y));

    accum += u_tex0.Sample(LinearClamp, float2(input.v_texCoord.x - radius, input.v_texCoord.y - radius));
    accum += u_tex0.Sample(LinearClamp, float2(input.v_texCoord.x + radius, input.v_texCoord.y - radius));
    accum += u_tex0.Sample(LinearClamp, float2(input.v_texCoord.x + radius, input.v_texCoord.y + radius));
    accum += u_tex0.Sample(LinearClamp, float2(input.v_texCoord.x - radius, input.v_texCoord.y + radius));

    accum *= u_threshold;
    accum.rgb = u_outlineColor * accum.a;
    accum.a = 1.0;

    normal = (accum * (1.0 - normal.a)) + (normal * normal.a);

    return input.v_color * normal;
}
