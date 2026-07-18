struct PS_IN {
    float4 v_position : POSITION;
};

cbuffer fs_ub {
    float4 u_startColor;
    float4 u_endColor;
    float2 u_center;
    float u_radius;
    float u_expand;
};

float4 main(PS_IN input) : SV_Target0
{
    float d = distance(input.v_position.xy, u_center) / u_radius;
    if (d <= 1.0)
    {
        if (d <= u_expand)
        {
            return u_startColor;
        }
        else
        {
            return lerp(u_startColor, u_endColor, (d - u_expand) / (1.0 - u_expand));
        }
    }
    else
    {
        return float4(0.0, 0.0, 0.0, 0.0);
    }
}
