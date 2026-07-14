#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
    float3 v_normal : NORMAL;
};

Texture2D u_details[4] : register(t0, space1); // will take slot 0~3
Texture2D u_alphaMap : register(t4, space1);
Texture2D u_lightMap : register(t5, space1);

cbuffer fs_ub : register(b1, space0) {
    int u_has_alpha;
    int u_has_light_map;
    vfloat_def(u_detailSize, 4);
    float4 u_lightDir;
};

float4 main(PS_IN input) : SV_Target0
{
    float4 lightColor;
    if (u_has_light_map <= 0)
    {
        lightColor = float4(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
        lightColor = u_lightMap.Sample(LinearClamp, input.v_texCoord);
    }
    float lightFactor = dot(-u_lightDir.xyz, input.v_normal);
    if (u_has_alpha <= 0)
    {
        return u_details[0].Sample(LinearWrap, input.v_texCoord) * lightColor * lightFactor;
    }
    else
    {
        float4 blendFactor = u_alphaMap.Sample(LinearClamp, input.v_texCoord);
        float4 color = u_details[0].Sample(LinearWrap, input.v_texCoord * vfloat_at(u_detailSize, 0)) * blendFactor.r +
        u_details[1].Sample(LinearWrap, input.v_texCoord * vfloat_at(u_detailSize, 1)) * blendFactor.g + u_details[2].Sample(LinearWrap, input.v_texCoord * vfloat_at(u_detailSize, 2)) * blendFactor.b
            + u_details[3].Sample(LinearWrap, input.v_texCoord * vfloat_at(u_detailSize, 3)) * (1.0 - blendFactor.a);
        return float4(color.rgb * lightColor.rgb * lightFactor, 1.0);
    }
}
