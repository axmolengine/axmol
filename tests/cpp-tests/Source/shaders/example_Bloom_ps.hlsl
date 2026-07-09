#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float4 v_color : COLOR0;
};

Texture2D u_tex0 : register(t0, space1);

const float blurSize = 1.0 / 512.0;
const float intensity = 0.35;

cbuffer fs_ub : register(b1, space0) {
    float2 resolution;
};

float4 main(PS_IN input) : SV_Target0 {
    float4 sum = float4(0.0, 0.0, 0.0, 0.0);
    float2 texcoord = input.v_texCoord.xy;
    int j;
    int i;

    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x - 4.0 * blurSize, texcoord.y)) * 0.05;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x - 3.0 * blurSize, texcoord.y)) * 0.09;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x - 2.0 * blurSize, texcoord.y)) * 0.12;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x - blurSize, texcoord.y)) * 0.15;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x, texcoord.y)) * 0.16;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x + blurSize, texcoord.y)) * 0.15;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x + 2.0 * blurSize, texcoord.y)) * 0.12;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x + 3.0 * blurSize, texcoord.y)) * 0.09;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x + 4.0 * blurSize, texcoord.y)) * 0.05;

    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x, texcoord.y - 4.0 * blurSize)) * 0.05;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x, texcoord.y - 3.0 * blurSize)) * 0.09;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x, texcoord.y - 2.0 * blurSize)) * 0.12;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x, texcoord.y - blurSize)) * 0.15;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x, texcoord.y)) * 0.16;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x, texcoord.y + blurSize)) * 0.15;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x, texcoord.y + 2.0 * blurSize)) * 0.12;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x, texcoord.y + 3.0 * blurSize)) * 0.09;
    sum += u_tex0.Sample(LinearClamp, float2(texcoord.x, texcoord.y + 4.0 * blurSize)) * 0.05;

    return sum * intensity + u_tex0.Sample(LinearClamp, texcoord);
}
