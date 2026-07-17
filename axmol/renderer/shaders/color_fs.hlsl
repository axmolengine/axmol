cbuffer fs_ub {
    float4 u_color;
};

float4 main() : SV_Target0
{
    return u_color;
}
