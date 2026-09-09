struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

float4 main(const PSInput input) : SV_Target
{
    return input.color;
}
