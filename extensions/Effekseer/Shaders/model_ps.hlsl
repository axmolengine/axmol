#include "base.hlsli"

cbuffer fs_ub
{
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;

    float4 fFlipbookParameter; // x:enable, y:interpolationType

    float4 fUVDistortionParameter; // x:intensity, y:blendIntensity, zw:uvInversed

    float4 fBlendTextureParameter; // x:blendType

    float4 fCameraFrontDirection;

    float4 fFalloffParameter; // x:enable, y:colorblendtype, z:pow
    float4 fFalloffBeginColor;
    float4 fFalloffEndColor;

    float4 fEmissiveScaling; // x:emissiveScaling

    float4 fEdgeColor;
    float4 fEdgeParameter; // x:threshold, y:colorScaling

    // which is used for only softparticle
    float4 softParticleParam;
    float4 reconstructionParam1;
    float4 reconstructionParam2;
    float4 mUVInversedBack;
    float4 miscFlags;
};

Texture2D u_tex0;

#if ENABLE_LIGHTING

Texture2D u_tex1;

#ifndef DISABLED_SOFT_PARTICLE
Texture2D u_tex2;
#endif

#else

#ifndef DISABLED_SOFT_PARTICLE
Texture2D u_tex1;
#endif

#endif

struct PS_IN
{
    float4 position : SV_Position;
    float4 v_color : COLOR;
    float2 v_texCoord : TEXCOORD0;
#if ENABLE_LIGHTING
    half3 v_worldN : TEXCOORD1;
    half3 v_worldB : TEXCOORD2;
    half3 v_worldT : TEXCOORD3;
#endif
    float4 v_posP : TEXCOORD4;
};

#include "SoftParticle.hlsli"
#include "Linear_sRGB.hlsli"

float4 main(PS_IN input) : SV_Target0
{
    bool convertColorSpace = miscFlags.x != 0.0f;

    float4 output = ConvertFromSRGBTexture(u_tex0.Sample(LinearClamp, input.v_texCoord), convertColorSpace) * input.v_color;

#if ENABLE_LIGHTING
    half3 texNormal = (u_tex1.Sample(LinearClamp, input.v_texCoord).xyz - 0.5) * 2.0;
    half3 localNormal = (half3)normalize(
        mul(
            texNormal,
            half3x3((half3)input.v_worldT, (half3)input.v_worldB, (half3)input.v_worldN)));

    float diffuse = max(dot(fLightDirection.xyz, localNormal.xyz), 0.0);
    output.xyz = output.xyz * (fLightColor.xyz * diffuse + fLightAmbient.xyz);
#endif

    output.rgb *= fEmissiveScaling.x;

#ifndef DISABLED_SOFT_PARTICLE
    // softparticle
    float4 screenPos = input.v_posP / input.v_posP.w;
    float2 screenUV = (screenPos.xy + 1.0f) / 2.0f;
    screenUV.y = AX_Y_UP(screenUV);
    screenUV.y = mUVInversedBack.x + mUVInversedBack.y * screenUV.y;

    if (softParticleParam.w != 0.0f)
    {
        float backgroundZ = u_tex1.Sample(LinearClamp, screenUV).x;
        output.a *= SoftParticle(
            backgroundZ,
            screenPos.z,
            softParticleParam,
            reconstructionParam1,
            reconstructionParam2);
    }
#endif

    if (output.a == 0.0)
        discard;

    return ConvertToScreen(output, convertColorSpace);
}
