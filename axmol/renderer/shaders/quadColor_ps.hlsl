struct PS_IN {
    float4 v_color : COLOR0;
};

cbuffer fs_ub : register(b1, space0) {
    float4 u_color;
};

float4 main(PS_IN input) : SV_Target0
{
    return input.v_color * u_color;
}
