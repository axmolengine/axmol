#include "base.hlsli"
#include "colorUtils.hlsli"

struct PS_IN {
    float4 v_color : COLOR0;
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0; // Y sample: LumaTexture
Texture2D u_tex1; // UV sample: ChromaTexture

cbuffer fs_ub {
    float4x4 colorTransform;
};

float4 main(PS_IN input) : SV_Target0
{
    float3 YUV;

    YUV.x = u_tex0.Sample(LinearClamp, input.v_texCoord).x;    // Y
    YUV.yz = u_tex1.Sample(PointClamp, input.v_texCoord).RG8_CHANNEL; // CbCr

    /* Convert YUV to RGB */
    float4 OutColor;
    OutColor.xyz = trasnformYUV(YUV, colorTransform);
    OutColor.w = 1.0;

    return input.v_color * OutColor;
}
