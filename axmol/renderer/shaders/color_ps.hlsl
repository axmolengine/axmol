cbuffer fs_ub : register(b1, space0) {
    float4 u_color;
};

float4 main() : SV_Target0
{
    return u_color;
}
