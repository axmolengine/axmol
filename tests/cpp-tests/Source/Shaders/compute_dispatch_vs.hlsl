StructuredBuffer<float4> Colors;
cbuffer VSConstants
{
    float4x4 mvp;
    int colorCount;
};

struct VSInput
{
    float3 pos : POSITION0;
    float id : TEXCOORD0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

VSOutput main(const VSInput input)
{
    VSOutput output;
    output.pos = mul(mvp, float4(input.pos, 1.0f));
    uint index = colorCount > 0 ? (uint(input.id) % uint(colorCount)) : 0u;
    output.color = Colors[index];
    return output;
}
