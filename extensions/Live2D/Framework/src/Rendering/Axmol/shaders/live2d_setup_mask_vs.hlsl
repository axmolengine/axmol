struct VS_IN
{
    float4 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 position : SV_Position;
    float2 v_texCoord : TEXCOORD0;
    float4 v_myPos : TEXCOORD1;
};

cbuffer vs_ub : register(b0, space0)
{
    float4x4 u_clipMatrix;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    float4 pos = float4(input.a_position.x, input.a_position.y, 0.0, 1.0);
    output.position = mul(u_clipMatrix, pos);
    output.v_myPos = mul(u_clipMatrix, pos);
    output.v_texCoord = input.a_texCoord;
    output.v_texCoord.y = 1.0 - output.v_texCoord.y;
    return output;
}
