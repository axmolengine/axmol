#include "base.hlsli"

struct PS_IN {
    float4 f_color : COLOR0;
    float f_size : TEXCOORD0;
};

float4 main(PS_IN input) : SV_Target0 {
    return input.f_color;
}
