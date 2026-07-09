#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
};

cbuffer fs_ub : register(b1, space0) {
    float4 u_color;
    float offset;
    float duration;
};

Texture2D u_sampler0 : register(t0, space1);
Texture2D u_sampler1 : register(t1, space1);

float4 main(PS_IN input) : SV_Target0 {
    float4 color = duration * float4(0.0, 0.8, 0.4, 1.0);
    return u_color * u_sampler0.Sample(LinearClamp, float2(input.v_texCoord.x - 2.0 * offset, input.v_texCoord.y)) * float4(0.3, 0.3, 0.3, 1.0)
        + u_sampler1.Sample(LinearClamp, float2(input.v_texCoord.x - offset, input.v_texCoord.y)).r * color;
}
