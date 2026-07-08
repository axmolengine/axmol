struct VS_IN {
    float2 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
    float4 a_color : COLOR0;
};

struct VS_OUT {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
    float4 position : SV_Position;
};

cbuffer vs_ub : register(b0, space0) {
    float4x4 u_MVPMatrix;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    output.v_texCoord = input.a_texCoord;
    output.v_color = input.a_color;
    output.position = mul(u_MVPMatrix, float4(input.a_position, 0.0, 1.0));
    return output;
}
