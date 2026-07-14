struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

float4 main(PS_IN input) : SV_Target0
{
    return input.v_color * step(0.0, 1.0 - length(input.v_texCoord));
}
