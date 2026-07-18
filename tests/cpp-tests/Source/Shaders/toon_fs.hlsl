#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float3 v_normal : NORMAL;
};

cbuffer fs_ub {
    float4 u_color;
};

Texture2D u_sampler0;

float4 main(PS_IN input) : SV_Target0 {
    float3 light_direction = float3(1.0, -1.0, -1.0);
    light_direction = normalize(light_direction);
    float3 light_color = float3(1.0, 1.0, 1.0);
    float3 normal = normalize(input.v_normal);
    float diffuse_factor = dot(normal, -light_direction);
    float4 diffuse_color = u_sampler0.Sample(LinearClamp, input.v_texCoord);

    if (diffuse_factor > 0.95)
        diffuse_factor = 1.0;
    else if (diffuse_factor > 0.75)
        diffuse_factor = 0.8;
    else if (diffuse_factor > 0.50)
        diffuse_factor = 0.6;
    else
        diffuse_factor = 0.4;

    light_color = light_color * diffuse_factor;
    return float4(light_color, 1.0) * diffuse_color * u_color;
}
