#include "base.hlsli"

struct VS_IN {
    float3 a_position : POSITION;
};

struct VS_OUT {
    float3 v_reflect : TEXCOORD0;
    float4 position : SV_Position;
};

cbuffer vs_ub {
    float4x4 u_cameraRot;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    float4 reflect = mul(u_cameraRot, float4(input.a_position, 1.0));
    output.v_reflect = reflect.xyz;
    output.position = float4(input.a_position.xy, 1.0, 1.0);
    return output;
}
