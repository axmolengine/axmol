#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float4 position : SV_Position;
};

Texture2D u_tex0;

float4 getColorByCoord(int y) {
    if (y < 5) {
        if (y == 0) {
            return float4(1.0, 0.0, 0.0, 1.0);
        } else if (y == 1) {
            return float4(0.0, 1.0, 0.0, 1.0);
        } else if (y == 2) {
            return float4(0.0, 0.0, 1.0, 1.0);
        } else if (y == 3) {
            return float4(0.0, 1.0, 1.0, 1.0);
        } else {
            return float4(1.0, 0.0, 1.0, 1.0);
        }
    } else {
        if (y == 5) {
            return float4(1.0, 1.0, 0.0, 1.0);
        } else if (y == 6) {
            return float4(1.0, 1.0, 1.0, 1.0);
        } else if (y == 7) {
            return float4(1.0, 0.5, 0.0, 1.0);
        } else if (y == 8) {
            return float4(1.0, 0.5, 0.5, 1.0);
        } else {
            return float4(0.5, 0.5, 1.0, 1.0);
        }
    }
}

float4 main(PS_IN input) : SV_Target0 {
    int y = int(fmod(input.position.y / 10.0, 10.0));
    return getColorByCoord(y) * u_tex0.Sample(LinearClamp, input.v_texCoord);
}
