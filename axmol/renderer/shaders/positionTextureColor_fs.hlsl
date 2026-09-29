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

Texture2D u_tex0;

float4 main(PS_IN input) : SV_Target0
{
    return input.v_color * u_tex0.Sample(TEXTURE_SAMPLER, input.v_texCoord);
}
