#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0 : register(t0, space1);

cbuffer fs_ub : register(b1, space0) {
    float4 u_effectColor;
    float4 u_textColor;
    int u_labelPass; // 0: text, 1: outline, 2: shadow
};

float4 main(PS_IN input) : SV_Target0
{
    float4 texColor = u_tex0.Sample(LinearClamp, input.v_texCoord);
    // fontAlpha == 1 means the area of solid text (without edge)
    // fontAlpha == 0 means the area outside text, including outline area
    // fontAlpha == (0, 1) means the edge of text
    float fontAlpha = texColor.y;

    // outlineAlpha == 1 means the area of 'solid text' and 'solid outline'
    // outlineAlpha == 0 means the transparent area outside text and outline
    // outlineAlpha == (0, 1) means the edge of outline
    float outlineAlpha = texColor.x;

    if (u_labelPass == 0) // draw text
    {
        return input.v_color * float4(u_textColor.rgb, u_textColor.a * fontAlpha);
    }
    else if (u_labelPass == 1) // draw outline
    {
        // multipy (1.0 - fontAlpha) to make the inner edge of outline smoother and make the text itself transparent.
        return input.v_color * float4(u_effectColor.rgb, u_effectColor.a * outlineAlpha * (1.0 - fontAlpha));
    }
    else // draw shadow
    {
        return input.v_color * float4(u_effectColor.rgb, u_effectColor.a * outlineAlpha);
    }
}
