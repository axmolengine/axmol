#include "base.hlsli"

struct PS_IN {
    float4 gl_FragCoord : SV_Position;
};

cbuffer fs_ub {
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
    float2 iResolution = resolution;
    float iGlobalTime = u_Time[1];

    float pointRadius = 0.06;
    float linkSize = 0.04;
    float noiseStrength = 0.08;

    float minDimension = min(iResolution.x, iResolution.y);
    float2 bounds = float2(iResolution.x / minDimension, iResolution.y / minDimension);
    float2 uv = (2.0 * fragCoord.xy - center.xy) / iResolution.xy;

    float3 pointR = float3(0.0, 0.0, 1.0);
    float3 pointG = float3(0.0, 0.0, 1.0);
    float3 pointB = float3(0.0, 0.0, 1.0);

    pointR.x += 0.32 * sin(1.32 * iGlobalTime);
    pointR.y += 0.3 * sin(1.03 * iGlobalTime);
    pointR.z += 0.4 * sin(1.32 * iGlobalTime);

    pointG.x += 0.31 * sin(0.92 * iGlobalTime);
    pointG.y += 0.29 * sin(0.99 * iGlobalTime);
    pointG.z += 0.38 * sin(1.24 * iGlobalTime);

    pointB.x += 0.33 * sin(1.245 * iGlobalTime);
    pointB.y += 0.3 * sin(1.41 * iGlobalTime);
    pointB.z += 0.41 * sin(1.11 * iGlobalTime);

    float2 midUV = float2(bounds.x * 0.5, bounds.y * 0.5);
    pointR.xy += midUV;
    pointG.xy += midUV;
    pointB.xy += midUV;

    float2 vecToR = pointR.xy - uv;
    float2 vecToG = pointG.xy - uv;
    float2 vecToB = pointB.xy - uv;

    float2 dirToR = normalize(vecToR.xy);
    float2 dirToG = normalize(vecToG.xy);
    float2 dirToB = normalize(vecToB.xy);

    float distToR = length(vecToR);
    float distToG = length(vecToG);
    float distToB = length(vecToB);

    float dotRG = dot(dirToR, dirToG);
    float dotGB = dot(dirToG, dirToB);
    float dotBR = dot(dirToB, dirToR);

    float4 FragColor;
    FragColor.x = 1.0 - smoothstep(distToR, 0.0, pointRadius * pointR.z);
    FragColor.y = 1.0 - smoothstep(distToG, 0.0, pointRadius * pointG.z);
    FragColor.z = 1.0 - smoothstep(distToB, 0.0, pointRadius * pointB.z);
    FragColor.w = 1.0;

    float linkStrengthRG = 1.0 - smoothstep(dotRG, -1.01, -1.0 + (linkSize * pointR.z * pointG.z));
    float linkStrengthGB = 1.0 - smoothstep(dotGB, -1.01, -1.0 + (linkSize * pointG.z * pointB.z));
    float linkStrengthBR = 1.0 - smoothstep(dotBR, -1.01, -1.0 + (linkSize * pointB.z * pointR.z));

    float sumDistRG = distToR + distToG;
    float sumDistGB = distToG + distToB;
    float sumDistBR = distToB + distToR;

    float contribRonRG = 1.0 - (distToR / sumDistRG);
    float contribRonBR = 1.0 - (distToR / sumDistBR);

    float contribGonRG = 1.0 - (distToG / sumDistRG);
    float contribGonGB = 1.0 - (distToG / sumDistGB);

    float contribBonGB = 1.0 - (distToB / sumDistGB);
    float contribBonBR = 1.0 - (distToB / sumDistBR);

    FragColor.x += (linkStrengthRG * contribRonRG) + (linkStrengthBR * contribRonBR);
    FragColor.y += (linkStrengthGB * contribGonGB) + (linkStrengthRG * contribGonRG);
    FragColor.z += (linkStrengthBR * contribBonBR) + (linkStrengthGB * contribBonGB);

    FragColor.xyz *= (1.0 - noiseStrength) + (noiseStrength * 0.0);

    return FragColor;
}
