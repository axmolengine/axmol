#version 310 es

precision highp float;

layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(location = SV_Target0) out vec4 FragColor;

layout(set = 1, binding = 0) uniform sampler2D u_tex0;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec4 u_baseColor;
    vec4 u_multiplyColor;
    vec4 u_screenColor;
};

void main()
{
    vec4 texColor = texture(u_tex0 , v_texCoord);
    texColor.rgb = texColor.rgb * u_multiplyColor.rgb;
    texColor.rgb = texColor.rgb + u_screenColor.rgb - (texColor.rgb * u_screenColor.rgb);
    vec4 color = texColor * u_baseColor;
    FragColor = vec4(color.rgb * color.a,  color.a);
}
