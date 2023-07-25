#version 310 es
precision highp float;
precision highp int;



layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    float u_alpha_value;
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    vec4 texColor = texture(u_tex0, v_texCoord);

// mimic: glAlphaFunc(GL_GREATER)
// pass if ( incoming_pixel >= u_alpha_value ) => fail if incoming_pixel < u_alpha_value

    if ( texColor.a <= u_alpha_value )
        discard;

    FragColor = texColor * v_color;
}
