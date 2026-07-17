#include "base.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0;
Texture2D u_normalMap;

cbuffer fs_ub {
    float u_kBump;
    float4 u_lightPosInLocalSpace;
    float2 u_contentSize;
    float3 u_diffuseL;
};

float4 main(PS_IN input) : SV_Target0 {
    float4 texColor = u_tex0.Sample(LinearClamp, input.v_texCoord);
    float3 normal = u_normalMap.Sample(LinearClamp, input.v_texCoord).rgb;
    normal = normal * 2.0 - 1.0;
    normal.y = -normal.y;
    if (u_kBump != 1.0) {
        normal = float3(normal.x, normal.y, normal.z / u_kBump);
        normal = normalize(normal);
    }
    float4 curPixelPosInLocalSpace = float4(input.v_texCoord.x * u_contentSize.x,
                                             (1.0 - input.v_texCoord.y) * u_contentSize.y, 0.0, 1.0);
    float4 lightDir = normalize(curPixelPosInLocalSpace - u_lightPosInLocalSpace);
    float3 posToLight = -lightDir.xyz;
    float normDotPosToLight = max(0.0, dot(normal, posToLight));
    float4 diffuse = float4(normDotPosToLight * u_diffuseL, 1.0);
    float4 ambient = float4(0.5, 0.5, 0.5, 1.0);
    return texColor * float4((diffuse + ambient).rgb, diffuse.a);
}
