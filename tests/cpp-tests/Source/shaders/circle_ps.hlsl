#include "base.hlsli"

struct PS_IN {
    float2 f_position : TEXCOORD0;
    float4 f_color : TEXCOORD1;
    float f_thickness : TEXCOORD2;
};

float4 main(PS_IN input) : SV_Target0 {
    float radius = 1.0;
    float dw = length(input.f_position);
    float d = abs(dw - radius);
    return float4(input.f_color.rgb, smoothstep(input.f_thickness, 0.0, d));
}
