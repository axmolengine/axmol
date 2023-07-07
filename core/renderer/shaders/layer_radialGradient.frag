#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
precision highp float;
#endif
layout(std140, binding = 0) uniform Block_0 {
    vec4 u_startColor;
    vec4 u_endColor;
    vec2 u_center;
    float u_radius;
    float u_expand;
};
#ifdef GL_ES
layout (location = 0) in lowp vec4 v_position;
#else
layout (location = 1) in vec4 v_position;
#endif
layout (location = 0) out vec4 FragColor;
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
