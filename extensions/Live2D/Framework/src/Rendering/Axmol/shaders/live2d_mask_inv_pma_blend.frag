#version 310 es

precision highp float;

layout(location = TEXCOORD0) in vec2 v_texCoord;
layout(location = TEXCOORD1) in vec2 v_blendCoord;
layout(location = TEXCOORD2) in vec4 v_clipPos;

layout(location = SV_Target0) out vec4 FragColor;

layout(set = 1, binding = 0) uniform sampler2D u_tex0;
layout(set = 1, binding = 1) uniform sampler2D u_tex1;
layout(set = 1, binding = 2) uniform sampler2D u_blendTexture;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec4 u_baseColor;
    vec4 u_multiplyColor;
    vec4 u_screenColor;
    vec4 u_channelFlag;
    ivec4 u_blendMode;
};

#include "live2d_blend_common.glsl"

void main()
{
    vec4 texColor = texture(u_tex0, v_texCoord);
    texColor.rgb *= u_multiplyColor.rgb;
    texColor.rgb = texColor.rgb + u_screenColor.rgb * texColor.a - (texColor.rgb * u_screenColor.rgb);
    vec4 colFormask = ConvertPremultipliedToStraight(texColor * u_baseColor);
    vec4 clipMask = (1.0 - texture(u_tex1, v_clipPos.xy / v_clipPos.w)) * u_channelFlag;
    float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;
    vec4 colorSource = vec4(colFormask.rgb, colFormask.a * (1.0 - maskVal));
    vec4 colorDestination = ConvertPremultipliedToStraight(texture(u_blendTexture, v_blendCoord));
    FragColor = AlphaBlend(ColorBlend(colorSource.rgb, colorDestination.rgb, u_blendMode.x), colorSource, colorDestination, u_blendMode.y);
}
