#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float4 v_position : POSITION;
};

cbuffer fs_ub : register(b1, space0) {
    float3 u_target_pos;
    float4 u_color;
};

Texture2D u_sampler0 : register(t0, space1);
Texture2D u_sampler1 : register(t1, space1);

float4 main(PS_IN input) : SV_Target0 {
    float Radius = 4.0;
    float3 UVector = float3(1.0, 0.0, 0.0) / (2.0 * Radius);
    float3 VVector = float3(0.0, 0.0, -1.0) / (-2.0 * Radius);
    float2 coord;
    coord.x = dot(input.v_position.xyz - u_target_pos, UVector) + 0.5;
    coord.y = dot(input.v_position.xyz - u_target_pos, VVector) + 0.5;

    return u_color * u_sampler0.Sample(LinearClamp, input.v_texCoord) * u_sampler1.Sample(LinearClamp, coord);
}
