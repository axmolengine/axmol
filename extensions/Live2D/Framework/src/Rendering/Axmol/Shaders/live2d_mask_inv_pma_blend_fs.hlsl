SamplerState LinearClamp;

struct PS_IN
{
    float2 v_texCoord : TEXCOORD0;
    float2 v_blendCoord : TEXCOORD1;
    float4 v_clipPos : TEXCOORD2;
};

cbuffer fs_ub
{
    float4 u_baseColor;
    float4 u_multiplyColor;
    float4 u_screenColor;
    float4 u_channelFlag;
    int4 u_blendMode;
};

Texture2D u_tex0;
Texture2D u_tex1;
Texture2D u_blendTexture;

#include "live2d_blend_common.hlsli"

float4 main(PS_IN input) : SV_Target0
{
    float4 texColor = u_tex0.Sample(LinearClamp, input.v_texCoord);
    texColor.rgb *= u_multiplyColor.rgb;
    texColor.rgb = texColor.rgb + u_screenColor.rgb * texColor.a - (texColor.rgb * u_screenColor.rgb);
    float4 colFormask = ConvertPremultipliedToStraight(texColor * u_baseColor);
    float4 clipMask = (1.0 - u_tex1.Sample(LinearClamp, input.v_clipPos.xy / input.v_clipPos.w)) * u_channelFlag;
    float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;
    float4 colorSource = float4(colFormask.rgb, colFormask.a * (1.0 - maskVal));
    float4 colorDestination = ConvertPremultipliedToStraight(u_blendTexture.Sample(LinearClamp, input.v_blendCoord));
    return AlphaBlend(ColorBlend(colorSource.rgb, colorDestination.rgb, u_blendMode.x), colorSource, colorDestination, u_blendMode.y);
}
