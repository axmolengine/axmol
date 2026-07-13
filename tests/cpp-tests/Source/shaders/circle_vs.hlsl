#include "base.hlsli"

struct VS_IN {
    float2 a_localPosition : LOCAL_POSITION;
    float4 a_instanceColor : INSTANCE_COLOR0;
    float4 a_instancePosAndRadius : INSTANCE_POS_AND_RADIUS0;
};

struct VS_OUT {
    float4 position : SV_Position;
    float2 f_position : TEXCOORD0;
    float4 f_color : TEXCOORD1;
    float f_thickness : TEXCOORD2;
};

cbuffer vs_ub : register(b0, space0) {
    float4x4 u_MVPMatrix;
    float u_pixelScale;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.f_position = input.a_localPosition;
    output.f_color = input.a_instanceColor;
    float radius = input.a_instancePosAndRadius.z;
    output.f_thickness = 3.0 / (u_pixelScale * radius);
    float2 p = float2(radius * input.a_localPosition.x, radius * input.a_localPosition.y)
             + input.a_instancePosAndRadius.xy;
    output.position = mul(u_MVPMatrix, float4(p, 0.0, 1.0));
    return output;
}
