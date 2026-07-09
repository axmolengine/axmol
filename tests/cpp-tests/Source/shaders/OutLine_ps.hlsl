cbuffer fs_ub : register(b1, space0)
{
    float3 OutLineColor;
    float4 u_color;
};

float4 main() : SV_Target0
{
    return float4(OutLineColor, 1.0) * u_color;
}
