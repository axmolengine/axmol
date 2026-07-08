#include "base.hlsli"
#include "colorUtils.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0 : register(t0, space1); // Y sample: LumaTexture
Texture2D u_tex1 : register(t1, space1); // U sample: ChromaTexture
Texture2D u_tex2 : register(t2, space1); // V sample: ChromaTexture

cbuffer fs_ub : register(b1, space0) {
    float4x4 colorTransform;
};

float4 main(PS_IN input) : SV_Target0
{
    float3 YUV;

    YUV.x = u_tex0.Sample(LinearClamp, input.v_texCoord).x; // Y
    YUV.y = u_tex1.Sample(PointClamp, input.v_texCoord).x;  // U
    YUV.z = u_tex2.Sample(PointClamp, input.v_texCoord).x;  // V

    /* Convert YUV to RGB */
    float4 OutColor;
    OutColor.xyz = trasnformYUV(YUV, colorTransform);
    OutColor.w = 1.0;

    return input.v_color * OutColor;
}
