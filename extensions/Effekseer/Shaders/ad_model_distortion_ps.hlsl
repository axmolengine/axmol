#include "base.hlsli"

Texture2D _colorTex;
SamplerState sampler_colorTex;

Texture2D _backTex;
SamplerState sampler_backTex;

Texture2D _alphaTex;
SamplerState sampler_alphaTex;

Texture2D _uvDistortionTex;
SamplerState sampler_uvDistortionTex;

Texture2D _blendTex;
SamplerState sampler_blendTex;

Texture2D _blendAlphaTex;
SamplerState sampler_blendAlphaTex;

Texture2D _blendUVDistortionTex;
SamplerState sampler_blendUVDistortionTex;

#ifndef DISABLED_SOFT_PARTICLE
Texture2D _depthTex;
SamplerState sampler_depthTex;
#endif

#include "Distortion_Common.hlsli"

struct PS_Input
{
	float4 PosVS : SV_POSITION;
	// xy uv z - FlipbookRate, w - AlphaThreshold
	linear centroid float4 UV_Others : TEXCOORD0;
	float4 ProjBinormal : TEXCOORD1;
	float4 ProjTangent : TEXCOORD2;
	float4 PosP : TEXCOORD3;
	linear centroid float4 Color : COLOR0;

	float4 Alpha_Dist_UV : TEXCOORD4;
	float4 Blend_Alpha_Dist_UV : TEXCOORD5;

	// BlendUV, FlipbookNextIndexUV
	float4 Blend_FBNextIndex_UV : TEXCOORD6;
};

#include "ad_common_ps.hlsli"
#include "SoftParticle.hlsli"

float4 main(const PS_Input Input)
	: SV_Target
{
	AdvancedParameter advancedParam = DisolveAdvancedParameter(Input);

	float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, advancedParam.UVDistortionUV, fUVDistortionParameter.zw, false);
	UVOffset *= fUVDistortionParameter.x;

	float4 Output = _colorTex.Sample(sampler_colorTex, Input.UV_Others + UVOffset);

	Output.a = Output.a * Input.Color.a;

	ApplyFlipbook(Output, _colorTex, sampler_colorTex, fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, advancedParam.FlipbookRate, false);

	// apply alpha texture
	float4 AlphaTexColor = _alphaTex.Sample(sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
	Output.a *= AlphaTexColor.r * AlphaTexColor.a;

	// blend texture uv offset
	float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, advancedParam.BlendUVDistortionUV, fUVDistortionParameter.zw, false);
	BlendUVOffset *= fUVDistortionParameter.y;

	float4 BlendTextureColor = _blendTex.Sample(sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
	float4 BlendAlphaTextureColor = _blendAlphaTex.Sample(sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
	BlendTextureColor.a *= BlendAlphaTextureColor.r * BlendAlphaTextureColor.a;

	ApplyTextureBlending(Output, BlendTextureColor, fBlendTextureParameter.x);

	// zero + alpha threshold
	if (Output.a <= max(0.0, advancedParam.AlphaThreshold))
	{
		discard;
	}

	float2 pos = Input.PosP.xy / Input.PosP.w;
	float2 posR = Input.ProjTangent.xy / Input.ProjTangent.w;
	float2 posU = Input.ProjBinormal.xy / Input.ProjBinormal.w;

	float xscale = (Output.x * 2.0 - 1.0) * Input.Color.x * g_scale.x;
	float yscale = (Output.y * 2.0 - 1.0) * Input.Color.y * g_scale.x;

	float2 uv = pos + (posR - pos) * xscale + (posU - pos) * yscale;

	uv = (uv + 1.0) * 0.5;
	uv.y = AX_Y_UP(uv);

	uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;

	float3 color = _backTex.Sample(sampler_backTex, uv);
	Output.xyz = color;

#ifndef DISABLED_SOFT_PARTICLE
	// softparticle
	float4 screenPos = Input.PosP / Input.PosP.w;
	float2 screenUV = (screenPos.xy + 1.0f) / 2.0f;
	screenUV.y = AX_Y_UP(screenUV);

	if (softParticleParam.w != 0.0f)
	{
		float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
		Output.a *= SoftParticle(
			backgroundZ,
			screenPos.z,
			softParticleParam,
			reconstructionParam1,
			reconstructionParam2);
	}
#endif


	return Output;
}
