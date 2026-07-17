#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0;

#define FILTER_SIZE 3
#define COLOR_LEVELS 7.0
#define EDGE_FILTER_SIZE 3
#define EDGE_THRESHOLD 0.05

cbuffer fs_ub {
    float2 resolution;
};

float4 edgeFilter(float2 texCoord, int px, int py)
{
    float4 color = float4(0.0, 0.0, 0.0, 0.0);

    for (int y = -EDGE_FILTER_SIZE; y <= EDGE_FILTER_SIZE; ++y)
    {
        for (int x = -EDGE_FILTER_SIZE; x <= EDGE_FILTER_SIZE; ++x)
        {
            color += u_tex0.Sample(LinearClamp, texCoord + float2(px + x, py + y) / resolution.xy);
        }
    }

    color /= float((2 * EDGE_FILTER_SIZE + 1) * (2 * EDGE_FILTER_SIZE + 1));

    return color;
}

float4 main(PS_IN input) : SV_Target0 {
    float4 color = float4(0.0, 0.0, 0.0, 0.0);

    for (int y = -FILTER_SIZE; y <= FILTER_SIZE; ++y)
    {
        for (int x = -FILTER_SIZE; x <= FILTER_SIZE; ++x)
        {
            color += u_tex0.Sample(LinearClamp, input.v_texCoord + float2(x, y) / resolution.xy);
        }
    }

    color /= float((2 * FILTER_SIZE + 1) * (2 * FILTER_SIZE + 1));

    for (int c = 0; c < 3; ++c)
    {
        color[c] = floor(COLOR_LEVELS * color[c]) / COLOR_LEVELS;
    }

    float4 sum = abs(edgeFilter(input.v_texCoord, 0, 1) - edgeFilter(input.v_texCoord, 0, -1));
    sum += abs(edgeFilter(input.v_texCoord, 1, 0) - edgeFilter(input.v_texCoord, -1, 0));
    sum /= 2.0;

    if (length(sum) > EDGE_THRESHOLD)
    {
        color.rgb = float3(0.0, 0.0, 0.0);
    }

    return color;
}
