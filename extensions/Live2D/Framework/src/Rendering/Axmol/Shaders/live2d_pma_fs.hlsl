SamplerState LinearClamp : register(s0, space1);

struct PS_IN
{
    float2 v_texCoord : TEXCOORD0;
};

cbuffer fs_ub : register(b1, space0)
{
    float4 u_baseColor;
    float4 u_multiplyColor;
    float4 u_screenColor;
};

Texture2D u_tex0 : register(t0, space1);

float4 main(PS_IN input) : SV_Target0
{
    float4 texColor = u_tex0.Sample(LinearClamp, input.v_texCoord);
    texColor.rgb = texColor.rgb * u_multiplyColor.rgb;
    texColor.rgb = texColor.rgb + u_screenColor.rgb * texColor.a - (texColor.rgb * u_screenColor.rgb);
    return texColor * u_baseColor;
}
