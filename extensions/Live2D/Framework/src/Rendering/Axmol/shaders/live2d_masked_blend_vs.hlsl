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
    float4 v_clipPos : TEXCOORD2;
};

cbuffer vs_ub : register(b0, space0)
{
    float4x4 u_matrix;
    float4x4 u_clipMatrix;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    float4 pos = float4(input.a_position.x, input.a_position.y, 0.0, 1.0);
    float4 gl_Position = mul(u_matrix, pos);
    output.position = gl_Position;
    output.v_clipPos = mul(u_clipMatrix, pos);
    output.v_clipPos = float4(output.v_clipPos.x, 1.0 - output.v_clipPos.y, output.v_clipPos.zw);
    output.v_texCoord = input.a_texCoord;
    output.v_texCoord.y = 1.0 - output.v_texCoord.y;

    float2 ndcPos = gl_Position.xy / gl_Position.w;
    output.v_blendCoord = ndcPos * 0.5 + 0.5;

    return output;
}
