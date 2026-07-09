#include "base.hlsli"

struct PS_IN {
    float4 gl_FragCoord : SV_Position;
};

cbuffer fs_ub : register(b1, space0) {
    float2 center;
    float2 resolution;
    float2 u_screenSize;
    float4 u_Time;
};

float4 main(PS_IN input) : SV_Target0 {
#if AXSLC_UV_TOP
    float2 fragCoord = float2(input.gl_FragCoord.x, u_screenSize.y - input.gl_FragCoord.y);
#else
    float2 fragCoord = input.gl_FragCoord.xy;
#endif
    float fragCoordY = fragCoord.y;
    float time = u_Time[1];
    float x = fragCoord.x - (center.x - resolution.x / 2.0);
    float y = fragCoordY - (center.y - resolution.y / 2.0);
    float mov0 = x + y + cos(sin(time) * 2.0) * 100.0 + sin(x / 100.0) * 1000.0;
    float mov1 = y / resolution.y / 0.2 + time;
    float mov2 = x / resolution.x / 0.2;
    float c1 = abs(sin(mov1 + time) / 2.0 + mov2 / 2.0 - mov1 - mov2 + time);
    float c2 = abs(sin(c1 + sin(mov0 / 1000.0 + time) + sin(y / 40.0 + time) + sin((x + y) / 100.0) * 3.0));
    float c3 = abs(sin(c2 + cos(mov1 + mov2 + c2) + cos(mov2) + sin(x / 1000.0)));
    return float4(c1, c2, c3, 1.0);
}
