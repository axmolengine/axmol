#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_uv : TEXCOORD0;
};

Texture2D u_fontAtlas : register(t0, space1);

float4 main(PS_IN input) : SV_Target0 {
    float alpha = u_fontAtlas.Sample(LinearClamp, input.v_uv).r;
    return float4(input.v_color.rgb, input.v_color.a * alpha);
}
