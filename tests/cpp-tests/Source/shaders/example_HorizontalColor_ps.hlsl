#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
    float4 pixelCoord : SV_Position;
};

Texture2D u_tex0 : register(t0, space1);

cbuffer fs_ub : register(b1, space0) {
    float2 u_screenSize;
};

float4 main(PS_IN input) : SV_Target0 {
    float4 optColor;
    float fragCoordY = input.pixelCoord.y;
    int y = int(fmod(fragCoordY / 3.0, 10.0));
    if (y == 0) optColor = float4(1.0, 0.0, 0.0, 1.0);
    else if (y == 1) optColor = float4(0.0, 1.0, 0.0, 1.0);
    else if (y == 2) optColor = float4(0.0, 0.0, 1.0, 1.0);
    else if (y == 3) optColor = float4(0.0, 1.0, 1.0, 1.0);
    else if (y == 4) optColor = float4(1.0, 0.0, 1.0, 1.0);
    else if (y == 5) optColor = float4(1.0, 1.0, 0.0, 1.0);
    else if (y == 6) optColor = float4(1.0, 1.0, 1.0, 1.0);
    else if (y == 7) optColor = float4(1.0, 0.5, 0.0, 1.0);
    else if (y == 8) optColor = float4(1.0, 0.5, 0.5, 1.0);
    else if (y == 9) optColor = float4(0.5, 0.5, 1.0, 1.0);

    return optColor * u_tex0.Sample(LinearClamp, input.v_texCoord);
}
