#include "base.hlsli"

struct VS_IN {
    float2 a_localPosition : POSITION;
    float4 a_instanceColor : TEXCOORD0;
    float4 a_instancePosAndRadius : TEXCOORD1;
};

struct VS_OUT {
    float2 f_position : TEXCOORD0;
    float4 f_color : TEXCOORD1;
    float f_thickness : TEXCOORD2;
    float4 position : SV_Position;
};

cbuffer vs_ub {
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
