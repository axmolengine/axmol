SamplerState LinearClamp;

struct PS_IN
{
    float2 v_texCoord : TEXCOORD0;
    float4 v_clipPos : TEXCOORD1;
};

cbuffer fs_ub
{
    float4 u_channelFlag;
    float4 u_baseColor;
    float4 u_multiplyColor;
    float4 u_screenColor;
};

Texture2D u_tex0;
Texture2D u_tex1;

float4 main(PS_IN input) : SV_Target0
{
    float4 texColor = u_tex0.Sample(LinearClamp, input.v_texCoord);
    texColor.rgb = texColor.rgb * u_multiplyColor.rgb;
    texColor.rgb = texColor.rgb + u_screenColor.rgb * texColor.a - (texColor.rgb * u_screenColor.rgb);
    float4 col_formask = texColor * u_baseColor;
    float4 clipMask = (1.0 - u_tex1.Sample(LinearClamp, input.v_clipPos.xy / input.v_clipPos.w)) * u_channelFlag;
    float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;
    col_formask = col_formask * maskVal;
    return col_formask;
}
