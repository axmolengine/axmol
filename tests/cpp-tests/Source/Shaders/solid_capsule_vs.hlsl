#include "base.hlsli"

struct VS_IN {
    float2 a_localPosition : POSITION;
    float4 a_instanceColor : TEXCOORD0;
    float4 a_instanceRadiusAndLength : TEXCOORD1;
    float4 a_instanceTransform : TEXCOORD2;
};

struct VS_OUT {
    float4 position : SV_Position;
    float2 f_position : TEXCOORD0;
    float4 f_color : TEXCOORD1;
    float f_length : TEXCOORD2;
    float f_thickness : TEXCOORD3;
};

cbuffer vs_ub : register(b0, space0) {
    float4x4 u_MVPMatrix;
    float u_pixelScale;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.f_position = input.a_localPosition;
    output.f_color = input.a_instanceColor;

    float radius = input.a_instanceRadiusAndLength.x;
    float length = input.a_instanceRadiusAndLength.y;

    // scale quad large enough to hold capsule
    float scale = radius + 0.5 * length;

    // quad range of [-1, 1] implies normalize radius and length
    output.f_length = length / scale;

    output.f_thickness = 3.0 / (u_pixelScale * scale);

    float x = input.a_instanceTransform.x;
    float y = input.a_instanceTransform.y;
    float c = input.a_instanceTransform.z;
    float s = input.a_instanceTransform.w;
    float2 p = float2(scale * input.a_localPosition.x, scale * input.a_localPosition.y);
    p = float2((c * p.x - s * p.y) + x, (s * p.x + c * p.y) + y);
    output.position = mul(u_MVPMatrix, float4(p, 0.0, 1.0));
    return output;
}
