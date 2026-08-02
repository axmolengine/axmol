#include "base.hlsli"

cbuffer fs_ub
{
    float4 g_scale;
    float4 mUVInversedBack;

    float4 fFlipbookParameter; // x:enable, y:interpolationType

    float4 fUVDistortionParameter; // x:intensity, y:blendIntensity, zw:uvInversed

    float4 fBlendTextureParameter; // x:blendType

    // which is used for only softparticle
    float4 softParticleParam;
    float4 reconstructionParam1;
    float4 reconstructionParam2;
};

Texture2D u_tex0;
Texture2D u_tex1;

#ifndef DISABLED_SOFT_PARTICLE
Texture2D u_tex2;
#endif

struct PS_IN
{
    float4 position : SV_Position;
    float4 v_color : COLOR;
    float2 v_texCoord : TEXCOORD0;
    float4 v_projBinormal : TEXCOORD1;
    float4 v_projTangent : TEXCOORD2;
    float4 v_posP : TEXCOORD3;
};

#include "SoftParticle.hlsli"

float4 main(PS_IN input) : SV_Target0
{
    float4 output = u_tex0.Sample(LinearClamp, input.v_texCoord);
    output.a = output.a * input.v_color.a;

    float2 pos = input.v_posP.xy / input.v_posP.w;
    float2 posR = input.v_projTangent.xy / input.v_projTangent.w;
    float2 posU = input.v_projBinormal.xy / input.v_projBinormal.w;

    float xscale = (output.x * 2.0 - 1.0) * input.v_color.x * g_scale.x;
    float yscale = (output.y * 2.0 - 1.0) * input.v_color.y * g_scale.x;

    float2 uv = pos + (posR - pos) * xscale + (posU - pos) * yscale;

    uv = (uv + 1.0) * 0.5;
    uv.y = AX_Y_UP(uv);

    uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;

    float3 color = u_tex1.Sample(LinearClamp, uv);
    output.xyz = color;

#ifndef DISABLED_SOFT_PARTICLE
    // softparticle
    float4 screenPos = input.v_posP / input.v_posP.w;
    float2 screenUV = (screenPos.xy + 1.0f) / 2.0f;
    screenUV.y = AX_Y_UP(screenUV);

    if (softParticleParam.w != 0.0f)
    {
        float backgroundZ = u_tex2.Sample(LinearClamp, screenUV).x;
        output.a *= SoftParticle(
            backgroundZ,
            screenPos.z,
            softParticleParam,
            reconstructionParam1,
            reconstructionParam2);
    }
#endif

    if (output.a == 0.0f)
        discard;

    return output;
}
