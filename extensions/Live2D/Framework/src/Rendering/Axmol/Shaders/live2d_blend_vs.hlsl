#include "base.hlsli"

struct VS_IN
{
    float4 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 position : SV_Position;
    float2 v_texCoord : TEXCOORD0;
    float2 v_blendCoord : TEXCOORD1;
};

cbuffer vs_ub
{
    float4x4 u_matrix;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    float4 pos = float4(input.a_position.x, input.a_position.y, 0.0, 1.0);
    float4 gl_Position = mul(u_matrix, pos);
    output.position = gl_Position;
    output.v_texCoord = input.a_texCoord;
    // a_texCoord is Live2D model UVs in top-left origin (V=0=top).
    // Must unconditionally flip for all backends.
    output.v_texCoord.y = 1.0 - output.v_texCoord.y;

    float2 ndcPos = gl_Position.xy / gl_Position.w;
    output.v_blendCoord = ndcPos * 0.5 + 0.5;
    output.v_blendCoord.y = AX_Y_UP(output.v_blendCoord);

    return output;
}
