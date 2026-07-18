struct PS_IN {
    float4 v_color : COLOR0;
};

float4 main(PS_IN input) : SV_Target0
{
    return input.v_color;
}
