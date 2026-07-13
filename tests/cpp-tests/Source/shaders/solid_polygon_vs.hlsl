#include "base.hlsli"

struct VS_IN {
    float2 a_localPosition : LOCAL_POSITION;
    float4 a_instanceTransform : INSTANCE_TRANSFORM0;
    float4 a_instancePoints12 : INSTANCE_POINTS12_0;
    float4 a_instancePoints34 : INSTANCE_POINTS34_0;
    float4 a_instancePoints56 : INSTANCE_POINTS56_0;
    float4 a_instancePoints78 : INSTANCE_POINTS78_0;
    int a_instanceCount : INSTANCE_COUNT0;
    float a_instanceRadius : INSTANCE_RADIUS0;
    float4 a_instanceColor : INSTANCE_COLOR0;
};

struct VS_OUT {
    float4 position : SV_Position;
    float2 f_position : TEXCOORD0;
    float4 f_color : TEXCOORD1;
    float2 f_points[8] : TEXCOORD2;
    nointerpolation int f_count : TEXCOORD10;
    float f_radius : TEXCOORD11;
    float f_thickness : TEXCOORD12;
};

cbuffer vs_ub : register(b0, space0) {
    float4x4 u_MVPMatrix;
    float u_pixelScale;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.f_position = input.a_localPosition;
    output.f_color = input.a_instanceColor;

    output.f_radius = input.a_instanceRadius;
    output.f_count = input.a_instanceCount;

    output.f_points[0] = input.a_instancePoints12.xy;
    output.f_points[1] = input.a_instancePoints12.zw;
    output.f_points[2] = input.a_instancePoints34.xy;
    output.f_points[3] = input.a_instancePoints34.zw;
    output.f_points[4] = input.a_instancePoints56.xy;
    output.f_points[5] = input.a_instancePoints56.zw;
    output.f_points[6] = input.a_instancePoints78.xy;
    output.f_points[7] = input.a_instancePoints78.zw;

    // Compute polygon AABB
    float2 lower = output.f_points[0];
    float2 upper = output.f_points[0];
    for (int i = 1; i < input.a_instanceCount; ++i) {
        lower = min(lower, output.f_points[i]);
        upper = max(upper, output.f_points[i]);
    }

    float2 center = 0.5 * (lower + upper);
    float2 width = upper - lower;
    float maxWidth = max(width.x, width.y);

    float scale = output.f_radius + 0.5 * maxWidth;
    float invScale = 1.0 / scale;

    // Shift and scale polygon points so they fit in 2x2 quad
    for (int j = 0; j < input.a_instanceCount; ++j) {
        output.f_points[j] = invScale * (output.f_points[j] - center);
    }

    // Scale radius as well
    output.f_radius = invScale * output.f_radius;

    output.f_thickness = 3.0 / (u_pixelScale * scale);

    // scale up and transform quad to fit polygon
    float x = input.a_instanceTransform.x;
    float y = input.a_instanceTransform.y;
    float c = input.a_instanceTransform.z;
    float s = input.a_instanceTransform.w;
    float2 p = float2(scale * input.a_localPosition.x, scale * input.a_localPosition.y) + center;
    p = float2((c * p.x - s * p.y) + x, (s * p.x + c * p.y) + y);
    output.position = mul(u_MVPMatrix, float4(p, 0.0, 1.0));
    return output;
}
