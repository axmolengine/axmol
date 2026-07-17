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
    float4 v_clipPos : TEXCOORD1;
};

cbuffer vs_ub
{
    float4x4 u_matrix;
    float4x4 u_clipMatrix;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    float4 pos = float4(input.a_position.x, input.a_position.y, 0.0, 1.0);
    output.position = mul(u_matrix, pos);
    output.v_clipPos = mul(u_clipMatrix, pos);
#if AXSLC_UV_TOP
    output.v_clipPos = float4(output.v_clipPos.x, 1.0 - output.v_clipPos.y, output.v_clipPos.zw);
#endif
    output.v_texCoord = input.a_texCoord;
    // a_texCoord is Live2D model UVs in top-left origin (V=0=top).
    // Must unconditionally flip for all backends.
    output.v_texCoord.y = 1.0 - output.v_texCoord.y;
    return output;
}
