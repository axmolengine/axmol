#version 310 es
precision highp float;
precision highp int;



layout(location = 0) in vec4 v_fragmentColor;
layout(location = 1) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec4 u_textColor;
};

layout(location = 0) out vec4 FragColor;

void main()
{
    vec4 color = texture(u_tex0, v_texCoord);
    //the texture use dual channel 16-bit output for distance_map
    //float dist = color.b+color.g/256.0;
    // the texture use single channel 8-bit output for distance_map
    float dist = color.a;
    //TODO: Implementation 'fwidth' for glsl 1.0
    //float width = fwidth(dist);
    //assign width for constant will lead to a little bit fuzzy,it's temporary measure.
    float width = 0.04;
    float alpha = smoothstep(0.5-width, 0.5+width, dist) * u_textColor.a;
    FragColor = v_fragmentColor * vec4(u_textColor.rgb,alpha);
}
