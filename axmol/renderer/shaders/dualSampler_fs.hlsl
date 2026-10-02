#include "base.hlsli"

#if defined(USE_POINT_SAMPLER) && USE_POINT_SAMPLER
#define TEXTURE_SAMPLER PointClamp
#else
#define TEXTURE_SAMPLER LinearClamp
#endif

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2DArray u_tex0;

float4 main(PS_IN input) : SV_Target0
{
    float3 uv0 = float3(input.v_texCoord, 0.0);
    float3 uv1 = float3(input.v_texCoord, 1.0);

    float3 rgb = u_tex0.Sample(TEXTURE_SAMPLER, uv0).rgb;
    float a    = u_tex0.Sample(TEXTURE_SAMPLER, uv1).r;

    float4 texColor = float4(rgb, a);
    texColor.rgb *= texColor.a;

    return input.v_color * texColor;
}
