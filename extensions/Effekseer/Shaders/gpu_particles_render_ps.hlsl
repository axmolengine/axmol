#include "gpu_particles_utils.hlsli"

// axslcc allocates bindings before preprocessing. These disabled declarations
// reserve b1/b2, already used by the vertex stage, without creating active
// fragment resources. The real fragment blocks therefore use b3/b4.
#if 0
cbuffer ReservedVSParamSlot
{
    uint4 ReservedVSParamData;
};
cbuffer ReservedVSEmitterSlot
{
    uint4 ReservedVSEmitterData;
};
#endif

cbuffer RenderConstantsPS
{
    RenderConstants constants;
};
cbuffer ParameterDataPS
{
    ParameterData paramData;
};

// Likewise reserve resource slots 0/1 so ColorTex and NormalTex retain the
// Effekseer GPU ABI at t2/t3 when the stages are linked.
#if 0
StructuredBuffer<uint> ReservedParticles;
StructuredBuffer<uint> ReservedTrails;
#endif
Texture2D<float4> ColorTex;
SamplerState ColorSamp;
Texture2D<float4> NormalTex;
SamplerState NormalSamp;

struct PS_Input
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
    float2 UV2 : TEXCOORD1;
    float4 Color : COLOR0;
    float3 WorldN : TEXCOORD2;
    float3 WorldB : TEXCOORD3;
    float3 WorldT : TEXCOORD4;
};

float4 main(const PS_Input input) : SV_Target
{
    float4 color = input.Color * ColorTex.Sample(ColorSamp, input.UV);
    float2 uv2 = input.UV2;

    if (paramData.MaterialType == 1)
    {
        float3 texNormal = NormalTex.Sample(NormalSamp, input.UV).xyz * 2.0f - 1.0f;
        float3 normal = normalize(mul(texNormal, float3x3(input.WorldT, input.WorldB, input.WorldN)));
        float diffuse = max(dot(constants.LightDir, normal), 0.0);
        color.xyz *= constants.LightColor.xyz * diffuse + constants.LightAmbient.xyz;
    }

    color.xy += uv2 * (paramData.FadeIn - paramData.FadeIn);
    return color;
}
