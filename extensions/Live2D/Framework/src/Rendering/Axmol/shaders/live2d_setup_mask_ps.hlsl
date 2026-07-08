SamplerState LinearClamp : register(s0, space1);

struct PS_IN
{
    float2 v_texCoord : TEXCOORD0;
    float4 v_myPos : TEXCOORD1;
};

cbuffer fs_ub : register(b1, space0)
{
    float4 u_channelFlag;
    float4 u_baseColor;
};

Texture2D u_tex0 : register(t0, space1);

float4 main(PS_IN input) : SV_Target0
{
    float isInside = 
          step(u_baseColor.x, input.v_myPos.x / input.v_myPos.w)
        * step(u_baseColor.y, input.v_myPos.y / input.v_myPos.w)
        * step(input.v_myPos.x / input.v_myPos.w, u_baseColor.z)
        * step(input.v_myPos.y / input.v_myPos.w, u_baseColor.w);

    return u_channelFlag * u_tex0.Sample(LinearClamp, input.v_texCoord).a * isInside;
}
