#version 310 es
precision highp float;
precision highp int;

layout(location = POSITION) in vec4 v_position;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec4 u_startColor;
    vec4 u_endColor;
    vec2 u_center;
    float u_radius;
    float u_expand;
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    float d = distance(v_position.xy, u_center) / u_radius;
    if (d <= 1.0)
    {
        if (d <= u_expand)
        {
            FragColor = u_startColor;
        }
        else
        {
            FragColor = mix(u_startColor, u_endColor, (d - u_expand) / (1.0 - u_expand));
        }
    }
    else
    {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
