struct VS_IN {
    float4 a_position : POSITION;
    float4 a_color : COLOR0;
    float4 a_color2 : COLOR1;
    float2 a_texCoord : TEXCOORD0;
};

struct VS_OUT {
    float4 v_light : COLOR0;
    float4 v_dark : COLOR1;
    float2 v_texCoord : TEXCOORD0;
    float4 position : SV_Position;
};

cbuffer vs_ub {
    float4x4 u_PMatrix;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.v_light = input.a_color;
    output.v_dark = input.a_color2;
    output.v_texCoord = input.a_texCoord;
    output.position = mul(u_PMatrix, input.a_position);
    return output;
}
