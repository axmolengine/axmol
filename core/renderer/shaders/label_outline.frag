#version 310 es
precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec4 u_effectColor;
    vec4 u_textColor;
    int u_labelPass; // 0: text, 1: outline, 2: shadow
};

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
    vec4 texColor = texture(u_tex0, v_texCoord);
    // fontAlpha == 1 means the area of solid text (without edge)
    // fontAlpha == 0 means the area outside text, including outline area
    // fontAlpha == (0, 1) means the edge of text
#ifndef GLES2
    float fontAlpha = texColor.y;
#else
    float fontAlpha = texColor.w;
#endif

    // outlineAlpha == 1 means the area of 'solid text' and 'solid outline'
    // outlineAlpha == 0 means the transparent area outside text and outline
    // outlineAlpha == (0, 1) means the edge of outline
    float outlineAlpha = texColor.x;

    if (u_labelPass == 0) // draw text
    {
        FragColor = v_color * vec4(u_textColor.rgb, u_textColor.a * fontAlpha);
    }
    else if (u_labelPass == 1) // draw outline
    {
        // multipy (1.0 - fontAlpha) to make the inner edge of outline smoother and make the text itself transparent.
        FragColor = v_color * vec4(u_effectColor.rgb, u_effectColor.a * outlineAlpha * (1.0 - fontAlpha));
    }
    else // draw shadow
    {
        FragColor = v_color * vec4(u_effectColor.rgb, u_effectColor.a * outlineAlpha);
    }
}
