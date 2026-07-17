SamplerState LinearClamp;

struct PS_IN
{
    float2 v_texCoord : TEXCOORD0;
};

cbuffer fs_ub
{
    float4 u_baseColor;
};

Texture2D u_tex0;

float4 main(PS_IN input) : SV_Target0
{
    return u_tex0.Sample(LinearClamp, input.v_texCoord) * u_baseColor;
}
